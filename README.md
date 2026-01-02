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

## Features

### Tangent UI System (Tui)

An element-based UI framework with a human-readable markup language.

| Component | Status |
|-----------|--------|
| Buttons | Visual |
| Text | Complete |
| Boxes | Complete |
| Lines | Complete |
| Checkboxes | Visual |
| Radio buttons | Visual |
| Multiple font sizes | Complete |
| Input boxes | Not Implemented |
| Interaction | Not Implemented |
| Navigation  | Not Implemented |

Tui operates as a standalone rendering system with hooks for external interaction, designed for integration with Tangent2 and other projects alike.

### Operating System

| Feature | Description |
|---------|-------------|
| **Desktop Environment** | Icons, taskbar, clock, backgrounds — fully customizable |
| **Custom Filesystem** | Optimized for maximum storage capacity |
| **Process Virtualization** | Unlimited\* concurrent processes v |
| **Native Applications** | Performance-critical apps written in C (video playback, file management) |

\*Unlimited meaning that the only limitation is ram size (24kb in this case).
Let's say for example a program only took up a handful of bytes, didn't require any of its own memory (only registers) theoretically, if stored in rom (and read from there too), you would only need the PC register if the only instruction was B, so you can create an infinite loop that takes up 2 bytes of physical ram, and with 24kb, you can run up to 12000 processes all at once!
---

## Tangent SDK

An IDE dedicated for working with the build tools and what not.

### Supported Languages

**TASM** (Tangent Assembly)
```asm
lewp:                                        ; define looop label
    syscall get_element_field "smth" "x" r0  ; store the x position of the element "smth" into r0
    inc r0                                   ; increment r0
    syscall set_element_field "smth" "x" r0  ; store r0 into the x position field of the element with the id of "smth"
    syscall render_element "smth"            ; render the element with the id of "smth"
    b lewp                                   : loop to the start
```

**TML** (Tangent Markup Language)
```
root {                          ; start of all Tml
    id: "tml_root"              ; give the root Tml an id (if you want to render everything for example)
    text {                      ; define an text element with the id of "smth"
        id: "smth"              ;
        x: 10                   ; initialize the x position field with 10
        y: 20                   ; initialize the y position field with 20
        colour: black           ; set the colour/text colour to black
        text: "Hello, World!"   ; set the text field to "Hello, World!"
    }
}
```

Explination:
These two files work together to make a piece of text (Hello, World!) move horizontally right across the screen in an infinite loop.

---

## SDK Features

### Editor

- **Tabbed Interface** — Work on multiple files simultaneously with a tabbed editor
- **Syntax Highlighting** — Full support for `.tasm` (assembly) and `.tml` (markup) files with customizable colors
- **Code Completion** — Context-aware suggestions for keywords, registers, labels, and project symbols
- **Customizable Fonts** — Choose any monospace font and size from your system
- **Auto-Save** — Automatically save files when the editor loses focus (optional)

### Project Management

- **Workspace Explorer** — File tree with project navigation
- **File/Folder Creation** — Create new files and folders directly from the explorer
- **Project Templates** — Start new projects with predefined structure

### Build System

- **One-Click Build** — Compile your project with a single click or keyboard shortcut
- **Integrated Console** — View build output, errors, and warnings with color-coded messages
- **Error Highlighting** — Console output highlights errors (red), warnings (yellow), and success messages (green)

### Run & Debug

- **Emulator Integration** — Run your compiled project directly in CasioEmuMSVC
- **RP2Ease Support** — Load programs onto physical hardware via RP2Ease/PicoEase
- **Download from Device** — Pull data back from the calculator

#### Chip Configuration

The SDK supports advanced chip configuration for the ML620909:

**Chip Password (32 bytes)**
- `FF` — All 0xFF bytes (default)
- `00` — All 0x00 bytes
- `Casio Default` — Factory default password
- `Custom` — Enter your own 64-character hex string

