// Gamaliel Aristondo, 8404071

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

#include "MyApp.hpp"

// using namespace gam;
using namespace al;

int asciiToKeyLabelIndex(int asciiKey) {
  switch (asciiKey) {
  case '2':
    return 30;
  case '3':
    return 31;
  case '5':
    return 33;
  case '6':
    return 34;
  case '7':
    return 35;
  case '9':
    return 37;
  case '0':
    return 38;

  case 'q':
    return 10;
  case 'w':
    return 11;
  case 'e':
    return 12;
  case 'r':
    return 13;
  case 't':
    return 14;
  case 'y':
    return 15;
  case 'u':
    return 16;
  case 'i':
    return 17;
  case 'o':
    return 18;
  case 'p':
    return 19;

  case 's':
    return 20;
  case 'd':
    return 21;
  case 'g':
    return 23;
  case 'h':
    return 24;
  case 'j':
    return 25;
  case 'l':
    return 27;
  case ';':
    return 28;

  case 'z':
    return 0;
  case 'x':
    return 1;
  case 'c':
    return 2;
  case 'v':
    return 3;
  case 'b':
    return 4;
  case 'n':
    return 5;
  case 'm':
    return 6;
  case ',':
    return 7;
  case '.':
    return 8;
  case '/':
    return 9;
  }
  return 0;
}

void MyApp::onCreate() {
    navControl().active(false);  // Disable navigation via keyboard, since we
                                 // will be using keyboard for note triggering

    // Set sampling rate for Gamma objects from app's audio
    gam::sampleRate(audioIO().framesPerSecond());

    imguiInit();

    // Cacluate the size of piano keys based on the app window size
    float w = float(width());
    float h = float(height());
    keyWidth = w / 10.f - keyPadding * 2.f;
    keyHeight = h / 2.f - keyPadding * 2.f;
    fontSize = keyWidth * 0.2;

    // Create a mesh that will be drawn as piano keys
    addRect(meshKey, 0, 0, keyWidth, keyHeight);

    // Set the font renderer
    fontRender.load(Font::defaultFont().c_str(), 60, 1024);

    // Play example sequence. Comment this line to start from scratch
    synthManager.synthSequencer().playSequence("synth1.synthSequence");
    synthManager.synthRecorder().verbose(true);
}

void MyApp::onSound(AudioIOData& io) {
    synthManager.render(io);  // Render audio
}

void MyApp::onAnimate(double dt) {
    // The GUI is prepared here
    imguiBeginFrame();
    // Draw a window that contains the synth control panel
    synthManager.drawSynthControlPanel();
    imguiEndFrame();
}

void MyApp::onDraw(Graphics& g) {
    g.clear();

    // This example uses only the orthogonal projection for 2D drawing
    g.camera(Viewpoint::ORTHO_FOR_2D);  // Ortho [0:width] x [0:height]

    // Drawing white piano keys
    for(int i = 0; i < 20; i++) {
      int index = i % 10;
      g.pushMatrix();
      
      float c = 0.9;
      float x = (keyWidth + keyPadding * 2) * index + keyPadding;
      float y = 0;

      if(i >= 10) {
        y = keyHeight + keyPadding * 2;
      }

      g.translate(x, y);
      g.color(c);
      g.tint(c);
      g.draw(meshKey);
      
      g.tint(1);
      fontRender.write(whitekeyLabels[i].c_str(), fontSize);
      fontRender.renderAt(g, {keyWidth * 0.5 - 5, keyHeight * 0.1, 0.0});

      g.popMatrix();
    }

    // Drawing balck piano keys
    for(int i = 0; i < 20; i++) {
      int index = i % 10;
      if(index == 2 || index == 6 || index == 9) continue;

      g.pushMatrix();
      
      float c = 0.5;      
      float x = (keyWidth + keyPadding * 2) * index + keyPadding + keyWidth * 0.5;
      float y = keyHeight * 0.5;

      if(i >= 10) {
        y = y + keyHeight + keyPadding * 2;
      }

      g.translate(x, y);
      g.scale(1, 0.5);
      g.color(c);
      g.tint(c);
      g.draw(meshKey);
      
      g.scale(1, 2);
      
      g.tint(1);
      fontRender.write(blackkeyLabels[i].c_str(), fontSize);
      fontRender.renderAt(g, {keyWidth * 0.5 - 5, keyHeight * 0.1, 0.0});
      
      g.popMatrix();
    }

    // Render the synth's graphics
    synthManager.render(g);

    // GUI is drawn here
    imguiDraw();
}

bool MyApp::onKeyDown(Keyboard const& k) {
    if (ParameterGUI::usingKeyboard()) {  // Ignore keys if GUI is using
                                          // keyboard
      return true;
    }
    if (k.shift()) {
      // If shift pressed then keyboard sets preset
      int presetNumber = asciiToIndex(k.key());
      synthManager.recallPreset(presetNumber);
    } else {
      // Otherwise trigger note for polyphonic synth
      int midiNote = asciiToMIDI(k.key());
      
      if (midiNote > 0) {
        // Check which key is pressed
        int keyIndex = asciiToKeyLabelIndex(k.key());
        
        bool isBlackKey = false;
        if(keyIndex >= 20) {
          keyIndex -= 20;
          isBlackKey = true;
        }

        synthManager.voice()->setInternalParameterValue(
            "frequency", ::pow(2.f, (midiNote - 69.f) / 12.f) * 220.f);

        float w = keyWidth;
        float h = keyHeight;
        float x = (keyWidth + keyPadding * 2) * (keyIndex % 10) + keyPadding;
        float y = 0;
        
        if(isBlackKey == true) {
          x += keyWidth * 0.5;
          y += keyHeight * 0.5;
          h *= 0.5;
        }
        
        if(keyIndex >= 10) {
          y += keyHeight + keyPadding * 2;
        }
        
        synthManager.voice()->setInternalParameterValue("pianoKeyWidth", w);
        synthManager.voice()->setInternalParameterValue("pianoKeyHeight", h);
        synthManager.voice()->setInternalParameterValue("pianoKeyX", x);
        synthManager.voice()->setInternalParameterValue("pianoKeyY", y);
        
        synthManager.triggerOn(midiNote);
      }
    }
    return true;
}

bool MyApp::onKeyUp(Keyboard const& k) {
    int midiNote = asciiToMIDI(k.key());
    if (midiNote > 0) {
      synthManager.triggerOff(midiNote);
    }
    return true;
}

void MyApp::onResize(int w, int h) {
    // Recaculate the size of piano keys based new window size
    keyWidth = w / 10.f - keyPadding * 2.f;
    keyHeight = h / 2.f - keyPadding * 2.f;
    fontSize = keyWidth * 0.2;
    addRect(meshKey, 0, 0, keyWidth, keyHeight);
}

void MyApp::onExit() { imguiShutdown(); }