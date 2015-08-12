#include "bkgMaker.h"
#include <iostream>
using namespace std;

class bkgMaker;

// -----------------------------------------------------------
bkgMaker::bkgMaker(int mDecayMode)
{
  decayMode = (DecayMode)mDecayMode;

  switch ( decayMode )
  {
    case kKstar:
      resMass = KstarMass;
      break;
    case kLambda:
      resMass = LambdaMass;
      break;
    case kDelta:
      resMass = DeltaPPMass;
      break;
    case kThreeBody:
      resMass = 0;
      break;
    default:
      cerr << "bkgMaker::bkgMaker: unknown decay mode." << endl;
      throw;
      break;
  }

  initHists();
}

// -----------------------------------------------------------
bkgMaker::bkgMaker(TFile* mSimFile, TFile* mBkgFile, TFile* mOutFile, TCut mBaseCut, int mDecayMode)
{
  bkgMaker(mDecayMode);

  SetBkgFile(mBkgFile);
  SetSimFile(mSimFile);
  SetOutFile(mOutFile);
  SetBaseCut(mBaseCut);
}

// -----------------------------------------------------------
void bkgMaker::initHists()
{
  // create DCA histograms
  for (int i = 0; i < 3; i++)
  {
    TString dcaNParticles;
    TString dcaParticles;
    switch (i)
    {
      case 0:
	dcaNParticles = "12";
	dcaParticles = "K #pi";
	break;
      case 1:
	dcaNParticles = "23";
	dcaParticles = "#pi p"
	break;
      case 2:
	dcaNParticles = "13";
	dcaParticles = "K p";
	break;
      default:
	cerr << "wierd" << endl;
	throw;
	break;
    } // switch (i)

    for (int j = 0; j < 20; j++)
    {
      DCAhists[i][j] = new TH1D(Form("DCAsim%s_%d", dcaNParticles.Data(), j),
	  Form("Sim. #Lambda_{c} p_{T} with DCA %s < %d #mu^{}m", dcaParticles.Data(), j*10 + 10),
	  100, 0, 13);
      DCAhists[i][j]->Sumw2();

      DCAbkg[i][j] = new TH1D(Form("DCAbkg%s_%d", dcaNParticles.Data(), j),
	  Form("Bkg. p_{T} with DCA %s < %d #mu^{}m", dcaParticles.Data(), j*10 + 10),
	  100, 0, 13);
      DCAbkg[i][j]->Sumw2();
    } //for (int j = 0; j < 20; j++) 
  } //for (int i = 0; i < 3; i++) 

  // create pt histograms
  for (int i = 0; i < 3; i++)
  {
    TString particle;

    switch(i)
    {
      case 0:
	particle = "K";
	break;
      case 1:
	particle = "#pi";
	break;
      case 2:
	particle = "p";
	break;
      default:
	cerr << "WTF?" << endl;
	throw;
	break;
    } // switch(i)

    for(int j = 0; j < 20; j++)
    {
      ptHists[i][j] = new TH1D(Form("ptSim%d_%d", i, j),
	  Form("Sim. #Lambda_{c} with %s p_{T} > %f GeV", particle.Data(), 0.05*j + 0.3),
	  100, 0, 13);
      ptHists[i][j]->Sumw2();

      ptBkg[i][j] = new TH1D(Form("ptBkg%d_%d", i, j),
	  Form("Bkg. with %s p_{T} > %f GeV", particle.Data(), 0.05*j + 0.3),
	  100, 0, 13);
      ptBkg[i][j]->Sumw2();
    } // for(int j = 0; j < 20; j++)
  } // for (int i = 0; i < 3; i++)

  // create decay length and cos(Theta) histograms
  for (int j = 0; j < 20; j++)
  {
    dLengthHists[j] = new TH1D(Form("dLengthSim%d", j),
	Form("Sim. #Lambda_{c} p_{T} with decay length > %d #mu^{}m", 5*j + 30),
	100, 0, 13);
    dLengthHists[j]->Sumw2();

    cosThetaHists[j] = new TH1D(Form("cosThetaSim%d", j),
	Form("Sim. #Lambda_{c} p_{T} with cos(#theta) > %f", 0.98 + 0.001*j),
	100, 0, 13);
    cosThetaHists[j]->Sumw2();

    dLengthBkg[j] = new TH1D(Form("dLengthBkg%d", j),
	Form("Bkg. p_{T} with decay length > %d #mu^{}m", 5*j + 30),
	100, 0, 13);
    dLengthBkg[j]->Sumw2();

    cosThetaBkg[j] = new TH1D(Form("cosThetaBkg%d", j),
	Form("Bkg. p_{T} with cos(#theta) > %f", 0.98 + 0.001*j),
	100, 0, 13);
    cosThetaBkg[j]->Sumw2();
  } // for (int j = 0; j < 20; j++)

  // create resonance mass histograms 
  for (int j = 0; j < 40; j++)
  {
    resHists[j] = new TH1D(Form("resMsim%d", j),
	Form("Sim. #Lambda_{c} p_{T} with %f < resonance mass < %f", resMass - 0.01*j - 0.01, resMass + 0.005*j + 0.01),
	100, 0, 13);
    resHists[j]->Sumw2();

    resBkg[j] = new TH1D(Form("resMbkg%d", j),
	Form("Bkg. p_{T} with %f < resonance mass < %f", resMass - 0.01*j - 0.01, resMass + 0.005*j + 0.01),
	100, 0, 13);
    resBkg[j]->Sumw2();
  } // for (int j = 0; j < 40; j++)
} // void bkgMaker::initHists()

