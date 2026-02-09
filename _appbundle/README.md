# DXX-Raytracer for macOS

A raytraced remaster of Descent (1995) running on macOS via Apple's Game Porting Toolkit.

## Requirements

- **macOS 14.0** (Sonoma) or later
- **Apple Silicon** Mac (M1/M2/M3/M4) — raytracing requires M3 or later
- **Game Porting Toolkit 3.0** (see install steps below)
- **Descent 1 game data** — `descent.hog` and `descent.pig` from a retail copy of Descent (1995)

## Step 1: Install Game Porting Toolkit

Open Terminal and run:

```
# Install Homebrew (skip if you already have it)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install Game Porting Toolkit
brew install --cask gcenx/wine/game-porting-toolkit
```

## Step 2: Install DXX-Raytracer

Drag **DXX-Raytracer.app** to your Applications folder (or anywhere you like).

## Step 3: Add Descent Game Data

You need `descent.hog` and `descent.pig` from a copy of Descent (1995).

1. Right-click **DXX-Raytracer.app** and choose **Show Package Contents**
2. Navigate to `Contents/Resources/game/`
3. Copy `descent.hog` and `descent.pig` into that folder

## Step 4: Launch

Double-click **DXX-Raytracer.app**. On first launch it will spend 10-30 seconds creating a Wine prefix — this only happens once.

## Controls

- **SHIFT + ALT + F1**: Open debug/graphics menus
- **F1**: Game keybindings help
- **ALT + F2**: Save game
- **ALT + F3**: Load game

## Troubleshooting

**"Game Porting Toolkit not found" dialog**: Complete Step 1 above.

**"Descent game data required" dialog**: Complete Step 3 above.

**No raytracing / software rendering**: Raytracing requires an M3 or M4 Mac. M1/M2 Macs can run the game but without ray traced lighting.

**Game crashes on launch**: Check the launch log for details:
```
cat ~/Library/Application\ Support/DXX-Raytracer/launch.log
```
Then try deleting the Wine prefix and relaunching:
```
rm -rf ~/Library/Application\ Support/DXX-Raytracer/prefix
```
