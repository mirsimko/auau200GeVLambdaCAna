#include <TFile.h>
#include <TNtuple.h>
#include <TH1D.h>
#include <iostream>
#include <TString.h>

using namespace std;

void bgRatio()
{
  TString dir = "/global/project/projectdirs/star/pwg/starhf/simkomir/LambdaC/";
  TString fileName = "bgSelectedAllStat.root";
  TFile *inf = new TFile( (dir + fileName).Data() );
  TNtuple *nt = inf->Get("secondarySelected");

  TH1D *mHist = new TH1D("mHist", "mass", 100, 2, 2.5);

  cout << "Reading data from " << fileName << endl;
  nt->Project("mHist", "m");

  double allLC = mHist->Integral();
  double selected = mHist->Integral(mHist->GetXaxis()->FindBin(2.266), mHist->GetXaxis()->FindBin(2.306) );
  double ratio = selected / allLC;

  cout << "Ratio = " << ratio << endl;
}