// -----------------------------------------------------------
void bkgMaker::fillHistos()
{
  // filling DCA histograms
  TString DCAparticleName[3] = {"12", "23", "13"};
  TString DCApartBkgName[3] = {"31", "23", "12"};
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 20; j++)
    {
      TCut DCAsimCut = Form("dca%s < %d", DCAparticleName[i].Data(), j*10+10);
      TCut DCAbkgCut = Form("dcaDaughters%s < %d", DCApartBkgName[i].Data(), j*10+10);

      simTuple->Project(Form("DCAsim%s_%d", DCAparticleName[i].Data(), j),
	  "rPt",
	  baseCut && DCAsimCut);


      bkgTuple->Project(Form("DCAbkg%s_%d", DCAparticleName[i].Data(), j),
	  "pt",
	  bkgCut && DCAbkgCut);
    }
  }

  // filling pT histograms
  TString PtParticleSimName[3] = {"k", "pi", "p"};
  int PtParticleBkgName[3] = {1, 3, 2};
  for(int i = 0; i < 3; i++)
  {
    for(int j =0; j < 20; j++)
    {
      TCut PtSimCut = Form("%sRPti > %f", PtParticleSimName[i], 0.3 + 0.05*j);
      TCut PtBkgCut = Form("p%dpt > %f", PtParticleBkgName[i], 0.3 + 0.05*j);

      simTuple->Project(Form("ptSim%d_%d", i, j),
	  "rPt",
	  baseCut && PtSimCut);

      bkgTuple->Project(Form("ptBkg%d_%d", i, j),
	  "pt",
	  bkgCut && PtBkgCut);
    }
  }

  // filling decay length and cos(theta) histograms
  for(int j = 0; j < 20; j++)
  {
    TCut dLsimCut = Form("decayLength > %d", 30 + 5*j);
    TCut dLbkgCut = Form("dLength > %f", 0.03 + 0.005*j);

    TCut cosTsimCut = Form("cosTheta > %f", 0.98 + 0.001*j);
    TCut cosTbkgCut = Form("cosThetaStar > %f", 0.98 + 0.001*j);

    simTuple->Project(Form("dLengthSim%d", j),
	"rPt",
	baseCut && dLsimCut);
    bkgTuple->Project(Form("dLengthBkg%d", j),
	"pt",
	bkgCut && dLbkgCut);

    simTuple->Project(Form("cosThetaSim%d", j),
	"rPt",
	baseCut && cosTsimCut);
    bkgTuple->Project(Form("cosThetaBkg%d", j),
	"pt",
	bkgCut && cosTbkgCut);
  }

  // filling resonance mass histograms
  if (decayMode != kThreeBody)
  {
    TString resName;
    switch (decayMode)
    {
      case kKstar:
	resName = "mKstar";
	break;
      case kLambda:
	resName = "mLambda1520";
	break;
      case kDelta:
	resName = "mDelta";
	break;
      default:
	cerr << "Unknown decay mode" << endl;
	throw;
	break;
    }
    for (int j =0; j < 40; j++)
    {
      TCut resMcutSim = Form("MRResonance > %f && %f > MRResonance",  resMass - 0.01*j - 0.01, resMass + 0.005*j + 0.01);
      TCut resMcutBkg = Form("%s > %f && %f > %s", resName.Data());

      simTuple->Project(Form("resMsim%d",j),
	  "rPt",
	  baseCut && resMcutSim);
      bkgTuple->Project(Form("resMbkg%d",j),
	  "pt",
	  bkgCut && resMcutBkg);
    } // for (int j =0; j < 40; j++)
  } // if (decayMode != kThreeBody)

} // void bkgMaker::fillHistos()

// --------------------------------------------------------
void bkgMaker::calculateRatios()
{
  double integralDCAsim[3][20];
  double integralDCAbkg[3][20];
  double errDCAsim[3][20];
  double errDCAbkg[3][20];
  for(int i =0; i < 3; i++)
  {
    for (int j =0; j < 20; j++)
    {
      integralDCAsim[i][j] = DCAhists[i][j]->IntegralAndError(1,100,errDCAsim[i][j]);
      integralDCAbkg[i][j] = DCAbkgCut[i][j]->IntegralAndError(1,100,errDCAbkg[i][j]);

    }
  }

}
