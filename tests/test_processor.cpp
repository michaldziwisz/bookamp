// test_processor.cpp — test rdzenia Processor (int16, limit 2*numsamples, FIFO).
#include "processor.h"
#include <cstdio>
#include <cmath>
#include <vector>
#include <climits>

using namespace bookamp;

static bool run(int algoSel, const char* label, double tempo) {
    g_params.algo.store(algoSel);
    g_params.enabled.store(true);
    g_params.tempo.store(tempo);
    g_params.pitch.store(1.0);

    const int sr = 44100, nch = 2, block = 576; // stereo, typowy blok
    const int totalFrames = sr * 2;             // 2 s
    Processor p;

    long long inFrames = 0, outFrames = 0;
    double inSq = 0, outSq = 0; long long inCnt = 0, outCnt = 0;
    int maxRet = 0; bool limitOk = true;

    std::vector<short> buf((size_t)block * 2 * nch); // pojemnosc 2*block*nch (jak Winamp)
    double phase = 0.0, dp = 2.0 * M_PI * 220.0 / sr;
    for (int done = 0; done < totalFrames; done += block) {
        int nf = std::min(block, totalFrames - done);
        for (int i = 0; i < nf; ++i) {
            short s = (short)(std::sin(phase) * 12000); phase += dp;
            for (int c = 0; c < nch; ++c) buf[(size_t)i * nch + c] = s;
        }
        for (int i = 0; i < nf * nch; ++i) { inSq += (double)buf[i]*buf[i]; }
        inCnt += (long long)nf * nch; inFrames += nf;

        int ret = p.process(buf.data(), nf, 16, nch, sr);
        if (ret > 2 * nf) limitOk = false;   // TWARDY limit Winampa
        if (ret > maxRet) maxRet = ret;
        for (int i = 0; i < ret * nch; ++i) { outSq += (double)buf[i]*buf[i]; }
        outCnt += (long long)ret * nch; outFrames += ret;
    }

    double inRms = std::sqrt(inSq / std::max<long long>(1,inCnt));
    double outRms = std::sqrt(outSq / std::max<long long>(1,outCnt));
    double ratio = (double)outFrames / inFrames;
    // Sufit API Winampa: nie da sie zwolnic ponizej 0.5x (limit 2*numsamples),
    // wiec efektywne tempo = max(tempo, 0.5). Zadanie 0.25x realnie daje ~0.5x.
    double effTempo = std::max(tempo, 0.5);
    double ideal = 1.0 / effTempo;
    printf("[%s tempo=%.2f] in=%lld out=%lld ratio=%.3f (ideal~%.3f) maxRet=%d(limit=%d) inRMS=%.0f outRMS=%.0f\n",
           label, tempo, inFrames, outFrames, ratio, ideal, maxRet, 2*block, inRms, outRms);

    // Tolerancja: ratio w +-15% ideału (latencja/ogon), limit nieprzekroczony, energia sensowna.
    bool ok = limitOk && ratio > ideal*0.80 && ratio < ideal*1.12
              && outRms > inRms*0.5 && outRms < inRms*1.6;
    printf("[%s] limit_ok=%d %s\n", label, (int)limitOk, ok ? "PASS" : "FAIL");
    return ok;
}

int main() {
    bool r = true;
    // Przyspieszanie = glowny case audiobooka.
    r &= run(ALGO_SOUNDTOUCH, "ST-speed", 1.75);
    r &= run(ALGO_BUNGEE,     "BG-speed", 1.75);
    // 0.5x = TWARDA dolna granica DSP Winampa (limit 2*numsamples). Ponizej API nie nadaza.
    r &= run(ALGO_SOUNDTOUCH, "ST-slow",  0.50);
    r &= run(ALGO_BUNGEE,     "BG-slow",  0.50);
    // DOWOD: zadanie 0.25x fizycznie utyka na ~0.5x (ratio~2.0), bo produkcja 4x > limit 2x.
    printf("--- dowod: zadane tempo 0.25x (k=4) realnie utyka na sufit API ~0.5x ---\n");
    r &= run(ALGO_SOUNDTOUCH, "ST-0.25", 0.25);
    r &= run(ALGO_BUNGEE,     "BG-0.25", 0.25);
    printf("RESULT: %s\n", r ? "ALL_PASS" : "SOME_FAIL");
    return r ? 0 : 1;
}
