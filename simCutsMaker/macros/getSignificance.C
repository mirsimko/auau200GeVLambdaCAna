#include "TFile.h"
#include "TNtuple.h"
#include <iostream>
#include "TMath.h"
#include "TH1.h"

using namespace std;

const float simScale = 0.0001;
const float bkgScale = 1./3.;

void setCutsFromIndex(int const *index, float *cuts)
{
  // ***********************************************
  // The original set of cuts:
  // ***********************************************
  // float MdLength = 0.003 + 0.004* index[0];
  // float MdcaDaughters = 0.02 - 0.004*index[1];
  // float MmaxVdist = 0.05 - 0.01*index[2];
  // float MpPt = 0.3 + 0.2*index[3];
  // float MpiPt = 0.3 + 0.2*index[4];
  // float MkPt = 0.3 + 0.2*index[5];
  // float McosTheta = 0.9825 + 0.0025*index[6];
  // ***********************************************

  // ***********************************************
  // second iteration:
  // ***********************************************
  // float MdLength = 0.012 + 0.004* index[0];
  // float MdcaDaughters = 0.018 - 0.002*index[1];
  // float MmaxVdist = 0.04 - 0.005*index[2];
  // float MpPt = 0.3 + 0.1*index[3];
  // float MpiPt = 0.3 + 0.1*index[4];
  // float MkPt = 0.3 + 0.1*index[5];
  // float McosTheta = 0.98 + 0.0025*index[6];
  // ***********************************************

  // ***********************************************
  // third iteration:
  // ***********************************************
  // float MdLength = 0.02 + 0.002* index[0];
  // float MdcaDaughters = 0.010 - 0.002*index[1];
  // float MmaxVdist = 0.028 - 0.002*index[2];
  // float MpPt = 0.6 + 0.1*index[3];
  // float MpiPt = 0.5 + 0.05*index[4];
  // float MkPt = 0.5 + 0.05*index[5];
  // float McosTheta = 0.9875 + 0.0025*index[6];
  // ***********************************************

  // ***********************************************
  // fourth iteration:
  // ***********************************************
  // float MdLength = 0.028 + 0.002* index[0];
  // float MdcaDaughters = 0.010 - 0.001*index[1];
  // float MmaxVdist = 0.025 - 0.001*index[2];
  // float MpPt = 1. + 0.1*index[3];
  // float MpiPt = 0.7 + 0.05*index[4];
  // float MkPt = 0.7 + 0.05*index[5];
  // float McosTheta = 0.995 + 0.001*index[6];
  // ***********************************************

  // ***********************************************
  // fifth iteration:
  // ***********************************************
  // float MdLength = 0.036 + 0.002* index[0];
  // float MdcaDaughters = 0.006 - 0.001*index[1];
  // float MmaxVdist = 0.021 - 0.001*index[2];
  // float MpPt = 1.4 + 0.1*index[3];
  // float MpiPt = 0.75 + 0.025*index[4];
  // float MkPt = 0.9 + 0.05*index[5];
  // float McosTheta = 0.998 + 0.0002*index[6];
  // ***********************************************

  // ***********************************************
  // sixth iteration:
  // ***********************************************
  // float MdLength = 0.028 + 0.002* index[0];
  // float MdcaDaughters = 0.006 - 0.0005*index[1];
  // float MmaxVdist = 0.025 - 0.001*index[2];
  // float MpPt = 1.8 + 0.1*index[3];
  // float MpiPt = 0.75 + 0.0125*index[4];
  // float MkPt = 1. + 0.025*index[5];
  // float McosTheta = 0.9988 + 0.0002*index[6];
  // ***********************************************

  // ***********************************************
  // seventh iteration:
  // ***********************************************
  // float MdLength = 0.034 + 0.001* index[0];
  // float MdcaDaughters = 0.0055 - 0.00025*index[1];
  // float MmaxVdist = 0.022 - 0.0005*index[2];
  // float MpPt = 1.7 + 0.05*index[3];
  // float MpiPt = 0.775 + 0.0125*index[4];
  // float MkPt = 1.0375 + 0.0125*index[5];
  // float McosTheta = 0.9986 + 0.0001*index[6];
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
  TFile *simF = new TFile("simCuts7thIter.root");
  TFile *bkgF = new TFile("bgCuts7thIter.root");

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
