#include <flutter/dart_project.h>
#include <flutter/flutter_view_controller.h>
#include <windows.h>

#include <filesystem>
#include <string>

#include "flutter_window.h"
#include "mlt_runtime_config.h"
#include "utils.h"

namespace {

std::wstring ProcessPath() {
  const DWORD size = ::GetEnvironmentVariableW(L"PATH", nullptr, 0);
  if (size == 0) return {};
  std::wstring path(size, L'\0');
  const DWORD length = ::GetEnvironmentVariableW(L"PATH", path.data(), size);
  if (length >= size) return {};
  path.resize(length);
  return path;
}

bool LoadNativeRuntime() {
  auto bin = std::filesystem::u8path(kR3nderMltRoot) / L"bin";
  bin.make_preferred();
  std::error_code error;
  if (!bin.is_absolute() ||
      !std::filesystem::is_regular_file(bin / L"libmlt-7.dll", error)) {
    return false;
  }
  if (!::AddDllDirectory(bin.c_str()) ||
      !::SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS)) {
    return false;
  }
  const std::wstring path = bin.wstring() + L";" + ProcessPath();
  if (!::SetEnvironmentVariableW(L"PATH", path.c_str())) return false;
  return ::LoadLibraryExW(L"r3nder_native.dll", nullptr,
                          LOAD_LIBRARY_SEARCH_DEFAULT_DIRS) != nullptr;
}

}  // namespace

int APIENTRY wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prev,
                      _In_ wchar_t *command_line, _In_ int show_command) {
  // Attach to console when present (e.g., 'flutter run') or create a
  // new console when running with a debugger.
  if (!::AttachConsole(ATTACH_PARENT_PROCESS) && ::IsDebuggerPresent()) {
    CreateAndAttachConsole();
  }

  if (!LoadNativeRuntime()) {
    const std::wstring message =
        L"R3nder could not load its native bridge or the MSYS2 UCRT64 MLT "
        L"runtime at:\n" +
        std::filesystem::u8path(kR3nderMltRoot).wstring() +
        L"\n\nCheck the MLT installation and rebuild R3nder if it moved.";
    ::MessageBoxW(nullptr, message.c_str(), L"R3nder Pro",
                  MB_OK | MB_ICONERROR);
    return EXIT_FAILURE;
  }

  // Initialize COM, so that it is available for use in the library and/or
  // plugins.
  ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

  flutter::DartProject project(L"data");

  std::vector<std::string> command_line_arguments =
      GetCommandLineArguments();

  project.set_dart_entrypoint_arguments(std::move(command_line_arguments));

  FlutterWindow window(project);
  Win32Window::Point origin(10, 10);
  Win32Window::Size size(1280, 720);
  if (!window.Create(L"r3nder", origin, size)) {
    return EXIT_FAILURE;
  }
  window.SetQuitOnClose(true);

  ::MSG msg;
  while (::GetMessage(&msg, nullptr, 0, 0)) {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }

  ::CoUninitialize();
  return EXIT_SUCCESS;
}
