# C++ Codebase Audit

**Date:** 2026-06-22  
**Scope:** Full codebase — all layers (API Gateway, Business Service, Data Service, SystemFramework, Shared)  
**Standard:** C++20, OWASP, CWE memory safety guidelines

Issues are grouped by severity: **Critical → High → Medium → Low**.

---

## Critical

---

### C-1 — `delete void*` in `ScopedContainer` destructor is undefined behaviour

**File:** `Shared/SystemFramework/DependencyInjection/ScopedContainer.h:36`

```cpp
~ScopedContainer() {
    for (std::pair<const std::type_index, void*>& pair : _instances) {
        delete pair.second;  // UB
    }
}
```

`delete` on a `void*` is undefined behaviour per the C++ standard (CWE-590). The compiler frees the raw memory but **never calls the destructor** of the pointed-to object. Any resource held by the stored service (open handles, allocated memory, etc.) leaks silently. Every resolved service in the system is affected by this.

**Fix:** Store a typed deleter alongside each instance so the correct destructor is invoked.

```cpp
std::unordered_map<std::type_index, std::pair<void*, std::function<void(void*)>>> _instances;

// In Register<T> factory result, store:
_instances[key] = { instance, [](void* p) { delete static_cast<TInterface*>(p); } };

// In destructor:
for (auto& [key, entry] : _instances) {
    entry.second(entry.first);
}
```

---

### C-2 — Missing virtual destructor on `IProductRepository`

**File:** `Interfaces/DataServiceInterface/IProductRepository.h`

`IProductRepository` declares pure virtual methods but has no virtual destructor. `ProductRepository` inherits from it. When `ScopedContainer` deletes an instance via an `IProductRepository*` (once C-1 is fixed), the `ProductRepository` destructor is never called — undefined behaviour (CWE-1045).

**Fix:** Add to `IProductRepository`:

```cpp
virtual ~IProductRepository() = default;
```

---

### C-3 — Mapper `Map()` is exception-unsafe and performs a pointless heap round-trip

**File:** `Shared/SystemFramework/Mapping/Mapping.h:21`

```cpp
template<typename TSource, typename TDest> TDest Map(const TSource& source) {
    TDest* result = static_cast<TDest*>(it->second((void*)&source));
    TDest value = *result;   // if TDest copy-ctor throws, result leaks
    delete result;
    return value;
}
```

There are two problems here:

1. **Exception leak:** If `TDest`'s copy constructor throws, `result` is never deleted — a heap leak with no remedy.
2. **Unnecessary allocation:** A heap object is created only to be immediately copied and deleted. This is pure overhead.

The stored lambda (`void*(void*)`) erases type information unnecessarily. The function signature can carry the type directly.

**Fix:** Store `std::function<TDest(const TSource&)>` directly and invoke it without touching the heap:

```cpp
// In _mappings, store the original mappingFunc by key — no void* needed.
// Map() becomes:
template<typename TSource, typename TDest> TDest Map(const TSource& source) {
    auto key = std::make_pair(std::type_index(typeid(TSource)), std::type_index(typeid(TDest)));
    auto it = _mappings.find(key);
    if (it == _mappings.end())
        throw std::runtime_error("No mapping registered for these types");
    return it->second(source);
}
```

This removes all `void*` usage, the heap allocation, and the exception-unsafety in one change.

---

## High

---

### H-1 — Connection string stored in plain-text config file

**File:** `Backend/appsettings.json` (loaded in `Main.cpp:27`)

The database connection string (which contains credentials) is read from a plain JSON file on disk. If this file is committed to version control or the working directory is accessible, credentials are exposed.

**Fix:** Read credentials from environment variables at startup. Keep non-sensitive settings (host, port, paths) in `appsettings.json`.

```cpp
const char* env = std::getenv("DB_CONNECTION_STRING");
if (!env) throw std::runtime_error("DB_CONNECTION_STRING env var not set");
connectionString = env;
```

---

### H-2 — No authentication or authorisation on any endpoint

**File:** `Services/APIGateway/ProductGateway/ProductGateway.cpp`

All five endpoints (`POST /product`, `GET /products`, `GET /product/:id`, `DELETE /product/:id`, `PUT /product/:id`) are publicly accessible with no token, session, or API-key check. Any caller on the network can create, read, update, or delete data.

**Fix:** Add an auth middleware handler in `MapMethods`. A minimal API-key approach:

```cpp
server.set_pre_routing_handler([&](const httplib::Request& req, httplib::Response& res) {
    if (req.get_header_value("X-API-Key") != expectedKey) {
        res.status = httplib::Unauthorized_401;
        return httplib::Server::HandlerResponse::Handled;
    }
    return httplib::Server::HandlerResponse::Unhandled;
});
```

