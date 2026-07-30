# Bookamp — audiobook DSP plugin for Winamp

*Polski opis znajduje się [poniżej](#bookamp--wtyczka-dsp-winampa-do-audiobooków).*

Bookamp is a DSP (Digital Signal Processing) plugin for Winamp, designed for
listening to audiobooks, podcasts and spoken-word recordings. It lets you
smoothly change playback tempo, pitch and "tape speed", and optionally enhances
the sound (loudness leveling and speech intelligibility). The interface is fully
accessible to screen readers (NVDA, JAWS).

Author: **Michał Dziwisz**. Subject-matter consultant: **Patryk Faliszewski**.

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
REM 0) apply the MSVC compatibility patch to Bungee (upstream targets GCC/Clang)
cd third_party\bungee && git apply ..\..\patches\bungee-msvc.patch && cd ..\..

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

<br>

═══════════════════════════════════════════════════════════════════════════

<br>

# Bookamp — wtyczka DSP Winampa do audiobooków

*The English description is [above](#bookamp--audiobook-dsp-plugin-for-winamp).*

Bookamp to wtyczka DSP (Digital Signal Processing) dla Winampa, zaprojektowana
z myślą o słuchaniu audiobooków, podcastów i nagrań mowy. Pozwala płynnie
zmieniać tempo, wysokość dźwięku oraz „prędkość taśmy", a dodatkowo opcjonalnie
wzbogaca dźwięk (wyrównanie głośności i poprawa czytelności mowy). Interfejs
jest w pełni dostępny dla czytników ekranu (NVDA, JAWS).

Autor: **Michał Dziwisz**. Konsultacja merytoryczna: **Patryk Faliszewski**.

---

## Pobieranie

Gotowa wtyczka (`dsp_bookamp.dll`, Windows 32-bit) oraz instalator są
publikowane jako **GitHub Releases** — budowane i hostowane przez GitHub,
do pobrania bez logowania:

**→ https://github.com/michaldziwisz/bookamp/releases/latest**

W repozytorium **nie ma** binarek — jak zbudować samodzielnie, patrz sekcja
„Budowanie" na dole.

---

## Dwujęzyczny interfejs (polski / angielski)

Bookamp automatycznie dobiera język okna do **języka interfejsu systemu
Windows**:

- Polski Windows → interfejs **polski**.
- Każdy inny język Windows → interfejs **angielski**.

Wykrycie następuje raz, przy starcie wtyczki (funkcja `GetUserDefaultUILanguage`).

---

## Funkcje

### Zmiana tempa, wysokości i „taśmy"

Trzy niezależne suwaki:

- **Tempo** — zmienia szybkość odtwarzania **bez** zmiany wysokości głosu.
  Zakres 0,5x–8,0x, wartość neutralna 1,00x ustawiona na 25% suwaka
  (żeby precyzyjnie zwalniać i mieć duży zapas na przyspieszanie).
- **Wysokość** — zmienia wysokość głosu **bez** zmiany tempa. Zakres
  ±1200 centów (±1 oktawa), w krokach 5 centów; 100 centów = 1 półton.
  Wartość neutralna: 0 (środek suwaka).
- **Rate (kaseta)** — zmienia tempo **i** wysokość jednocześnie (jak zmiana
  prędkości magnetofonu), z filtrem antyaliasingowym. Zakres 0,5x–4,0x.

Wartości są odczytywane przez czytnik ekranu jako realne liczby (np.
„1,50x", „+2 półtony (+200 centów)"), a nie jako procent położenia suwaka.

### Dwa silniki time-stretch

- **Bungee** (phase-vocoder) — domyślny, wysoka jakość.
- **SoundTouch** (WSOLA) — alternatywa zoptymalizowana pod mowę.

Każdy silnik ma własne parametry zaawansowane (osobne okno).

### Wzbogacanie dźwięku (opcjonalne, domyślnie wyłączone)

Dwa niezależne 5-stopniowe efekty (poziomy: Wyłączone / Delikatnie /
Umiarkowanie / Średnio / Mocno / Maksymalnie):

- **Wyrównywanie głośności** — brickwall limiter typu true-peak z ochroną
  przed przesterowaniem międzypróbkowym (ISP, 4-krotny oversampling).
  Wyrównuje głośność cichych i głośnych fragmentów.
- **Czytelność** — bateria filtrów allpass (phase rotator), na wzór
  procesorów nadawczych (Orban/Omnia). Poprawia zrozumiałość mowy na słabych
  głośnikach.

Gdy oba poziomy = 0, moduł jest całkowicie pomijany (zero narzutu, zero zmian
w dźwięku).

---

## Skróty klawiszowe

Skróty działają **w każdym oknie Winampa** (gdy Winamp jest aktywny), również
gdy okno konfiguracji Bookamp jest zamknięte. Nie działają w innych aplikacjach
ani gdy piszesz w polu tekstowym.

| Skrót                    | Działanie                          |
|--------------------------|------------------------------------|
| `-` / `=`                | tempo w dół / w górę (krok 0,05x)  |
| `Shift` + `-` / `=`      | wysokość −1 / +1 półton            |
| `Ctrl`+`Shift`+ `-` / `=`| rate (kaseta) w dół / w górę       |
| `Shift` + `Backspace`    | reset wszystkich suwaków           |

(Używany jest górny blok klawiszy, nie klawiatura numeryczna.)

---

## Instalacja

**Instalatorem (zalecane):** pobierz `bookamp-setup-vX.Y.Z.exe` z Releases i
uruchom. Instalator wykryje ścieżkę Winampa z rejestru i skopiuje wtyczkę do
katalogu `Plugins`. Kreator jest dwujęzyczny (polski/angielski) i dostępny dla
czytników ekranu.

**Ręcznie:** skopiuj `dsp_bookamp.dll` do katalogu `Plugins` Winampa
(zwykle `C:\Program Files (x86)\Winamp\Plugins`), uruchom Winamp, wejdź w
Preferencje → Wtyczki → DSP/Effect i wybierz Bookamp.

Wtyczka działa na Winampie 5.5x oraz 5.9x (i zgodnych, np. WACUP).

---

## Budowanie

Wymagania: Windows, Visual Studio 2022 (toolset v143), CMake.

Kod bibliotek SoundTouch i Bungee dołączony jest jako **submoduły git** — po
sklonowaniu trzeba je pobrać:

```
git clone --recurse-submodules https://github.com/michaldziwisz/bookamp
cd bookamp
```

(Jeśli sklonowałeś bez `--recurse-submodules`:
`git submodule update --init --recursive`.)

Następnie z „x86 Native Tools Command Prompt for VS 2022":

```
REM 0) nałóż łatkę zgodności z MSVC na Bungee (upstream celuje w GCC/Clang)
cd third_party\bungee && git apply ..\..\patches\bungee-msvc.patch && cd ..\..

REM 1) zbuduj biblioteki third-party (x86, /MT)
third_party\_build_soundtouch.bat
third_party\_build_bungee.bat

REM 2) zbuduj wtyczkę
_build_plugin.bat
```

Wynik: `build\dsp_bookamp.dll` (32-bit).

Budowanie odbywa się też automatycznie w GitHub Actions — po wypchnięciu tagu
`vX.Y.Z` powstaje publiczny Release z gotową wtyczką i instalatorem.

---

## Licencja

Bookamp jest wydany na licencji **GNU GPL v3** (patrz [LICENSE](LICENSE)).

Wtyczka korzysta z bibliotek open source (SoundTouch — LGPL 2.1, Bungee —
MPL 2.0 i inne). Pełna lista i noty licencyjne: [NOTICE.md](NOTICE.md).
