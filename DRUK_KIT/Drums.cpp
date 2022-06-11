#include "Drums.hpp"

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

void Drums::onTriggerOn() { env.reset(), LFO.reset(); }

void Drums::onTriggerOff() { env.release(); }

void Kick::onTriggerOn() { env.reset(), LFO.reset(); }

void Kick::onTriggerOff() { env.release(); }

void Snare::onTriggerOn() { env.reset(), env2.reset(), LFO.reset(); }

void Snare::onTriggerOff() { env.release(), env2.release(); }

void HiHat::onTriggerOn() { env.reset(), LFO.reset(); }

void HiHat::onTriggerOff() { env.release(); }

void Drums::init() {
    // Kick parameters
    env.attack(0.1);
    env.amp(0.9);
    env.decay(0.25);
    LFO.freq(72);

    //createInternalTriggerParameter("amplitude", 0.9, 0.0, 1.0);
    createInternalTriggerParameter("frequency_kick", 80, 25, 120);
    createInternalTriggerParameter("frequency_snare", 165, 80, 300);
    createInternalTriggerParameter("attack", 0.05, 0.05, 0.5);
    createInternalTriggerParameter("decay", 0.25, 0.05, 2);
    createInternalTriggerParameter("pan", 0.0, -1.0, 1.0);
}

void Drums::onProcess(AudioIOData& io) {
    // Get the values from the parameters and apply them to the corresponding
    // unit generators. You could place these lines in the onTrigger() function,
    // but placing them here allows for realtime prototyping on a running
    // voice, rather than having to trigger a new voice to hear the changes.
    // Parameters will update values once per audio callback because they
    // are outside the sample processing loop.
    float f_k = getInternalParameterValue("frequency_kick");
    sine.freq(f_k * LFO.downU());


    float a = getInternalParameterValue("amplitude");
    env.lengths()[0] = (getInternalParameterValue("attack"));
    env.lengths()[2] = (getInternalParameterValue("decay"));
    pan.pos(getInternalParameterValue("pan"));
    while (io()) {
      float s1 = env() * (sine() * a);
      
      float s2;
      pan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }
    // We need to let the synth know that this voice is done
    // by calling the free(). This takes the voice out of the
    // rendering chain
    if (env.done()) free();
}

void Kick::init() {
    // Kick parameters
    env.attack(0.001);
    env.amp(0.9);
    env.decay(0.35);
    LFO.mod(0.9);

    createInternalTriggerParameter("amplitude", 0.9, 0.0, 1.0);
    createInternalTriggerParameter("frequency_kick", 60, 40, 120);
    createInternalTriggerParameter("attack", 0.001, 0.001, 0.5);
    createInternalTriggerParameter("decay", 0.45, 0.05, 2);
    createInternalTriggerParameter("pan", 0.0, -1.0, 1.0);
}

void Kick::onProcess(AudioIOData& io) {
    // Get the values from the parameters and apply them to the corresponding
    // unit generators. You could place these lines in the onTrigger() function,
    // but placing them here allows for realtime prototyping on a running
    // voice, rather than having to trigger a new voice to hear the changes.
    // Parameters will update values once per audio callback because they
    // are outside the sample processing loop.
    float f_k = getInternalParameterValue("frequency_kick");
    sine.freq(f_k * LFO.downU());


    float a = getInternalParameterValue("amplitude");
    env.lengths()[0] = (getInternalParameterValue("attack"));
    env.lengths()[2] = (getInternalParameterValue("decay"));
    pan.pos(getInternalParameterValue("pan"));
    while (io()) {
      float s1 = env() * (sine() * a);
      
      float s2;
      pan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }
    // We need to let the synth know that this voice is done
    // by calling the free(). This takes the voice out of the
    // rendering chain
    if (env.done()) free();
}

void HiHat::init() {
    // HiHat parameters
    env.attack(0.05);
    env.decay(0.1);

    LFO.freq(100);
    filter.res(4);
    filter.level(2);

    createInternalTriggerParameter("amplitude", 0.8, 0.0, 1.0);
    createInternalTriggerParameter("attack", 0.05, 0.05, 0.5);
    createInternalTriggerParameter("decay", 0.1, 0.05, 0.5);
    createInternalTriggerParameter("pan", 0.0, -1.0, 1.0);
    createInternalTriggerParameter("cutoff", 10000, 5000, 20000);
}

void HiHat::onProcess(AudioIOData& io) {
    // Get the values from the parameters and apply them to the corresponding
    // unit generators. You could place these lines in the onTrigger() function,
    // but placing them here allows for realtime prototyping on a running
    // voice, rather than having to trigger a new voice to hear the changes.
    // Parameters will update values once per audio callback because they
    // are outside the sample processing loop.
    filter.type(RESONANT);
    float cutoff = getInternalParameter("cutoff");
    filter.freq(cutoff * LFO.pulseU());


    float a = getInternalParameterValue("amplitude");
    env.decay(getInternalParameter("decay"));
    pan.pos(getInternalParameterValue("pan"));
    while (io()) {
      float s1 = env() * (filter(noise()) * a/10);
      
      float s2;
      pan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }
    // We need to let the synth know that this voice is done
    // by calling the free(). This takes the voice out of the
    // rendering chain
    if (env.done()) free();
}

void Snare::init() {
    // HiHat parameters
    env.attack(0.05);
    env.amp(0.8);
    env.decay(0.15);

    env2.attack(0.05);
    env2.amp(0.65);
    env2.decay(0.14);

    LFO.mod(0.3);


    createInternalTriggerParameter("amplitude", 0.8, 0.0, 1.0);
    createInternalTriggerParameter("frequency_snare", 190, 80, 300);
    createInternalTriggerParameter("attack", 0.05, 0.05, 0.5);
    createInternalTriggerParameter("decay", 0.3, 0.05, 2);
    createInternalTriggerParameter("pan", 0.0, -1.0, 1.0);
    createInternalTriggerParameter("tightness", 10, 5, 20);
    createInternalTriggerParameter("cutoff", 10000, 5000, 20000);
}

void Snare::onProcess(AudioIOData& io) {
    // Get the values from the parameters and apply them to the corresponding
    // unit generators. You could place these lines in the onTrigger() function,
    // but placing them here allows for realtime prototyping on a running
    // voice, rather than having to trigger a new voice to hear the changes.
    // Parameters will update values once per audio callback because they
    // are outside the sample processing loop.
    filter.type(HIGH_PASS);
    float f = getInternalParameterValue("frequency_snare");
    sine.freq(f * LFO.downU());
    sine2.freq(f * LFO.downU());
    sine3.freq(f * LFO.downU());

    float cutoff = getInternalParameterValue("cutoff");
    filter.freq(cutoff * LFO.pulseU());

    float a = getInternalParameterValue("amplitude");
    float t = getInternalParameterValue("tightness");
    env.lengths()[0] = (getInternalParameterValue("attack"));
    env.lengths()[2] = (getInternalParameterValue("decay"));
    pan.pos(getInternalParameterValue("pan"));
    while (io()) {
      float s1 = env() * (sine() * a*1.2) + env2() * (sine2()*1.57824 * a/3) + 
                  env2() * (sine3()*2.138492 * a/13) + env() * (filter(noise()) * a/t) + env2() * (sine3()*3.010293 * a/20);
      
      float s2;
      pan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }
    // We need to let the synth know that this voice is done
    // by calling the free(). This takes the voice out of the
    // rendering chain
    if (env.done() && env2.done()) free();
}