---

### H-3 — No request body size limit

**File:** `Services/APIGateway/ProductGateway/ProductGateway.cpp:9, 126`

`AddProduct` and `UpdateProduct` check `request.body.empty()` but place no upper bound on body size. A caller can send an arbitrarily large body, potentially exhausting server memory.

**Fix:** Reject oversized bodies early:

```cpp
constexpr size_t MAX_BODY_BYTES = 64 * 1024; // 64 KB
if (request.body.size() > MAX_BODY_BYTES) {
    response.status = httplib::PayloadTooLarge_413;
    return;
}
```

httplib also exposes `set_payload_max_length()` at the server level.

---

### H-4 — New database connection opened per query

**File:** `Services/DataService/ProductRepository/ProductRepository.cpp` (every method)

Every repository method opens and closes a `nanodbc::connection`. Under any meaningful load this causes:

- Connection setup latency on every request.
- Potential exhaustion of the DB server's connection limit.
- No connection reuse across the lifetime of a `ScopedContainer`.

**Fix:** Inject the connection (or a connection pool) through the constructor rather than building one inside each method. At minimum, the connection should live for the duration of a request scope.

---

## Medium

---

### M-1 — Pervasive pass-by-value for strings and objects

Multiple function signatures copy objects that should be passed by const reference, causing unnecessary heap allocations on every call.

| Location | Current | Should be |
|---|---|---|
| `IProductRepository::WriteProductToDB(Entities::PRDProduct)` | by value | `const Entities::PRDProduct&` |
| `IProductRepository::UpdateProduct(Entities::PRDProduct)` | by value | `const Entities::PRDProduct&` |
| `IProductRepository(std::string, ...)` | by value | `const std::string&` or `std::string` + `std::move` |
| `ProductRepository(std::string, ...)` | by value | same |
| `IProductService::ProcessProduct(Models::Product)` | by value | `const Models::Product&` |
| `IProductService::UpdateProduct(Models::Product)` | by value | `const Models::Product&` |
| `PRDProduct(int, std::string, double)` | by value | `std::string` + `std::move` into initializer list |
| `Product(int, std::string, double)` | by value | same |
| `DBUtils::CreateProductFromResult(nanodbc::result)` | by value | `nanodbc::result&` |
| `ILogger::SetServiceLevel(std::string, std::string)` | by value | `const std::string&` |

---

### M-2 — `LoggerColours.h` has no include guard

**File:** `Shared/SystemFramework/Logging/LoggerColours.h`

The file defines constants inside a namespace but has no `#ifndef` guard or `#pragma once`. If it is ever included from more than one translation unit — or transitively included twice — the compiler will emit redefinition errors.

**Fix:** Add `#pragma once` (or matching `#ifndef`/`#define`/`#endif`) at the top of the file.

---

### M-3 — `SELECT *` used in all queries

**File:** `Services/DataService/ProductRepository/ProductRepository.cpp:35, 59`

```sql
SELECT * FROM PRD_Products WHERE ID = ?
SELECT * FROM PRD_Products
```

`SELECT *` means:

- Adding a column to the table can silently break result parsing.
- Any sensitive column added later is immediately returned.
- The DB cannot optimise with a covering index on specific columns.

**Fix:** Name columns explicitly:

```sql
SELECT ID, Name, Price FROM PRD_Products WHERE ID = ?
```

---

### M-4 — `nanodbc::index_range_error` caught by value, not const reference

**File:** `Services/DataService/DBUtils/DBUtils.cpp:11`

```cpp
catch (nanodbc::index_range_error) {   // copy-constructs the exception object
```

Catching by value invokes the copy constructor and slices polymorphic exception types. The standard idiom is `const&`.

**Fix:**

```cpp
catch (const nanodbc::index_range_error&) {
```

---

### M-5 — `namespace SF = SystemFramework` alias declared in a header

**File:** `Interfaces/BusinessServiceInterface/IProductService.h:11`

```cpp
namespace SF = SystemFramework;
```

Namespace aliases at file scope in headers are injected into every translation unit that includes the file. This pollutes the global namespace of including files and can silently cause name conflicts.

**Fix:** Remove the alias from the header. Use the full name `SystemFramework::` in the header, or restrict the alias to within the class body if desired.

---

### M-6 — Price field not validated against non-finite floating-point values

**File:** `Services/BusinessService/ProductService/ProductService.cpp:9, 89`

The price validation only checks `<= 0`. IEEE 754 allows `NaN`, `+Infinity`, and `-Infinity`, which all pass this check and can corrupt downstream numeric operations or JSON serialisation.

