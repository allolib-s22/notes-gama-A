// Gamaliel Aristondo, 5-1-2022

#ifndef MYAPP_HPP
#define MYAPP_HPP

#include <cstdio>  // for printing to stdout

#include "Gamma/Analysis.h"
#include "Gamma/Effects.h"
#include "Gamma/Envelope.h"
#include "Gamma/Oscillator.h"

#include "al/app/al_App.hpp"
#include "al/io/al_MIDI.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/scene/al_PolySynth.hpp"
#include "al/scene/al_SynthSequencer.hpp"
#include "al/ui/al_ControlGUI.hpp"
#include "al/ui/al_Parameter.hpp"

#include "al/graphics/al_Shapes.hpp"
#include "al/graphics/al_Font.hpp"

#include "Drums.hpp"

// using namespace gam;
using namespace al;

class MyApp : public App, public MIDIMessageHandler {
public:
    SynthGUIManager<Kick> synthManagerKick{"Kick"};
    SynthGUIManager<Snare> synthManagerSnare{"Snare"};
    SynthGUIManager<HiHat> synthManagerHiHat{"HiHat"};

    SynthGUIManager<Drums> synthManager{"Drums"};

    RtMidiIn midiIn;

    // MIDI Functions
    void onInit() override;

    void onMIDIMessage(const MIDIMessage& m) override;

    // App Functions
    void onCreate() override;

    void onSound(AudioIOData &io) override;

    void onAnimate(double dt) override;

    void onDraw(Graphics &g) override;

    bool onKeyDown(Keyboard const &k) override;

    bool onKeyUp(Keyboard const &k) override;

    void onExit() override;

};

#endif