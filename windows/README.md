# R3nder Pro on Windows x64

Use the existing MLT Player Foundation development environment: Flutter for
Windows, Visual Studio with Desktop development with C++, and MSYS2 UCRT64
with GCC, **MLT 7.40.0**, and FFmpeg. The MLT Player reference was tested with
Flutter 3.41.8. Run these commands from the repository root in PowerShell:

```powershell
.\tools\run_windows.cmd
```

The launcher defaults to `C:\msys64\ucrt64`. Set `R3NDER_MLT_ROOT` if the
same runtime is installed elsewhere:

```powershell
$env:R3NDER_MLT_ROOT = 'D:\msys64\ucrt64'
.\tools\run_windows.cmd
```

For a release build and launch using the same external runtime:

```powershell
.\tools\run_windows.cmd build
.\tools\run_windows.cmd release
```

The launcher sets the runtime search paths for MLT and FFmpeg for that session.
R3nder builds its small bridge from the existing native project clock and MLT
decoder. Windows preview uses the same CPU image path as R3nder's compositor.
Preview audio uses `ffplay` on the default Windows device and picture timing
uses the monotonic project clock.

## Verified on Windows

The development build opened on Windows x64, imported footage into EDIT with
the existing MSYS2 UCRT64 MLT 7.40.0 runtime, and rendered a video with BAKE.

## MLT module discovery

R3nder passes the launcher's `MLT_REPOSITORY` directory directly to MLT. A
Windows MLT factory initialized without that directory looks relative to
`r3nder.exe`. This can leave EDIT unable to probe footage even when
`melt.exe` reports the expected version.

If import fails, use the launcher and check the UCRT64 module path. The
decoder needs the `loader` and `avformat` producers. Warnings for optional Qt,
SoX, and other modules also appeared during the Windows foundation test.

## Repeat the media check

For a repeatable check, import the same H.264/AAC test clip used in the
[MLT Player Foundation test](https://github.com/nathanfx330/MLT-Player/blob/main/docs/mlt-player-windows-foundation-findings.md#4-test-media-fixture).
Check the reported 90 frames, scrub to frames 45 and 60, then add a second clip
in MOSAIC and BAKE to H.264. Check audio sync after seeking and in the export.