**Fix:**

```cpp
#include <cmath>
if (!std::isfinite(newProduct.Price) || newProduct.Price <= 0) { ... }
```

---

## Low

---

### L-1 — Constructors use assignment instead of member initializer lists

**Files:** `Shared/Entities/PRD/PrdProduct.h`, `Shared/Models/PRD/Product.h`, `Interfaces/DataServiceInterface/IProductRepository.h`, `Interfaces/BusinessServiceInterface/IProductService.h`

```cpp
PRDProduct(int id, std::string name, double price) {
    this->ID = id;      // default-constructs then assigns
    this->Name = name;  // same — copies name twice
    this->Price = price;
}
```

Default-constructing then assigning is two operations where one suffices. For `std::string` this means a default construction followed by a copy or assignment.

**Fix:** Use initializer lists with `std::move` for string parameters:

```cpp
PRDProduct(int id, std::string name, double price)
    : ID(id), Name(std::move(name)), Price(price) {}
```

---

### L-2 — Null pointer initialised with `0` instead of `nullptr`

**File:** `Backend/DependencyInjection/DependencyInjectionConfig.h:15`

```cpp
Logger* BusinessServiceLogger = 0;
Logger* DataServiceLogger = 0;
Mapper* Mapper = 0;
```

`0` is implicitly convertible to a null pointer but its intent is ambiguous. `nullptr` is the typed null pointer constant introduced in C++11 for exactly this purpose.

**Fix:** Use `nullptr` on all pointer default values.

---

### L-3 — `DependencyRequirements::Mapper` field name shadows the `Mapper` type alias

**File:** `Backend/DependencyInjection/DependencyInjectionConfig.h:12,18`

```cpp
using Mapper = SystemFramework::Mapping::Mapper;

struct DependencyRequirements {
    Mapper* Mapper = 0;   // field named 'Mapper' shadows type alias 'Mapper'
};
```

The field name and type alias share the same identifier. This compiles because the type is resolved before the identifier becomes a member name, but it is confusing and error-prone for readers and tools.

**Fix:** Rename the field:

```cpp
Mapper* MapperInstance = nullptr;
```

---

### L-4 — Redundant `this->` prefix throughout service implementations

**Files:** `Services/BusinessService/ProductService/ProductService.cpp`, `Services/DataService/ProductRepository/ProductRepository.cpp`

```cpp
bool hasWrittenProduct = this->_productRepository->WriteProductToDB(dbProduct);
```

`this->` is only needed to disambiguate a member from a local variable or template base member. In all cases here the member names are unambiguous. The prefix adds visual noise without meaning.

**Fix:** Remove `this->` where it serves no disambiguating purpose.

---

### L-5 — Constructor logic in abstract interface classes

**Files:** `Interfaces/DataServiceInterface/IProductRepository.h:13`, `Interfaces/BusinessServiceInterface/IProductService.h:16`

Both abstract interfaces have constructors that accept dependencies and call virtual-adjacent methods (`SetServiceLevel`). Interfaces should declare a contract, not own state or perform initialisation.

**Consequences:**
- Concrete classes are forced to pass arguments up to the interface constructor via the initializer list.
- The interface now carries protected data members, coupling all implementors to the same storage layout.
- Unit-testing with mock implementations requires matching this constructor signature.

**Fix:** Move `_connectionString`, `_logger`, and `_mapper` into the concrete classes. Keep the interfaces as pure abstract (`virtual` methods + virtual destructor only).

---

### L-6 — `#include <iostream>` in `IProductRepository` with no usage

**File:** `Interfaces/DataServiceInterface/IProductRepository.h:4`

`<iostream>` is included but the interface uses nothing from it. This adds unnecessary compilation overhead to every translation unit that includes the interface.

**Fix:** Remove the include.

---

### L-7 — Inconsistent include guard style across the codebase

Some files use `#pragma once` (`DBUtils.h`), others use `#ifndef`/`#define`/`#endif` (`DependencyContainer.h`, etc.), and `LoggerColours.h` uses neither (see M-2). Mixing styles makes the codebase harder to maintain and creates the risk of gaps like M-2.

**Fix:** Standardise on `#pragma once` across all headers (supported by MSVC, GCC, Clang).

---

### L-8 — Public data members on entity and model classes

**Files:** `Shared/Entities/PRD/PrdProduct.h`, `Shared/Models/PRD/Product.h`

All fields (`ID`, `Name`, `Price`, `Id`) are `public`. This means any code can mutate entity state without going through any validation, and changing internal representation later is a breaking change across all callers.

