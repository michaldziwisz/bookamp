// test_settings.cpp — round-trip persystencji: save -> zmien -> load -> weryfikacja.
// Uzywa realnego WritePrivateProfileString do pliku w %TEMP% (nie Winamp).
#include <windows.h>
#include "params.h"
#include <cstdio>
#include <cmath>

namespace bookamp {
    void settingsInit(HWND);
    void settingsLoad();
    void settingsSave();
    // wersja testowa: wstrzyknij sciezke ini bezposrednio (ANSI)
    extern char* testIniPathPtr();
}
using namespace bookamp;

static int fails = 0;
static void chk(const char* w, bool ok) { printf("  %-34s %s\n", w, ok?"OK":"FAIL"); if(!ok)++fails; }

int main() {
    // ini w temp (ANSI)
    char tmp[MAX_PATH], ini[MAX_PATH];
    GetTempPathA(MAX_PATH, tmp);
    wsprintfA(ini, "%sbookamp_test.ini", tmp);
    DeleteFileA(ini);
    lstrcpynA(testIniPathPtr(), ini, MAX_PATH);

    // pitch 1.5 nie jest dokladna wielokrotnoscia centa; uzyj wartosci ktora
    // przetrwa round-trip przez centy: +700 centow (kwinta) -> mnoznik staly.
    const double pitchMul = 1.5;  // ~+702 centy; test sprawdza centy, nie mnoznik

    // ustaw nietypowe wartosci i zapisz
    g_params.algo.store(1);
    g_params.enabled.store(false);
    g_params.tempo.store(1.75);
    g_params.pitch.store(pitchMul);
    g_params.rate.store(0.8);
    g_params.st_aa_len.store(64);
    g_params.st_sequence_ms.store(82);
    g_params.bg_hop_adjust.store(-1);
    settingsSave();

    // zmien na inne (symulacja restartu)
    g_params.algo.store(0);
    g_params.enabled.store(true);
    g_params.tempo.store(1.0);
    g_params.pitch.store(1.0);
    g_params.rate.store(1.0);
    g_params.st_aa_len.store(32);
    g_params.st_sequence_ms.store(0);
    g_params.bg_hop_adjust.store(0);

    // wczytaj z ini
    settingsLoad();

    chk("algo=1", g_params.algo.load() == 1);
    chk("enabled=false", g_params.enabled.load() == false);
    chk("tempo=1.75", g_params.tempo.load() == 1.75);
    // pitch przez centy: 1.5 -> 702 centy -> mnoznik ~1.4999 (tolerancja 1 cent)
    chk("pitch~1.5 (przez centy)", std::fabs(g_params.pitch.load() - pitchMul) < 0.001);
    chk("rate=0.8", g_params.rate.load() == 0.8);
    chk("st_aa_len=64", g_params.st_aa_len.load() == 64);
    chk("st_sequence_ms=82", g_params.st_sequence_ms.load() == 82);
    chk("bg_hop_adjust=-1", g_params.bg_hop_adjust.load() == -1);

    DeleteFileA(ini);
    printf("RESULT: %s\n", fails==0 ? "ALL_PASS" : "SOME_FAIL");
    return fails==0 ? 0 : 1;
}
