/* *********************************************************************
 *  ROOT macro - Toy Monte Carlo Simulation for D0 decay
 *  Includes Momentum Resolution, DCA, hft ration, TPC efficiency ...
 *
 *  Authors:
 *            Guannan Xie (guannanxie@lbl.gov)
 *            **Mustafa Mustafa (mmustafa@lbl.gov)
 *            Hao Qiu (hqiu@lbl.gov)
 *
 *  ** Code Maintainer
 *
 * *********************************************************************
*/

#include <iostream>
#include <fstream>

#include "TFile.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TNtuple.h"
#include "TMath.h"
#include "TF1.h"
#include "TClonesArray.h"
#include "TPythia6.h"
#include "TPythia6Decayer.h"
#include "TRandom.h"
#include "TParticle.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TGraph.h"
#include "TMath.h"
#include "phys_constants.h"
#include "SystemOfUnits.h"

using namespace std;

void setDecayChannels(int const mdme);
void decayAndFill(int const kf, TLorentzVector* b, TClonesArray& daughters, int decayMode);
void getKinematics(TLorentzVector& b, double const mass);
TLorentzVector smearMom(TLorentzVector const& b, TF1 const * const fMomResolution);
TVector3 smearPos(TLorentzVector const& mom, TLorentzVector const& rMom, TVector3 const& pos);
TVector3 smearPosData(int const cent, TLorentzVector const& rMom, TVector3 const& pos);
float dca(TVector3 const& p, TVector3 const& pos, TVector3 const& vertex);
float dca1To2(TVector3 const& p1, TVector3 const& pos1, TVector3 const& p2, TVector3 const& pos2, TVector3& v0);
bool matchHft(int const centrality, TLorentzVector const& mom);
void bookObjects();
void write();
int getptIndex(double);
float resMass(TLorentzVector const &pMom, TLorentzVector const &kMom, TLorentzVector const &piMom, int decayMode);

TPythia6Decayer* pydecay;
TNtuple* nt;
TFile* result;

TF1* fKaonMomResolution = NULL;
TF1* fPionMomResolution = NULL;
const Int_t nCent = 9;
const Int_t nPtBins = 35;
const Double_t ptEdge[nPtBins + 1] = { 0.0, 0.2, 0.4,  0.6,  0.8,
                                       1.0, 1.2, 1.4,  1.6,  1.8,
                                       2.0, 2.2, 2.4,  2.6,  2.8,
                                       3.0, 3.2, 3.4,  3.6,  3.8,
                                       4.0, 4.2, 4.4,  4.6,  4.8,
                                       5.0, 5.4, 5.8,  6.2,  6.6,
                                       7.0, 8.0, 9.0, 10.0, 11.0, 12.0,
                                     };

TH1D* hHftRatio[nCent];
TH1D* h1DcaZ[nCent][nPtBins];
TH1D* h1DcaXY[nCent][nPtBins];

string outFileName;
std::pair<int, int> const decayChannels(4277, 4354); // first and last Lc decay annel number
std::pair<float, float> const momentumRange(0.3, 12);

float const acceptanceEta = 1.0;
float const sigmaPos0 = 15.2;
float const pxlLayer1Thickness = 0.00486;
float const sigmaVertexCent[nCent] = {31., 18.1, 12.8, 9.3, 7.2, 5.9, 5., 4.6, 4.};

// centrality and p_T distributions
TH1D *nBinCent;
float const nBin[nCent] = {1012, 15., 805, 577, 365, 221, 127, 66.8, 32.4};
TF1* fLevy;

float const Dyield = 0.704442; /*D0 yiield per event*/ 
float const LambdaDratio = 0.2;/*ratio between Lambda_c and D0*/ 

enum DecayMode {kKstarProton, kLambda1520Pion, kDeltaPPkaon, kPionKaonProton, kLambdaPion, kKshortProton};

