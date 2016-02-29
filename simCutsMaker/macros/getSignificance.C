#include "TLine.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include <vector>
#include "TNtuple.h"
#include <iostream>
#include <fstream>
#include "TMath.h"
#include "TH1.h"
#include <cmath>
#include <climits>
#include "TString.h"
#include "TF1.h"

#include "simCutsMaker/cutsConsts.h"
// using namespace firstIter;
// using namespace secondIter;
using namespace thirdIter;
// using namespace fourthIter;

using namespace std;

inline int indexInArray(int index[]) ;

const float bkgRatio = 0.0918578;      // background triplets in the Lambda_C peak window
const float simScale = 0.001;          // 1000 times more LC were produced
const float bkgScale = bkgRatio*1./3.; // third because there are 3 times more bkg combinations than LC combinations

const double AllBkg = 900215583*bkgScale;
const double AllSim = 105791*simScale;

const int Nplots = 5*5*5*5*5*5*5;

struct significanceVariables
{
  double significance[Nplots];
  double bkgRejection[Nplots];
  double simRejection[Nplots];
  double ratio[Nplots];
};

void setCutsFromIndex(int const *index, float *cuts)
{
  // cut constants are in cutsConsts.h
  // namespace has to be chosen depending on the iteration number
  // ***********************************************
  float MdLength = DLstart + DLinc* index[0];
  float MdcaDaughters = dcaDaughtersStart - dcaDaughtersInc*index[1];
  float MmaxVdist = maxVdistStart - maxVdistInc*index[2];
  float MpPt = pPtStart + pPtInc*index[3];
  float MpiPt = piPtStart + piPtInc*index[4];
  float MkPt = KPtStart + KPtInc*index[5];
  float McosTheta = cosThetaStart + cosThetaInc*index[6];
  // ***********************************************

  unsigned int iArr = 0;                                  
  cuts[iArr++]=MdLength;                                  
  cuts[iArr++]=MdcaDaughters;                             
  cuts[iArr++]=MmaxVdist;                                 
  cuts[iArr++]=MpPt;                                      
  cuts[iArr++]=MpiPt;                                     
  cuts[iArr++]=MkPt;                                      
  cuts[iArr++]=McosTheta;    
}