**Clock Speed (16 bytes)**
- `Default` — 2 MHz standard clock
- `Overclocked` — 3.9 MHz with 21 MHz boost
- `Custom` — Enter your own 32-character hex configuration

Note:
If you were to select custom for these two options, make sure you know what you are doing, as it could *potentially* brick your device.

### Discord Rich Presence

Show off your development work with Discord integration:

- **Enable/Disable** — Toggle Discord presence on or off
- **Custom Text Format** — Define your own display text using variables:
  - `${file}` — Full file name (e.g., `main.tasm`)
  - `${file.baseName}` — File name without extension (e.g., `main`)
  - `${file.extension}` — File extension (e.g., `tasm`)
  - `${language}` — Programming language name
  - `${project}` — Project folder name
- **Elapsed Time Reset** — Choose when the timer resets:
  - On IDE startup
  - When opening a new project
  - When opening a new file

---

## Settings

Access settings via **File → Settings** or the toolbar. Settings are organized into tabs:

### General Tab

| Setting | Description |
|---------|-------------|
| **Workspace Folder** | Default folder for projects |
| **Auto-Save** | Enable automatic saving when editor loses focus |

### Editor Tab

#### Font Settings
- **Font Family** — Select from available monospace fonts
- **Font Size** — Adjust text size (6-72pt)
- Right-click to reset font settings to defaults

#### Code Completion
- **Enable/Disable** — Toggle the auto-completion popup
- **Minimum Characters** — Characters required before suggestions appear (1-10)

#### Syntax Highlighting Colors

Customize colors for each syntax element. Click the color swatch to open the color picker, check the Bold box for bold text.

**TASM Colors:**
- Keywords, Registers, Labels, Comments, Addresses, Directives

**TML Colors:**
- Keywords, Fields, Label References, Bracket Colors (3 levels)

**Common Colors:**
- Numbers, Strings

**Console Colors:**
- Error, Warning, Success, Normal text

Right-click any color to reset it to default, or use "Reset to Defaults" to reset all colors.

#### Syntax Extensions

Manage syntax definition extensions with priority ordering:

- **Import Extension** — Add a JSON extension file
- **Create Extension** — Create a new blank extension
- **Edit Default JSON** — Open the default definitions for reference
- **Reorder** — Use ▲/▼ buttons to change priority (higher position = takes precedence)
- **Enable/Disable** — Toggle extensions on or off via checkbox
- **Delete** — Remove custom extensions (right-click menu)

### Discord Tab

Configure Discord Rich Presence (see Discord Rich Presence section above).

### Run/Debug Tab

#### Paths
- **Emulator Executable** — Path to CasioEmuMSVC.exe
- **Model Folder** — Path to calculator model files
- **RP2Ease Executable** — Path to rp2ease.exe

#### Chip Password & Clock Speed
Configure hardware settings (see Run & Debug section above).

---

## Syntax Extensions

The SDK uses a JSON-based syntax definition system that supports custom extensions. Extensions are loaded in priority order, with higher-priority extensions overriding lower ones.

### Extension Files

Extensions are stored in:
- **Default:** `<app_dir>/syntax/default.json` (built-in, read-only reference)
- **User Settings:** `<AppData>/tangentsdk/syntax/edited.json` (your customizations)
- **Custom Extensions:** `<AppData>/tangentsdk/syntax/extensions/`

The `edited.json` file is automatically created and always exists — you can edit it directly to customize syntax colors without using the UI.

### Extension JSON Format

```json
{
    "name": "My Extension",
    "languages": {
        "tasm": {
            "name": "Tangent Assembly",
            "extensions": ["tasm"],
            "rules": {
                "myRule": {
                    "displayName": "My Custom Rule",
                    "color": "#FF5500",
                    "bold": false,
                    "pattern": "\\bmy_pattern\\b",
                    "keywords": ["word1", "word2"]
                }
            },
            "completions": {
                "myCategory": ["completion1", "completion2"]
            }
        }
    },
    "common": {
        "rules": {
            "number": {
                "displayName": "Numbers",
                "color": "#B5CEA8",
                "pattern": "\\b[0-9]+\\b"
            }
        }
    },
    "console": {
        "error": {
            "displayName": "Error Text",
            "color": "#F44747",
            "bold": true
        }
    }
}
```

