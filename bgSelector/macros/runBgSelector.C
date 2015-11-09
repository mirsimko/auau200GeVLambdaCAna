#include "bgSelector/bgSelector.h"
#include <iostream>
#include <TFile.h>
#include <TNtuple.h>

using namespace std;

void runBgSelector(char *fileName = "lambdaCTMVAkPionKaonProtonCuts.root", char *NtupleName = "ntTMVA")
{
  // TFile *inF = new TFile("/global/project/projectdirs/star/pwg/starhf/simkomir/LambdaC/picoHFLambdaCMaker.root");
  // TNtuple *nt = (TNtuple*) inF->Get("secondary");
  TFile *inF = new TFile(fileName);
  TNtuple *nt = (TNtuple*) inF->Get(NtupleName);
  cout << "initializing bgSelector ..." << endl;
  bgSelector T("SimSelectedLcMassCut.root", nt);
  cout << "Looping over events ..." << endl;
  T.Loop();
  cout << "Writing into file ..." << endl;
  T.Write();
}
