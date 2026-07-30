// harness.cpp — test silnikow poza Winampem. Dowod TRESCI: ramki + RMS.
// Sinus 200 Hz, 1 s, mono, 44100. Sprawdzamy tempo 1.5x (przyspieszenie):
//  - liczba ramek wyjscia ~ in/1.5
//  - RMS wyjscia zblizone do wejscia (energia zachowana, nie cisza/szum)
#include "engine.h"
#include <cstdio>
#include <cmath>
#include <vector>

using namespace bookamp;

static double rms(const std::vector<float>& v) {
    if (v.empty()) return 0.0;
    double s = 0; for (float x : v) s += (double)x * x;
    return std::sqrt(s / v.size());
}

static bool testEngine(Algo a, const char* label) {
    const int sr = 44100, nch = 1, N = 44100; // 1 s mono
    std::vector<float> in(N);
    for (int i = 0; i < N; ++i) in[i] = 0.5f * std::sin(2.0 * M_PI * 200.0 * i / sr);
    double inRms = rms(in);

    IStretchEngine* e = createEngine(a);
    if (!e) { printf("[%s] createEngine=NULL\n", label); return false; }
    e->configure(sr, nch);
    e->setTempo(1.5);   // przyspieszenie 1.5x
    e->setPitch(1.0);

    std::vector<float> out; out.reserve(N);
    const int blk = 1152;
    std::vector<float> tmp(blk * 4);
    for (int off = 0; off < N; off += blk) {
        int f = std::min(blk, N - off);
        e->putSamples(&in[off], f);
        int got;
        while ((got = e->receiveSamples(tmp.data(), blk * 4)) > 0)
            for (int i = 0; i < got; ++i) out.push_back(tmp[i]);
    }
    // domkniecie ogona (SoundTouch flush -> tu przez wielokrotne receive juz zebrane)
    int got;
    while ((got = e->receiveSamples(tmp.data(), blk * 4)) > 0)
        for (int i = 0; i < got; ++i) out.push_back(tmp[i]);

    double outRms = rms(out);
    double ratio = (double)out.size() / N;
    printf("[%s] name=%s in=%d out=%zu ratio=%.3f (ideal ~0.667) inRMS=%.4f outRMS=%.4f\n",
           label, e->name(), N, out.size(), ratio, inRms, outRms);

    bool ok = out.size() > (size_t)(N * 0.55) && out.size() < (size_t)(N * 0.78)
              && outRms > inRms * 0.5 && outRms < inRms * 1.6;
    printf("[%s] %s\n", label, ok ? "PASS" : "FAIL");
    delete e;
    return ok;
}

int main() {
    bool a = testEngine(ALGO_SOUNDTOUCH, "SoundTouch");
    bool b = testEngine(ALGO_BUNGEE, "Bungee");
    printf("RESULT: %s\n", (a && b) ? "ALL_PASS" : "SOME_FAIL");
    return (a && b) ? 0 : 1;
}
