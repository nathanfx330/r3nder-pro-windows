#include "flutter_window.h"

#include <commdlg.h>
#include <flutter/standard_method_codec.h>
#include <shellapi.h>

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "flutter/generated_plugin_registrant.h"
#include "utils.h"

namespace {

void PickVideo(HWND owner,
               flutter::MethodResult<flutter::EncodableValue>& result) {
  std::vector<wchar_t> path(32768);
  OPENFILENAMEW dialog{};
  dialog.lStructSize = sizeof(dialog);
  dialog.hwndOwner = owner;
  dialog.lpstrFilter =
      L"Video files\0*.mp4;*.mov;*.mkv;*.webm;*.m4v;*.avi\0All files\0*.*\0";
  dialog.lpstrFile = path.data();
  dialog.nMaxFile = static_cast<DWORD>(path.size());
  dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR |
                 OFN_EXPLORER;
  if (::GetOpenFileNameW(&dialog)) {
    result.Success(flutter::EncodableValue(Utf8FromUtf16(path.data())));
  } else if (const DWORD error = ::CommDlgExtendedError()) {
    result.Error("file_dialog", "Windows file picker failed",
                 flutter::EncodableValue(static_cast<int32_t>(error)));
  } else {
    result.Success();
  }
}

flutter::EncodableValue ReadDrop(HWND window, HDROP drop) {
  flutter::EncodableList paths;
  const UINT count = ::DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0);
  for (UINT index = 0; index < count; ++index) {
    const UINT length = ::DragQueryFileW(drop, index, nullptr, 0);
    std::vector<wchar_t> path(length + 1);
    if (::DragQueryFileW(drop, index, path.data(), length + 1) != 0) {
      paths.emplace_back(Utf8FromUtf16(path.data()));
    }
  }
  POINT point{};
  const BOOL has_position = ::DragQueryPoint(drop, &point);
  ::DragFinish(drop);
  if (!has_position) return flutter::EncodableValue(std::move(paths));

  const double scale = ::GetDpiForWindow(window) / 96.0;
  return flutter::EncodableValue(flutter::EncodableMap{
      {flutter::EncodableValue("paths"), flutter::EncodableValue(std::move(paths))},
      {flutter::EncodableValue("x"), flutter::EncodableValue(point.x / scale)},
      {flutter::EncodableValue("y"), flutter::EncodableValue(point.y / scale)},
  });
}

}  // namespace

FlutterWindow::FlutterWindow(const flutter::DartProject& project)
    : project_(project) {}

FlutterWindow::~FlutterWindow() {}

bool FlutterWindow::OnCreate() {
  if (!Win32Window::OnCreate()) {
    return false;
  }

  RECT frame = GetClientArea();

  // The size here must match the window dimensions to avoid unnecessary surface
  // creation / destruction in the startup path.
  flutter_controller_ = std::make_unique<flutter::FlutterViewController>(
      frame.right - frame.left, frame.bottom - frame.top, project_);
  // Ensure that basic setup of the controller was successful.
  if (!flutter_controller_->engine() || !flutter_controller_->view()) {
    return false;
  }
  RegisterPlugins(flutter_controller_->engine());
  SetChildContent(flutter_controller_->view()->GetNativeWindow());
  file_channel_ =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          flutter_controller_->engine()->messenger(), "r3nder/drop",
          &flutter::StandardMethodCodec::GetInstance());
  file_channel_->SetMethodCallHandler([this](const auto& call, auto result) {
    if (call.method_name() == "pickVideo") {
      PickVideo(GetHandle(), *result);
    } else {
      result->NotImplemented();
    }
  });
  ::DragAcceptFiles(GetHandle(), TRUE);

  flutter_controller_->engine()->SetNextFrameCallback([&]() {
    this->Show();
  });

  // Flutter can complete the first frame before the "show window" callback is
  // registered. The following call ensures a frame is pending to ensure the
  // window is shown. It is a no-op if the first frame hasn't completed yet.
  flutter_controller_->ForceRedraw();

  return true;
}

void FlutterWindow::OnDestroy() {
  if (file_channel_) {
    file_channel_->SetMethodCallHandler(nullptr);
    file_channel_.reset();
  }
  if (flutter_controller_) {
    flutter_controller_ = nullptr;
  }

  Win32Window::OnDestroy();
}

LRESULT
FlutterWindow::MessageHandler(HWND hwnd, UINT const message,
                              WPARAM const wparam,
                              LPARAM const lparam) noexcept {
  if (message == WM_DROPFILES && file_channel_) {
    file_channel_->InvokeMethod(
        "onDrop", std::make_unique<flutter::EncodableValue>(
                      ReadDrop(hwnd, reinterpret_cast<HDROP>(wparam))));
    return 0;
  }

  // Give Flutter, including plugins, an opportunity to handle window messages.
  if (flutter_controller_) {
    std::optional<LRESULT> result =
        flutter_controller_->HandleTopLevelWindowProc(hwnd, message, wparam,
                                                      lparam);
    if (result) {
      return *result;
    }
  }

  switch (message) {
    case WM_FONTCHANGE:
      flutter_controller_->engine()->ReloadSystemFonts();
      break;
  }

  return Win32Window::MessageHandler(hwnd, message, wparam, lparam);
}
