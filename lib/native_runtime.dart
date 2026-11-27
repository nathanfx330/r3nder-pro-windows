import 'dart:ffi';
import 'dart:io';

import 'diag.dart';

DynamicLibrary openNativeRuntime() {
  if (Platform.isWindows) return DynamicLibrary.open('r3nder_native.dll');
  if (Platform.isLinux) return DynamicLibrary.process();
  throw UnsupportedError('R3nder native media requires Linux or Windows.');
}

DynamicLibrary openNativeAllocator() =>
    DynamicLibrary.open(Platform.isWindows ? 'ucrtbase.dll' : 'libc.so.6');

void logWindowsMltVersion() {
  if (!Platform.isWindows) return;
  try {
    // Opening by name returns the copy already loaded by the runner.
    final int version = DynamicLibrary.open(
      'libmlt-7.dll',
    ).lookupFunction<Int32 Function(), int Function()>('mlt_version_get_int')();
    final String label =
        '${version >> 16}.${(version >> 8) & 0xff}.${version & 0xff}';
    if (label != '7.40.0') {
      diag('mlt', 'Loaded MLT $label. Windows baseline is MLT 7.40.0.');
    }
  } on ArgumentError catch (error) {
    diag('mlt', 'Could not read the loaded MLT version: $error');
  }
}
