#include "bgSelector/bgSelector.h"
#include <iostream>
using namespace std;

void runBgSelector()
{
  TFile *inF = new TFile("/global/project/projectdirs/star/pwg/starhf/simkomir/LambdaC/picoHFLambdaCMaker.root");
  TNtuple *nt = (TNtuple*) inF->Get("secondary");
  cout << "initializing bgSelector ..." << endl;
  bgSelector T("bgSelectedLc.root", nt);
  cout << "Looping over events ..." << endl;
  T.Loop();
  cout << "Writing into file ..." << endl;
  T.Write();
}
