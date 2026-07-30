Bookamp — wykorzystane oprogramowanie open source / third-party notices
========================================================================

Bookamp (c) 2026 Michał Dziwisz. Licencja: GNU GPL v3 (patrz plik LICENSE).
Konsultacja merytoryczna: Patryk Faliszewski.

Wtyczka korzysta z następujących bibliotek open source. Ich kod NIE jest
zawarty w tym repozytorium — dołączony jest jako submoduły git (katalog
third_party/), a binarną wersję buduje się ze źródeł (lokalnie lub w CI).

------------------------------------------------------------------------
1. SoundTouch
   Silnik time-stretch/pitch (WSOLA).
   Autor: Olli Parviainen.
   Źródło: https://codeberg.org/soundtouch/soundtouch
   Licencja: GNU LGPL v2.1 (patrz third_party/soundtouch/COPYING.TXT).

------------------------------------------------------------------------
2. Bungee
   Silnik time-stretch/pitch (phase-vocoder).
   Źródło: https://github.com/bungee-audio-stretch/bungee  (tag v2.4.24)
   Licencja: Mozilla Public License 2.0 (patrz third_party/bungee/LICENSE).

   Bungee używa dalszych bibliotek (jako własne submoduły):
   - Eigen  — MPL 2.0 — https://gitlab.com/libeigen/eigen
   - PFFFT  — licencja typu BSD/FFTPACK — https://bitbucket.org/jpommier/pffft
   - cxxopts — MIT — https://github.com/jarro2783/cxxopts
     (używane tylko przez narzędzie CLI Bungee, nie przez wtyczkę).

------------------------------------------------------------------------
Winamp DSP SDK
   Nagłówek interfejsu wtyczki DSP (src/dsp.h) pochodzi z Winamp SDK
   (Nullsoft). Użyty wyłącznie jako definicja ABI interfejsu hosta.

------------------------------------------------------------------------
Zgodność licencji
   GPL v3 (Bookamp) jest zgodny z LGPL v2.1 (SoundTouch) oraz MPL 2.0
   (Bungee, Eigen): oba te typy licencji zezwalają na łączenie z kodem
   GPL, a dystrybucja całości następuje na warunkach GPL v3.
