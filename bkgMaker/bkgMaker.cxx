#include "TMath.h"
#include "bkgMaker.h"
#include <iostream>
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TString.h"
using namespace std;

class bkgMaker;

// -----------------------------------------------------------
bkgMaker::bkgMaker(int mDecayMode, TFile* mSimFile, TFile* mBkgFile, TFile* mOutFile, TCut mBaseCut, const char* mOutFileBaseName, Long64_t nentries)
{
  nEntries = nentries;
  decayMode = (DecayMode)mDecayMode;
  bkgCut = "charges < 0";
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

  if(mOutFileBaseName)
    SetOutFileBasename(mOutFileBaseName);
  if(mBkgFile && mBkgFile->IsOpen())
    SetBkgFile(mBkgFile);
  if(mSimFile && mSimFile->IsOpen())
    SetSimFile(mSimFile);
  if (mOutFile && mOutFile->IsOpen())
    SetOutFile(mOutFile);
  if (mBaseCut)
    SetBaseCut(mBaseCut);

  initHists();
}
// -----------------------------------------------------------
bkgMaker::~bkgMaker()
{
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 20; ++j)
    {
      delete DCAhists[i][j];
      delete DCAbkg[i][j];

      delete ptHists[i][i];
      delete ptBkg[i][j];
    }
  }
  for (int j = 0; j < 20; ++j)
  {
    delete dLengthHists[j];
    delete dLengthBkg[j];

    delete cosThetaHists[j];
    delete cosThetaBkg[j];
  }
  for (int j = 0; j < 40; ++j)
  {
    delete resHists[j];
    delete resBkg[j];
  }
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
	dcaParticles = "#pi p";
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
    cout << "filling DCA " << dcaParticles << " histograms" << endl;

    for (int j = 0; j < 20; j++)
    {
      dcaCut[j] = j*10 + 10;
      DCAhists[i][j] = new TH1D(Form("DCAsim%s_%d", dcaNParticles.Data(), j),
	  Form("Sim. #Lambda_{c} p_{T} with DCA %s < %d #mu^{}m", dcaParticles.Data(), (int) dcaCut[j]),
	  100, 0, 13);
      DCAhists[i][j]->Sumw2();

      DCAbkg[i][j] = new TH1D(Form("DCAbkg%s_%d", dcaNParticles.Data(), j),
	  Form("Bkg. p_{T} with DCA %s < %d #mu^{}m", dcaParticles.Data(), (int) dcaCut[j]),
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
      ptCut[j] = 0.05*j + 0.3;

      ptHists[i][j] = new TH1D(Form("ptSim%d_%d", i, j),
	  Form("Sim. #Lambda_{c} with %s p_{T} > %f GeV", particle.Data(), ptCut[j]),
	  100, 0, 13);
      ptHists[i][j]->Sumw2();

      ptBkg[i][j] = new TH1D(Form("ptBkg%d_%d", i, j),
	  Form("Bkg. with %s p_{T} > %f GeV", particle.Data(), ptCut[j]),
	  100, 0, 13);
      ptBkg[i][j]->Sumw2();
    } // for(int j = 0; j < 20; j++)
  } // for (int i = 0; i < 3; i++)

  // create decay length and cos(Theta) histograms
  for (int j = 0; j < 20; j++)
  {
    dLengthCut[j] = 5*j + 30;
    cosThetaCut[j] = 0.98 + 0.001*j;

    dLengthHists[j] = new TH1D(Form("dLengthSim%d", j),
	Form("Sim. #Lambda_{c} p_{T} with decay length > %d #mu^{}m", (int)dLengthCut[j]),
	100, 0, 13);
    dLengthHists[j]->Sumw2();

    cosThetaHists[j] = new TH1D(Form("cosThetaSim%d", j),
	Form("Sim. #Lambda_{c} p_{T} with cos(#theta) > %f", cosThetaCut[j]),
	100, 0, 13);
    cosThetaHists[j]->Sumw2();

    dLengthBkg[j] = new TH1D(Form("dLengthBkg%d", j),
	Form("Bkg. p_{T} with decay length > %d #mu^{}m", (int)dLengthCut[j]),
	100, 0, 13);
    dLengthBkg[j]->Sumw2();

    cosThetaBkg[j] = new TH1D(Form("cosThetaBkg%d", j),
	Form("Bkg. p_{T} with cos(#theta) > %f", cosThetaCut[j]),
	100, 0, 13);
    cosThetaBkg[j]->Sumw2();
  } // for (int j = 0; j < 20; j++)

  // create resonance mass histograms 
  for (int j = 0; j < 40; j++)
  {
    resMcut[j] = 0.005*j + 0.01;
    resHists[j] = new TH1D(Form("resMsim%d", j),
	Form("Sim. #Lambda_{c} p_{T} with %f < resonance mass < %f", resMass - resMcut[j], resMass + resMcut[j]),
	100, 0, 13);
    resHists[j]->Sumw2();

    resBkg[j] = new TH1D(Form("resMbkg%d", j),
	Form("Bkg. p_{T} with %f < resonance mass < %f", resMass - resMcut[j], resMass + resMcut[j]),
	100, 0, 13);
    resBkg[j]->Sumw2();
  } // for (int j = 0; j < 40; j++)
} // void bkgMaker::initHists()

