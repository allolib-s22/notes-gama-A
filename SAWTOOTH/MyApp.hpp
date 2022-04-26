// Gamaliel Aristondo, 8404071

#ifndef MYAPP_HPP
#define MYAPP_HPP

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

#include "Sawtooth.hpp"

// using namespace gam;
using namespace al;

int asciiToKeyLabelIndex(int asciiKey);

class MyApp : public App {
 public:
  // GUI manager for SineEnv voices
  // The name provided determines the name of the directory
  // where the presets and sequences are stored
  SynthGUIManager<Sawtooth> synthManager{"Sawtooth"};
  
  // Mesh and variables for drawing piano keys
  Mesh meshKey;
  float keyWidth, keyHeight;
  float keyPadding = 2.f;
  float fontSize;
  std::string whitekeyLabels[20] = {"Z","X","C","V","B","N","M",",",".","/",
                                    "Q","W","E","R","T","Y","U","I","O","P"};
  std::string blackkeyLabels[20] = {"S","D","","G","H","J","","L",";","",
                                    "2","3","","5","6","7","","9","0",""};
  // Font renderder
  FontRenderer fontRender;

  // This function is called right after the window is created
  // It provides a grphics context to initialize ParameterGUI
  // It's also a good place to put things that should
  // happen once at startup.
  void onCreate() override;

  // The audio callback function. Called when audio hardware requires data
  void onSound(AudioIOData& io) override;

  void onAnimate(double dt) override;

  // The graphics callback function.
  void onDraw(Graphics& g) override;

  // Whenever a key is pressed, this function is called
  bool onKeyDown(Keyboard const& k) override;

  // Whenever a key is released this function is called
  bool onKeyUp(Keyboard const& k) override;
  
  // Whenever the window size changes this function is called
  void onResize(int w, int h) override;

  void onExit() override;
};

#endif