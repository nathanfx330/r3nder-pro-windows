# R3nder Pro for Windows

R3nder Pro is a script-driven motion graphics and video editor for documentaries, explainers, and technical presentations. Write a terminal sequence, cut live footage, build a multi-pane desktop, and render the whole piece from one timeline.

**The script is the project.** When you trim a clip, move a cut, or change a layout in the GUI, R3nder writes that edit back into the document. The project stays readable, diffable, and ready to version with your other production files.

[Windows setup](windows/README.md) · [First-project walkthrough](MANUAL.md) · [Script reference](docs/REFERENCE.md) · [Original R3nder Pro repo](https://github.com/nathanfx330/R3nder-Pro)

## What you can make

| Tool | What it does |
| --- | --- |
| **TEXT** | Types terminal text and drives pauses, window choreography, captions, dossier cards, and evidence galleries. |
| **EDIT** | Imports footage and edits source-backed sequences with trims, splits, V1/V2 overlays, and independently timed crossfades. |
| **MOSAIC** | Arranges whole EDIT sequences in panes. Cut or crossfade between sequences within a pane and change layouts over time. |
| **STRUCT** | Places an EDIT or MOSAIC inside TEXT at the point it should appear in the presentation. |
| **PREVIEW and BAKE** | Plays the presentation and renders H.264, an H.264 fill and matte pair, or ProRes 4444 with alpha. |

R3nder owns project time and asks MLT for the source frame that belongs there. The same authored time drives scrubbing, preview, and export. FFmpeg encodes the final render.

## Run on Windows x64

Install [Flutter for Windows](https://docs.flutter.dev/platform-integration/windows/setup), Visual Studio with **Desktop development with C++**, and MSYS2 **UCRT64** with GCC, MLT 7, and FFmpeg. The Windows reference setup uses **Flutter 3.41.8 and MLT 7.40.0**.

In PowerShell:

```powershell
git clone https://github.com/nathanfx330/r3nder-pro-windows.git
cd r3nder-pro-windows
.\tools\run_windows.cmd
```

To build the Release EXE:

```powershell
.\tools\run_windows.cmd build
Invoke-Item .\build\windows\x64\runner\Release
```

Open **r3nder.exe** in Explorer. Keep it with its DLLs and `data` folder. The build uses the MSYS2 runtime at `C:\msys64\ucrt64` by default. For a different install location, use the prefix and clean-build instructions in the [Windows setup guide](windows/README.md).

## Make your first shot

Open **EDIT** from the main screen and paste this into the TEXT editor:

```text
[CONFIG:RENDERNAME:archive_transfer]
[SPEED:2]
Connecting to archive.
[PAUSE:20]

[GREEN]Connection established.[NORMAL]
[PAUSE:30]

Retrieving records.
[PAUSE:20]

Transfer complete.
```

Play it in the editor. At 30 authored frames per second, `[PAUSE:30]` holds the picture for one second. Use **BAKE** from the main screen to render it. A first 1080p H.264 render uses the name `archive_transfer_1080p_v001.mp4`. Later bakes get their own version numbers.

To bring footage into the shot:

1. In the structural workspace, create an EDIT sequence named `main` and choose **ADD VIDEO**. R3nder copies the media into your workspace and writes a workspace-relative reference.
2. Add `[STRUCT:EDIT.main]` to TEXT where the sequence should play.
3. For a multi-pane scene, create a MOSAIC named `wall`, put EDIT sequences into its panes, and place it with `[STRUCT:MOSAIC.wall]`.
4. Return to the main screen, watch the whole piece in **PREVIEW**, then **BAKE** it.

## Windows status

- **Verified on a Windows 11 x64 workstation** with MLT 7.40.0. The development build imported footage in EDIT and produced a BAKE render. The Release EXE also opened directly with MSYS2 removed from its inherited `PATH`.
- **Release import and BAKE with a minimal PATH** are the remaining media check for the direct-launch build. The [Windows setup guide](windows/README.md#repeat-the-media-check) includes a repeatable fixture and a way to measure preview and export sync.

## Documentation

- [User manual](MANUAL.md) for workspaces, the editor, audio, and your first finished project
- [Language and media reference](docs/REFERENCE.md) for tags and authored timing
- [Render outputs](docs/RENDER_OUTPUTS.md) for file formats, versioning, and render names
- [Engineering index](docs/README.md) for architecture and the development history
- [Building a deterministic NLE](docs/BUILDING_A_DETERMINISTIC_NLE.md) for project time, media decode, and preview/export design

## License

MIT License

Copyright (c) 2026 Nathaniel Westveer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
