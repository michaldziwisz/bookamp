// settings.cpp — persystencja konfiguracji w winamp.ini (sekcja [Bookamp]).
// Sciezke do ini pobieramy OD Winampa przez IPC_GETINIFILE (334) — jedyny poprawny
// sposob (Winamp wie czy jest portable czy per-user AppData). Zapis obok DLL w
// Program Files\Plugins pada pod UAC. Zrodlo: wa_ipc.h (RAPORT_WINAMP_INI.md).
#include <windows.h>
#include <shlobj.h>
#include <cmath>
#include "params.h"

#pragma comment(lib, "shell32.lib")

namespace bookamp {

#define WM_WA_IPC        WM_USER      // 0x400
#define IPC_GETINIFILE   334          // char* pelna sciezka do winamp.ini (ANSI, Winamp 2.9+)

static char g_iniPath[MAX_PATH] = {0};

// dostep dla testu round-trip (wstrzykniecie sciezki ini bez Winampa)
char* testIniPathPtr() { return g_iniPath; }

// Ustal sciezke winamp.ini pytajac Winampa; fallback %APPDATA%\Winamp\Bookamp.ini.
void settingsInit(HWND hwndWinamp) {
    g_iniPath[0] = '\0';
    if (hwndWinamp) {
        LRESULT r = SendMessage(hwndWinamp, WM_WA_IPC, 0, IPC_GETINIFILE);
        const char* p = (const char*)r;
        if (p && r != 1 && p[0] != '\0') {          // odrzuc NULL i (char*)1
            lstrcpynA(g_iniPath, p, MAX_PATH);       // KOPIUJ natychmiast (bufor Winampa)
            return;
        }
    }
    // Fallback: %APPDATA%\Winamp\Bookamp.ini (per-user, zawsze zapisywalny)
    char appdata[MAX_PATH] = {0};
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appdata))) {
        char dir[MAX_PATH];
        wsprintfA(dir, "%s\\Winamp", appdata);
        CreateDirectoryA(dir, NULL);                 // ignoruj ERROR_ALREADY_EXISTS
        wsprintfA(g_iniPath, "%s\\Bookamp.ini", dir);
    }
}

static const char* SEC = "Bookamp";

static int  getI(const char* k, int def)  { return (int)GetPrivateProfileIntA(SEC, k, def, g_iniPath); }
static void setI(const char* k, int v)    { char b[32]; wsprintfA(b, "%d", v); WritePrivateProfileStringA(SEC, k, b, g_iniPath); }
// tempo/rate: 3 miejsca po przecinku (krok 0,05 wymaga >=2; 3 z zapasem) -> int*1000
static double getMilli(const char* k, double def) { return getI(k, (int)(def * 1000.0 + 0.5)) / 1000.0; }
static void   setMilli(const char* k, double v)   { setI(k, (int)(v * 1000.0 + 0.5)); }

void settingsLoad() {
    if (!g_iniPath[0]) return;
    g_params.algo.store(getI("algo", g_params.algo.load()));
    g_params.enabled.store(getI("enabled", 1) != 0);
    g_params.tempo.store(getMilli("tempo", 1.0));
    g_params.rate.store(getMilli("rate", 1.0));
    // wysokosc: przechowywana jako CENTY (dokladnie, bez utraty na mnozniku)
    int cents = getI("pitch_cents", 0);
    g_params.pitch.store(std::pow(2.0, cents / 1200.0));
    g_params.st_quickseek.store(getI("st_quickseek", 0) != 0);
    g_params.st_aafilter.store(getI("st_aafilter", 1) != 0);
    g_params.st_aa_len.store(getI("st_aa_len", 32));
    g_params.st_sequence_ms.store(getI("st_sequence_ms", 0));
    g_params.st_seekwindow_ms.store(getI("st_seekwindow_ms", 0));
    g_params.st_overlap_ms.store(getI("st_overlap_ms", 0));
    g_params.bg_hop_adjust.store(getI("bg_hop_adjust", 0));
    g_params.enh_loud.store(getI("enh_loud", 0));
    g_params.enh_clarity.store(getI("enh_clarity", 0));
    g_params.bumpGen();
}

void settingsSave() {
    if (!g_iniPath[0]) return;
    setI("algo", g_params.algo.load());
    setI("enabled", g_params.enabled.load() ? 1 : 0);
    setMilli("tempo", g_params.tempo.load());
    setMilli("rate", g_params.rate.load());
    // mnoznik wysokosci -> centy (calkowite, dokladne)
    int cents = (int)lround(1200.0 * (std::log(g_params.pitch.load()) / std::log(2.0)));
    setI("pitch_cents", cents);
    setI("st_quickseek", g_params.st_quickseek.load() ? 1 : 0);
    setI("st_aafilter", g_params.st_aafilter.load() ? 1 : 0);
    setI("st_aa_len", g_params.st_aa_len.load());
    setI("st_sequence_ms", g_params.st_sequence_ms.load());
    setI("st_seekwindow_ms", g_params.st_seekwindow_ms.load());
    setI("st_overlap_ms", g_params.st_overlap_ms.load());
    setI("bg_hop_adjust", g_params.bg_hop_adjust.load());
    setI("enh_loud", g_params.enh_loud.load());
    setI("enh_clarity", g_params.enh_clarity.load());
    WritePrivateProfileStringA(NULL, NULL, NULL, g_iniPath);  // flush cache na dysk
}

} // namespace bookamp
