#include "simCutsMaker/simCutsMaker.h"
#include <iostream>

using namespace std;

void runSimCutsmaker()
{
  cout << "Initializing simCutsMaker ..." << endl;
  simCutsMaker mSimCutsMaker;
  cout << "Running on events ..." << endl;
  mSimCutsMaker.Loop();
  cout << "Finished" << endl;
  cout << "*******************************************" << endl;
}

