//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Sep  7 08:28:03 2015 by ROOT version 5.34/09
// from TTree ntTMVA/
// found on file: lambdaCTMVAkPionKaonProtonCuts.root
//////////////////////////////////////////////////////////

#ifndef simCutsMaker_h
#define simCutsMaker_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include "TH1D.h"
#include <iostream>
#include <climits>

using namespace std;

const char* outfileName = "BgCutsPlots.root";
// Header file for the classes stored in the TTree if any.
// Fixed size dimensions of array or collections stored in the TTree if any.

// --------------------------------------------------
class simCutsMaker {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Float_t         m;
   Float_t         pt;
   Float_t         charges;
   Float_t         phi;
   Float_t         eta;
   Float_t         dcaDaugthers31;
   Float_t         dcaDaugthers23;
   Float_t         dcaDaugthers12;
   Float_t         cosPntAngle;
   Float_t         dLength;
   Float_t         p1pt;
   Float_t         p2pt;
   Float_t         p3pt;
   Float_t         p1Dca;
   Float_t         p2Dca;
   Float_t         p3Dca;
   Float_t         maxVertexDist;

   // List of branches
   TBranch        *b_m;   //!
   TBranch        *b_pt;   //!
   TBranch        *b_charges;   //!
   TBranch        *b_phi;   //!
   TBranch        *b_eta;   //!
   TBranch        *b_dcaDaugthers31;   //!
   TBranch        *b_dcaDaugthers23;   //!
   TBranch        *b_dcaDaugthers12;   //!
   TBranch        *b_cosPntAngle;   //!
   TBranch        *b_dLength;   //!
   TBranch        *b_p1pt;   //!
   TBranch        *b_p2pt;   //!
   TBranch        *b_p3pt;   //!
   TBranch        *b_p1Dca;   //!
   TBranch        *b_p2Dca;   //!
   TBranch        *b_p3Dca;   //!
   TBranch        *b_maxVertexDist;   //!

   // histograms
   TH1D *H[15625]; // workaround of six dimensional array of TH1D* (5^6 = 15625) 
		   // Root does not support 6 dimensional arrays
   inline int indexInArray(int ii, int jj, int kk, int ll, int mm, int nn);
   // cuts and index to cuts
   float cuts[6];
   int indices[6];
   // output file
   TFile *outf;

   simCutsMaker(TTree *tree=0);
   virtual ~simCutsMaker();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(Long64_t first = 1, Long64_t last = LLONG_MAX);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   void bookHistograms();
   void calculateIndices();
   void setCutsFromIndex(int const* indexArray);
   void write();
};

#endif

// --------------------------------------------------
#ifdef simCutsMaker_cxx
simCutsMaker::simCutsMaker(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("lambdaCTMVAkPionKaonProtonCuts.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("lambdaCTMVAkPionKaonProtonCuts.root");
      }
      f->GetObject("ntTMVA",tree);

   }
   Init(tree);

}

// --------------------------------------------------
simCutsMaker::~simCutsMaker()
{
   outf->Close();
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

// --------------------------------------------------
Int_t simCutsMaker::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
// --------------------------------------------------
Long64_t simCutsMaker::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

// --------------------------------------------------
void simCutsMaker::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("m", &m, &b_m);
   fChain->SetBranchAddress("pt", &pt, &b_pt);
   fChain->SetBranchAddress("charges", &charges, &b_charges);
   fChain->SetBranchAddress("phi", &phi, &b_phi);
   fChain->SetBranchAddress("eta", &eta, &b_eta);
   fChain->SetBranchAddress("dcaDaugthers31", &dcaDaugthers31, &b_dcaDaugthers31);
   fChain->SetBranchAddress("dcaDaugthers23", &dcaDaugthers23, &b_dcaDaugthers23);
   fChain->SetBranchAddress("dcaDaugthers12", &dcaDaugthers12, &b_dcaDaugthers12);
   fChain->SetBranchAddress("cosPntAngle", &cosPntAngle, &b_cosPntAngle);
   fChain->SetBranchAddress("dLength", &dLength, &b_dLength);
   fChain->SetBranchAddress("p1pt", &p1pt, &b_p1pt);
   fChain->SetBranchAddress("p2pt", &p2pt, &b_p2pt);
   fChain->SetBranchAddress("p3pt", &p3pt, &b_p3pt);
   fChain->SetBranchAddress("p1Dca", &p1Dca, &b_p1Dca);
   fChain->SetBranchAddress("p2Dca", &p2Dca, &b_p2Dca);
   fChain->SetBranchAddress("p3Dca", &p3Dca, &b_p3Dca);
   fChain->SetBranchAddress("maxVertexDist", &maxVertexDist, &b_maxVertexDist);
   Notify();

   outf = new TFile(outfileName, "RECREATE");
   bookHistograms();
}

// --------------------------------------------------
Bool_t simCutsMaker::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

// --------------------------------------------------
void simCutsMaker::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
// --------------------------------------------------
Int_t simCutsMaker::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
// --------------------------------------------------
#endif // #ifdef simCutsMaker_cxx
