// processor.h — rdzen DSP: int16<->float, FIFO, limit 2*numsamples, reakcja na format.
#pragma once
#include "engine.h"
#include "enhance.h"
#include "params.h"
#include <vector>
#include <deque>

namespace bookamp {

// Zwraca liczbe ramek zapisanych do samples (w zakresie [0 ; 2*numsamples]).
// bufor samples ma pojemnosc 2*numsamples*nch int16 (gwarancja Winampa).
class Processor {
    IStretchEngine *engine_ = nullptr;
    Enhancer enhancer_;                     // wzbogacanie dzwieku (po time-stretch)
    int enhLoud_ = -1, enhClar_ = -1;       // ostatnio zastosowane poziomy
    int algo_ = -1, srate_ = 0, nch_ = 0, bps_ = 0;
    unsigned gen_ = (unsigned)-1;            // ostatnio zastosowana generacja parametrow
    bool passthrough_ = false;              // srate>48k dla SoundTouch itp.
    double tempo_ = 1.0, pitch_ = 1.0, rate_ = 1.0;

    std::deque<short> outFifo_;             // interleaved int16 gotowe do oddania
    std::vector<float> fin_, fout_;         // bufory konwersji
    std::vector<float> enhBuf_;             // bufor float dla Enhancera
    int enhSrate_ = 0, enhNch_ = 0;         // format pod jaki skonfigurowano Enhancer

    void rebuild(int algo, int srate, int nch);
    void applyEnhance(short *samples, int frames, int nch, int srate);
public:
    ~Processor();
    // Glowny punkt wejscia z ModifySamples.
    int process(short *samples, int numsamples, int bps, int nch, int srate);
};

} // namespace bookamp
