// shortcuts.cpp — skroty klawiszowe dzialajace ZAWSZE w oknach Winampa,
// niezaleznie od zainstalowanych wtyczek (global hotkey/systray/jump-to-file).
//
// Mechanizm: WH_KEYBOARD_LL (low-level, systemowy) — przechwytuje klawisz ZANIM
// dotrze do Winampa (omija akceleratory/rozdzial klawiszy miedzy okna, ktory
// zawodzil przy WH_KEYBOARD na watek). Filtr: reagujemy TYLKO gdy okno
// pierwszoplanowe nalezy do procesu Winampa (nasz proces — DSP DLL zyje w
// winamp.exe). W innych aplikacjach klawisze nietkniete.
//
// Klawisze (gorny blok, nie numeryczny):
//   -   / =            tempo w dol / gore (krok 0,05x)
//   Shift+- / Shift+=  wysokosc -1 / +1 polton
//   Ctrl+Shift+- / =   rate (kaseta) w dol / gore
//   Shift+Backspace    reset
#include <windows.h>
#include "params.h"
#include "mapping.h"

namespace bookamp {

void nudgeTempo(double deltaX);
void nudgeRate(double deltaX);
void nudgePitchCents(int delta);
void resetSlidersToNeutral();

static HANDLE   g_hkThread = nullptr;
static DWORD    g_hkThreadId = 0;
static HHOOK    g_hook = nullptr;
static HINSTANCE g_hInst = nullptr;

// Czy okno pierwszoplanowe nalezy do NASZEGO procesu (czyli do Winampa)?
static bool winampIsForeground() {
    HWND fg = GetForegroundWindow();
    if (!fg) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(fg, &pid);
    return pid == GetCurrentProcessId();
}

// Czy fokus jest w polu edycji? (cross-thread przez GUITHREADINFO — GetFocus
// dziala tylko dla wlasnego watku). Wtedy skroty NIE dzialaja (user pisze).
static bool focusInEdit() {
    HWND fg = GetForegroundWindow();
    if (!fg) return false;
    DWORD tid = GetWindowThreadProcessId(fg, nullptr);
    GUITHREADINFO gti; gti.cbSize = sizeof(gti);
    if (!GetGUIThreadInfo(tid, &gti) || !gti.hwndFocus) return false;
    wchar_t cls[64] = {0};
    GetClassNameW(gti.hwndFocus, cls, 63);
    if (_wcsicmp(cls, L"Edit") == 0) return true;
    if (_wcsnicmp(cls, L"RichEdit", 8) == 0) return true;
    return false;
}

// Wykonaj akcje dla klawisza. Zwraca true jesli klawisz obsluzony (pochlonac).
static bool handleKey(DWORD vk) {
    bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    bool ctrl  = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    switch (vk) {
    case VK_OEM_MINUS:
        if (ctrl && shift)  nudgeRate(-RATE_STEP);
        else if (shift)     nudgePitchCents(-100);   // -1 polton
        else                nudgeTempo(-TEMPO_STEP);
        return true;
    case VK_OEM_PLUS:
        if (ctrl && shift)  nudgeRate(+RATE_STEP);
        else if (shift)     nudgePitchCents(+100);   // +1 polton
        else                nudgeTempo(+TEMPO_STEP);
        return true;
    case VK_BACK:
        if (shift) { resetSlidersToNeutral(); return true; }
        return false;
    default:
        return false;
    }
}

static LRESULT CALLBACK llProc(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION && (wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN)) {
        const KBDLLHOOKSTRUCT* k = (const KBDLLHOOKSTRUCT*)lp;
        if (winampIsForeground() && !focusInEdit()) {
            if (handleKey(k->vkCode)) return 1;  // pochlon klawisz (Winamp go nie zobaczy)
        }
    }
    return CallNextHookEx(g_hook, code, wp, lp);
}

// Dedykowany, dlugozyjacy watek z wlasna petla komunikatow (wymog MSDN dla LL hook).
static DWORD WINAPI hookThread(LPVOID) {
    g_hook = SetWindowsHookExW(WH_KEYBOARD_LL, llProc, g_hInst, 0);
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    if (g_hook) { UnhookWindowsHookEx(g_hook); g_hook = nullptr; }
    return 0;
}

// Zaloz skroty na CALY czas zycia wtyczki (wolane z Init(), nie z okna Configure).
void installShortcuts(HINSTANCE hInst) {
    if (g_hkThread) return;
    g_hInst = hInst;
    g_hkThread = CreateThread(nullptr, 0, hookThread, nullptr, 0, &g_hkThreadId);
}

void removeShortcuts() {
    if (g_hkThread) {
        PostThreadMessageW(g_hkThreadId, WM_QUIT, 0, 0);   // zakoncz petle -> zdejmie hak
        WaitForSingleObject(g_hkThread, 2000);
        CloseHandle(g_hkThread);
        g_hkThread = nullptr; g_hkThreadId = 0;
    }
}

} // namespace bookamp
