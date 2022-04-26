// Gamaliel Aristondo, 4-25-2022
#ifndef SAWTOOTH_HPP
#define SAWTOOTH_HPP

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

class Sawtooth : public SynthVoice {
 public:
  // Unit generators
  gam::Pan<> mPan;
  gam::Sine<> mOsc1;
  gam::Sine<> mOsc2;
  gam::Sine<> mOsc3;
  gam::Sine<> mOsc4;
  gam::Sine<> mOsc5;
  gam::Sine<> mOsc6;
  gam::Sine<> mOsc7;
  gam::Sine<> mOsc8;
  gam::Sine<> mOsc9;
  gam::Sine<> mOsc10;
  gam::Sine<> mOsc11;
  gam::Sine<> mOsc12;

  gam::Env<3> mAmpEnv;
  // envelope follower to connect audio output to graphics
  gam::EnvFollow<> mEnvFollow;

  Mesh mMesh;

  // Initialize voice. This function will only be called once per voice when
  // it is created. Voices will be reused if they are idle.
  void init() override;

  // The audio processing function
  void onProcess(AudioIOData& io) override;

  // The graphics processing function
  void onProcess(Graphics& g) override;

  // The triggering functions just need to tell the envelope to start or release
  // The audio processing function checks when the envelope is done to remove
  // the voice from the processing chain.
  void onTriggerOn() override;

  void onTriggerOff() override;
};

#endif