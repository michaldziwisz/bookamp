# Bookamp — wtyczka DSP Winampa do audiobooków

Bookamp to wtyczka DSP (Digital Signal Processing) dla Winampa, zaprojektowana
z myślą o słuchaniu audiobooków, podcastów i nagrań mowy. Pozwala płynnie
zmieniać tempo, wysokość dźwięku oraz „prędkość taśmy", a dodatkowo opcjonalnie
wzbogaca dźwięk (wyrównanie głośności i poprawa czytelności mowy). Interfejs
jest w pełni dostępny dla czytników ekranu (NVDA, JAWS).

Autor: **Michał Dziwisz**. Konsultacja merytoryczna: **Patryk Faliszewski**.

Wersja anglojęzyczna tego pliku: [README.md](README.md).

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
