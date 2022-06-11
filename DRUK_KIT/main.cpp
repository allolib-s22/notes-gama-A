#include <iostream>

// for master branch
// #include "al/core.hpp"

// for devel branch
#include "al/app/al_App.hpp"
#include "al/io/al_MIDI.hpp"
#include "al/graphics/al_Shapes.hpp"

#include "MyApp.hpp"

using namespace al;

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