//============== main  program ==================
void toyMcEffLambdaC( int nEvts = 100, const char* outFile = "lambdaC.root", int modeOfDecay = 3 /*3=kPionKaonProton*/)
{
   // int stop;
   // cin >> stop;

   DecayMode mDecayMode = (DecayMode) modeOfDecay;   
   outFileName = outFile;
   
   gRandom->SetSeed();
   bookObjects();

   pydecay = TPythia6Decayer::Instance();

   // Reading new decay channels
   pydecay-> SetDecayTableFile("StRoot/StLambdaCToyMC/macros/AddedDecays.list");
   pydecay-> ReadDecayTable();

   pydecay->Init();
   // TPythia6::Instance()->Pylist(12); // this is for writing the Decay table to std_out

   // selecting decay channels
   double branchingRatio;
   switch(mDecayMode)
   {
     case kKstarProton:
       TPythia6::Instance()->SetMDME(617,1,1);
       TPythia6::Instance()->SetMDME(618,1,0);
       TPythia6::Instance()->SetMDME(619,1,0);

       setDecayChannels(4294);

       branchingRatio = 0.016;
       break;
     case kLambda1520Pion:
       TPythia6::Instance()->SetMDME(4276,1,1);

       setDecayChannels(4344);

       branchingRatio = 0.018;
       break;
     case kDeltaPPkaon:
       TPythia6::Instance()->SetMDME(1052,1,1);

       setDecayChannels(4291);

       branchingRatio = 0.0086;
       break;
     case kPionKaonProton:
       setDecayChannels(4343);

       branchingRatio = 0.028;
       break;
     default:
       break;       
   }

   double npart = Dyield * LambdaDratio * branchingRatio * nEvts - 1;
   cout << "Number of produced Lambda_C: " << (int) floor(npart) << endl;

   TLorentzVector* b_d = new TLorentzVector;
   TClonesArray ptl("TParticle", 10);
   for (int ipart = 0; ipart < npart; ipart++)
   {
      // cout << "creating Lambda_c number: " << ipart << endl;
      if (!(ipart % 100000))
         cout << "____________ ipart = " << ipart << " ________________" << endl;

      getKinematics(*b_d, M_LAMBDA_C_PLUS);

      decayAndFill(4122, b_d, ptl, mDecayMode);
      decayAndFill(-4122, b_d, ptl, mDecayMode);
   }

   write();
}

void setDecayChannels(int const mdme)
{
   for (int idc = decayChannels.first; idc < decayChannels.second + 1; idc++) TPythia6::Instance()->SetMDME(idc, 1, 0);
   TPythia6::Instance()->SetMDME(mdme, 1, 1);
}

