#define bgSelector_cxx
#include "bgSelector.h"
#include <TH2.h>
#include <limits>
#include <TStyle.h>
#include <TCanvas.h>
#include <TNtuple.h>
#include <iostream>

using namespace std;

class bgSelector;

//__________________________________________________________________
void bgSelector::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L bgSelector.C
//      Root > bgSelector t
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

   Long64_t nentries = fChain->GetEntriesFast();
   cout << "Entries to process: " << nentries << endl;
   cout << "*********************************************" <<endl;

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;

      if (ientry % 1000000 == 0)
	cout << "Processing ientry " << ientry << endl;


      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // cuts
      if (Cut() < 0) continue;

      /* Variables in the Ntuple:
	 "p1pt:p2pt:p3pt:"
	 "charges:"
	 "m:pt:eta:phi:"
	 "cosPntAngle:dLength:"
	 "p1Dca:p2Dca:p3Dca:"
	 "cosThetaStar:"
	 "dcaDaugthers12:dcaDaugthers23:dcaDaugthers31:"
	 "mLambda1520:mDelta:mKstar:"
	 "pNSigma:KNSigma:piNSigma:"
	 "pTOFbeta:KTOFbeta:piTOFbeta:"
	 "maxVertexDist"
      */
      // cout << "dcaDaugthers12 = " << dcaDaugthers12 << endl;
      Float_t Arr[100] = {p1pt, p2pt, p3pt,
			  charges,
			  m,pt,eta,phi,
      			  cosPntAngle,dLength,
      			  p1Dca,p2Dca,p3Dca,
      			  cosThetaStar,
      			  dcaDaugthers12,dcaDaugthers23,dcaDaugthers31,
      			  mLambda1520,mDelta,mKstar,
      			  pNSigma,KNSigma,piNSigma,
      			  pTOFbeta,KTOFbeta,piTOFbeta,
      			  maxVertexDist
			  };
      
      outNT->Fill(Arr);
   }
   cout << "*********************************************" <<endl;
   cout << "Finished"<< endl;
   cout << "*********************************************" <<endl;
}

//__________________________________________________________________
Int_t bgSelector::Cut(/*Long64_t entry*/)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   // GetEntry(entry);

   // get only wrong charge background
   if (charges > 0.)
     return -1;
   // dEdx cuts
   if ( TMath::Abs(pNSigma) > pNSigmaCut || TMath::Abs(piNSigma) > piNSigmaCut || TMath::Abs(KNSigma) > KNSigmaCut)
    return -1;
   // decay length cut
   // if (dLength < 0.02)
   // {
   //   // cout << "Rejected: decay length = " << dLength << endl;
   //   return -1;
   // }
   // cout << "Not rejected: decay Length = " << dLength << endl;
   // mass cut
   //
   // *********************************************************************
   //ii      jj      kk      ll      mm      nn      oo      dLength dcaD    Vdist   pPt     piPt    kPt     cos(t)  significance    nSim    nBKG    ratio
   //3       2       0       4       2       2       4       0.033   0.01    0.022   1.4     0.8     0.8     0.996   0.0979951       0.629   40.5705 0.0155039
   //
   // pt cut
   if (p1pt < 0.5 || p2pt < 0.8 || p3pt < 0.5) //  kaon, proton, pion
     return -1;

   //dLength
   if (dLength < 0.025)
     return -1;

   // DCA daughters
   if (dcaDaugthers12 > 0.015 || dcaDaugthers23 > 0.015 || dcaDaugthers31 > 0.015)
     return -1;

   // pairs vertices distance
   if (maxVertexDist > 0.07)
     return -1;

   // cos(theta)
   if (cosPntAngle < 0.99)
     return -1;

   // Require TOF for kaons and protons
   const float TOFnotUsed = std::numeric_limits<float>::quiet_NaN();
   if (KTOFbeta == TOFnotUsed || pTOFbeta == TOFnotUsed)
     return -1;

   // event was not rejected
   // cout << "Event selected" << endl;
   return 1;
}

//__________________________________________________________________
void bgSelector::Write()
{
  outFile->cd();
  outNT->Write();
}

//__________________________________________________________________
