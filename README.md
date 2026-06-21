# CPPFullBackend

A C++ HTTP backend built with [cpp-httplib](https://github.com/yhirose/cpp-httplib), using a layered architecture with CMake and vcpkg.

---

## Table of Contents

- [Project Structure](#project-structure)
- [Architecture](#architecture)
- [How CMake Works in This Project](#how-cmake-works-in-this-project)
  - [The Build Graph](#the-build-graph)
  - [Target Types](#target-types)
  - [Linking Visibility](#linking-visibility)
  - [Include Directories](#include-directories)
  - [CMakePresets](#cmakepresets)
  - [vcpkg Integration](#vcpkg-integration)
- [Prerequisites](#prerequisites)
- [Setting Up This Project](#setting-up-this-project)
- [Database Setup](#database-setup)
- [Setting Up a Similar Project From Scratch](#setting-up-a-similar-project-from-scratch)

---

## Project Structure

```
Backend/
├── CMakeLists.txt              # Root: wires together all sub-projects
├── CMakePresets.json           # Build presets (x64/x86, Debug/Release)
├── vcpkg.json                  # Dependency manifest
│
├── Backend/                    # Entry point — main executable
│   ├── Main.cpp
│   ├── Main.h
│   └── appsettings.json        # Runtime config (DB connection string, port)
│
├── Services/
│   ├── APIGateway/             # HTTP route handlers (lib: APIService)
│   ├── BusinessService/        # Business logic (lib: BusinessService)
│   └── DataService/            # Database access (lib: DataService)
│       └── DBUtils/            # Shared DB helpers
│
├── Interfaces/
│   ├── BusinessServiceInterface/   # IProductService (header-only interface lib)
│   └── DataServiceInterface/       # IProductRepository (header-only interface lib)
│
└── Shared/
    ├── Models/                 # API-facing models e.g. Product (header-only lib)
    └── Entities/               # DB-facing entities e.g. PRDProduct (header-only lib)
```

---

## Architecture

The project follows a clean layered architecture where each layer only knows about the layer directly below it — and only through an interface, not a concrete class.

```
[ HTTP Client ]
      │
  APIGateway          ← parses HTTP, calls IProductService
      │
  BusinessService     ← business logic, calls IProductRepository
      │
  DataService         ← SQL queries via nanodbc
      │
  [ SQL Server ]
```

**Interfaces** (`IProductService`, `IProductRepository`) sit between layers so the layers above never depend on concrete implementations. This makes each layer independently testable.

**Shared** contains data types used across layers:
- `Models::Product` — the shape of data going in/out of the API (JSON-serialisable)
- `Entities::PRDProduct` — the shape of a database row

---

## How CMake Works in This Project

### The Build Graph

CMake works by building a graph of **targets** (executables and libraries). Each target declares what it needs, and CMake figures out the right compile flags, include paths, and link order automatically.

The root `CMakeLists.txt` ties everything together:

```cmake
add_subdirectory("Shared")      # defines: Models, Entities
add_subdirectory("Interfaces")  # defines: BusinessServiceInterface, DataServiceInterface
add_subdirectory("Services")    # defines: APIService, BusinessService, DataService
add_subdirectory("Backend")     # defines: Backend (the .exe)
```

Order matters here — a target must be defined before another target can link to it.

---

### Target Types

There are three kinds of targets used in this project:

#### `add_executable`
Produces a `.exe`. Only used for the final `Backend` target.

```cmake
add_executable(Backend "Main.cpp" "Main.h")
```

#### `add_library(... STATIC)`
Compiles source files into a `.lib` that gets linked into whatever uses it. Used for `APIService`, `BusinessService`, and `DataService`.

```cmake
add_library(DataService STATIC
    "ProductRepository.cpp"
    "ProductRepository.h"
)
```

#### `add_library(... INTERFACE)`
A header-only library — no source files, no compiled output. Used for the interfaces and shared types (`Models`, `Entities`, `BusinessServiceInterface`, `DataServiceInterface`). An INTERFACE library is purely a way to package include paths and compile settings so other targets can consume them cleanly.

```cmake
add_library(BusinessServiceInterface INTERFACE)
target_include_directories(BusinessServiceInterface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
```

---

### Linking Visibility

When you call `target_link_libraries`, you choose a visibility keyword. This controls whether the dependency is exposed to targets that link *you*.

| Keyword | Meaning |
|---|---|
| `PRIVATE` | Only this target uses it. Not exposed to downstream consumers. |
| `PUBLIC` | This target uses it, and anyone linking this target also gets it. |
| `INTERFACE` | This target does *not* use it directly, but anyone linking this target gets it. Used on INTERFACE libraries. |

**Example from this project:**

```cmake
# DataService needs nanodbc to compile, but callers of DataService don't need nanodbc headers
target_link_libraries(DataService PRIVATE nanodbc Entities DataServiceInterface)

# BusinessService exposes its include directory so Backend can #include <ProductService.h>
target_include_directories(BusinessService PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
```

---

### Include Directories

`target_include_directories` tells the compiler where to look for `#include` files.

```cmake
target_include_directories(APIService PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}                         # Services/APIGateway/
    ${CMAKE_SOURCE_DIR}/Interfaces/BusinessServiceInterface
    ${CMAKE_SOURCE_DIR}/Shared/Models
)
```

- `CMAKE_CURRENT_SOURCE_DIR` — the folder containing the current `CMakeLists.txt`
- `CMAKE_SOURCE_DIR` — the root folder of the whole project (where the top-level `CMakeLists.txt` lives)

This is why headers can be included with just `#include <ProductGateway.h>` rather than needing a full relative path.

---

### CMakePresets

`CMakePresets.json` defines named build configurations so you don't have to remember a wall of CMake flags. VS reads this file and populates its configuration dropdown.

```json
{
  "name": "x64-debug",
  "inherits": "windows-base",
  "cacheVariables": {
    "CMAKE_BUILD_TYPE": "Debug"
  }
}
```

The `windows-base` preset (which the others inherit from) sets:
- **Generator**: Ninja (faster than MSBuild for CMake projects)
- **Build output**: `out/build/<presetName>/`
- **Install output**: `out/install/<presetName>/`
- **Toolchain**: points vcpkg so `find_package` can locate installed packages

---

### vcpkg Integration

`vcpkg.json` is the dependency manifest — the C++ equivalent of `package.json`:

```json
{
  "dependencies": [
    "cpp-httplib",
    "openssl",
    "nlohmann-json",
    "nanodbc"
  ]
}
```

vcpkg downloads and builds these packages into `vcpkg_installed/` (which is gitignored). The CMakePresets toolchain file wires vcpkg into CMake so that `find_package(httplib CONFIG REQUIRED)` just works.

In each `CMakeLists.txt`, packages are found and linked like this:

```cmake
find_package(httplib CONFIG REQUIRED)
target_link_libraries(APIService PRIVATE httplib::httplib)
```

---

## Prerequisites

| Tool | Notes |
|---|---|
| [Visual Studio 2022](https://visualstudio.microsoft.com/) | Install the **Desktop development with C++** workload |
| [vcpkg](https://github.com/microsoft/vcpkg) | Clone it and run `bootstrap-vcpkg.bat` |
| [ODBC Driver 18 for SQL Server](https://learn.microsoft.com/en-us/sql/connect/odbc/download-odbc-driver-for-sql-server) | Required by nanodbc at runtime |
| SQL Server (local) | Express edition is fine |

---

## Setting Up This Project

**1. Clone vcpkg** (if you haven't already) and note the path:

```
git clone https://github.com/microsoft/vcpkg C:/dev/vcpkg
C:/dev/vcpkg/bootstrap-vcpkg.bat
```

**2. Clone this repo:**

```
git clone https://github.com/CameronCS/CPPFullBackend.git
cd CPPFullBackend
```

**3. Update the toolchain path** in `CMakePresets.json` if your vcpkg is not at `C:/dev/vcpkg`:

```json
"CMAKE_TOOLCHAIN_FILE": "C:/your/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"
```

**4. Open in Visual Studio:**

- File → Open → Folder → select the repo root
- VS detects `CMakeLists.txt`, runs CMake configure, and downloads all vcpkg dependencies automatically
- Select a preset from the toolbar (e.g. `x64-debug`)
- Build → Build All

**5. Configure the database** (see [Database Setup](#database-setup) below).

**6. Run** — the server starts on `http://localhost:8080`.

---

## Database Setup

The app connects to a local SQL Server database called `CPPBackend` using Windows Authentication.

Create the database and table:

```sql
CREATE DATABASE CPPBackend;
GO

USE CPPBackend;
GO

CREATE TABLE PRD_Products (
    ID    INT IDENTITY(1,1) PRIMARY KEY,
    Name  NVARCHAR(255) NOT NULL,
    Price FLOAT         NOT NULL
);
```

The connection string is in `Backend/appsettings.json` and can be changed there without recompiling.

---

## Setting Up a Similar Project From Scratch

### 1. Install vcpkg

```
git clone https://github.com/microsoft/vcpkg C:/dev/vcpkg
C:/dev/vcpkg/bootstrap-vcpkg.bat
```

### 2. Create the folder structure

```
MyProject/
├── MyApp/          # executable
├── Services/       # static libraries
├── Interfaces/     # interface (header-only) libraries
└── Shared/         # shared types (header-only)
```

### 3. Root CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20)
project("MyProject")

add_subdirectory("Shared")
add_subdirectory("Interfaces")
add_subdirectory("Services")
add_subdirectory("MyApp")
```

### 4. Add a vcpkg manifest

Create `vcpkg.json` at the root:

```json
{
  "name": "myproject",
  "version": "0.1.0",
  "dependencies": [
    "cpp-httplib",
    "nlohmann-json"
  ]
}
```

### 5. Add CMakePresets.json

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "windows-base",
      "hidden": true,
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/out/build/${presetName}",
      "installDir": "${sourceDir}/out/install/${presetName}",
      "cacheVariables": {
        "CMAKE_C_COMPILER": "cl.exe",
        "CMAKE_CXX_COMPILER": "cl.exe",
        "CMAKE_TOOLCHAIN_FILE": "C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake",
        "VCPKG_INSTALLED_DIR": "${sourceDir}/vcpkg_installed"
      },
      "condition": {
        "type": "equals",
        "lhs": "${hostSystemName}",
        "rhs": "Windows"
      }
    },
    {
      "name": "x64-debug",
      "displayName": "x64 Debug",
      "inherits": "windows-base",
      "architecture": { "value": "x64", "strategy": "external" },
      "cacheVariables": { "CMAKE_BUILD_TYPE": "Debug" }
    },
    {
      "name": "x64-release",
      "displayName": "x64 Release",
      "inherits": "x64-debug",
      "cacheVariables": { "CMAKE_BUILD_TYPE": "Release" }
    }
  ]
}
```

### 6. A header-only shared library

`Shared/Models/CMakeLists.txt`:

```cmake
add_library(Models INTERFACE)
target_include_directories(Models INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(Models INTERFACE nlohmann_json::nlohmann_json)
target_compile_features(Models INTERFACE cxx_std_20)
```

### 7. A static library

`Services/MyService/CMakeLists.txt`:

```cmake
add_library(MyService STATIC "MyService.cpp" "MyService.h")

target_include_directories(MyService PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Shared/Models
)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(MyService PRIVATE nlohmann_json::nlohmann_json Models)
target_compile_features(MyService PRIVATE cxx_std_20)
```

### 8. The executable

`MyApp/CMakeLists.txt`:

```cmake
add_executable(MyApp "Main.cpp")

find_package(httplib CONFIG REQUIRED)
target_link_libraries(MyApp PRIVATE MyService httplib::httplib)
target_compile_definitions(MyApp PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)
target_compile_features(MyApp PRIVATE cxx_std_20)
```

### 9. Open in Visual Studio

File → Open → Folder → select the project root. VS will pick up `CMakeLists.txt` and `CMakePresets.json`, run configure, and pull vcpkg dependencies automatically.

---

### Quick Reference: CMake Commands

| Command | Purpose |
|---|---|
| `add_executable(Name files...)` | Creates a `.exe` target |
| `add_library(Name STATIC files...)` | Creates a `.lib` target |
| `add_library(Name INTERFACE)` | Creates a header-only target (no sources) |
| `target_link_libraries(A PRIVATE B)` | A links B; B is not exposed to A's consumers |
| `target_link_libraries(A PUBLIC B)` | A links B; B is also exposed to A's consumers |
| `target_include_directories(A PUBLIC dir)` | Adds an include path to A and its consumers |
| `target_compile_features(A PRIVATE cxx_std_20)` | Enables C++20 for A |
| `target_compile_definitions(A PRIVATE FOO)` | Adds `-DFOO` to A's compile flags |
| `find_package(X CONFIG REQUIRED)` | Finds a vcpkg/CMake package — errors if missing |