void decayAndFill(int const kf, TLorentzVector* b, TClonesArray& daughters, int decayMode)
{
   pydecay->Decay(kf, b);
   pydecay->ImportParticles(&daughters);

   TLorentzVector kMom;
   TLorentzVector piMom;
   TLorentzVector pMom;
   TVector3 v00;

   int nTrk = daughters.GetEntriesFast();
   for (int iTrk = 0; iTrk < nTrk; ++iTrk)
   {
      TParticle* ptl0 = (TParticle*)daughters.At(iTrk);
      // cout << "Daughter PDG number: " << ptl0->GetPdgCode() << endl;

      switch (abs(ptl0->GetPdgCode()))
      {
         case 321:
            ptl0->Momentum(kMom);
            v00.SetXYZ(ptl0->Vx() * 1000., ptl0->Vy() * 1000., ptl0->Vz() * 1000.); // converted to μm
	    // cout << "Kaon" << endl;
            break;
         case 211:
            ptl0->Momentum(piMom);
	    // cout << "Pion" << endl;
            break;
         case 2212:
            ptl0->Momentum(pMom);
	    // cout << "Proton" << endl;
            break;
         default:
            break;
      }
   }
   daughters.Clear();

   // smear momentum
   TLorentzVector const kRMom = smearMom(kMom, fKaonMomResolution);
   TLorentzVector const piRMom = smearMom(piMom, fPionMomResolution);
   TLorentzVector const pRMom = smearMom(pMom, fPionMomResolution);

   int const cent = floor(nBinCent->GetRandom());
   // cout << "centrality: " << cent << endl;
   // if (cent == 9)
   //   cout << "centrality bin 9" << endl;
   // if (cent == 0)
   //   cout << "centrality bin 0" << endl;

   // smear position
   TVector3 const kRPos = smearPosData(cent, kRMom, v00);
   TVector3 const piRPos = smearPosData(cent, piRMom, v00);
   TVector3 const pRPos = smearPosData(cent, pRMom, v00);
   // TVector3 const kRPos = smearPos(kMom, kRMom, v00);
   // TVector3 const pRPos = smearPos(kMom, pRMom, v00);

   // smear primary vertex
   // float const sigmaVertex = sigmaVertexCent[cent];
   // TVector3 const vertex(gRandom->Gaus(0, sigmaVertex), gRandom->Gaus(0, sigmaVertex), gRandom->Gaus(0, sigmaVertex));
   TVector3 const vertex(0., 0., 0.);

   // reconstruct
   TLorentzVector const rMom = kRMom + piRMom + pRMom;
   float const kDca = dca(kMom.Vect(), v00, vertex);
   float const piDca = dca(piMom.Vect(), v00, vertex);
   float const pDca = dca(pMom.Vect(), v00, vertex);
   float const kRDca = dca(kRMom.Vect(), kRPos, vertex);
   float const piRDca = dca(piRMom.Vect(), piRPos, vertex);
   float const pRDca = dca(pRMom.Vect(), pRPos, vertex);

   TVector3 v0;
   float const dca12 = dca1To2(kRMom.Vect(), kRPos, piRMom.Vect(), piRPos, v0);
   float const dca23 = dca1To2(piRMom.Vect(), piRPos, pRMom.Vect(), pRPos, v0);
   float const dca13 = dca1To2(kRMom.Vect(), kRPos, pRMom.Vect(), pRPos, v0);
   float const decayLength = (v0 - vertex).Mag();
   float const dcaToPv = dca(rMom.Vect(), v0, vertex);
   float const cosTheta = (v0 - vertex).Unit().Dot(rMom.Vect().Unit());

                       // save
   float arr[100];
   int iArr = 0;
   arr[iArr++] = cent;
   arr[iArr++] = vertex.X();
   arr[iArr++] = vertex.Y();
   arr[iArr++] = vertex.Z();

   arr[iArr++] = kf;
   arr[iArr++] = b->M();
   arr[iArr++] = b->Perp();
   arr[iArr++] = b->PseudoRapidity();
   arr[iArr++] = b->Rapidity();
   arr[iArr++] = b->Phi();
   arr[iArr++] = v00.X();
   arr[iArr++] = v00.Y();
   arr[iArr++] = v00.Z();

   arr[iArr++] = rMom.M();
   arr[iArr++] = rMom.Perp();
   arr[iArr++] = rMom.PseudoRapidity();
   arr[iArr++] = rMom.Rapidity();
   arr[iArr++] = rMom.Phi();

   arr[iArr++] = dca12;
   arr[iArr++] = dca23;
   arr[iArr++] = dca13;
   arr[iArr++] = decayLength;
   arr[iArr++] = dcaToPv;
   arr[iArr++] = cosTheta;

   arr[iArr++] = kMom.M();
   arr[iArr++] = kMom.Perp();
   arr[iArr++] = kMom.PseudoRapidity();
   arr[iArr++] = kMom.Rapidity();
   arr[iArr++] = kMom.Phi();
   arr[iArr++] = kDca;

   arr[iArr++] = kRMom.M();
   arr[iArr++] = kRMom.Perp();
   arr[iArr++] = kRMom.PseudoRapidity();
   arr[iArr++] = kRMom.Rapidity();
   arr[iArr++] = kRMom.Phi();
   arr[iArr++] = kRPos.X();
   arr[iArr++] = kRPos.Y();
   arr[iArr++] = kRPos.Z();
   arr[iArr++] = kRDca;

   arr[iArr++] = piMom.M();
   arr[iArr++] = piMom.Perp();
   arr[iArr++] = piMom.PseudoRapidity();
   arr[iArr++] = piMom.Rapidity();
   arr[iArr++] = piMom.Phi();
   arr[iArr++] = piDca;

   arr[iArr++] = piRMom.M();
   arr[iArr++] = piRMom.Perp();
   arr[iArr++] = piRMom.PseudoRapidity();
   arr[iArr++] = piRMom.Rapidity();
   arr[iArr++] = piRMom.Phi();
   arr[iArr++] = piRPos.X();
   arr[iArr++] = piRPos.Y();
   arr[iArr++] = piRPos.Z();
   arr[iArr++] = piRDca;

   arr[iArr++] = pMom.M();
   arr[iArr++] = pMom.Perp();
   arr[iArr++] = pMom.PseudoRapidity();
   arr[iArr++] = pMom.Rapidity();
   arr[iArr++] = pMom.Phi();
   arr[iArr++] = pDca;

   arr[iArr++] = pRMom.M();
   arr[iArr++] = pRMom.Perp();
   arr[iArr++] = pRMom.PseudoRapidity();
   arr[iArr++] = pRMom.Rapidity();
   arr[iArr++] = pRMom.Phi();
   arr[iArr++] = pRPos.X();
   arr[iArr++] = pRPos.Y();
   arr[iArr++] = pRPos.Z();
   arr[iArr++] = pRDca;

   arr[iArr++] = matchHft(cent, kRMom);
   arr[iArr++] = matchHft(cent, piRMom);
   arr[iArr++] = matchHft(cent, pRMom);

   arr[iArr++] = resMass(pMom, kMom, piMom, decayMode);
   arr[iArr++] = resMass(pRMom, kRMom, piRMom, decayMode);
   nt->Fill(arr);
}

