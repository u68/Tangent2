<div align="center">

# Tangent2

**A custom operating system built from scratch for calculators**

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-ML620909-orange.svg)]()
[![Built with](https://img.shields.io/badge/SDK-Qt6-green.svg)]()

---

*Because not everyone can say they built an OS from scratch for a calculator.*

</div>

## Overview

Tangent2 is a complete ground-up rewrite of the original Tangent OS experiment. The goal is to create a functional, usable operating system targeting calculator hardware, complete with a custom development toolkit.

## ✦ Features

### Tangent UI System (Tui)

An element-based UI framework with a human-readable markup language that compiles directly to pixels.

| Component | Status |
|-----------|--------|
| Buttons | Visual |
| Text | Complete |
| Boxes | Complete |
| Lines | Complete |
| Checkboxes | Complete |
| Radio buttons | Complete |
| Multiple font sizes | Complete |

Tui operates as a standalone rendering system with hooks for external interaction, designed for seamless integration with Tangent2.

### Operating System

| Feature | Description |
|---------|-------------|
| **Desktop Environment** | Icons, taskbar, clock, backgrounds — fully customizable |
| **Custom Filesystem** | Optimized for maximum storage capacity on limited hardware |
| **Process Virtualization** | Unlimited concurrent processes via VM emulation |
| **Native Applications** | Performance-critical apps written in C (video playback, file management) |

## ✦ Tangent SDK

A purpose-built IDE for Tangent development featuring:

- **Syntax Highlighting** — Full support for `.tasm` (assembly) and `.tml` (markup) files
- **Code Completion** — Context-aware suggestions for keywords, registers, and labels
- **Project Management** — Workspace explorer with file/folder creation
- **Build Integration** — One-click compilation with integrated console output
- **Discord Rich Presence** — Show off what you're working on
- **Theming** — Customizable editor colors

### Supported Languages

**TASM** (Tangent Assembly)
```asm
lewp:
    syscall get_element_field "smth" "x" r0
    inc r0
    syscall set_element_field "smth" "x" r0
    syscall render_element "smth"
    b lewp
```

**TML** (Tangent Markup Language)
```
root {
    id: "tml_root"
    text {
        id: "smth"
        x: 10
        y: 20
        colour: black
        text: "Hello, World!" 
    }
}
```

## ✦ Building

### Prerequisites

- **MinGW64** (or compatible toolchain)
- **Qt6** (Widgets module)
- **DMD** (D compiler, for build tools)
- **CMake** + **Ninja**

### Build Steps

```bash
cd TangentSDK
./build.sh
```

The build script will:
1. Compile the assembler, linker, and ML compiler
2. Build the Qt-based IDE using CMake/Ninja
3. Launch the SDK

> **Note:** The OS itself has some compiler-specific dependencies that make standalone builds tricky. The SDK includes prebuilt OS binaries and will eventually support building the OS directly inside the SDK, allowing you to modify it to your needs.

### Having Issues?

If the build doesn't work on your setup — fork it, fix it, and open a pull request. Contributions are welcome.

## ✦ Project Structure

```
Tangent2/
├── Tangent2/
│   └── Lapis/           # Target hardware definitions
│   └── src/Tui/         # UI system source
├── TangentSDK/
│   └── src/
│       ├── app/         # Application context & Discord RPC
│       ├── editor/      # Syntax highlighting & completion
│       ├── theme/       # Theming system
│       ├── ui/          # Qt widgets (MainWindow, Editor, Console)
│       ├── buildtools/  # Assembler, compiler, linker (D source)
│       └── resources/   # Icons, themes, templates
└── README.md
```

## ✦ License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.

---

<div align="center">

**[Report Bug](../../issues) · [Request Feature](../../issues)**

</div>