For simple data-transfer types a `struct` with all-public fields is acceptable C++ idiom, but the types are declared as `class` which implies intent to encapsulate. Pick one: either make them `struct` to signal they are plain data, or add accessor methods.

---

### L-9 — `static` free functions in `Main.cpp` should use an anonymous namespace

**File:** `Backend/Main.cpp:3,12`

```cpp
static nlohmann::json LoadConfig(const std::string& path) { ... }
static std::string connectionString;
static void Heartbeat(...) { ... }
```

`static` at namespace scope is a C-era way to restrict linkage. The modern C++ idiom is an anonymous namespace, which also covers types (not just objects and functions).

**Fix:**

```cpp
namespace {
    std::string connectionString;
    nlohmann::json LoadConfig(const std::string& path) { ... }
    void Heartbeat(...) { ... }
}
```

---

### L-10 — `connectionString` is a global variable instead of a closure capture

**File:** `Backend/Main.cpp:11,55`

`connectionString` is a file-scope global solely so `Heartbeat` can access it. Globals complicate reasoning about state, especially in a multi-threaded server where httplib dispatches handlers on worker threads.

**Fix:** Capture by value in the lambda:

```cpp
server.Get("/system/heartbeat", [connectionString](const httplib::Request& req, httplib::Response& res) {
    // use connectionString directly
});
```

This removes the global and makes the dependency explicit.

---

### L-11 — `[[nodiscard]]` missing on bool and optional return values

Functions that return success/failure booleans or `std::optional` results are easy to call and accidentally ignore. Callers that drop the return value silently suppress error information.

Candidate functions:
- All `IProductRepository` and `IProductService` methods returning `bool` or `std::optional`.
- `Mapper::Map<>`.

**Fix:** Add `[[nodiscard]]` to these declarations:

```cpp
[[nodiscard]] virtual bool WriteProductToDB(const Entities::PRDProduct&) = 0;
[[nodiscard]] virtual std::optional<Entities::PRDProduct> GetProductById(int id) = 0;
```

---

### L-12 — ProductService.cpp redundantly re-includes `<Models.h>`

**File:** `Services/BusinessService/ProductService/ProductService.cpp:2`

`ProductService.h` already includes `<Models.h>`. The `#include <Models.h>` in the `.cpp` is redundant. Include guards prevent a compile error, but redundant includes add noise.

**Fix:** Remove `#include <Models.h>` from `ProductService.cpp`.

---

## Summary Table

| ID | Severity | File(s) | Issue |
|---|---|---|---|
| C-1 | Critical | `ScopedContainer.h` | `delete void*` is UB — destructors never called |
| C-2 | Critical | `IProductRepository.h` | No virtual destructor — UB on polymorphic delete |
| C-3 | Critical | `Mapping.h` | Heap round-trip in `Map()` is exception-unsafe |
| H-1 | High | `appsettings.json` / `Main.cpp` | DB credentials in plain-text config file |
| H-2 | High | `ProductGateway.cpp` | No authentication on any endpoint |
| H-3 | High | `ProductGateway.cpp` | No request body size limit |
| H-4 | High | `ProductRepository.cpp` | New DB connection per query |
| M-1 | Medium | Throughout | Pass-by-value for strings and objects |
| M-2 | Medium | `LoggerColours.h` | No include guard |
| M-3 | Medium | `ProductRepository.cpp` | `SELECT *` in all queries |
| M-4 | Medium | `DBUtils.cpp` | Exception caught by value not const reference |
| M-5 | Medium | `IProductService.h` | Namespace alias in header pollutes includers |
| M-6 | Medium | `ProductService.cpp` | No NaN/Infinity check on price |
| L-1 | Low | `PrdProduct.h`, `Product.h`, interfaces | Assignment in constructors instead of initializer lists |
| L-2 | Low | `DependencyInjectionConfig.h` | `0` used instead of `nullptr` |
| L-3 | Low | `DependencyInjectionConfig.h` | Field name `Mapper` shadows type alias `Mapper` |
| L-4 | Low | Service `.cpp` files | Redundant `this->` prefix |
| L-5 | Low | Both interface headers | Constructor logic in abstract interfaces |
| L-6 | Low | `IProductRepository.h` | Unused `#include <iostream>` |
| L-7 | Low | Codebase-wide | Mixed include guard styles |
| L-8 | Low | `PrdProduct.h`, `Product.h` | Public data members on `class` types |
| L-9 | Low | `Main.cpp` | `static` instead of anonymous namespace |
| L-10 | Low | `Main.cpp` | Global `connectionString` variable |
| L-11 | Low | Interface headers | Missing `[[nodiscard]]` on return values |
| L-12 | Low | `ProductService.cpp` | Redundant `#include <Models.h>` |