void getKinematics(TLorentzVector& b, double const mass)
{
   float const pt = fLevy->GetRandom();
   // cout << "pt: " << pt << endl;
   float const y = gRandom->Uniform(-acceptanceEta, acceptanceEta);
   float const phi = TMath::TwoPi() * gRandom->Rndm();

   float const mT = sqrt(pt*pt + mass*mass);
   float const pz = mT*sinh(y);
   float const energy = mT*cosh(y);

   b.SetPxPyPzE(pt * cos(phi), pt * sin(phi), pz, energy);
}

float dca(TVector3 const& p, TVector3 const& pos, TVector3 const& vertex)
{
   TVector3 posDiff = pos - vertex;
   return fabs(p.Cross(posDiff.Cross(p)).Unit().Dot(posDiff));
}

float dca1To2(TVector3 const& p1, TVector3 const& pos1, TVector3 const& p2, TVector3 const& pos2, TVector3& v0)
{
   TVector3 posDiff = pos2 - pos1;
   TVector3 pu1 = p1.Unit();
   TVector3 pu2 = p2.Unit();
   double pu1Pu2 = pu1.Dot(pu2);
   double g = posDiff.Dot(pu1);
   double k = posDiff.Dot(pu2);
   double s2 = (k - pu1Pu2 * g) / (pu1Pu2 * pu1Pu2 - 1.);
   double s1 = g + s2 * pu1Pu2;
   TVector3 posDca1 = pos1 + pu1 * s1;
   TVector3 posDca2 = pos2 + pu2 * s2;
   v0 = 0.5 * (posDca1 + posDca2);
   return (posDca1 - posDca2).Mag();
}

TLorentzVector smearMom(TLorentzVector const& b, TF1 const * const fMomResolution)
{
   float const pt = b.Perp();
   float const sPt = gRandom->Gaus(pt, pt * fMomResolution->Eval(pt));

   TLorentzVector sMom;
   sMom.SetXYZM(sPt * cos(b.Phi()), sPt * sin(b.Phi()), sPt * sinh(b.PseudoRapidity()), b.M());
   return sMom;
}

TVector3 smearPos(TLorentzVector const& mom, TLorentzVector const& rMom, TVector3 const& pos)
{
   float thetaMCS = 13.6 / mom.Beta() / rMom.P() / 1000 * sqrt(pxlLayer1Thickness / fabs(sin(mom.Theta())));
   float sigmaMCS = thetaMCS * 28000 / fabs(sin(mom.Theta()));
   float sigmaPos = sqrt(pow(sigmaMCS, 2) + pow(sigmaPos0, 2));

   return TVector3(gRandom->Gaus(pos.X(), sigmaPos), gRandom->Gaus(pos.Y(), sigmaPos), gRandom->Gaus(pos.Z(), sigmaPos));
}

int getptIndex(double pT)
{
   for (int i = 0; i < nPtBins; i++)
   {
      if ((pT >= ptEdge[i]) && (pT < ptEdge[i + 1]))
         return i;
   }
}

TVector3 smearPosData(int const cent, TLorentzVector const& rMom, TVector3 const& pos)
{
   int ptIndex = getptIndex(rMom.Perp());
   float sigmaPosZ = h1DcaZ[cent][ptIndex]->GetRandom() * 1e4;
   float sigmaPosXY = h1DcaXY[cent][ptIndex]->GetRandom() * 1e4;

   TVector3 newPos(pos);
   newPos.SetZ(0);
   TVector3 momPerp(-rMom.Vect().Y(), rMom.Vect().X(), 0.0);
   newPos += momPerp.Unit() * sigmaPosXY;

   return TVector3(newPos.X(), newPos.Y(), pos.Z() + sigmaPosZ);
}

bool matchHft(int const cent, TLorentzVector const& mom)
{
   int const bin = hHftRatio[cent]->FindBin(mom.Perp());
   return gRandom->Rndm() < hHftRatio[cent]->GetBinContent(bin);
}

