#include "TFile.h"
#include "TNtuple.h"
#include <iostream>
#include "TMath.h"
#include "TH1.h"

#include "simCutsMaker/cutsConsts.h"
// using namespace firstIter;
// using namespace secondIter;
using namespace thirdIter;

using namespace std;

const float bkgRatio = 0.0918578;      // background triplets in the Lambda_C peak window
const float simScale = 0.001;          // 1000 times more LC were produced
const float bkgScale = bkgRatio*1./3.; // third because there are 3 times more bkg combinations than LC combinations

void setCutsFromIndex(int const *index, float *cuts)
{
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
  TFile *simF = new TFile("simCutsPlots2ndIter.root");
  TFile *bkgF = new TFile("bkgCutsPlots2ndIter.root");

  cout << "************************************************" << endl;
  cout << "ii\tjj\tkk\tll\tmm\tnn\too\tdLength\tdcaD\tVdist\tpPt\tpiPt\tkPt\tcos(t)\tsignificance\tnSim\tnBKG\tratio" << endl;
  cout << endl;
  
  float max = 0;
  float maxRatio = 0;
  float maxSimCounts = 0;
  float maxBkgCounts = 0;
  float maxCuts[7] = {0,0,0,0,0,0};
  float maxIdx[7] = {0,0,0,0,0,0};

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

		cout << significance << "\t";
		cout << simCounts << "\t";
		cout << bkgCounts << "\t";
		cout << ratio << endl;

		if (significance > max)
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
}
