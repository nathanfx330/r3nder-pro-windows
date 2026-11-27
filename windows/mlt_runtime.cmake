# Use the existing MSYS2 UCRT64 MLT installation and its matching compiler.
# The Flutter runner still builds with Visual Studio and loads the bridge via FFI.
set(R3NDER_MLT_ROOT "$ENV{R3NDER_MLT_ROOT}")
if(NOT R3NDER_MLT_ROOT)
  set(R3NDER_MLT_ROOT "C:/msys64/ucrt64")
endif()
file(TO_CMAKE_PATH "${R3NDER_MLT_ROOT}" R3NDER_MLT_ROOT)
set(mlt_pkg_config "${R3NDER_MLT_ROOT}/lib/pkgconfig/mlt-framework-7.pc")
set(mlt_compiler "${R3NDER_MLT_ROOT}/bin/g++.exe")
if(NOT EXISTS "${mlt_pkg_config}" OR NOT EXISTS "${mlt_compiler}")
  message(FATAL_ERROR "R3nder needs MSYS2 UCRT64 g++ and MLT 7.40.0 at ${R3NDER_MLT_ROOT}. See windows/README.md.")
endif()
file(STRINGS "${mlt_pkg_config}" mlt_version REGEX "^Version:")
if(NOT mlt_version MATCHES "^Version:[ \t]*7[.]40[.]0[ \t]*$")
  message(FATAL_ERROR "R3nder needs MLT 7.40.0 at ${R3NDER_MLT_ROOT}, found '${mlt_version}'.")
endif()
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${mlt_pkg_config}")

set(native_source_dir "${CMAKE_CURRENT_SOURCE_DIR}/../linux/runner")
set(R3NDER_NATIVE_DLL "${CMAKE_CURRENT_BINARY_DIR}/r3nder_native.dll")
add_custom_command(
  OUTPUT "${R3NDER_NATIVE_DLL}"
  COMMAND "${CMAKE_COMMAND}" -E env
    "PATH=${R3NDER_MLT_ROOT}/bin;$ENV{PATH}"
    "${mlt_compiler}" -std=c++17 -O2 -shared -static-libgcc -static-libstdc++
    -Wl,--export-all-symbols
    "-I${R3NDER_MLT_ROOT}/include/mlt-7"
    "${native_source_dir}/project_clock.cc"
    "${native_source_dir}/media_decoder.cc"
    "-L${R3NDER_MLT_ROOT}/lib" -lmlt-7
    -o "${R3NDER_NATIVE_DLL}"
  DEPENDS
    "${native_source_dir}/project_clock.cc"
    "${native_source_dir}/project_clock.h"
    "${native_source_dir}/media_decoder.cc"
    "${native_source_dir}/media_decoder.h"
    "${mlt_pkg_config}"
  VERBATIM
)
add_custom_target(r3nder_native DEPENDS "${R3NDER_NATIVE_DLL}")