//___________
void bookObjects()
{
   result = new TFile(outFileName.c_str(), "recreate");
   result->cd();

   TH1::AddDirectory(false);
   nt = new TNtuple("nt", "", "cent:vx:vy:vz:"
                    "pid:m:pt:eta:y:phi:v0x:v0y:v0z:"
                    "rM:rPt:rEta:rY:rPhi:"
                    "dca12:dca23:dca13:decayLength:dcaToPv:cosTheta:" // Rc pair
                    "kM:kPt:kEta:kY:kPhi:kDca:" // MC Kaon
                    "kRM:kRPt:kREta:kRY:kRPhi:kRVx:kRVy:kRVz:kRDca:" // Rc Kaon
                    "piM:piPt:piEta:piY:piPhi:piDca:" // MC Pion
                    "piRM:piRPt:piREta:piRY:piRPhi:piRVx:piRVy:piRVz:piRDca:" // Rc Pion
                    "pM:pPt:pEta:pY:pPhi:pDca:" // MC Proton
                    "pRM:pRPt:pREta:pRY:pRPhi:pRVx:pRVy:pRVz:pRDca:" // Rc Proton
                    "kHft:piHft:pHft:"
		    "MResonance:MRResonance");

   TFile f("momentum_resolution.root");
   fPionMomResolution = (TF1*)f.Get("fPion")->Clone("fPionMomResolution");
   fKaonMomResolution = (TF1*)f.Get("fKaon")->Clone("fKaonMomResolution");
   f.Close();

   TFile fHftRatio("HFT_Ratio_VsPt_Centrality.root");
   TFile fDca("Dca_VsPt_Centrality.root");

   for (int ii = 0; ii < 9; ++ii)
   {
      hHftRatio[ii] = (TH1D*)(fHftRatio.Get(Form("mh1HFTRatio1_%i", ii))->Clone(Form("mh1HFTRatio1_%i", ii)));
      result->cd();
      hHftRatio[ii]->Write();
   }

   result->cd();
   for (int ii = 0; ii < nCent; ++ii)
   {
      for (int jj = 0; jj < nPtBins; ++jj)
      {
         h1DcaXY[ii][jj] = (TH1D*)((fDca.Get(Form("mh3DcaXy_Cent%i_Pt%i", ii, jj)))->Clone(Form("mh3DcaXy_Cent%i_Pt%i", ii, jj)));
         h1DcaZ[ii][jj]  = (TH1D*)((fDca.Get(Form("mh3DcaZ_Cent%i_Pt%i", ii, jj)))->Clone(Form("mh3DcaZ_Cent%i_Pt%i", ii, jj)));
      }
   }

   fHftRatio.Close();
   fDca.Close();

   // setting the centrality dependence histogram
   nBinCent = new TH1D("nBinCent", "Number of bins vs centrality", nCent, 0, nCent);
   for (int i = 0; i < nCent; ++i)   
   {
      nBinCent->SetBinContent(i+1, nBin[i]);
   }

   // p_T spectrum
   fLevy = new TF1("fLevy", "[0]*TMath::Exp(-[1]/(x-[3]))*TMath::Power(x-[3],1-[2])*x", momentumRange.first, momentumRange.second);
   // in latex: $A  \left( \frac{\mu}{x-\phi} \right)^{1-\alpha} \exp \left(- \frac{\mu}{x-\phi}\right)$
   // parameters: A = 8.17808e+06, \mu = 1.92432e+01, \alpha = 1.39339e+01, \phi = -9.04949e-01
   //
   // this was fitted from published D0 data
   // The additional p_T is from Jacobian
   //
   fLevy->SetParameters(8.17808e+06, 1.92432e+01, 1.39339e+01, -9.04949e-01);

}
//___________
void write()
{
   result->cd();
   nt->Write();
   result->Close();
}
//___________
float resMass(TLorentzVector const &pMom, TLorentzVector const &kMom, TLorentzVector const &piMom, int decayMode)
{
  TLorentzVector resMom;
  switch (decayMode)
  {
    case kPionKaonProton:
      return 0;
      break;
    case kKstarProton:
      resMom = kMom + piMom;
      return resMom.M();
      break;
    case kLambda1520Pion:
      resMom = pMom + kMom;
      return resMom.M();
      break;
    case kDeltaPPkaon:
      resMom = pMom + piMom;
      return resMom.M();
      break;
    default:
      return -1;
      break;
  };
}
