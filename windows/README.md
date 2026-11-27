# R3nder Pro on Windows x64

Use the existing MLT Player Foundation development environment: Flutter for
Windows, Visual Studio with Desktop development with C++, and **MSYS2 UCRT64**
with GCC, MLT 7, and FFmpeg. The verified combination is **MLT 7.40.0** and
Flutter 3.41.8. The build warns when it finds another MLT 7 version. Run from
the repository root in PowerShell:

```powershell
.\tools\run_windows.cmd
```

The build defaults to `C:\msys64\ucrt64`. Set `R3NDER_MLT_ROOT` before
building if your UCRT64 installation is elsewhere:

```powershell
$env:R3NDER_MLT_ROOT = 'D:\msys64\ucrt64'
flutter clean
.\tools\run_windows.cmd
```

Build a release:

```powershell
.\tools\run_windows.cmd build
```

Open `build\windows\x64\runner\Release\r3nder.exe` directly. The executable
uses the UCRT64 directory selected at build time for MLT and FFmpeg. Run
`flutter clean` before rebuilding whenever you change `R3NDER_MLT_ROOT`.
R3nder records the loaded MLT version in `r3nder_trace.log` at startup and
warns there when the version differs from 7.40.0.

To confirm direct launch with a minimal PATH, open a separate Command Prompt
in the repository root and run:

```bat
set "PATH=%SystemRoot%\System32;%SystemRoot%"
start "" "build\windows\x64\runner\Release\r3nder.exe"
```

Import a clip into EDIT and run BAKE in that session.

R3nder builds its small bridge from the existing native project clock and MLT
decoder. Windows preview uses the same CPU image path as R3nder's compositor.
Preview audio uses `ffplay` on the default Windows device and picture timing
uses the monotonic project clock.

## Verified on Windows

The development build opened on Windows x64, imported footage into EDIT with
the existing MSYS2 UCRT64 MLT 7.40.0 runtime, and rendered a video with BAKE.

## MLT module discovery

R3nder passes the configured `lib\mlt` directory directly to MLT. A Windows
MLT factory initialized without that directory looks relative to `r3nder.exe`.
This can leave EDIT unable to probe footage even when `melt.exe` reports the
expected version.

If import fails, check the module path reported by R3nder. The decoder needs
the `loader` and `avformat` producers. Warnings for optional Qt, SoX, and other
modules also appeared during the Windows foundation test.

## Repeat the media check

For a repeatable check, import the same H.264/AAC test clip used in the
[MLT Player Foundation test](https://github.com/nathanfx330/MLT-Player/blob/main/docs/mlt-player-windows-foundation-findings.md#4-test-media-fixture).
Check the reported 90 frames, scrub to frames 45 and 60, then add a second clip
in MOSAIC and BAKE to H.264.

### Measure audio offset

Generate a 30 fps clip with a white frame and a 1 kHz tone both starting at
one second. Use the FFmpeg executable from your UCRT64 installation:

```powershell
& 'C:\msys64\ucrt64\bin\ffmpeg.exe' -f lavfi -i "color=c=black:s=640x360:r=30:d=3,drawbox=color=white:t=fill:enable='gte(t,1)'" -f lavfi -i "sine=frequency=1000:sample_rate=48000:duration=0.2" -af 'adelay=1000:all=1,apad' -t 3 -c:v libx264 -pix_fmt yuv420p -c:a aac r3nder-sync-check.mp4
```

1. Import the clip into EDIT and enable clip audio for preview. BAKE it with
   clip audio enabled.
2. Use one screen recording with system audio to capture the original clip,
   the R3nder preview from the start, the preview after seeking to half a
   second, and playback of the exported file. Keep the same audio output
   device throughout.
3. In the recording, subtract the first white-frame timestamp from the tone
   onset for each run. Subtract the original clip's measured offset from
   each result to account for capture skew. Record the resulting offsets in
   milliseconds and the recorder's frame interval.

Preview audio runs through `ffplay` while picture uses the project clock.
The preview offset can include device buffering and startup time. BAKE mixes
audio at the authored project time.
