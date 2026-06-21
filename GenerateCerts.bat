@echo off

:: Install mkcert if not already installed
where mkcert >nul 2>&1
if %errorlevel% neq 0 (
    echo mkcert not found, installing...
    choco install mkcert -y
) else (
    echo mkcert already installed, skipping...
)

:: Install local CA
mkcert -install

:: Generate certs
mkcert localhost

:: Move to Backend folder
move ./localhost.pem ./Backend/cert.pem
move ./localhost-key.pem ./Backend/key.pem

echo Done! Certs generated and moved to Backend folder.
pause