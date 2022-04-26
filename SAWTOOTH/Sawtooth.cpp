// Gamaliel Aristondo, 8404071

#include "Sawtooth.hpp"

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

// using namespace gam;
using namespace al;

void Sawtooth::init() {
    // Intialize envelope
    mAmpEnv.curve(0);  // make segments lines
    mAmpEnv.levels(0, 1, 1, 0);
    mAmpEnv.sustainPoint(2);  // Make point 2 sustain until a release is issued

    addRect(mMesh, 0, 0, 1, 1);
    // This is a quick way to create parameters for the voice. Trigger
    // parameters are meant to be set only when the voice starts, i.e. they
    // are expected to be constant within a voice instance. (You can actually
    // change them while you are prototyping, but their changes will only be
    // stored and aplied when a note is triggered.)

    createInternalTriggerParameter("amplitude", 0.15, 0.0, 1.0);
    createInternalTriggerParameter("frequency", 60, 20, 5000);
    createInternalTriggerParameter("attackTime", 0.1, 0.01, 3.0);
    createInternalTriggerParameter("releaseTime", 0.1, 0.1, 10.0);
    createInternalTriggerParameter("pan", 0.0, -1.0, 1.0);
    
    createInternalTriggerParameter("pianoKeyX");
    createInternalTriggerParameter("pianoKeyY");
    createInternalTriggerParameter("pianoKeyWidth");
    createInternalTriggerParameter("pianoKeyHeight");
}

void Sawtooth::onProcess(AudioIOData& io) {
    // Get the values from the parameters and apply them to the corresponding
    // unit generators. You could place these lines in the onTrigger() function,
    // but placing them here allows for realtime prototyping on a running
    // voice, rather than having to trigger a new voice to hear the changes.
    // Parameters will update values once per audio callback because they
    // are outside the sample processing loop.
    float f = getInternalParameterValue("frequency");
    mOsc1.freq(f);
    mOsc2.freq(f * 2);
    mOsc3.freq(f * 3);
    mOsc4.freq(f * 4);
    mOsc5.freq(f * 5);
    mOsc6.freq(f * 6);
    mOsc7.freq(f * 7);
    mOsc8.freq(f * 8);
    mOsc9.freq(f * 9);
    mOsc10.freq(f * 10);
    mOsc11.freq(f * 11);
    mOsc12.freq(f * 12);


    float a = getInternalParameterValue("amplitude");
    mAmpEnv.lengths()[0] = getInternalParameterValue("attackTime");
    mAmpEnv.lengths()[2] = getInternalParameterValue("releaseTime");
    mPan.pos(getInternalParameterValue("pan"));
    while (io()) {
      float s1 = mAmpEnv() * 2 * a * (mOsc1() * 1 +
                              -mOsc2() * (1 / 2.0) +
                              mOsc3() * (1 / 3.0) +
                              -mOsc4() * (1 / 4.0) +
                              mOsc5() * (1 / 5.0) +
                              -mOsc6() * (1 / 6.0) +
                              mOsc7() * (1/ 7.0) +
                              -mOsc8() * (1 / 8.0) +
                              mOsc9() * (1 / 9.0) +
                              -mOsc10() * (1 / 10.0) +
                              mOsc11() * (1 / 11.0) +
                              -mOsc12() * (1 / 12.0));
      
      float s2;
      mEnvFollow(s1);
      mPan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }
    // We need to let the synth know that this voice is done
    // by calling the free(). This takes the voice out of the
    // rendering chain
    if (mAmpEnv.done() && (mEnvFollow.value() < 0.001f)) free();
}

void Sawtooth::onProcess(Graphics& g) {
    // Get the paramter values on every video frame, to apply changes to the
    // current instance
    float frequency = getInternalParameterValue("frequency");
    float amplitude = getInternalParameterValue("amplitude");

    float x = getInternalParameterValue("pianoKeyX");
    float y = getInternalParameterValue("pianoKeyY");

    float w = getInternalParameterValue("pianoKeyWidth");
    float h = getInternalParameterValue("pianoKeyHeight");

    float hue = (frequency - 200) / 1000;
    float sat = amplitude;
    float val = 0.9;
    
    g.pushMatrix();
    g.translate(x, y);
    g.scale(w, h);
    
    g.color(Color(HSV(hue, sat, val), mEnvFollow.value() * 30));
    
    g.draw(mMesh);
    g.popMatrix();
}

void Sawtooth::onTriggerOn() { mAmpEnv.reset(); }

void Sawtooth::onTriggerOff() { mAmpEnv.release(); }
