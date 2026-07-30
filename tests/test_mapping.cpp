// test_mapping.cpp — weryfikacja krzywej suwakow (bez GUI).
#include "mapping.h"
#include <cstdio>
#include <cmath>
using namespace bookamp;

static int fails = 0;
static void chk(const char* what, double got, double exp, double tol) {
    bool ok = std::fabs(got - exp) <= tol;
    printf("  %-30s got=%.4f exp=%.4f %s\n", what, got, exp, ok ? "OK" : "FAIL");
    if (!ok) ++fails;
}

int main() {
    printf("SLIDER_MAX=%d line=%d page=%d\n", SLIDER_MAX, SLIDER_LINE, SLIDER_PAGE);
    int tz = tempoNeutralPos(), pz = pitchNeutralPos();
    printf("tempoNeutralPos=%d (25%%), pitchNeutralPos=%d (50%%)\n", tz, pz);

    printf("TEMPO (neutral 25%%):\n");
    chk("pos=0 -> 0.5x",           posToTempo(0),          0.50, 1e-6);
    chk("pos=tz -> 1.0x",          posToTempo(tz),         1.00, 1e-3);
    chk("pos=MAX -> 8.0x",         posToTempo(SLIDER_MAX), 8.00, 1e-6);
    double prev = -1; bool mono = true;
    for (int i = 0; i <= SLIDER_MAX; ++i) { double t = posToTempo(i); if (t < prev-1e-9) mono=false; prev=t; }
    printf("  tempo monotoniczne: %s\n", mono?"OK":"FAIL"); if(!mono)++fails;
    int midUp = tz + (SLIDER_MAX - tz)/2;
    printf("  tempo w polowie gornego=%.3f (<4.5 = krzywa ostra): %s\n",
           posToTempo(midUp), posToTempo(midUp) < 4.5 ? "OK":"FAIL");
    if (!(posToTempo(midUp) < 4.5)) ++fails;
    // odwrotnosc krzywej: tempoToPos(posToTempo(x)) ~ x
    chk("odwrotnosc @1.0x -> tz",  tempoToPos(1.0), tz, 1.0);
    chk("odwrotnosc @2.0x round",  posToTempo(tempoToPos(2.0)), 2.0, 0.02);
    // staly krok strzalki: 1.00x -> +0.05 -> ~1.05x (rowny w calym zakresie)
    chk("krok +0,05 od 1.0x",      posToTempo(tempoToPos(1.0 + TEMPO_STEP)), 1.05, 0.01);
    chk("krok +0,05 od 3.0x",      posToTempo(tempoToPos(3.0 + TEMPO_STEP)), 3.05, 0.02);
    // snap-to-neutral: krok przez 1.0x zatrzymuje sie DOKLADNIE na 1.0x
    chk("snap 0.98->1.0 (up)",     stepToward(0.98, TEMPO_STEP, TEMPO_MID), 1.0, 1e-9);
    chk("snap 1.03->1.0 (down)",   stepToward(1.03, -TEMPO_STEP, TEMPO_MID), 1.0, 1e-9);

    printf("WYSOKOSC (centy, srodek 0):\n");
    chk("pos=0 -> -1200 centow",   posToPitchCents(0),               -1200.0, 1e-9);
    chk("pos=pz -> 0",             posToPitchCents(pz),                  0.0, 1e-9);
    chk("pos=MAX -> +1200",        posToPitchCents(PITCH_SLIDER_MAX),1200.0, 1e-9);
    chk("mul(+1200c)=2.0 oktawa",  pitchCentsToMul(1200),  2.0, 1e-9);
    chk("mul(-1200c)=0.5 oktawa",  pitchCentsToMul(-1200), 0.5, 1e-9);
    chk("mul(0)=1.0",              pitchCentsToMul(0),     1.0, 1e-9);
    chk("mul(+100c=polton)=1.0595",pitchCentsToMul(100), 1.059463, 1e-5);
    // krok strzalki (5 centow) zawsze zmienia wartosc -> brak martwej strefy
    bool step = (posToPitchCents(pz+PITCH_LINE) != posToPitchCents(pz));
    printf("  krok strzalki (5c) zmienia wartosc: %s\n", step?"OK":"FAIL"); if(!step)++fails;
    // REGRESJA: init z mnoznika 1.0 (0 centow) MUSI dac pozycje neutralna (nie oktawe!)
    chk("init 1.0 -> pitchNeutralPos",  pitchCentsToPos(mulToPitchCents(1.0)), pitchNeutralPos(), 0);
    chk("init 2.0 -> MAX pozycja",      pitchCentsToPos(mulToPitchCents(2.0)), PITCH_SLIDER_MAX, 0);
    chk("init 0.5 -> pozycja 0",        pitchCentsToPos(mulToPitchCents(0.5)), 0, 0);
    // skrot Shift+-/= = 1 polton = 100 centow (nie kwinta!)
    chk("polton = 100 centow",          posToPitchCents(pitchCentsToPos(100)), 100.0, 1e-9);

    printf("RATE (kaseta, krzywa jak tempo):\n");
    chk("rate pos=0 -> 0.5x",      posToRate(0),          0.50, 1e-6);
    chk("rate neutral -> 1.0x",    posToRate(rateNeutralPos()), 1.00, 1e-3);
    chk("rate pos=MAX -> 4.0x",    posToRate(SLIDER_MAX), 4.00, 1e-6);
    chk("rate krok +0,05 od 1.0",  posToRate(rateToPos(1.0 + RATE_STEP)), 1.05, 0.01);

    printf("RESULT: %s\n", fails==0 ? "ALL_PASS" : "SOME_FAIL");
    return fails==0 ? 0 : 1;
}
