#include "simCutsMaker/simCutsMaker.h"
#include <iostream>

using namespace std;

void runSimCutsMaker()
{
  // int in;
  // cin >> in;
  cout << "Initializing simCutsMaker ..." << endl;
  simCutsMaker mSimCutsMaker;
  cout << "Running on events ..." << endl;
  mSimCutsMaker.Loop();
  cout << "Finished" << endl;
  cout << "*******************************************" << endl;
}