void getSignificance()
{
  significanceVariables s;

  TFile *simF = new TFile("simCutsPlots3rdIter.root");
  TFile *bkgF = new TFile("bkgCutsPlots3rdIter.root");

  TString iter = "3rd iteration";
  TString outfileName = "signiTable3rdIter.txt";
  // changging output of cout to outf
  ofstream outf(outfileName);
  cout << "Writing significance table into \"" << outfileName << "\"" << endl;
  streambuf *oldBuf = cout.rdbuf(outf.rdbuf());

  // head
  cout << "************************************************" << endl;
  cout << "ii\tjj\tkk\tll\tmm\tnn\too\tdLength\tdcaD\tVdist\tpPt\tpiPt\tkPt\tcos(t)\tsignificance\tnSim\tnBKG\tratio" << endl;
  cout << endl;
  
  // max significance variables
  float max = 0;
  float maxRatio = 0;
  float maxSimCounts = 0;
  float maxBkgCounts = 0;
  float maxCuts[7] = {0,0,0,0,0,0};
  float maxIdx[7] = {0,0,0,0,0,0};

  float maximumRatio = 0;

  for (int ii = 0; ii < 5; ++ii)
  {
    for (int jj = 0; jj < 5; ++jj)
    {
      for (int kk = 0; kk < 5; ++kk)
      {
	for(int ll = 0; ll < 5; ++ll)
	{
	  for(int mm = 0; mm < 5; ++mm)
	  {
	    for(int nn = 0; nn < 5; ++nn)
	    {
	      for(int oo = 0; oo < 5; ++oo)
	      {
		float cuts[7];
		int index[7] = {ii,jj,kk,ll,mm,nn,oo};

		setCutsFromIndex(index, cuts);

		TH1D* simHist = (TH1D*) simF->Get(Form("H%d%d%d%d%d%d%d", ii, jj, kk, ll, mm, nn, oo));
		TH1D* bkgHist = (TH1D*) bkgF->Get(Form("H%d%d%d%d%d%d%d", ii, jj, kk, ll, mm, nn, oo));
		if(!(simHist && bkgHist))
		{
		  cerr << "histogram not found" << endl;
		  throw;
		}

		float simCounts = simHist->Integral()*simScale;
		float bkgCounts = bkgHist->Integral()*bkgScale;

		float significance = simCounts/TMath::Sqrt(simCounts + bkgCounts);
		float ratio = simCounts/bkgCounts;

		// printing
		cout << ii << "\t";
		cout << jj << "\t";
		cout << kk << "\t";
		cout << ll << "\t";
		cout << mm << "\t";
		cout << nn << "\t";
		cout << oo << "\t";

		cout << cuts[0] << "\t";
		cout << cuts[1] << "\t";
		cout << cuts[2] << "\t";
		cout << cuts[3] << "\t";
		cout << cuts[4] << "\t";
		cout << cuts[5] << "\t";
		cout << cuts[6] << "\t";

		int ind = indexInArray(index);

		s.significance[ind]= significance;
		s.ratio[ind] = ratio;
		if(simCounts)
		  s.simRejection[ind] = AllSim/simCounts;
		else
		  s.simRejection[ind] = std::numeric_limits<double>::quiet_NaN();

		if(bkgCounts)
		  s.bkgRejection[ind] = AllBkg/bkgCounts;
		else
		  s.bkgRejection[ind] = std::numeric_limits<double>::quiet_NaN();

		cout << significance << "\t";
		cout << simCounts << "\t";
		cout << bkgCounts << "\t";
		cout << ratio << endl;

		if ( ratio > maximumRatio && isnormal(ratio)) 
		  maximumRatio = ratio;

		if (significance > max && AllSim/simCounts < 2000) // get maximum ... 2000 is protection against overtraining
		{
		  max =  significance;
		  maxRatio = ratio;
		  maxSimCounts = simCounts;
		  maxBkgCounts = bkgCounts;
		  for (int i = 0; i < 7; ++i)
		  {
		    maxCuts[i] = cuts[i];
		    maxIdx[i] = index[i];
		  }
		}
		delete simHist;
		delete bkgHist;
	      }
	    }
	  }
	}
      }
    }
  }
  cout << "************************************************" << endl;

  cout << "Max significance" << endl;
  cout << "ii\tjj\tkk\tll\tmm\tnn\too\tdLength\tdcaD\tVdist\tpPt\tpiPt\tkPt\tcos(t)\tsignificance\tnSim\tnBKG\tratio" << endl;

  for (int i = 0; i < 7; ++i)
  {
    cout << maxIdx[i] << "\t";
  }
  for (int i = 0; i < 7; ++i)
  {
    cout << maxCuts[i] << "\t" ;
  }
  cout << max << "\t";
  cout << maxSimCounts << "\t";
  cout << maxBkgCounts << "\t";
  cout << maxRatio << endl;
  // changing cout back
  cout.rdbuf(oldBuf);

  TCanvas *C1 = new TCanvas("C1", "significance", 600,400);
  TCanvas *C2 = new TCanvas("C2", "BKG rejection", 600,400);
  TCanvas *C3 = new TCanvas("C3", "ratio", 600,400);

  TGraph *signiGraph = new TGraph(Nplots,s.simRejection,s.significance);
  TGraph *bkgRejectionGraph = new TGraph(Nplots,s.simRejection,s.bkgRejection);
  TGraph *ratioGraph = new TGraph(Nplots,s.simRejection,s.ratio);

  //setting the axes labels
  signiGraph->GetXaxis()->SetTitle("sig0/sig []"); //x
  bkgRejectionGraph->GetXaxis()->SetTitle("sig0/sig []");
  ratioGraph->GetXaxis()->SetTitle("sig0/sig []");

  signiGraph->GetYaxis()->SetTitle("s/#sqrt{s + b} []"); //y
  bkgRejectionGraph->GetYaxis()->SetTitle("bkg0/bkg []");
  ratioGraph->GetYaxis()->SetTitle("s/b []");

  // marker style
  signiGraph->SetMarkerStyle(2);
  bkgRejectionGraph->SetMarkerStyle(2);
  ratioGraph->SetMarkerStyle(2);
  signiGraph->SetMarkerColor(38);
  bkgRejectionGraph->SetMarkerColor(38);
  ratioGraph->SetMarkerColor(38);

  signiGraph->SetTitle(iter.Data());
  bkgRejectionGraph->SetTitle(iter.Data());
  ratioGraph->SetTitle(iter.Data());

  TF1 *unity = new TF1("unity", "x", 0, 10000000);
  for (int i = 0; i < Nplots; ++i)
  {
    if(!isnormal(s.simRejection[i]))
    {
      ratioGraph->RemovePoint(i);
      signiGraph->RemovePoint(i);
      bkgRejectionGraph->RemovePoint(i);
    }
    else
    {
      if(!isnormal(s.significance[i]))
	signiGraph->RemovePoint(i);
      if(!isnormal(s.bkgRejection[i]))
	bkgRejectionGraph->RemovePoint(i);
      if(!isnormal(s.ratio[i]))
	ratioGraph->RemovePoint(i);
    }
  }

  C1->cd();
  C1->SetLogx();
  signiGraph->Draw("AP");

  TLine *l = new TLine(2000,0,2000,0.1);
  l->SetLineColor(kRed);
  l->Draw();;
  C2->cd();
  C2->SetLogx();
  C2->SetLogy();
  C2->SetLogx();
  bkgRejectionGraph->Draw("AP");
  unity->Draw("same");
  C3->cd();
  C3->SetLogx();
  ratioGraph->GetYaxis()->SetRangeUser(0.,maximumRatio*1.05);
  ratioGraph->Draw("AP");
}

inline int indexInArray(int index[]) 
{
    return index[6]+5*index[5]+25*index[4]+125*index[3]+625*index[2]+3125*index[1]+15625*index[0];
}