### Rule Properties

| Property | Type | Description |
|----------|------|-------------|
| `displayName` | string | Name shown in settings UI |
| `color` | string | Hex color code (e.g., `#FF5500`) |
| `bold` | boolean | Whether text should be bold |
| `pattern` | string | Single regex pattern |
| `patterns` | array | Multiple regex patterns |
| `keywords` | array | List of keywords (auto-converted to pattern) |
| `dynamic` | boolean | If true, patterns loaded from project context (e.g., labels) |

### Extension Priority

Extensions are processed in order from top to bottom:
1. **Custom Extensions** — Your imported/created extensions (highest priority)
2. **Editor Settings (Your Settings)** — Colors customized via the Settings dialog
3. **Default (Built-in)** — Base syntax definitions (lowest priority)

Items at the top of the list have the highest priority and override items below them.

---

## Building

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

The build script will compil everything, and automatically launch the app when completed.

> **Note:** The OS itself has some compiler-specific dependencies that make standalone builds tricky. The SDK includes prebuilt OS binaries and will eventually support building the OS directly inside the SDK, allowing you to modify it to your needs.

### Having Issues?

If the build doesn't work on your setup — fork it, fix it, and open a pull request. Contributions are welcome.

---

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+S` | Save current file |
| `Ctrl+Shift+S` | Save all files |
| `Ctrl+B` | Build project |
| `Ctrl+,` | Open settings |

---

## What's Next?

Right now, it isn't much, you will *probably* like it when it is done, so here is my current path:
- Refactor Tui to be more compatable with many things, and look more readable/nicer
- Finish OS to be able to test run stuff, (Kernel and FS)
- Finish build tools so you guys can use it without having to manually create c arrays, this is what I have to work with for the time being, I'm sure it's not the *ideal* way of doing things, but it's purely for debugging/testing etc.:
```c
const byte test_element_const[] = {
	'<',
		TML_ROOT,
		E_FIELD_ID, 0, 0,
		E_FIELD_PARENT_ID, 0, 0,
		E_FIELD_FIELD_SIZE, 2,
		'<',
			TML_BUTTON,
			E_FIELD_ID, 0, 1,
			E_FIELD_PARENT_ID, 0, 0,
			E_FIELD_FIELD_SIZE, 27,
			E_FIELD_X, 10,
			E_FIELD_WIDTH, 80,
			E_FIELD_HEIGHT, 20,
			E_FIELD_Y, 10,
			E_FIELD_FONT_SIZE, FONT_SIZE_8x12,
			E_FIELD_TEXT_COLOUR, COLOUR_DARK_GREY,
			E_FIELD_BORDER_THICKNESS, 1,
			E_FIELD_BORDER_STYLE, LINE_STYLE_SOLID,
			E_FIELD_TEXT_ALIGN, ALIGN_MIDDLE_CENTER,
			E_FIELD_COLOUR, COLOUR_BLACK,
			E_FIELD_TEXT,
			'D','V','D',0,
		'>',
	'>'
};
```
- Create a desktop for desktop stuff
- Create bundled programs (text editor, debugger, file explorer, media viewer etc), a few of these built in apps such as the media viewer (specifically for video playing) will be written in c, and not actually run directly on the os, because if it were, it would be incredibly slow, (in the future I may support more accelerated stuff for media playing).
- Make some videos about it showcasing the whole thing
- Make Tangent3

Also FYI, I don't care too much about the IDE, mainly the os itself and build tools, so I just got claude to do it for me because I'm a lazy humanoid creature.
Sorry to dissapoint you all. 

---

## License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.

---

<div align="center">

**[Report Bug](../../issues) · [Request Feature](../../issues)**

</div>

