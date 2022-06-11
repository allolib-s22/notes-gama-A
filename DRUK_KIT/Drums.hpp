#ifndef DRUMS_HPP
#define DRUMS_HPP

#include <cstdio>  // for printing to stdout

#include "Gamma/Analysis.h"
#include "Gamma/Effects.h"
#include "Gamma/Envelope.h"
#include "Gamma/Oscillator.h"

#include "al/app/al_App.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/scene/al_PolySynth.hpp"
#include "al/scene/al_SynthSequencer.hpp"
#include "al/ui/al_ControlGUI.hpp"
#include "al/ui/al_Parameter.hpp"

#include "al/graphics/al_Shapes.hpp"
#include "al/graphics/al_Font.hpp"

using namespace gam;
using namespace al;

class Drums : public SynthVoice {
public:
    Sine<> sine;
    Sine<> sine2;
    Sine<> sine3;
    NoiseWhite<> noise;
    Pan<> pan;
    AD<> env;
    AD<> env2;
    LFO<> LFO;
    Biquad<> filter;

    void init() override;

    // The audio processing function
    void onProcess(AudioIOData& io) override;

    // The triggering functions just need to tell the envelope to start or release
    // The audio processing function checks when the envelope is done to remove
    // the voice from the processing chain.
    void onTriggerOn() override;

    void onTriggerOff() override;
};

class Kick : public Drums {
public:
    void init() override;

    void onProcess(AudioIOData& io) override;

    void onTriggerOn() override;

    void onTriggerOff() override;
};

class HiHat : public Drums {
public:
    void init() override;

    void onProcess(AudioIOData& io) override;

    void onTriggerOn() override;

    void onTriggerOff() override;
};

class Snare : public Drums {
public:
    void init() override;

    void onProcess(AudioIOData& io) override;

    void onTriggerOn() override;

    void onTriggerOff() override;
};

#endif