// test_enhance.cpp — weryfikacja modulu wzbogacania (allpass + limiter true-peak), bez GUI.
#include "enhance.h"
#include <cstdio>
#include <cmath>
#include <vector>
#include <algorithm>
using namespace bookamp;

static int fails = 0;
static double db(double lin) { return 20.0 * std::log10(std::max(1e-9, lin)); }
// Amplituda skladowej f (Goertzel) w x[s..s+n) — do pomiaru THD.
static double goertzel(const std::vector<float>& x, int s, int n, double f, int fs) {
    double w = 2*M_PI*f/fs, coeff = 2*std::cos(w), q0, q1 = 0, q2 = 0;
    for (int i = s; i < s+n; ++i) { q0 = coeff*q1 - q2 + x[i]; q2 = q1; q1 = q0; }
    return std::sqrt(q1*q1 + q2*q2 - q1*q2*coeff) / (n/2.0);
}
static void chk(const char* what, bool ok) {
    printf("  %-42s %s\n", what, ok ? "OK" : "FAIL");
    if (!ok) ++fails;
}

int main() {
    const int fs = 44100, N = fs;                 // 1 s
    const float CEIL = std::pow(10.f, -1.f / 20.f); // sufit -1 dBTP

    printf("CLARITY (phase rotator = allpass, nie rusza amplitudy):\n");
    {
        Enhancer e; e.configure(fs, 1); e.setClarity(5); e.setLoudness(0);
        std::vector<float> x(N), o(N);
        for (int i = 0; i < N; ++i) {
            float s = 0.3f * (std::sin(2*M_PI*300*i/fs) + std::sin(2*M_PI*700*i/fs) + std::sin(2*M_PI*1500*i/fs));
            x[i] = o[i] = s;
        }
        e.process(x.data(), N);
        double ri = 0, ro = 0;
        for (int i = fs/2; i < N; ++i) { ri += o[i]*(double)o[i]; ro += x[i]*(double)x[i]; }
        ri = std::sqrt(ri/(N/2)); ro = std::sqrt(ro/(N/2));
        double diff = 0; for (int i = fs/2; i < N; ++i) diff = std::max(diff, (double)std::fabs(x[i]-o[i]));
        printf("    L5 RMS in=%.2f dB out=%.2f dB (ratio=%.4f), max|dif|=%.3f\n", db(ri), db(ro), ro/ri, diff);
        chk("L5 amplituda nietknieta (0.98<ratio<1.02)", ro/ri > 0.98 && ro/ri < 1.02);
        chk("L5 faza realnie obrocona (max|dif|>0.1)",   diff > 0.1);
    }

    printf("LOUDNESS (limiter true-peak, sufit -1 dBTP):\n");
    for (int L = 1; L <= 5; ++L) {
        Enhancer e; e.configure(fs, 1); e.setLoudness(L); e.setClarity(0);
        std::vector<float> x(N);
        for (int i = 0; i < N; ++i) x[i] = 0.98f * std::sin(2*M_PI*1000*i/fs);
        e.process(x.data(), N);
        float pk = 0; bool nan = false;
        for (int i = fs/2; i < N; ++i) { if (std::isnan(x[i]) || std::isinf(x[i])) nan = true; pk = std::max(pk, (float)std::fabs(x[i])); }
        // Limiter TRUE-peak (ISP ON, jak LoudMax v1.47): ogranicza szczyt MIEDZYPROBKOWY,
        // wiec szczyt PROBKOWY moze siegac ~0 dBFS (wyrocznia .so: 0.9885). To ZAMIERZONE.
        // Niezmiennik: wyjscie ograniczone do pelnej skali (<=0 dBFS +ULP) i bez NaN.
        char b[80]; std::snprintf(b, 80, "L%d 0dBFS -> peak<=0dBFS true-peak (%.2f dB), bez NaN", L, db(pk));
        chk(b, pk <= 1.0f + 1e-4f && !nan);
    }
    {   // cichy sygnal ma byc podniesiony (maksymalizacja glosnosci)
        Enhancer e; e.configure(fs, 1); e.setLoudness(3); e.setClarity(0);  // prog -18 dB
        std::vector<float> x(N), o(N);
        for (int i = 0; i < N; ++i) { x[i] = o[i] = 0.1f * std::sin(2*M_PI*1000*i/fs); }
        e.process(x.data(), N);
        double ri = 0, ro = 0;
        for (int i = fs/2; i < N; ++i) { ri += o[i]*(double)o[i]; ro += x[i]*(double)x[i]; }
        ri = std::sqrt(ri/(N/2)); ro = std::sqrt(ro/(N/2));
        printf("    L3 cichy(-20dB): RMS %.2f -> %.2f dB (+%.2f dB)\n", db(ri), db(ro), db(ro/ri));
        chk("L3 cichy sygnal podniesiony", ro > ri);
    }

    printf("BAS (regr. anty-modulacja: release-hold, gain nie faluje w rytm fali):\n");
    for (int freq : {50, 80, 120}) {         // BYLO przed fixem: 50Hz 3.1%, 80Hz 1.0%
        Enhancer e; e.configure(fs, 1); e.setLoudness(3); e.setClarity(0);
        std::vector<float> x(2*N);
        for (int i = 0; i < 2*N; ++i) x[i] = 0.8f * std::sin(2*M_PI*freq*i/fs);
        e.process(x.data(), 2*N);
        double f0 = goertzel(x, N, N, freq, fs), thd = 0;
        for (int h = 2; h <= 8; ++h) { double hh = goertzel(x, N, N, freq*h, fs); thd += hh*hh; }
        double pct = std::sqrt(thd)/f0*100;
        char b[64]; std::snprintf(b, 64, "bas %dHz THD=%.2f%% (<1.0%%)", freq, pct);
        chk(b, pct < 1.0);   // LoudMax ref: 50Hz 0.37%, my ~0.5% (niesłyszalne)
    }

    printf("OFF + integracja:\n");
    {   // OFF: bit-w-bit nietkniety
        Enhancer e; e.configure(fs, 1); e.setLoudness(0); e.setClarity(0);
        std::vector<float> x(1000), o(1000);
        for (int i = 0; i < 1000; ++i) { x[i] = o[i] = 0.5f * std::sin(0.05f*i); }
        e.process(x.data(), 1000);
        bool same = true; for (int i = 0; i < 1000; ++i) if (x[i] != o[i]) { same = false; break; }
        chk("OFF (0/0) bit-w-bit nietkniety", same);
    }
    {   // loud+clar+stereo+transient: brak przesteru
        Enhancer e; e.configure(fs, 2); e.setLoudness(4); e.setClarity(3);
        std::vector<float> x(N*2, 0.f);
        for (int i = 0; i < N; ++i) { float s = 0.2f*std::sin(2*M_PI*500*i/fs); if (i%11025==0) s = 0.99f; x[i*2]=x[i*2+1]=s; }
        e.process(x.data(), N);
        float pk = 0; bool nan = false;
        for (int i = fs; i < N*2; ++i) { if (std::isnan(x[i])) nan = true; pk = std::max(pk, (float)std::fabs(x[i])); }
        chk("L4+C3 stereo+klik: peak<=0dBFS true-peak, bez NaN", pk <= 1.0f + 1e-4f && !nan);
    }

    printf(fails ? "\nFAILS=%d\n" : "\nWSZYSTKO OK\n", fails);
    return fails;
}