// -----------------------------------------------------------
void bkgMaker::fillHistos()
{
  // filling DCA histograms
  cout << "Filling DCA histograms" << endl;
  TString DCAparticleName[3] = {"12", "23", "13"};
  TString DCApartBkgName[3] = {"31", "23", "12"};
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 20; j++)
    {
      TCut DCAsimCut = Form("dca%s < %d", DCAparticleName[i].Data(), (int)dcaCut[j]);
      TCut DCAbkgCut = Form("dcaDaugthers%s < %d", DCApartBkgName[i].Data(), (int)dcaCut[j]);

      simTuple->Project(Form("DCAsim%s_%d", DCAparticleName[i].Data(), j),
	  "rPt",
	  baseCut && DCAsimCut, "", nEntries);


      bkgTuple->Project(Form("DCAbkg%s_%d", DCAparticleName[i].Data(), j),
	  "pt",
	  bkgCut && DCAbkgCut, "", nEntries);
    }
  }

  // filling pT histograms
  cout << "*******************************************" << endl;
  cout << "Filling pt histograms" << endl;
  TString PtParticleSimName[3] = {"k", "pi", "p"};
  int PtParticleBkgName[3] = {1, 3, 2};
  for(int i = 0; i < 3; i++)
  {
    cout << "Filling " << PtParticleSimName << " histograms" << endl;
    for(int j =0; j < 20; j++)
    {
      TCut PtSimCut = Form("%sRPti > %f", PtParticleSimName[i].Data(), ptCut[j]);
      TCut PtBkgCut = Form("p%dpt > %f", PtParticleBkgName[i], ptCut[j]);

      simTuple->Project(Form("ptSim%d_%d", i, j),
	  "rPt",
	  baseCut && PtSimCut, "", nEntries);

      bkgTuple->Project(Form("ptBkg%d_%d", i, j),
	  "pt",
	  bkgCut && PtBkgCut, "", nEntries);
    }
  }

  // filling decay length and cos(theta) histograms
  cout << "*******************************************" << endl;
  cout << "Filling decay length and cos(theta) histograms" << endl;
  for(int j = 0; j < 20; j++)
  {
    TCut dLsimCut = Form("decayLength > %d", (int)dLengthCut[j]);
    TCut dLbkgCut = Form("dLength > %f", 0.001*dLengthCut[j]);

    TCut cosTsimCut = Form("cosTheta > %f", cosThetaCut[j]);
    TCut cosTbkgCut = Form("cosThetaStar > %f", cosThetaCut[j]);

    simTuple->Project(Form("dLengthSim%d", j),
	"rPt",
	baseCut && dLsimCut, "", nEntries);
    bkgTuple->Project(Form("dLengthBkg%d", j),
	"pt",
	bkgCut && dLbkgCut, "", nEntries);

    simTuple->Project(Form("cosThetaSim%d", j),
	"rPt",
	baseCut && cosTsimCut, "", nEntries);
    bkgTuple->Project(Form("cosThetaBkg%d", j),
	"pt",
	bkgCut && cosTbkgCut, "", nEntries);
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
    cout << "*******************************************" << endl;
    cout << "filling resonance mass histograms" << endl;
    for (int j =0; j < 40; j++)
    {
      TCut resMcutSim = Form("MRResonance > %f && %f > MRResonance",  resMass - resMcut[j], resMass + resMcut[j]);
      TCut resMcutBkg = Form("%s > %f && %f > %s", resName.Data(), resMass - resMcut[j], resMass + resMcut[j], resName.Data());

      simTuple->Project(Form("resMsim%d",j),
	  "rPt",
	  baseCut && resMcutSim, "", nEntries);
      bkgTuple->Project(Form("resMbkg%d",j),
	  "pt",
	  bkgCut && resMcutBkg, "", nEntries);
    } // for (int j =0; j < 40; j++)
  } // if (decayMode != kThreeBody)

  cout << "*******************************************" << endl;
} // void bkgMaker::fillHistos()

