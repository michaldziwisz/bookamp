// dsp_bookamp.cpp — wtyczka DSP Winampa (Bookamp).
// Wpina Processor (silniki time-stretch) + natywne GUI Win32.
// C++/Win32 gole API, x86, /MT. Licencja: GPLv3.
#include <windows.h>
#include "dsp.h"
#include "processor.h"
#include "params.h"

#define PLUGIN_VERSION "v1.0.0"

// Nazwa na liscie DSP Winampa (ANSI char*). PL w CP1250 gdy jezyk UI=polski.
static const char* pluginName() {
    static const bool pl = (PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_POLISH);
    return pl ? "Bookamp " PLUGIN_VERSION " \x97 DSP do audiobook\xF3w (tempo i wysoko\x9C\x86)"
              : "Bookamp " PLUGIN_VERSION " \x97 audiobook DSP (tempo and pitch)";
}

namespace bookamp {
    HWND createMainWindow(HINSTANCE, HWND);
    void destroyMainWindow();
    void settingsInit(HWND);
    void settingsLoad();
    void settingsSave();
    void installShortcuts(HINSTANCE);   // skroty zyja caly czas (Init->Quit)
    void removeShortcuts();
}

static winampDSPModule g_module;
static winampDSPHeader g_header;
static bookamp::Processor g_proc;

static void Config(winampDSPModule *this_mod) {
    // ponowne otwarcie/uwidocznienie okna sterowania
    bookamp::createMainWindow(this_mod->hDllInstance, this_mod->hwndParent);
}

static int Init(winampDSPModule *this_mod) {
    bookamp::settingsInit(this_mod->hwndParent);   // sciezka winamp.ini przez IPC
    bookamp::settingsLoad();
    bookamp::installShortcuts(this_mod->hDllInstance);  // skroty na caly czas dzialania
    bookamp::createMainWindow(this_mod->hDllInstance, this_mod->hwndParent);
    return 0;
}

static int ModifySamples(winampDSPModule *this_mod, short int *samples,
                         int numsamples, int bps, int nch, int srate) {
    return g_proc.process(samples, numsamples, bps, nch, srate);
}

static void Quit(winampDSPModule *this_mod) {
    bookamp::removeShortcuts();
    bookamp::settingsSave();
    bookamp::destroyMainWindow();
}

static winampDSPModule *getModule(int which) {
    if (which != 0) return NULL;
    g_module.description   = (char *)pluginName();
    g_module.hwndParent    = NULL;
    g_module.hDllInstance  = NULL;
    g_module.Config        = Config;
    g_module.Init          = Init;
    g_module.ModifySamples = ModifySamples;
    g_module.Quit          = Quit;
    g_module.userData      = NULL;
    return &g_module;
}

extern "C" __declspec(dllexport) winampDSPHeader *winampDSPGetHeader2(void) {
    g_header.version     = DSP_HDRVER;
    g_header.description = (char *)pluginName();
    g_header.getModule   = getModule;
    g_header.sf          = NULL;
    return &g_header;
}

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) DisableThreadLibraryCalls(hInst);
    return TRUE;
}
