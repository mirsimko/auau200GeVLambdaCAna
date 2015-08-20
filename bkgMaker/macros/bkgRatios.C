#include "TNtuple.h"
#include "TFile.h"
#include "TH1.h"
#include "TCut.h"
#include "TString.h"
#include "bkgMaker/bkgMaker.h"
#include "bkgMaker/bkgMaker.cxx"
#include "TSystem.h"
#include <iostream>

using namespace std;

// extern TSystem *gSystem;
// 
// gSystem->Load("bkgMaker/bkgMaker.cxx++g");
class bkgMaker;

// ------------------------------------------------
void bkgRatios(int analysisMode=0, int decayMode=0)
{
  // declare variables
  enum DecayMode{kKstar, kLambda, kDelta, kThreeBody};

  DecayMode dm = (DecayMode)decayMode;

  TString outFileName = "";
  TString decayName = "";
  // cuts
  TCut PtCut = "kRPt > 0.3 && pRPt > 0.3 && piRPt > 0.3";
  TCut dcaCut = "dca12 < 200 && dca23 < 200 && dca13 < 200";
  TCut dLengthCut = "decayLength > 30";
  TCut cosThetaCut = "cosTheta > 0.98";
  TCut HftCut = "pHft > 0.5 && kHft > 0.5 && piHft > 0.5";
  TCut MassCut = "rM > 2. && rM < 2.5";
  TCut EtaCut = "TMath::Abs(pREta) < 1. && TMath::Abs(piREta) < 1. && TMath::Abs(kREta) < 1.";
  TCut AllCuts = PtCut && dcaCut && dLengthCut && cosThetaCut && HftCut && MassCut && EtaCut;

  // getting the Ntuple and setting variables for all decay modes
  TFile *readF1;
  switch(dm)
  {
    case kKstar:
      readF1 = new TFile("lambdaCkKstarProton.root");
      outFileName = "lambdaCRatioskKstarProton";
      decayName = "K* p";
      break;
    case kLambda:
      readF1 = new TFile("lambdaCkLambda1520Pion.root");
      outFileName = "lambdaCRatioskLambda1520Pion";
      decayName = "#Lambda(1520) #pi";
      break;
    case kDelta:
      readF1 = new TFile("lambdaCkDeltaPPkaon.root");
      outFileName = "lambdaCRatioskDeltaPPkaon";
      decayName = "#Delta^{++} K";
      break;
    case kThreeBody:
      readF1 = new TFile("lambdaCkPionKaonProton.root");
      outFileName = "lambdaCRatioskPionKaonProton";
      decayName = "#pi K p";
      break;
    default:
      cerr << "Wrong decay mode" << endl;
      return;
      break;
  }
  TFile *readF2 = new TFile("../kPionKaonProton.root");
  TFile *outFile = new TFile(Form("%s.root", outFileName.Data()), "RECREATE");
  
  cout << "initiating bkgMaker" << endl;
  bkgMaker mBkgMaker(analysisMode, decayMode,readF1, readF2, outFile, AllCuts, outFileName.Data());

  cout << "Calculating ratios" << endl;
  mBkgMaker.calculateRatios();
  cout << "Done" << endl;
  cout << "Plotting and saving" << endl;
  if(analysisMode == 0)
    mBkgMaker.Plot(true);

  mBkgMaker.Write();
  outFile->Close();
  readF1->Close();
  readF2->Close();
  cout << "********************************************" << endl;
  cout << "Finished" << endl;

}
