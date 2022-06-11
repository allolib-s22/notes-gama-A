// Gamaliel Aristondo, 5-1-2022

#include <cstdio>  // for printing to stdout

#include "Gamma/Analysis.h"
#include "Gamma/Effects.h"
#include "Gamma/Envelope.h"
#include "Gamma/Oscillator.h"

#include "al/app/al_App.hpp"
#include "al/io/al_MIDI.hpp"
#include "al/scene/al_SequencerMIDI.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/scene/al_PolySynth.hpp"
#include "al/scene/al_SynthSequencer.hpp"
#include "al/ui/al_ControlGUI.hpp"
#include "al/ui/al_Parameter.hpp"

#include "al/graphics/al_Shapes.hpp"
#include "al/graphics/al_Font.hpp"

#include "MyApp.hpp"

using namespace gam;
using namespace al;

void MyApp::onInit() {
    // Check for connected MIDI devices
    if (midiIn.getPortCount() > 0) {
      // Bind ourself to the RtMidiIn object, to have the onMidiMessage()
      // callback called whenever a MIDI message is received
      MIDIMessageHandler::bindTo(midiIn);

      // Open the last device found
      unsigned int port = midiIn.getPortCount() - 1;
      midiIn.openPort(port);
      printf("Opened port to %s\n", midiIn.getPortName(port).c_str());
    } else {
      printf("Error: No MIDI devices found.\n");
    }
  }

void MyApp::onMIDIMessage(const MIDIMessage& m) {
    printf("%s: ", MIDIByte::messageTypeString(m.status()));

    // Here we demonstrate how to parse common channel messages
    switch (m.type()) {
      case MIDIByte::NOTE_ON: {
        int midiNote = m.noteNumber();
        if(midiNote == 36 && m.velocity() > 0.001) {
          synthManagerKick.triggerOn(midiNote);
        }
        if(midiNote == 42 && m.velocity() > 0.001) {
          synthManagerHiHat.voice()->setInternalParameterValue("decay", 0.05);
          synthManagerHiHat.triggerOn(midiNote);
        }
        if(midiNote == 46 && m.velocity() > 0.001) {
          synthManagerHiHat.voice()->setInternalParameterValue("decay", 1.8);
          synthManagerHiHat.triggerOn(midiNote);
        }
        if(midiNote == 40 && m.velocity() > 0.001) {
          synthManagerSnare.triggerOn(midiNote);
        }
        break;
      
      }

      case MIDIByte::NOTE_OFF: {
        int midiNote = m.noteNumber();
        synthManagerKick.triggerOff(midiNote);
        synthManagerHiHat.triggerOff(midiNote);
        synthManagerSnare.triggerOff(midiNote);
        break;
      }

      // Control messages need to be parsed again...
      case MIDIByte::CONTROL_CHANGE:
        printf("%s ", MIDIByte::controlNumberString(m.controlNumber()));
        switch (m.controlNumber()) {
          case MIDIByte::MODULATION:
            printf("%f", m.controlValue());
            break;

          case MIDIByte::EXPRESSION:
            printf("%f", m.controlValue());
            break;
        }
        break;
      default:;
    }

    // If it's a channel message, print out channel number

    // Print the raw byte values and time stamp
    printf("\tBytes = ");
    for (unsigned i = 0; i < 3; ++i) {
      printf("%3u ", (int)m.bytes[i]);
    }
    printf(", time = %g\n", m.timeStamp());
}



void MyApp::onCreate() {
    navControl().active(false);

    gam::sampleRate(audioIO().framesPerSecond());

    al::imguiInit();

    synthManagerKick.synthRecorder().verbose(true);
    synthManagerKick.synthSequencer().verbose(true);
    synthManagerKick.synthSequencer().setDirectory("Kick-data");

    synthManagerHiHat.synthRecorder().verbose(true);
    synthManagerHiHat.synthSequencer().verbose(true);
    synthManagerHiHat.synthSequencer().setDirectory("HiHat-data");

    synthManagerSnare.synthRecorder().verbose(true);
    synthManagerSnare.synthSequencer().verbose(true);
    synthManagerSnare.synthSequencer().setDirectory("Snare-data");
    
    /*
    synthManager.synthRecorder().verbose(true);
    synthManager.synthSequencer().verbose(true);
    synthManager.synthSequencer().setDirectory("Drums-data");
    */
  }

void MyApp::onSound(AudioIOData &io) {
      synthManagerKick.render(io);
      synthManagerHiHat.render(io);
      synthManagerSnare.render(io);
}

bool recordLoop = false;
bool playLoop = false;
int sequenceFileNum = 0;

