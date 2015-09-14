#include "simCutsMaker/simCutsMaker.h"
#include <iostream>
#include "TNtuple.h"
#include "TTree.h"
#include "TFile.h"

using namespace std;

void runSimCutsMaker()
{
  // int in;
  // cin >> in;
  cout << "Getting tree" << endl;
  TFile *inf = new TFile("2015-08-28_16-27_472.picoHFLambdaCMaker.root");  
  TNtuple *nt = (TNtuple*) inf->Get("secondary");
  cout << "Initializing simCutsMaker ..." << endl;
  simCutsMaker mSimCutsMaker((TTree*) nt);
  cout << "Running on events ..." << endl;
  mSimCutsMaker.Loop();
  cout << "Finished" << endl;
  cout << "*******************************************" << endl;
}