// --------------------------------------------------------
void bkgMaker::calculateRatios()
{
  fillHistos();

  double ratio[3][20];
  double err[3][20];
  TString particleSpecies[3] = {"K #pi", "#pi p", "K p"};
  TString particleNames[3] = {"Kpi", "piP", "Kp"};
  for(int i =0; i < 3; i++)
  {
    for (int j =0; j < 20; j++)
    {
      double errSim;
      double errBkg;
      double intgSim = DCAhists[i][j]->IntegralAndError(1,100, errSim);
      double intgBkg = DCAbkg[i][j]->IntegralAndError(1,100, errBkg);

      ratio[i][j] = intgSim/intgBkg;
      err[i][j] = ratio[i][j]*TMath::Sqrt(errSim*errSim/(intgSim*intgSim) + errBkg*errBkg/(intgBkg*intgBkg));
    }
    DCA[i] = new TGraphErrors(20, dcaCut, ratio[i], 0, err[i]);
    DCA[i]->SetNameTitle(Form("DCA%s", particleNames[i].Data()), Form("DCA %s", particleSpecies[i].Data()));
  }

  double ratioPt[3][20];
  double errPt[3][20];
  TString particleDefinitions[3] = {"K", "pi", "p"};
  for (int i =0; i < 3; i++)
  {
    for (int j =0; j < 20; ++j)
    {
      double errSim;
      double errBkg;
      double intgSim = ptHists[i][j]->IntegralAndError(1,100, errSim);
      double intgBkg = ptBkg[i][j]->IntegralAndError(1,100, errBkg);

      ratioPt[i][j] = intgSim/intgBkg;
      errPt[i][j] = ratioPt[i][j]*TMath::Sqrt(errSim*errSim/(intgSim*intgSim) + errBkg*errBkg/(intgBkg*intgBkg));
    }
    pt[i] = new TGraphErrors(20, ptCut, ratioPt[i], 0, errPt[i]);
    pt[i]->SetNameTitle(Form("%sPt",particleDefinitions[i].Data()), Form("%s p_{T}",particleDefinitions[i].Data()));
  }

  double ratioDL[20];
  double errDL[20];
  double ratioCosT[20];
  double errCosT[20];
  for(int j = 0; j < 20; ++j)
  {
    double errSim;
    double errBkg;
    double intgSim = dLengthHists[j]->IntegralAndError(1,100, errSim);
    double intgBkg = dLengthBkg[j]->IntegralAndError(1, 100, errBkg);

    ratioDL[j] = intgSim/intgBkg;
    errDL[j] = ratioDL[j]*TMath::Sqrt(errSim*errSim/(intgSim*intgSim) + errBkg*errBkg/(intgBkg*intgBkg));

    intgSim = cosThetaHists[j]->IntegralAndError(1,100, errSim);
    intgBkg = cosThetaBkg[j]->IntegralAndError(1,100, errBkg);

    ratioCosT[j] = intgSim/intgBkg;
    errCosT[j] = ratioCosT[j]*TMath::Sqrt(errSim*errSim/(intgSim*intgSim) + errBkg*errBkg/(intgBkg*intgBkg));
  }
  dLength = new TGraphErrors(20, dLengthCut, ratioDL, 0, errDL);
  cosTheta = new TGraphErrors(20, cosThetaCut, ratioCosT, 0, errCosT);
  dLength->SetNameTitle("dLength", "decay length cut");
  cosTheta->SetNameTitle("cosTheta", "cos(#theta*)");

  double ratioRM[40];
  double errRM[40];
  for (int j = 0; j < 40; ++j)
  {
    double errSim;
    double errBkg;
    double intgSim = resHists[j]->IntegralAndError(1,100,errSim);
    double intgBkg = resBkg[j]->IntegralAndError(1,100,errBkg);

    ratioRM[j] = intgSim/intgBkg;
    errRM[j] = ratioRM[j]*TMath::Sqrt(errSim*errSim/(intgSim*intgSim) + errBkg*errBkg/(intgBkg*intgBkg));
  }
  resM = new TGraphErrors(40, resMcut, ratioRM, 0, errRM);
  resM->SetNameTitle("resM", "cut on resonance mass");
}

