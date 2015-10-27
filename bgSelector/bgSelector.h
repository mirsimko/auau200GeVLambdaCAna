//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Sep  5 15:33:10 2015 by ROOT version 5.34/09
// from TTree secondary/secondary
// found on file: picoHFLambdaCMaker.root
//////////////////////////////////////////////////////////

#ifndef bgSelector_h
#define bgSelector_h

#include <TROOT.h>
#include <TChain.h>
#include <TNtuple.h>
#include <TFile.h>
#include <utility>

// Declaration of cuts
// dEdx cuts
static const Float_t pNSigmaCut = 2.;
static const Float_t piNSigmaCut = 2.;
static const Float_t KNSigmaCut = 2.;
static const pair<float,float> mCut (2.266, 2.306);


// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class bgSelector {
public :
   
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // output Ntuple
   TNtuple *outNT;
   //output File
   TFile *outFile;

   // Declaration of leaf types
   Float_t         p1pt;
   Float_t         p2pt;
   Float_t         p3pt;
   Float_t         charges;
   Float_t         m;
   Float_t         pt;
   Float_t         eta;
   Float_t         phi;
   Float_t         cosPntAngle;
   Float_t         dLength;
   Float_t         p1Dca;
   Float_t         p2Dca;
   Float_t         p3Dca;
   Float_t         cosThetaStar;
   Float_t         dcaDaugthers12;
   Float_t         dcaDaugthers23;
   Float_t         dcaDaugthers31;
   Float_t         mLambda1520;
   Float_t         mDelta;
   Float_t         mKstar;
   Float_t         pNSigma;
   Float_t         KNSigma;
   Float_t         piNSigma;
   Float_t         pTOFbeta;
   Float_t         KTOFbeta;
   Float_t         piTOFbeta;
   Float_t         maxVertexDist;

   // List of branches
   TBranch        *b_p1pt;   //!
   TBranch        *b_p2pt;   //!
   TBranch        *b_p3pt;   //!
   TBranch        *b_charges;   //!
   TBranch        *b_m;   //!
   TBranch        *b_pt;   //!
   TBranch        *b_eta;   //!
   TBranch        *b_phi;   //!
   TBranch        *b_cosPntAngle;   //!
   TBranch        *b_dLength;   //!
   TBranch        *b_p1Dca;   //!
   TBranch        *b_p2Dca;   //!
   TBranch        *b_p3Dca;   //!
   TBranch        *b_cosThetaStar;   //!
   TBranch        *b_dcaDaugthers12;   //!
   TBranch        *b_dcaDaugthers23;   //!
   TBranch        *b_dcaDaugthers31;   //!
   TBranch        *b_mLambda1520;   //!
   TBranch        *b_mDelta;   //!
   TBranch        *b_mKstar;   //!
   TBranch        *b_pNSigma;   //!
   TBranch        *b_KNSigma;   //!
   TBranch        *b_piNSigma;   //!
   TBranch        *b_pTOFbeta;   //!
   TBranch        *b_KTOFbeta;   //!
   TBranch        *b_piTOFbeta;   //!
   TBranch        *b_maxVertexDist;   //!

   bgSelector(const char *outFileName = "bgSelectedLc.root", TTree *tree=0);
   virtual ~bgSelector();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(const char* outFileName, TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   void Write();
};

#endif

#ifdef bgSelector_cxx
bgSelector::bgSelector(const char *outFileName, TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/global/project/projectdirs/star/pwg/starhf/simkomir/LambdaC/picoHFLambdaCMaker.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/global/project/projectdirs/star/pwg/starhf/simkomir/LambdaC/picoHFLambdaCMaker.root");
      }
      f->GetObject("secondary",tree);

   }
   Init(outFileName, tree);
}

bgSelector::~bgSelector()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t bgSelector::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t bgSelector::LoadTree(Long64_t entry)
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

void bgSelector::Init(const char* outFileName, TTree *tree)
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

   fChain->SetBranchAddress("p1pt", &p1pt, &b_p1pt);
   fChain->SetBranchAddress("p2pt", &p2pt, &b_p2pt);
   fChain->SetBranchAddress("p3pt", &p3pt, &b_p3pt);
   fChain->SetBranchAddress("charges", &charges, &b_charges);
   fChain->SetBranchAddress("m", &m, &b_m);
   fChain->SetBranchAddress("pt", &pt, &b_pt);
   fChain->SetBranchAddress("eta", &eta, &b_eta);
   fChain->SetBranchAddress("phi", &phi, &b_phi);
   fChain->SetBranchAddress("cosPntAngle", &cosPntAngle, &b_cosPntAngle);
   fChain->SetBranchAddress("dLength", &dLength, &b_dLength);
   fChain->SetBranchAddress("p1Dca", &p1Dca, &b_p1Dca);
   fChain->SetBranchAddress("p2Dca", &p2Dca, &b_p2Dca);
   fChain->SetBranchAddress("p3Dca", &p3Dca, &b_p3Dca);
   fChain->SetBranchAddress("cosThetaStar", &cosThetaStar, &b_cosThetaStar);
   fChain->SetBranchAddress("dcaDaugthers12", &dcaDaugthers12, &b_dcaDaugthers12);
   fChain->SetBranchAddress("dcaDaugthers23", &dcaDaugthers23, &b_dcaDaugthers23);
   fChain->SetBranchAddress("dcaDaugthers31", &dcaDaugthers31, &b_dcaDaugthers31);
   fChain->SetBranchAddress("mLambda1520", &mLambda1520, &b_mLambda1520);
   fChain->SetBranchAddress("mDelta", &mDelta, &b_mDelta);
   fChain->SetBranchAddress("mKstar", &mKstar, &b_mKstar);
   fChain->SetBranchAddress("pNSigma", &pNSigma, &b_pNSigma);
   fChain->SetBranchAddress("KNSigma", &KNSigma, &b_KNSigma);
   fChain->SetBranchAddress("piNSigma", &piNSigma, &b_piNSigma);
   fChain->SetBranchAddress("pTOFbeta", &pTOFbeta, &b_pTOFbeta);
   fChain->SetBranchAddress("KTOFbeta", &KTOFbeta, &b_KTOFbeta);
   fChain->SetBranchAddress("piTOFbeta", &piTOFbeta, &b_piTOFbeta);
   fChain->SetBranchAddress("maxVertexDist", &maxVertexDist, &b_maxVertexDist);
   Notify();

   outFile = new TFile(outFileName, "RECREATE");

   outNT = new TNtuple("secondarySelected", "secondary", 
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
				    );

}

Bool_t bgSelector::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void bgSelector::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t bgSelector::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   GetEntry(entry);

   // get only wrong charge background
   if (charges > 0.)
     return -1;
   // dEdx cuts
   if ( TMath::Abs(pNSigma) > pNSigmaCut || TMath::Abs(piNSigma) > piNSigmaCut || TMath::Abs(KNSigma) > KNSigmaCut)
     return -1;
   if (m < mCut.first || m > mCut.second)
     return -1;

   return 1;
}

void bgSelector::Write()
{
  outFile->cd();
  outNT->Write();
}

#endif // #ifdef bgSelector_cxx
