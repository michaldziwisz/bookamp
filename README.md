# Bookamp — audiobook DSP plugin for Winamp

Bookamp is a DSP (Digital Signal Processing) plugin for Winamp, designed for
listening to audiobooks, podcasts and spoken-word recordings. It lets you
smoothly change playback tempo, pitch and "tape speed", and optionally enhances
the sound (loudness leveling and speech intelligibility). The interface is fully
accessible to screen readers (NVDA, JAWS).

Author: **Michał Dziwisz**. Subject-matter consultant: **Patryk Faliszewski**.

Polish version of this file: [README.pl.md](README.pl.md).

---

## Download

The ready-to-use plugin (`dsp_bookamp.dll`, Windows 32-bit) and the installer
are published as **GitHub Releases** — built and hosted by GitHub, downloadable
without logging in:

**→ https://github.com/michaldziwisz/bookamp/releases/latest**

There are **no** binaries in the repository — to build it yourself, see the
"Building" section below.

---

## Bilingual interface (English / Polish)

Bookamp picks the window language based on the **Windows UI language**:

- Polish Windows → **Polish** interface.
- Any other Windows language → **English** interface.

Detection happens once, at plugin startup (`GetUserDefaultUILanguage`).

---

## Features

### Tempo, pitch and "tape" control

Three independent sliders:

- **Tempo** — changes playback speed **without** changing pitch. Range
  0.5x–8.0x, neutral 1.00x placed at 25% of the slider (for fine slow-down and
  plenty of head-room for speeding up).
- **Pitch** — changes pitch **without** changing tempo. Range ±1200 cents
  (±1 octave) in 5-cent steps; 100 cents = 1 semitone. Neutral: 0 (slider
  center).
- **Rate (tape)** — changes tempo **and** pitch together (like changing a tape
  deck's speed), with an anti-aliasing filter. Range 0.5x–4.0x.

Values are announced by the screen reader as real numbers (e.g. "1.50x",
"+2 semitones (+200 cents)"), not as a slider-position percentage.

### Two time-stretch engines

- **Bungee** (phase-vocoder) — default, high quality.
- **SoundTouch** (WSOLA) — an alternative tuned for speech.

Each engine has its own advanced parameters (separate window).

### Sound enhancement (optional, off by default)

Two independent 5-level effects (levels: Off / Subtle / Moderate / Medium /
Strong / Maximum):

- **Loudness** — a true-peak brickwall limiter with inter-sample peak
  protection (ISP, 4x oversampling). Evens out quiet and loud passages.
- **Speech intelligibility** — a bank of allpass filters (phase rotator),
  modeled after broadcast processors (Orban/Omnia). Improves speech clarity on
  poor speakers.

When both levels are 0, the module is skipped entirely (zero overhead, zero
change to the audio).

---

## Keyboard shortcuts

Shortcuts work **in any Winamp window** (while Winamp is focused), even when the
Bookamp configuration window is closed. They do not fire in other applications
or while you are typing in a text field.

| Shortcut                  | Action                              |
|---------------------------|-------------------------------------|
| `-` / `=`                 | tempo down / up (step 0.05x)        |
| `Shift` + `-` / `=`       | pitch −1 / +1 semitone              |
| `Ctrl`+`Shift`+ `-` / `=` | rate (tape) down / up               |
| `Shift` + `Backspace`     | reset all sliders                   |

(The top row keys are used, not the numeric keypad.)

---

## Installation

**With the installer (recommended):** download `bookamp-setup-vX.Y.Z.exe` from
Releases and run it. The installer detects the Winamp path from the registry and
copies the plugin into the `Plugins` folder. The wizard is bilingual
(English/Polish) and screen-reader accessible.

**Manually:** copy `dsp_bookamp.dll` into Winamp's `Plugins` folder (usually
`C:\Program Files (x86)\Winamp\Plugins`), start Winamp, go to
Preferences → Plug-ins → DSP/Effect and select Bookamp.

The plugin works on Winamp 5.5x and 5.9x (and compatibles such as WACUP).

---

## Building

Requirements: Windows, Visual Studio 2022 (toolset v143), CMake.

The SoundTouch and Bungee library sources are included as **git submodules** —
fetch them after cloning:

```
git clone --recurse-submodules https://github.com/michaldziwisz/bookamp
cd bookamp
```

(If you cloned without `--recurse-submodules`:
`git submodule update --init --recursive`.)

Then, from an "x86 Native Tools Command Prompt for VS 2022":

```
REM 1) build the third-party libraries (x86, /MT)
third_party\_build_soundtouch.bat
third_party\_build_bungee.bat

REM 2) build the plugin
_build_plugin.bat
```

Output: `build\dsp_bookamp.dll` (32-bit).

Building also runs automatically in GitHub Actions — pushing a `vX.Y.Z` tag
produces a public Release with the ready plugin and installer.

---

## License

Bookamp is released under the **GNU GPL v3** (see [LICENSE](LICENSE)).

The plugin uses open-source libraries (SoundTouch — LGPL 2.1, Bungee — MPL 2.0
and others). Full list and license notices: [NOTICE.md](NOTICE.md).
