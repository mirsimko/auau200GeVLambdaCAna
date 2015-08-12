#ifndef _BKG_MAKER_H_
#define _BKG_MAKER_H_


#include "TH1D.h"
#include "TCut.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TString.h"

class bkgMaker
{
public:
  enum DecayMode{kKstar, kLambda, kDelta, kThreeBody};
  
protected:
  TH1D *DCAhists[3][20];
  TH1D *dLengthHists[20];
  TH1D *cosThetaHists[20];
  TH1D *ptHists[3][20];
  TH1D *resHists[40];

  TH1D *DCAbkg[3][20];
  TH1D *dLengthBkg[20];
  TH1D *cosThetaBkg[20];
  TH1D *ptBkg[3][20];
  TH1D *resBkg[40];

  TFile* simFile;
  TFile* bkgFile;

  TNtuple* simTuple;
  TNtuple* bkgTuple;

  TFile* outFile;

  DecayMode decayMode;

  void initHists();

  TCut baseCut;
  TCut bkgCut = "charges < 0"

  const float KstarMass = 0.89610;
  const float LambdaMass = 1.51950;
  const float DeltaPPMass = 1.231;

  float resMass;
public:
  bkgMaker(int mDecayMode);
  bkgMaker(TFile* mSimFile, TFile* mBkgFile, TFile* mOutFile, TCut mBaseCut, int mDecayMode);
  ~bkgMaker();

  void SetSimFile(TFile* mSimFile){simFile = mSimFile; simTuple = mSimFile->Get("nt");}
  void SetBkgFile(TFile* mBkgFile){bkgFile = mBkgFile; bkgTuple = mBkgFile->Get("secondary");}
  void SetOutFile(TFile* mOutFile){outFile = mOutFile;}

  void SetBaseCut(TCut mBaseCut){baseCut = mBaseCut;}

  void fillHistos();

  void calculateRatios();
};

#endif
