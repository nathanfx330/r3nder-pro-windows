import 'dart:ffi';
import 'dart:io';

DynamicLibrary openNativeRuntime() {
  if (Platform.isWindows) return DynamicLibrary.open('r3nder_native.dll');
  if (Platform.isLinux) return DynamicLibrary.process();
  throw UnsupportedError('R3nder native media requires Linux or Windows.');
}

DynamicLibrary openNativeAllocator() =>
    DynamicLibrary.open(Platform.isWindows ? 'ucrtbase.dll' : 'libc.so.6');
