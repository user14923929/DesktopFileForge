# DesktopFileForge

A small Qt6 GUI tool for creating and editing `.desktop` launcher files
(the [freedesktop.org Desktop Entry Specification](https://specifications.freedesktop.org/desktop-entry-spec/latest/)),
without hand-editing INI syntax in a text editor.

## Features

- Guided form for all common `.desktop` fields (`Name`, `Exec`, `Icon`, `Categories`, `MimeType`, `Keywords`, ...)
- Icon picker with live preview, supporting both icon-theme names and absolute file paths
- Checkbox-based category picker using the well-known Desktop Menu Specification categories
- Built-in validator: catches missing required fields, unresolvable executables, and unknown icons/categories
- Open and edit existing `.desktop` files
- One-click save to `~/.local/share/applications`, or save-as to any custom path

## Building

Requires CMake ≥ 3.16, a C++20 compiler, and Qt6 (Widgets).

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/desktopforge
```

### Arch Linux

```bash
sudo pacman -S cmake ninja qt6-base
```

### Debian / Ubuntu

```bash
sudo apt install cmake ninja-build g++ qt6-base-dev
```

## Installing

```bash
sudo cmake --install build
```

## Installing (AUR)

```bash
paru -S desktopforge
```
or
```bash
yay -S desktopforge
```

This installs the binary, a `.desktop` launcher entry, and an app icon.

## License

GPL-3.0