void MyApp::onAnimate(double dt) {
    // The GUI is prepared here
    // Draw a window that contains the synth control panel
    bool active = true;
    imguiBeginFrame();
    ImGui::Begin("Percussion", &active, ImGuiWindowFlags_MenuBar);

    ImGui::Checkbox("Record Loop", &recordLoop);
    ImGui::Checkbox("Play Loop", &playLoop);

    int freq_k, freq_s, tone_h, tone_s, tight;
    float amp_k, amp_s, amp_h;
    ImGui::Text("Kick Parameters");
    freq_k = synthManagerKick.voice()->getInternalParameterValue("frequency_kick");
    amp_k = synthManagerKick.voice()->getInternalParameterValue("amplitude");
    ImGui::SliderInt("Frequency_K", &freq_k, 40, 120);
    ImGui::SliderFloat("Volume_K", &amp_k, 0.0, 1.0);
    synthManagerKick.voice()->setInternalParameterValue("frequency_kick", float(freq_k));
    synthManagerKick.voice()->setInternalParameterValue("amplitude", amp_k);

    ImGui::Text("Snare Parameters");
    freq_s = synthManagerSnare.voice()->getInternalParameterValue("frequency_snare");
    amp_s = synthManagerSnare.voice()->getInternalParameterValue("amplitude");
    tight = synthManagerSnare.voice()->getInternalParameterValue("tightness");
    tone_s = synthManagerSnare.voice()->getInternalParameterValue("cutoff");
    ImGui::SliderInt("Frequency_S", &freq_s, 80, 300);
    ImGui::SliderFloat("Volume_S", &amp_s, 0.0, 1.0);
    ImGui::SliderInt("Tone_S", &tone_s, 5000, 20000);
    ImGui::SliderInt("Tightness", &tight, 5, 20);
    synthManagerSnare.voice()->setInternalParameterValue("frequency_snare", float(freq_s));
    synthManagerSnare.voice()->setInternalParameterValue("amplitude", amp_s);
    synthManagerSnare.voice()->setInternalParameterValue("tightness", tight);
    synthManagerSnare.voice()->setInternalParameterValue("cutoff", tone_s);

    ImGui::Text("HiHat Parameters");
    amp_h = synthManagerHiHat.voice()->getInternalParameterValue("amplitude");
    tone_h = synthManagerHiHat.voice()->getInternalParameterValue("cutoff");
    ImGui::SliderInt("Tone", &tone_h, 5000, 20000);
    ImGui::SliderFloat("Volume_H", &amp_h, 0.0, 1.0);
    synthManagerHiHat.voice()->setInternalParameterValue("amplitude", amp_h);
    synthManagerHiHat.voice()->setInternalParameterValue("cutoff", float(tone_h));

    ImGui::End();

    imguiEndFrame();
}

void MyApp::onDraw(Graphics &g) {
    g.clear();
    // Render the synth's graphics
    synthManagerKick.render(g);
    synthManagerHiHat.render(g);
    synthManagerSnare.render(g);

    // GUI is drawn here
    imguiDraw();

    if(!synthManagerKick.synthSequencer().playing() && playLoop == true ){ // keeps looping if playLoop is on
        std::string sequenceName = "sound_k" + std::to_string(sequenceFileNum-1); //loop prev file 
        std::cout<<"play sequence: " << sequenceName <<std::endl;
        synthManagerKick.synthSequencer().playSequence(sequenceName + ".synthSequence");
    }
    if(!synthManagerHiHat.synthSequencer().playing() && playLoop == true ){ // keeps looping if playLoop is on
        std::string sequenceName = "sound_h" + std::to_string(sequenceFileNum-1); //loop prev file 
        std::cout<<"play sequence: " << sequenceName <<std::endl;
        synthManagerHiHat.synthSequencer().playSequence(sequenceName + ".synthSequence");
    }
}

bool MyApp::onKeyDown(Keyboard const& k) {
    if (ParameterGUI::usingKeyboard()) {  // Ignore keys if GUI is using
                                          // keyboard
      return true;
    }
    if(ParameterGUI::usingInput()) {
      return true;
    }
    if( k.key() == ' ' && recordLoop == false ){ //start recording loop
          //set recordLoop to true
          recordLoop = true;
          
          std::string sequenceName_k = "sound_k" + std::to_string(sequenceFileNum);
          std::cout<<"start recording in file: sequenceName " << sequenceName_k <<std::endl;
          synthManagerKick.synthRecorder().startRecord(sequenceName_k, true);

          std::string sequenceName_h = "sound_h" + std::to_string(sequenceFileNum);
          std::cout<<"start recording in file: sequenceName " << sequenceName_k <<std::endl;
          synthManagerHiHat.synthRecorder().startRecord(sequenceName_h, true);
          
     }
      else if( k.key() == ' ' && recordLoop == true ){ //start playing recorded loop on repeat with sequencer 
          std::cout<<"stop recording " <<std::endl;
          synthManagerKick.synthRecorder().stopRecord();
          synthManagerHiHat.synthRecorder().startRecord();
          
          playLoop = true;
          recordLoop = false;
          std::string sequenceName = "sound" + std::to_string(sequenceFileNum);
          std::cout<<"play sequence: " << sequenceName <<std::endl;
          sequenceFileNum++;
          
    }
    /*else if (k.isNumber()) {
        if(k.key() == '1') {
            synthManagerKick.voice();
            synthManagerKick.triggerOn(k.key());
        }
        if(k.key() == '5') {
          synthManagerHiHat.voice()->setInternalParameterValue("decay", 0.05);
          synthManagerHiHat.triggerOn(k.key());
        }
        if(k.key() == '6') {
          synthManagerHiHat.voice()->setInternalParameterValue("decay", 1.8);
          synthManagerHiHat.triggerOn(k.key());
        }
        if(k.key() == '3') {
          synthManagerSnare.voice();
          synthManagerSnare.triggerOn(k.key());
        }
    }*/
    else {

      // Otherwise trigger note for polyphonic synth
      int midiNote = asciiToMIDI(k.key());
      if (midiNote > 0) {
        synthManagerKick.voice();
        synthManagerKick.triggerOn(midiNote);
      }
    }
    return true;
}

bool MyApp::onKeyUp(Keyboard const& k) {
    int midiNote = asciiToMIDI(k.key());
    if (midiNote > 0) {
      synthManagerKick.triggerOff(midiNote);
    }
    return true;
  }

void MyApp::onExit() { imguiShutdown(); } 