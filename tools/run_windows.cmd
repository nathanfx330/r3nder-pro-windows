@echo off
setlocal
pushd "%~dp0.." || exit /b 1
if /i "%~1"=="build" (
    call flutter build windows --release
) else if /i "%~1"=="release" (
    "build\windows\x64\runner\Release\r3nder.exe"
) else (
    call flutter run -d windows %*
)
set "r3nder_status=%errorlevel%"
popd
exit /b %r3nder_status%
