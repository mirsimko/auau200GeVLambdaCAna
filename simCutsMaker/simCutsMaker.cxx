#define simCutsMaker_cxx
#include "simCutsMaker.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <cmath>
#include "TH1D.h"
#include "TFile.h"

// --------------------------------------------------
void simCutsMaker::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L simCutsMaker.C
//      Root > simCutsMaker t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;


   for (Long64_t jentry=0; jentry<nentries;jentry++) 
   {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;

      if (jentry % 1000000 == 0)
	cout << "Running on entry # " << jentry << " ______________"<< endl;

      fChain->GetEntry(jentry);

      calculateIndices();

      for (int i = 0; i < indices[0]; ++i)
      {
	for (int j = 0; j < indices[1]; ++j)
	{
	  for (int k = 0; k < indices[2]; ++k)
	  {
	    for (int l = 0; l < indices[3]; ++l)
	    {
	      for (int m = 0; m < indices[4]; ++m)
	      {
		for (int n = 0; n < indices[5]; ++n)
		{
		  H[i][j][k][l][m][n]->Fill(pt);
		}
	      }
	    }
	  }
	}
      }
   }

   write();
}
// --------------------------------------------------

void simCutsMaker::bookHistograms()
{
  for (int i = 0; i < 5; ++i) // dLength
  {
    for (int j = 0; j < 5; ++j) // dcaDaughters
    {
      for (int k = 0; k < 5; ++k) // maxVDist
      {
	for (int l = 0; l < 5; ++l) // pPt
	{
	  for (int m = 0; m < 5; ++m) // piPt
	  {
	    for (int n  = 0; n < 5; ++n) // kPt
	    {
	      int index [6] = {i, j, k, l, m, n};
	      setCutsFromIndex(index);
	      H[i][j][k][l][m][n] = new TH1D(Form("H%d%d%d%d%d%d", i, j, k, l, m, n),
					     From("p_{T} spectrum {dLength>%1.3fcm,dcaDaughters<%1.3fcm,maxVdist<%1.3f,pPt<%1.1fGeV,piPt<%1.1fGeV,kPt<%1.1fGeV}", 
		  			          cuts[0],cuts[1],cuts[2],cuts[3],cuts[4],cuts[5]),
		  			     20,0.,13.);
	      H[i][j][k][l][m][n] -> Sumw2();
	      H[i][j][k][l][m][n] -> GetXaxis() -> SetTitle("p_{T} [GeV]");
	    } // kPt
	  } // piPt
	} // pPt
      } // maxVDist
    } // dcaDaughters
  } // dLength
} // void simCutsMaker::bookHistograms()
// --------------------------------------------------

void simCutsMaker::setCutsFromIndex(int const *index)
{
  float MdLength = 0.003 + 0.004* index[0];
  float MdcaDaughters = 0.02 - 0.004*index[1];
  float MmaxVdist = 0.05 - 0.01*index[2];
  float MpPt = 0.3 + 0.2*index[3];
  float MpiPt = 0.3 + 0.2*index[4];
  float MkPt = 0.3 + 0.2*index[5];

  unsigned int iArr = 0;
  cuts[iArr++]=MdLength;
  cuts[iArr++]=MdcaDaughters;
  cuts[iArr++]=MmaxVdist;
  cuts[iArr++]=MpPt;
  cuts[iArr++]=MpiPt;
  cuts[iArr++]=MkPt;
}
// --------------------------------------------------

void simCutsMaker::calculateIndices()
{
  unsigned int iArr = 0;
  // dLength cut
  indices[iArr++] = int(floor( (dLength - 0.003)/0.004 ));
  // daughters DCA cut
  // calculate maximum of daughters DCA
  float maxDcaDaughters = dcaDaugthers12 > dcaDaugthers23 ? dcaDaugthers12 : dcaDaugthers23;
  maxDcaDaughters = maxDcaDaughters > dcaDaugthers31 ? maxDcaDaughters : dcaDaugthers31;
  indices[iArr++] = int(ceil( (-maxDcaDaughters + 0.02 )/0.004 ));
  // maxVDist cut
  indices[iArr++] = int(ceil( (-maxVertexDist + 0.05)/0.01 ));
  // daughters pT
  indices[iArr++] = int(floor( (p2pt - 0.3)/0.2 )); // proton
  indices[iArr++] = int(floor( (p3pt - 0.3)/0.2 )); // pion
  indices[iArr++] = int(floor( (p1pt - 0.3)/0.2 )); // kaon

  for(int i = 0; i < 6; ++i)
  {
    if (indices[i] > 5)
      indices[i] = 5;
  }
}
// --------------------------------------------------

void simCutsMaker::write()
{
  outf->cd();
  for(int i = 0; i < 5; ++i)
  {
    for(int j = 0; j < 5; ++j)
    {
      for(int k = 0; k < 5; ++k)
      {
	for(int l = 0; l < 5; ++l)
	{
	  for (int m = 0; m < 5; ++m)
	  {
	    for (int n = 0; n < 5; ++n)
	    {
	      H[i][j][k][l][m][n]->Write();
	    }
	  }
	}
      }
    }
  }
} // write()
// --------------------------------------------------
