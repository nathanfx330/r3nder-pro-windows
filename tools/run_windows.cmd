@echo off
setlocal
if not defined R3NDER_MLT_ROOT set "R3NDER_MLT_ROOT=C:\msys64\ucrt64"
set "PATH=%R3NDER_MLT_ROOT%\bin;%PATH%"
set "MLT_REPOSITORY=%R3NDER_MLT_ROOT:\=/%/lib/mlt"
set "MLT_DATA=%R3NDER_MLT_ROOT:\=/%/share/mlt"
set "MLT_PROFILES_PATH=%MLT_DATA%/profiles"
if not exist "%R3NDER_MLT_ROOT%\bin\melt.exe" (
    echo MLT 7.40.0 is required. Set R3NDER_MLT_ROOT to your MSYS2 UCRT64 prefix.
    exit /b 1
)
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
