# Building & Packaging DXX-Raytracer

This guide covers building the DXX-Raytracer from a fresh clone on Windows (native) and packaging it as a macOS .app bundle via Game Porting Toolkit.

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Clone the Repository](#clone-the-repository)
3. [Install vcpkg](#install-vcpkg)
4. [Build on Windows](#build-on-windows)
5. [Build Output](#build-output)
6. [Adding Descent 1 Data Files](#adding-descent-1-data-files)
7. [Creating a Release Package (Windows)](#creating-a-release-package-windows)
8. [Packaging the macOS .app Bundle](#packaging-the-macos-app-bundle)
9. [Build Presets Reference](#build-presets-reference)
10. [Troubleshooting](#troubleshooting)

---

## Prerequisites

### Required

| Tool | Version | Notes |
|------|---------|-------|
| **Windows 10/11** | Build 19041+ | Native build target |
| **Visual Studio 2022** | Latest | Need the "Desktop development with C++" workload for MSVC |
| **CMake** | 3.21+ | Included with VS 2022, or install separately |
| **Ninja** | Any recent | Included with VS 2022, or install separately |
| **vcpkg** | See below | Package manager for audio codec dependencies |
| **Git** | Any recent | To clone the repo |

### Optional

| Tool | Purpose |
|------|---------|
| **Descent 1 (1995)** | Full game data (`descent.hog`, `descent.pig`). Without these, only shareware levels are playable. |
| **NVIDIA NVTT** | Offline texture compression (PNG to DDS). Only needed if creating custom PBR textures. |

---

## Clone the Repository

```bash
git clone https://github.com/BredaUniversityGames/DXX-Raytracer.git
cd DXX-Raytracer
```

There are no git submodules. All source dependencies (SDL2, SDL_mixer, PhysicsFS, libfresample, DXC compiler, FSR2, ImGui) are bundled directly in the repo.

---

## Install vcpkg

vcpkg provides the audio codec libraries needed by SDL_mixer. If you already have vcpkg installed and `VCPKG_ROOT` set, skip this step.

```bash
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
```

Set the environment variable (persistent):

```powershell
# PowerShell
[System.Environment]::SetEnvironmentVariable("VCPKG_ROOT", "C:\vcpkg", "User")
```

Or in Command Prompt for the current session:

```cmd
set VCPKG_ROOT=C:\vcpkg
```

The project's `vcpkg.json` declares these dependencies (installed automatically during CMake configure):
- `libflac`, `libogg`, `libvorbis` — audio codecs
- `libmikmod` — tracker music format
- `mpg123` — MP3 decoder

---

## Build on Windows

### Option A: Command Line (recommended)

Open a **Developer Command Prompt for VS 2022** (or any terminal with MSVC on PATH), then:

```bash
# Shipping build (optimized, no console window)
cmake --preset directx12-win-ship
cmake --build out/build/directx12-win-ship

# OR: Release with debug info (good for development)
cmake --preset directx12-win-release
cmake --build out/build/directx12-win-release

# OR: Debug build (console window, unoptimized)
cmake --preset directx12-win-debug
cmake --build out/build/directx12-win-debug
```

The `cmake --preset` step runs vcpkg automatically to fetch and build the audio dependencies on first configure. This takes a few minutes the first time.

### Option B: Visual Studio 2022

1. Open the repo folder in VS 2022 (File > Open > Folder)
2. VS detects `CMakePresets.json` and shows available presets in the toolbar
3. Select a preset (e.g., `directx12-win-release`)
4. Build > Build All

### Option C: If MSVC is not on PATH

If you're not in a Developer Command Prompt, set up the environment first:

```cmd
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
cmake --preset directx12-win-release
cmake --build out/build/directx12-win-release
```

---

## Build Output

After a successful DX12 build, the output directory (e.g., `out/build/directx12-win-ship/d1/`) contains:

```
out/build/directx12-win-ship/d1/
  descent1.exe          # Game executable
  Renderer.dll          # DX12 raytracing renderer
  dxcompiler.dll        # Microsoft DXC shader compiler runtime
  dxil.dll              # DirectX IL validation
  FLAC.dll              # Audio codecs (from vcpkg)
  mpg123.dll
  ogg.dll
  vorbis.dll
  vorbisfile.dll
  assets/
    shaders/            # HLSL shaders (compiled at runtime by DXC)
    textures/           # PBR materials and noise textures
    cockpit_textures/
    render_presets/
    splash-logo/
    cockpit_prototype.bin
    cockpit_prototype.gltf
```

Shaders are **not** pre-compiled. The renderer loads `.hlsl` files from `assets/shaders/` and compiles them at startup using the bundled DXC compiler (`dxcompiler.dll`). This means the `assets/shaders/` directory must always be present alongside the executable.

---

## Adding Descent 1 Data Files

The game works without these files (shareware levels only), but for the full game you need `descent.hog` and `descent.pig` from a retail copy of Descent (1995).

**Automatic copying during build:** Place the files in the repo root before building:

```
DXX-Raytracer/
  descent.hog    <-- here
  descent.pig    <-- here
  CMakeLists.txt
  ...
```

CMake's post-build step copies them into the output directory automatically.

**Manual copying after build:** Copy them directly into the build output directory next to `descent1.exe`.

---

## Creating a Release Package (Windows)

To create a distributable zip, gather these files from the build output directory:

```bash
mkdir DXX-Raytracer-release
cd out/build/directx12-win-ship/d1/

# Binaries
copy descent1.exe     ..\..\..\..\DXX-Raytracer-release\
copy Renderer.dll     ..\..\..\..\DXX-Raytracer-release\
copy dxcompiler.dll   ..\..\..\..\DXX-Raytracer-release\
copy dxil.dll         ..\..\..\..\DXX-Raytracer-release\
copy FLAC.dll         ..\..\..\..\DXX-Raytracer-release\
copy mpg123.dll       ..\..\..\..\DXX-Raytracer-release\
copy ogg.dll          ..\..\..\..\DXX-Raytracer-release\
copy vorbis.dll       ..\..\..\..\DXX-Raytracer-release\
copy vorbisfile.dll   ..\..\..\..\DXX-Raytracer-release\

# Assets (required - shaders are compiled at runtime)
xcopy /E /I assets    ..\..\..\..\DXX-Raytracer-release\assets\

# Game data (optional - for full game)
copy descent.hog      ..\..\..\..\DXX-Raytracer-release\
copy descent.pig      ..\..\..\..\DXX-Raytracer-release\
```

Create an empty `lights/` directory (the game expects it to exist):

```bash
mkdir DXX-Raytracer-release\lights
```

Zip the `DXX-Raytracer-release/` folder for distribution.

---

## Packaging the macOS .app Bundle

The repo includes a `DXX-Raytracer.app` bundle that wraps the Windows build for running on macOS via Game Porting Toolkit 3.0. The .app is self-contained — it bundles all game files and auto-creates a Wine prefix on first launch.

### macOS requirements

- macOS 14.0 (Sonoma) or later
- Apple Silicon (M1/M2/M3/M4) — raytracing requires M3 or later
- Game Porting Toolkit 3.0: `brew install --cask gcenx/wine/game-porting-toolkit`

### Populating the .app bundle

After building on Windows (or using a pre-built release), copy the game files into the bundle:

```bash
# From the repo root on macOS
APP="DXX-Raytracer.app/Contents/Resources/game"
mkdir -p "$APP"

# Copy all build output files
cp descent1.exe Renderer.dll dxcompiler.dll dxil.dll "$APP/"
cp FLAC.dll mpg123.dll ogg.dll vorbis.dll vorbisfile.dll "$APP/"
cp -R assets "$APP/"
mkdir -p "$APP/lights"

# Copy Descent 1 data (lowercase filenames required)
cp descent.hog "$APP/descent.hog"
cp descent.pig "$APP/descent.pig"
```

### How the .app launcher works

The launcher script (`Contents/MacOS/launch`) does the following:

1. Resolves its own path within the .app bundle (works from any location)
2. Checks that GPTK is installed at `/usr/local/bin/wine64` — shows a native macOS dialog if missing
3. Verifies game files exist inside the bundle
4. On first launch, creates a Wine prefix at `~/Library/Application Support/DXX-Raytracer/prefix/`
5. Converts the macOS path to a Wine Z: drive path (Wine maps `Z:` to `/`)
6. Launches with raytracing env vars: `D3DM_SUPPORT_DXR=1`, `WINEESYNC=1`, `ROSETTA_ADVERTISE_AVX=1`

### Testing

Double-click `DXX-Raytracer.app` in Finder, or from a terminal:

```bash
open DXX-Raytracer.app
```

The first launch takes 10-30 seconds extra while creating the Wine prefix. Subsequent launches are immediate.

### Structure reference

```
DXX-Raytracer.app/
  Contents/
    Info.plist              # Bundle metadata (v1.3.0-ar7, macOS 14.0+)
    MacOS/
      launch                # Bash launcher script
    Resources/
      game/                 # All game files (~60MB)
        descent1.exe
        Renderer.dll
        dxcompiler.dll
        dxil.dll
        FLAC.dll, mpg123.dll, ogg.dll, vorbis.dll, vorbisfile.dll
        descent.hog
        descent.pig
        assets/
          shaders/          # HLSL shaders
          textures/         # PBR materials
          ...
        lights/             # Dynamic light cache (initially empty)
```

---

## Build Presets Reference

All presets use Ninja as the generator and require `VCPKG_ROOT` to be set.

| Preset | Build Type | Graphics | Description |
|--------|-----------|----------|-------------|
| `directx12-win-ship` | Release | DX12 | Shipping build (optimized, no console) |
| `directx12-win-release` | RelWithDebInfo | DX12 | Release with debug symbols |
| `directx12-win-debug` | Debug | DX12 | Full debug, console window |
| `directx12-win-debug-quick` | Debug | DX12 | Debug, skips main menu |
| `directx12-win-release-quick` | RelWithDebInfo | DX12 | Release, skips main menu |
| `openGL-x64-debug` | Debug | OpenGL | No raytracing, debug |
| `openGL-x64-release` | Release | OpenGL | No raytracing, optimized |

---

## Troubleshooting

### `VCPKG_ROOT` not set

```
CMake Error: Could not find toolchain file: $env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
```

Set the `VCPKG_ROOT` environment variable to your vcpkg installation directory.

### Ninja not found

```
CMake Error: CMake was unable to find a build program corresponding to "Ninja".
```

Install Ninja (`choco install ninja`, `scoop install ninja`, or download from https://ninja-build.org/) or use a Developer Command Prompt for VS 2022 which includes it.

### MSVC not found

```
No CMAKE_C_COMPILER could be found.
```

Run the build from a Developer Command Prompt for VS 2022, or run `vcvars64.bat` first.

### Shaders fail to compile at runtime

The game loads shaders from `assets/shaders/` relative to the executable. Ensure the `assets/` directory was copied alongside `descent1.exe`. If running from the build directory, CMake's custom targets handle this automatically.

### macOS: "Game Porting Toolkit not found" dialog

Install GPTK:

```bash
brew install --cask gcenx/wine/game-porting-toolkit
```

### macOS: Game crashes on launch

Ensure the working directory contains all required files. The .app bundle handles this automatically by using Wine's Z: drive to point directly into `Contents/Resources/game/`. If running manually via `wine64`, use `start /d` to set the working directory:

```bash
wine64 start /d 'C:\path\to\game' 'C:\path\to\game\descent1.exe'
```

### macOS: No raytracing (software rendering)

Raytracing via D3DMetal requires:
- `D3DM_SUPPORT_DXR=1` environment variable
- Apple Silicon M3 or later (M1/M2 lack hardware raytracing)
- Game Porting Toolkit 3.0 (not 2.x)
