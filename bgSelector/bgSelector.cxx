#define bgSelector_cxx
#include "bgSelector.h"
#include <TH2.h>
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
   // if (charges > 0.)
   //   return -1;
   // dEdx cuts
   // if ( TMath::Abs(pNSigma) > pNSigmaCut || TMath::Abs(piNSigma) > piNSigmaCut || TMath::Abs(KNSigma) > KNSigmaCut)
   //  return -1;
   // decay length cut
   if (dLength < 0.02)
   {
     // cout << "Rejected: decay length = " << dLength << endl;
     return -1;
   }
   // cout << "Not rejected: decay Length = " << dLength << endl;
   // mass cut
   if (m < mCut.first || m > mCut.second)
   {
     // cout << "m = " << m << " is not selected" << endl;
     return -1;
   }
   // pt cut
   if (p1pt < 0.4 || p2pt < 0.4 || p3pt < 0.4)
     return -1;

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