// --------------------------------------------------------
void bkgMaker::Plot(bool saveIt)
{
  TCanvas *C1 = new TCanvas("C1", "", 1200, 900);
  C1->Divide(2,2);
  for(int i = 0; i < 3; i++)
  {
    C1->cd(i+1);
    DCA[i]->Draw();
  }
  C1->cd(4);
  dLength->Draw();
  TCanvas *C2 = new TCanvas("C2", "", 1200, 900);
  C2->cd(1);
  cosTheta->Draw();
  for (int i = 0; i < 3; i++)
  {
    C2->cd(i+2);
    pt[i]->Draw();
  }
  TCanvas *C3 = new TCanvas("C3", "", 1200, 900);
  C3->cd();
  resM->Draw();

  if(saveIt)
  {
    C1->SaveAs(Form("%s_1.pdf", outFileBaseName.Data()));
    C2->SaveAs(Form("%s_2.pdf", outFileBaseName.Data()));
    C3->SaveAs(Form("%s_3.pdf", outFileBaseName.Data()));

    C1->SaveAs(Form("%s_1.png", outFileBaseName.Data()));
    C2->SaveAs(Form("%s_2.png", outFileBaseName.Data()));
    C3->SaveAs(Form("%s_3.png", outFileBaseName.Data()));
  }
}

// --------------------------------------------------------
void bkgMaker::Write()
{
  outFile->cd();

  dLength->Write();
  cosTheta->Write();
  resM->Write();
  for (int i = 0; i < 3; ++i)
  {
    DCA[i]->Write();
    pt[i]->Write();
    for (int j = 0; j < 20; ++j)
    {
      DCAhists[i][j]->Write();
      ptHists[i][j]->Write();

      DCAhists[i][j]->Write();
      ptHists[i][j]->Write();
    }
  }
  for (int j =0; j < 20; ++j)
  {
    dLengthHists[j]->Write();
    dLengthBkg[j]->Write();

    cosThetaHists[j]->Write();
    cosThetaBkg[j]->Write();
  }
  
  for (int j =0; j < 40; ++j)
  {
    resHists[j]->Write();
    resBkg[j]->Write();
  }
}
