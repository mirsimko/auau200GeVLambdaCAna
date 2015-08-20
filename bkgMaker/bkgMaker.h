#ifndef _BKG_MAKER_H_
#define _BKG_MAKER_H_


#include "TH1D.h"
#include "TCut.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TString.h"
#include "TGraphErrors.h"

class bkgMaker
{
public:
  enum DecayMode{kKstar, kLambda, kDelta, kThreeBody};
  enum Analysis{kAll, kDCA, kPt, kDLengthCosTheta, kResM};

protected:
  // members
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

  TGraphErrors *DCA[3];
  TGraphErrors *dLength;;
  TGraphErrors *cosTheta;
  TGraphErrors *pt[3];
  TGraphErrors *resM;

  double dcaCut[20];
  double ptCut[20];
  double dLengthCut[20];
  double cosThetaCut[20];
  double resMcut[40];

  TFile* simFile;
  TFile* bkgFile;

  TNtuple* simTuple;
  TNtuple* bkgTuple;

  TFile* outFile;

  DecayMode decayMode;
  Analysis anaMode;


  TCut baseCut;
  TCut bkgCut;

  static const float KstarMass = 0.89610;
  static const float LambdaMass = 1.51950;
  static const float DeltaPPMass = 1.231;

  float resMass;

  TString outFileBaseName;

  Long64_t nEntries;
  // protected procedures
  void initHists();

  void initDCAhists();
  void initPtHists();
  void initDlengthCosThists();
  void initRMhists();

  void fillDCA();
  void fillPt();
  void fillDLcosT();
  void fillRM();

  void calcDCA();
  void calcPt();
  void calcDLcosT();
  void calcRM();
public:
  bkgMaker(int analysisMode = 0, int mDecayMode = 3, TFile* mSimFile = 0, TFile* mBkgFile = 0, TFile* mOutFile = 0, TCut mBaseCut = "", const char* mOutFileBaseName = "LambdaCratios", Long64_t nentries = 1000000000);
  
  ~bkgMaker();

  void SetSimFile(TFile* mSimFile){simFile = mSimFile; simTuple = (TNtuple*)mSimFile->Get("nt");}
  void SetBkgFile(TFile* mBkgFile){bkgFile = mBkgFile; bkgTuple = (TNtuple*)mBkgFile->Get("secondary");}
  void SetOutFile(TFile* mOutFile){outFile = mOutFile;}
  void SetOutFileBasename(const char* name){outFileBaseName = name;}

  void SetBaseCut(TCut mBaseCut){baseCut = mBaseCut;}

  void fillHistos();
  void calculateRatios();

  void Write();
  void Plot(bool saveIt = 0);
};

#endif
