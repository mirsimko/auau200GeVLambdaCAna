#include "simCutsMaker/simCutsMaker.h"
#include <iostream>
#include "TNtuple.h"
#include "TTree.h"
#include "TFile.h"
#include <climits>
#include <iostream>

using namespace std;

void runSimCutsMaker(Long64_t first = 1, Long64_t last = LLONG_MAX)
{
  // int in;
  // cin >> in;
  cout << "Getting tree" << endl;
  // Obsolete:
  TFile *inf = new TFile("bgSelect1stIterProtons.root");  
  TNtuple *nt = (TNtuple*) inf->Get("secondarySelected");
  //
  // TFile *inf = new TFile("SimSelectedLcMassCut.root");
  // TFile *inf = new TFile("LC.toyMC.root");
  // TFile *inf = new TFile("bgSelected7thIter.root");
  // TNtuple *nt = (TNtuple*) inf->Get("ntTMVA");
  // TNtuple *nt = (TNtuple*) inf->Get("secondarySelected");
  cout << "Initializing simCutsMaker ..." << endl;
  // simCutsMaker mSimCutsMaker((TTree*) nt, Form("bgCutsPlots_%09lld_%09lld.root", first, last) );
  simCutsMaker mSimCutsMaker((TTree*) nt,"bkgCutsPlots3rdIter.root" );

  cout << "Running on events ..." << endl;
  mSimCutsMaker.Loop(first, last);
  cout << "Finished" << endl;
  cout << "*******************************************" << endl;
}

