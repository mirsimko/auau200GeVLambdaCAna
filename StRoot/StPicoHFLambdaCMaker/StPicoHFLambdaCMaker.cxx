#include <vector>
#include <iostream>
#include <fstream>
#include <limits>
#include "TNtuple.h"
#include "TClonesArray.h"
#include "TTime.h"
#include "TMath.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TList.h"

#include "StThreeVectorF.hh"
#include "StLorentzVectorF.hh"
#include "phys_constants.h"

#include "StPicoDstMaker/StPicoDst.h"
#include "StPicoDstMaker/StPicoDstMaker.h"
#include "StPicoDstMaker/StPicoEvent.h"
#include "StPicoDstMaker/StPicoTrack.h"
#include "StPicoDstMaker/StPicoBTofPidTraits.h"

#include "StPicoHFMaker/StPicoHFEvent.h"
#include "StPicoHFMaker/StHFCuts.h"
#include "StPicoHFMaker/StHFPair.h"
#include "StPicoHFMaker/StHFClosePair.h"
#include "StPicoHFMaker/StHFTriplet.h"

#include "StPicoHFLambdaCMaker.h"
#include "StRefMultCorr/StRefMultCorr.h"

using namespace std;
ClassImp(StPicoHFLambdaCMaker)

// _________________________________________________________
StPicoHFLambdaCMaker::StPicoHFLambdaCMaker(char const* name, StPicoDstMaker* picoMaker, char const* outputBaseFileName,  
					   char const* inputHFListHFtree = "") :
  StPicoHFMaker(name, picoMaker, outputBaseFileName, inputHFListHFtree),
  mDecayChannel(kPionKaonProton), mNtupleSecondary(NULL), mNtupleTertiary(NULL), mSinglePartList(NULL),
  mRefmultCorrUtil(NULL), mFillParticleHistos(true) {
  // constructor
}

// _________________________________________________________
StPicoHFLambdaCMaker::~StPicoHFLambdaCMaker() {
  // destructor
}

// _________________________________________________________
int StPicoHFLambdaCMaker::InitHF() {

  if (isMakerMode() != StPicoHFMaker::kWrite) {
    if (isDecayMode() == StPicoHFEvent::kTwoAndTwoParticleDecay) {
      mNtupleSecondary = new TNtuple("secondary", "secondary", "p1pt:p2pt:charges:m:pt:eta:phi:cosPntAngle:dLength:p1Dca:p2Dca:cosThetaStar:dcaDaugthers:Dm:DcosPntAngle:DdLength:Dp1Dca:Dp2Dca:DcosThetaStar:DdcaDaugthers");
      
      mNtupleTertiary = new TNtuple("tertiary", "tertiary", "p1pt:p2pt:charges:m:pt:eta:phi:cosPntAngle:dLength:p1Dca:p2Dca:cosThetaStar:dcaDaugthers"); 
    }
    else
      mNtupleSecondary = new TNtuple("secondary", "secondary", 
				     "p1pt:p2pt:p3pt:"
				     "charges:"
	  			     "m:pt:eta:phi:"
	  			     "cosPntAngle:dLength:"
				     "DCAtoPV:"
	  			     "p1Dca:p2Dca:p3Dca:"
	  			     "dcaDaughters12:dcaDaughters23:dcaDaughters31:"
	  			     "mLambda1520:mDelta:mKstar:"
	  			     "KNSigma:pNSigma:piNSigma:"
	  			     "KTOFinvBetaDiff:pTOFinvBetaDiff:piTOFinvBetaDiff:"
				     "KEta:pEta:piEta:"
				     "KPhi:pPhi:piPhi:"
	  			     "maxVertexDist:"
				     "centrality:centralityCorrection"
	  			     );
    nTriplets = 0;
    nProcessed = 0;
  }
  // Single particle tracks control hists
  mOutList->Add(new TList);
  mSinglePartList = static_cast<TList*>( mOutList->Last());
  mSinglePartList->SetOwner(true);
  mSinglePartList->SetName("HFSinglePartHists");

  // create single particle hists
  mSinglePartList->Add(new TH1D("centrality","centrality", 10, -1.5, 8.5));
  mSinglePartList->Add(new TH1D("centralityCorrection","centrality corrected", 10, -1.5, 8.5));

  mSinglePartList->Add(new TH2D("piEtaPhi","pi Eta phi distribution", 100, -TMath::Pi(), TMath::Pi(), 100, -1.1, 1.1));
  mSinglePartList->Add(new TH2D("pEtaPhi","p Eta phi distribution", 100, -TMath::Pi(), TMath::Pi(), 100, -1.1, 1.1));
  mSinglePartList->Add(new TH2D("KEtaPhi","K Eta phi distribution", 100, -TMath::Pi(), TMath::Pi(), 100, -1.1, 1.1));

  mSinglePartList->Add(new TH2D("piPhiPt", "pi phi vs pT", 100, 0, 15, 100, -TMath::Pi(), TMath::Pi()));
  mSinglePartList->Add(new TH2D("pPhiPt", "p phi vs pT", 100, 0, 15, 100, -TMath::Pi(), TMath::Pi()));
  mSinglePartList->Add(new TH2D("KPhiPt", "K phi vs pT", 100, 0, 15, 100, -TMath::Pi(), TMath::Pi()));

  mSinglePartList->Add(new TH2D("piNSigmaPt","pi nSigma vs pT", 100, 0, 10, 50, -4, 4));
  mSinglePartList->Add(new TH2D("pNSigmaPt","p nSigma vs pT", 100, 0, 10, 50, -4, 4));
  mSinglePartList->Add(new TH2D("KNSigmaPt","K nSigma vs pT", 100, 0, 10, 50, -4, 4));
  mRunNumber = 0;

  return kStOK;
}

// _________________________________________________________
int StPicoHFLambdaCMaker::FinishHF() {
  if (isMakerMode() != StPicoHFMaker::kWrite) {
    mNtupleSecondary->Write();
    
    if (isDecayMode() == StPicoHFEvent::kTwoAndTwoParticleDecay)
      mNtupleTertiary->Write();

    if (mDecayChannel == StPicoHFLambdaCMaker::kPionKaonProton)
    {
      cout << nProcessed << " triplets processed" << endl;
      cout << "Number of triplets " << nTriplets << endl;
    }
  }
  
  return kStOK;
}

// _________________________________________________________
int StPicoHFLambdaCMaker::MakeHF() {
  // For debugging: redirecting cout
  // streambuf* coutbuf = cout.rdbuf();
  // ofstream out;
  // out.open("/global/project/projectdirs/star/pwg/starhf/simkomir/LambdaC/dbg.log", ofstream::out | ofstream::app); // open for append
  // cout.rdbuf(out.rdbuf());

  // LOG_INFO << "Starting \"StPicoHFLambdaCMaker::MakeHF\"" << endm;

  calculateCentrality();

  if(mFillParticleHistos)
    fillControlHistos();

  if (isMakerMode() == StPicoHFMaker::kWrite) {
    createCandidates();
  }
  else if (isMakerMode() == StPicoHFMaker::kRead) {
    // -- the reading back of the perviously written trees happens in the background
    analyzeCandidates();
  }
  else if (isMakerMode() == StPicoHFMaker::kAnalyze) {
    createCandidates();
    analyzeCandidates();
  }

  // redirecting cout back
  // cout.rdbuf(coutbuf);
  // out.close();

  return kStOK;
}

// _________________________________________________________
int StPicoHFLambdaCMaker::createCandidates() {
  // create candidate pairs/ triplet and fill them in arrays (in StPicoHFEvent)

  // LOG_INFO << "Starting \"StPicoHFLambdaCMaker::createCandidates\"" << endm;
  // LOG_INFO << " N pions    : " << mIdxPicoPions.size()              << endm;
  // LOG_INFO << " N kaons    : " << mIdxPicoKaons.size()              << endm;
  // LOG_INFO << " N protons  : " << mIdxPicoProtons.size()            << endm;
  
  // -- Decay channel proton - K0Short (pi+ - pi-)
  if (mDecayChannel == StPicoHFLambdaCMaker::kProtonK0short) {

    createTertiaryK0Shorts();

    if (mPicoHFEvent->nHFTertiaryVertices() > 0) {
      TClonesArray const * ak0Short = mPicoHFEvent->aHFTertiaryVertices();

      for (unsigned int idxK0Short = 0; idxK0Short < mPicoHFEvent->nHFTertiaryVertices(); ++idxK0Short) {
	StHFPair* k0Short = static_cast<StHFPair*>(ak0Short->At(idxK0Short));

	for (unsigned int idxProton = 0; idxProton < mIdxPicoProtons.size(); ++idxProton) {
	  StPicoTrack const *proton = mPicoDst->track(mIdxPicoProtons[idxProton]);
	  
	  if (mIdxPicoProtons[idxProton] == k0Short->particle1Idx() || mIdxPicoProtons[idxProton] == k0Short->particle1Idx()) 
	    continue;
	  
	  // -- Secondary vertex
	  StHFPair lambdaC(proton, k0Short, 
			   mHFCuts->getHypotheticalMass(StHFCuts::kProton), mHFCuts->getHypotheticalMass(StHFCuts::kK0Short), 
			   mIdxPicoProtons[idxProton], idxK0Short, mPrimVtx, mBField);
	  if (!mHFCuts->isGoodSecondaryVertexPair(lambdaC)) 
	    continue;
	  
	  // -- get corrected TOF beta
	  // ----------------------------

	  // -- check if proton is still good proton
	  if ( ! mHFCuts->isHybridTOFProton(proton, mHFCuts->getTofBeta(proton, lambdaC.lorentzVector(), lambdaC.decayVertex()) , lambdaC.decayVertex()))
	    continue;
	  
	  // -- check if both pions are still good pions
	  StPicoTrack const *pion1 = mPicoDst->track(k0Short->particle1Idx());	  
	  StPicoTrack const *pion2 = mPicoDst->track(k0Short->particle2Idx());	  
	  
	  if ( ! mHFCuts->isHybridTOFPion(pion1, mHFCuts->getTofBeta(pion1, lambdaC.lorentzVector(), k0Short->decayVertex(), 
								     k0Short->lorentzVector(), k0Short->decayVertex()), k0Short->decayVertex()) ||
	       ! mHFCuts->isHybridTOFPion(pion2, mHFCuts->getTofBeta(pion2, lambdaC.lorentzVector(), lambdaC.decayVertex(), 
								     k0Short->lorentzVector(), k0Short->decayVertex()), k0Short->decayVertex()) )
	    continue;

	  mPicoHFEvent->addHFSecondaryVertexPair(&lambdaC);

	} // for (unsigned int idxProton = 0; idxProton < mIdxPicoProtons.size(); ++idxProton) {	  
      } // for (unsigned int idxK0Short = 0; idxK0Short <  mPicoHFEvent->nHFTertiaryVertices(); ++idxK0Short) {
    } //  if (mPicoHFEvent->nHFTertiaryVertices() > 0) {

    // LOG_INFO << "      N K0Shorts : " << mPicoHFEvent->nHFTertiaryVertices() << endm;
    // LOG_INFO << "      N Lambda_C : " << mPicoHFEvent->nHFSecondaryVertices() << endm;

  } // if (mDecayChannel == StPicoHFLambdaCMaker::kProtonK0short) {

  // == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == 
  // == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == 
  // == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == 

  // -- Decay channel  pi+ - lambda (proton - pi-)
  else if (mDecayChannel == StPicoHFLambdaCMaker::kLambdaPion) {

    createTertiaryLambdas();

    if (mPicoHFEvent->nHFTertiaryVertices() > 0) {
      TClonesArray const *aLambda = mPicoHFEvent->aHFTertiaryVertices();
      
      for (unsigned int idxLambda = 0; idxLambda < mPicoHFEvent->nHFTertiaryVertices(); ++idxLambda) {
	StHFPair const* lambda = static_cast<StHFPair*>(aLambda->At(idxLambda));
	
	for (unsigned int idxPion = 0; idxPion < mIdxPicoPions.size(); ++idxPion) {
	  StPicoTrack const *pion = mPicoDst->track(mIdxPicoPions[idxPion]);
	  
	  if (mIdxPicoPions[idxPion] == lambda->particle1Idx() || mIdxPicoPions[idxPion] == lambda->particle1Idx()) 
	    continue;

	  // -- Secondary vertex
	  StHFPair lambdaC(pion, lambda, 
			   mHFCuts->getHypotheticalMass(StHFCuts::kPion), mHFCuts->getHypotheticalMass(StHFCuts::kLambda), 
			   mIdxPicoPions[idxPion], idxLambda, mPrimVtx, mBField);
	  if (!mHFCuts->isGoodSecondaryVertexPair(lambdaC)) 
	    continue;

	  // -- get corrected TOF beta
	  // ----------------------------

	  // -- check if pion is still good pion
	  if ( ! mHFCuts->isHybridTOFPion(pion, mHFCuts->getTofBeta(pion, lambdaC.lorentzVector(), lambdaC.decayVertex()), lambdaC.decayVertex()) )
	    continue;

	  // -- check if both lambda daughthers (proton + pi-) are still good 
	  StPicoTrack const *proton1 = mPicoDst->track(lambda->particle1Idx());
	  StPicoTrack const *pion2   = mPicoDst->track(lambda->particle2Idx());

	  if ( ! mHFCuts->isHybridTOFProton(proton1, mHFCuts->getTofBeta(proton1, lambdaC.lorentzVector(), lambdaC.decayVertex(), 
									 lambda->lorentzVector(), lambda->decayVertex()), lambda->decayVertex()) ||
	       ! mHFCuts->isHybridTOFPion(pion2, mHFCuts->getTofBeta(pion2, lambdaC.lorentzVector(), lambdaC.decayVertex(), 
								     lambda->lorentzVector(), lambda->decayVertex()), lambda->decayVertex()) )
	    continue;

	  mPicoHFEvent->addHFSecondaryVertexPair(&lambdaC);

	} // for (unsigned int idxPion = 0; idxPion < mIdxPicoPions.size(); ++idxPion) {
      } // for (unsigned int idxLambda = 0; idxLambda <  mPicoHFEvent->nHFTertiaryVertices(); ++idxLambda) {
    } //  if (mPicoHFEvent->nHFTertiaryVertices() > 0) {

    // cout << "      N Lambdas  : " << mPicoHFEvent->nHFTertiaryVertices() << endl;
    // cout << "      N Lambda_C : " << mPicoHFEvent->nHFSecondaryVertices() << endl;
  }

  // == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == 
  // == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == 
  // == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == 

  // -- Decay channel proton pi+ K-
  else  if (mDecayChannel == StPicoHFLambdaCMaker::kPionKaonProton) {

    if (mIdxPicoPions.size() == 0 || mIdxPicoKaons.size() == 0)
    {
      return kStOK;
    }

    for (unsigned short idxProton = 0; idxProton < mIdxPicoProtons.size(); ++idxProton) {
      StPicoTrack const *proton = mPicoDst->track(mIdxPicoProtons[idxProton]);

      for (unsigned short idxKaon = 0; idxKaon < mIdxPicoKaons.size(); ++idxKaon) {
	StPicoTrack const *kaon = mPicoDst->track(mIdxPicoKaons[idxKaon]);

	if (proton == kaon)
	  continue;

	StHFClosePair tmpKaonProton(kaon, proton,
				    mHFCuts->getHypotheticalMass(StHFCuts::kKaon), mHFCuts->getHypotheticalMass(StHFCuts::kProton),
				    mIdxPicoKaons[idxKaon], mIdxPicoProtons[idxProton],
				    mPrimVtx, mBField);

	if (!mHFCuts->isClosePair(tmpKaonProton))
	  continue;

	for (unsigned short idxPion = 0; idxPion < mIdxPicoPions.size(); ++idxPion) {
	  StPicoTrack const *pion = mPicoDst->track(mIdxPicoPions[idxPion]);

	// StHFPair tmpKaonPion(kaon, pion,
	// 			  mHFCuts->getHypotheticalMass(StHFCuts::kKaon), mHFCuts->getHypotheticalMass(StHFCuts::kPion),
	// 			  mIdxPicoKaons[ idxKaon ], mIdxPicoPions[ idxPion ], 
	// 			  mPrimVtx, mBField);
        //
	// if(!mHFCuts->isClosePair(tmpKaonPion))
	//   continue;
	//   
	// if (tmpKaonPion.particle1Idx() > mIdxPicoKaons.size() || tmpKaonPion.particle2Idx() > mIdxPicoPions.size())
	//   continue;
        //
	  if (mIdxPicoProtons[idxProton] == mIdxPicoPions[idxPion] || mIdxPicoKaons[idxKaon] == mIdxPicoPions[idxPion]) 
	    continue;

	  // StHFTriplet lambdaC(&tmpKaonProton, pion,
			      // mHFCuts->getHypotheticalMass(StHFCuts::kPion),
			      // mIdxPicoPions[idxPion], mPrimVtx, mBField);
	  // StHFTriplet lambdaC(&tmpKaonPion, proton,
		   	      // mHFCuts->getHypotheticalMass(StHFCuts::kProton), 
		   	      // mIdxPicoProtons[idxProton], mPrimVtx, mBField);
	  StHFTriplet lambdaC(kaon, proton, pion,
			      mHFCuts->getHypotheticalMass(StHFCuts::kKaon), mHFCuts->getHypotheticalMass(StHFCuts::kProton), mHFCuts->getHypotheticalMass(StHFCuts::kPion),
			      mIdxPicoKaons[idxKaon], mIdxPicoProtons[idxProton], mIdxPicoPions[idxPion],
			      mPrimVtx, mBField);
	  if (!mHFCuts->isGoodSecondaryVertexTriplet(lambdaC)) 
	    continue;

	  // -- get corrected TOF beta
	  // ----------------------------

	  // -- check if all particles are still good
	  // if ( ! mHFCuts->isHybridTOFProton(proton, mHFCuts->getTofBeta(proton, lambdaC.lorentzVector(), lambdaC.decayVertex()), lambdaC.decayVertex()) ||
	  //      ! mHFCuts->isHybridTOFKaon(  kaon,   mHFCuts->getTofBeta(kaon,   lambdaC.lorentzVector(), lambdaC.decayVertex()), lambdaC.decayVertex()) ||
	  //      ! mHFCuts->isHybridTOFPion(  pion,   mHFCuts->getTofBeta(pion,   lambdaC.lorentzVector(), lambdaC.decayVertex()), lambdaC.decayVertex()) )
	  //   continue;
	  if ( ! mHFCuts->isTOFProton(proton) ||
	       ! mHFCuts->isTOFKaon(kaon)     ||
	       ! mHFCuts->isHybridTOFPion(pion) )
	    continue;
	  mPicoHFEvent->addHFSecondaryVertexTriplet(&lambdaC);

	} // for (unsigned short idxPion = 0; idxPion < mIdxPicoPions.size(); ++idxPion) {
      } // for (unsigned short idxKaon = 0; idxKaon < mIdxPicoKaons.size(); ++idxKaon) {
    } // for (unsigned short idxProton = 0; idxProton < mIdxPicoProtons.size(); ++idxProton) {

    //    cout << "      N Lambda_C : " << mPicoHFEvent->nHFSecondaryVertices() << endl;
    
  } // else  if (mDecayChannel == StPicoHFLambdaCMaker::kPionKaonProton) {
  
  return kStOK;
}

// _________________________________________________________
int StPicoHFLambdaCMaker::analyzeCandidates() {
  // -- analyze pairs/triplet, which have been filled in arrays 
  //    (in StPicoHFEvent) before ( via createCandidates() or via readCandidates() )
  //    fill histograms or nTuples ... etc

  // -- Decay channel proton - K0Short (pi+ - pi-)
  if (mDecayChannel == StPicoHFLambdaCMaker::kProtonK0short) {

    // -- fill nTuple / hists for tertiary K0shorts 
    // -----------------------------------------------
    TClonesArray const * aK0Short = mPicoHFEvent->aHFTertiaryVertices();

    for (unsigned int idxK0Short = 0; idxK0Short < mPicoHFEvent->nHFTertiaryVertices(); ++idxK0Short) {
      StHFPair const* k0Short = static_cast<StHFPair*>(aK0Short->At(idxK0Short));

      StPicoTrack const* pion1  = mPicoDst->track(k0Short->particle1Idx());
      StPicoTrack const* pion2  = mPicoDst->track(k0Short->particle2Idx());
      
      //      if (!mHFCuts->isGoodTertiaryVertexPair(k0Short)) 
      //	continue;

      float aTertiary[] = {pion1->gPt(), pion2->gPt(), Float_t(pion1->charge()*pion2->charge()),
			   k0Short->m(), k0Short->pt(), k0Short->eta(), k0Short->phi(), std::cos(k0Short->pointingAngle()),
			   k0Short->decayLength(), k0Short->particle1Dca(), k0Short->particle2Dca(),  
			   k0Short->cosThetaStar(), k0Short->dcaDaughters()};
      mNtupleTertiary->Fill(aTertiary);
			    			 
    } // for (unsigned int idxK0Short = 0; idxK0Short <  mPicoHFEvent->nHFTertiaryVertices(); ++idxK0Short) {


    // -- fill nTuple / hists for secondary lambdaCs
    // -----------------------------------------------
    TClonesArray const * aLambdaC = mPicoHFEvent->aHFSecondaryVertices();
    
    for (unsigned int idxLambdaC = 0; idxLambdaC < mPicoHFEvent->nHFSecondaryVertices(); ++idxLambdaC) {
      StHFPair const* lambdaC = static_cast<StHFPair*>(aLambdaC->At(idxLambdaC));
      StHFPair const* k0Short = static_cast<StHFPair*>(aK0Short->At(lambdaC->particle2Idx()));

      StPicoTrack const* proton = mPicoDst->track(lambdaC->particle1Idx());
      StPicoTrack const* pion1  = mPicoDst->track(k0Short->particle1Idx());
      StPicoTrack const* pion2  = mPicoDst->track(k0Short->particle2Idx());

      if ( ! mHFCuts->isHybridTOFHadron(pion1, mHFCuts->getTofBeta(pion1, lambdaC->lorentzVector(), lambdaC->decayVertex(),
								   k0Short->lorentzVector(), k0Short->decayVertex()), StPicoCutsBase::kPion, k0Short->decayVertex()) 
	   ||
	   ! mHFCuts->isHybridTOFHadron(pion2, mHFCuts->getTofBeta(pion2, lambdaC->lorentzVector(), lambdaC->decayVertex(),
								   k0Short->lorentzVector(), k0Short->decayVertex()), StPicoCutsBase::kPion, k0Short->decayVertex()) 
	   ||
	   ! mHFCuts->isHybridTOFHadron(proton, mHFCuts->getTofBeta(proton, lambdaC->lorentzVector(), lambdaC->decayVertex()), StPicoCutsBase::kProton, lambdaC->decayVertex()))
	continue;

      if (!mHFCuts->isGoodTertiaryVertexPair(k0Short)) 
      	continue;

      if (!mHFCuts->isGoodSecondaryVertexPair(lambdaC)) 
      	continue;
     
      float aSecondary[] = {proton->gPt(), k0Short->pt(), Float_t(pion1->charge()*pion2->charge()),
			    lambdaC->m(), lambdaC->pt(), lambdaC->eta(), lambdaC->phi(), std::cos(lambdaC->pointingAngle()),
			    lambdaC->decayLength(), lambdaC->particle1Dca(), lambdaC->particle2Dca(), 
			    lambdaC->cosThetaStar(), lambdaC->dcaDaughters(),
			    k0Short->m(), std::cos(k0Short->pointingAngle()),
			    k0Short->decayLength(), k0Short->particle1Dca(), k0Short->particle2Dca(),  
			    k0Short->cosThetaStar(), k0Short->dcaDaughters() };

      mNtupleSecondary->Fill(aSecondary);
            
    } // for (unsigned int idxLambdaC = 0; idxLambdaC <  mPicoHFEvent->nHFSecondaryVertices(); ++idxLambdaC) {
  } // if (mDecayChannel == StPicoHFLambdaCMaker::kProtonK0short) {

  // == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == 
  // == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == 
  // == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == 

  // -- Decay channel pi+ - lambda (proton - pi-)
  if (mDecayChannel == StPicoHFLambdaCMaker::kLambdaPion) {

    // -- fill nTuple / hists for tertiary Lambda
    // -----------------------------------------------
    TClonesArray const * aLambda = mPicoHFEvent->aHFTertiaryVertices();

    for (unsigned int idxLambda = 0; idxLambda < mPicoHFEvent->nHFTertiaryVertices(); ++idxLambda) {
      StHFPair const* lambda = static_cast<StHFPair*>(aLambda->At(idxLambda));

      StPicoTrack const* proton = mPicoDst->track(lambda->particle1Idx());
      StPicoTrack const* pion   = mPicoDst->track(lambda->particle2Idx());
      
      // if (!mHFCuts->isGoodTertiaryVertexPair(k0Short)) 
      // 	continue;
      
      float aTertiary[] = {proton->gPt(), pion->gPt(), Float_t(proton->charge()*pion->charge()),
			   lambda->m(), lambda->pt(), lambda->eta(), lambda->phi(), std::cos(lambda->pointingAngle()),
			   lambda->decayLength(), lambda->particle1Dca(), lambda->particle2Dca(),  
			   lambda->cosThetaStar(), lambda->dcaDaughters()};

      mNtupleTertiary->Fill(aTertiary);
			    			 
    } // for (unsigned int idxLambda = 0; idxLambda < mPicoHFEvent->nHFTertiaryVertices(); ++idxLambda) {


    // -- fill nTuple / hists for secondary lambdaCs
    // -----------------------------------------------
    TClonesArray const * aLambdaC = mPicoHFEvent->aHFSecondaryVertices();
    
    for (unsigned int idxLambdaC = 0; idxLambdaC < mPicoHFEvent->nHFSecondaryVertices(); ++idxLambdaC) {
      StHFPair const* lambdaC = static_cast<StHFPair*>(aLambdaC->At(idxLambdaC));
      StHFPair const* lambda = static_cast<StHFPair*>(aLambda->At(lambdaC->particle2Idx()));

      StPicoTrack const* pion1  = mPicoDst->track(lambdaC->particle1Idx());

      StPicoTrack const* proton = mPicoDst->track(lambda->particle1Idx());
      StPicoTrack const* pion2  = mPicoDst->track(lambda->particle2Idx());

      if ( ! mHFCuts->isHybridTOFHadron(proton, mHFCuts->getTofBeta(proton, lambdaC->lorentzVector(), lambdaC->decayVertex(),
								   lambda->lorentzVector(), lambda->decayVertex()), StPicoCutsBase::kProton, lambda->decayVertex()) ||
	   ! mHFCuts->isHybridTOFHadron(pion2, mHFCuts->getTofBeta(pion2, lambdaC->lorentzVector(), lambdaC->decayVertex(),
								   lambda->lorentzVector(), lambda->decayVertex()), StPicoCutsBase::kPion, lambda->decayVertex()) ||
	   ! mHFCuts->isHybridTOFHadron(pion1, mHFCuts->getTofBeta(pion1, lambdaC->lorentzVector(), lambdaC->decayVertex()), StPicoCutsBase::kPion, lambdaC->decayVertex()) )
	continue;
      
      if (!mHFCuts->isGoodTertiaryVertexPair(lambda)) 
       	continue;

      if (!mHFCuts->isGoodSecondaryVertexPair(lambdaC)) 
      	continue;
     
      float aSecondary[] = {pion1->gPt(), lambda->pt(), Float_t(proton->charge()*pion2->charge()),
			    lambdaC->m(), lambdaC->pt(), lambdaC->eta(), lambdaC->phi(), std::cos(lambdaC->pointingAngle()),
			    lambdaC->decayLength(), lambdaC->particle1Dca(), lambdaC->particle2Dca(), 
			    lambdaC->cosThetaStar(), lambdaC->dcaDaughters(),
			    lambda->m(), std::cos(lambda->pointingAngle()),
			    lambda->decayLength(), lambda->particle1Dca(), lambda->particle2Dca(),  
			    lambda->cosThetaStar(), lambda->dcaDaughters()};

      mNtupleSecondary->Fill(aSecondary);
            
    } // for (unsigned int idxLambdaC = 0; idxLambdaC <  mPicoHFEvent->nHFSecondaryVertices(); ++idxLambdaC) {
  } // if (mDecayChannel == StPicoHFLambdaCMaker::kLambdaPion) {

  // == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == 
  // == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == 
  // == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == 

  // -- Decay channel proton pi+ K-
  else  if (mDecayChannel == StPicoHFLambdaCMaker::kPionKaonProton) {
    
    // -- fill nTuple / hists for secondary lambdaCs
    // -----------------------------------------------
    TClonesArray const * aLambdaC = mPicoHFEvent->aHFSecondaryVertices();
    
    for (unsigned int idxLambdaC = 0; idxLambdaC <  mPicoHFEvent->nHFSecondaryVertices(); ++idxLambdaC) { 
      StHFTriplet const* lambdaC = static_cast<StHFTriplet*>(aLambdaC->At(idxLambdaC));

      // if (mPicoHFEvent->eventId() != 164112)
	// continue;
      // if(idxLambdaC != 43)
	// continue;
      // cout << idxLambdaC << endl;

      ++nProcessed;

      if (!mHFCuts->isGoodSecondaryVertexTriplet(lambdaC)) 
	continue;

      StPicoTrack const* kaon   = mPicoDst->track(lambdaC->particle1Idx());
      StPicoTrack const* proton = mPicoDst->track(lambdaC->particle2Idx());
      StPicoTrack const* pion   = mPicoDst->track(lambdaC->particle3Idx());
     
      if(!isKaon(kaon) || !isProton(proton) || !isPion(pion))
	continue;

      float const pionBeta = mHFCuts->getTofBeta(pion);
      float const kaonBeta = mHFCuts->getTofBeta(kaon);
      float const protonBeta = mHFCuts->getTofBeta(proton);

      StThreeVectorF const vtx = lambdaC->decayVertex();

      float const betaInvDiffPion = getBetaInvDiff(pion->gMom(vtx, mBField).mag(), pionBeta, mHFCuts->getHypotheticalMass(StHFCuts::kPion));
      float const betaInvDiffKaon = getBetaInvDiff(kaon->gMom(vtx, mBField).mag(), kaonBeta, mHFCuts->getHypotheticalMass(StHFCuts::kKaon));
      float const betaInvDiffProton = getBetaInvDiff(proton->gMom(vtx, mBField).mag(), protonBeta, mHFCuts->getHypotheticalMass(StHFCuts::kProton));
            
      // JMT - recalculate topological cuts with updated secondary vertex
      
      StHFPair KstarPair(pion, kaon,
	  mHFCuts->getHypotheticalMass(StHFCuts::kPion),
	  mHFCuts->getHypotheticalMass(StHFCuts::kKaon),
	  lambdaC->particle3Idx(),
	  lambdaC->particle1Idx(),
	  mPrimVtx, mBField);

      StHFPair LambdaPair(proton, kaon,
	  mHFCuts->getHypotheticalMass(StHFCuts::kProton),
	  mHFCuts->getHypotheticalMass(StHFCuts::kKaon),
	  lambdaC->particle2Idx() ,
	  lambdaC->particle1Idx() ,
	  mPrimVtx, mBField);
      
      StHFPair DeltaPair(proton, pion,
	  mHFCuts->getHypotheticalMass(StHFCuts::kProton),
	  mHFCuts->getHypotheticalMass(StHFCuts::kPion),
	  lambdaC->particle2Idx() ,
	  lambdaC->particle3Idx() ,
	  mPrimVtx, mBField);
      
      // distatces between closest points of pairs
      StThreeVectorF const vertexDistVec1 = LambdaPair.decayVertex() - DeltaPair.decayVertex(); // pK - pip
      StThreeVectorF const vertexDistVec2 = DeltaPair.decayVertex() - KstarPair.decayVertex();  // pip - piK
      StThreeVectorF const vertexDistVec3 = KstarPair.decayVertex() - LambdaPair.decayVertex(); // piK - pK

      float const vertexDist1 = vertexDistVec1.mag(); // pK - pip
      float const vertexDist2 = vertexDistVec2.mag(); // pip - piK
      float const vertexDist3 = vertexDistVec3.mag(); // piK - pK

      // calculating max distance between two v0s
      float maxVdist =  vertexDist1 > vertexDist2 ? vertexDist1 : vertexDist2;
      maxVdist = maxVdist > vertexDist3 ? maxVdist : vertexDist3;

      // sing bits the correct combinations are 3(011b Lambda_c^+) and 4(100b Lambda_c^-)
      int signBits = kaon->charge() > 0;
      signBits <<=1;
      signBits += (proton->charge() > 0);
      signBits <<=1;
      signBits += (pion->charge() > 0);

      float isCorrectSign = (kaon->charge() != pion->charge() && pion->charge() == proton->charge()) ? 1. : -1.;

      float const KEta = kaon->gMom(mPrimVtx, mBField).pseudoRapidity();
      float const pEta = proton->gMom(mPrimVtx, mBField).pseudoRapidity();
      float const piEta = pion->gMom(mPrimVtx, mBField).pseudoRapidity();
           
      float const KPhi = kaon->gMom(mPrimVtx, mBField).phi();
      float const pPhi = proton->gMom(mPrimVtx, mBField).phi();
      float const piPhi = pion->gMom(mPrimVtx, mBField).phi();

      int const centrality = mRefmultCorrUtil->getCentralityBin9() ;
      float const centralityCorrection = mRefmultCorrUtil->getWeight();

      float aSecondary[] = {kaon->gPt(), proton->gPt(), pion->gPt(), 
			    static_cast<Float_t>(signBits),
			    lambdaC->m(), lambdaC->pt(), lambdaC->eta(), lambdaC->phi(), 
			    static_cast<Float_t>( TMath::Cos( static_cast<Double_t>(lambdaC->pointingAngle()) ) ), lambdaC->decayLength(), 
			    static_cast<Float_t>( std::sin(lambdaC->pointingAngle())*lambdaC->decayLength() ),
			    lambdaC->particle1Dca(), lambdaC->particle2Dca(), lambdaC->particle3Dca(),
			    lambdaC->dcaDaughters12(), lambdaC->dcaDaughters23(), lambdaC->dcaDaughters31(),
			    LambdaPair.m(), DeltaPair.m(), KstarPair.m(),
			    kaon->nSigmaKaon(), proton->nSigmaProton(), pion->nSigmaPion(),
			    betaInvDiffKaon, betaInvDiffProton, betaInvDiffPion,
			    KEta, pEta, piEta, 
			    KPhi, pPhi, piPhi, 
			    maxVdist,
			    static_cast<Float_t>(centrality), centralityCorrection
			    };

      cout << "triplet in event number " << mPicoHFEvent->eventId() << endl;
      ++nTriplets;
      mNtupleSecondary->Fill(aSecondary);
      
    } // for (unsigned int idxLambdaC = 0; idxLambdaC <  mPicoHFEvent->nHFSecondaryVertices(); ++idxLambdaC) {
  } // else  if (mDecayChannel == StPicoHFLambdaCMaker::kPionKaonProton) {

  return kStOK;
}
// _________________________________________________________

int StPicoHFLambdaCMaker::fillSingleParticleHistos(int pidFlag) {
  std::string partName;
  std::vector<unsigned short>* partIdxVector;
  switch ( pidFlag )
  {
  case StHFCuts::kProton: 
    partName = "p";
    partIdxVector = &mIdxPicoProtons;
    break;
  case StHFCuts::kKaon:
    partName = "K";
    partIdxVector = &mIdxPicoKaons;
    break;
  case StHFCuts::kPion:
    partName = "pi";
    partIdxVector = &mIdxPicoPions;
    break;
  default:
    std::cerr << "StPicoHFLambdaCMaker::fillSingleParticleHistos: wrong particle code." << endl;
    return -1;
  }

  TH2D *etaPhiHist = static_cast<TH2D*>(mSinglePartList->FindObject( Form("%sEtaPhi", partName.data()) ));
  TH2D *phiPtHist = static_cast<TH2D*>(mSinglePartList->FindObject( Form("%sPhiPt", partName.data()) ));
  TH2D *nSigmaHist = static_cast<TH2D*>(mSinglePartList->FindObject( Form("%sNSigmaPt", partName.data()) ));
  
  for(unsigned short idxPart = 0; idxPart < partIdxVector->size(); ++idxPart)
  {
    StPicoTrack *const trk = mPicoDst->track((*partIdxVector)[idxPart]);

    float const pt = trk->gPt();
    StThreeVectorF const gMom = trk->gMom(mPrimVtx, mBField);
    etaPhiHist->Fill(gMom.phi(), gMom.pseudoRapidity());
    phiPtHist->Fill(pt, gMom.phi());

    float nSigma;
    switch (pidFlag)
    {
    case StHFCuts::kProton:
      nSigma = trk->nSigmaProton();
      break;
    case StHFCuts::kKaon:
      nSigma = trk->nSigmaKaon();
      break;
    case StHFCuts::kPion:
      nSigma = trk->nSigmaPion();
      break;
    }
    nSigmaHist->Fill(pt, nSigma);
  }
}
// _________________________________________________________

void StPicoHFLambdaCMaker::calculateCentrality()
{
  // getting centrality
  int const currentRun = mPicoHFEvent->runId();

  if(currentRun != mRunNumber)
  {
    // init a new run
    mRunNumber = currentRun;

    mRefmultCorrUtil->init(mRunNumber);
    mRefmultCorrUtil->setVzForWeight(6, -6.0, 6.0);
    mRefmultCorrUtil->readScaleForWeight("StRoot/StRefMultCorr/macros/weight_grefmult_vpd30_vpd5_Run14.txt");
  }

  mRefmultCorrUtil->initEvent(mPicoDst->event()->grefMult(), mPrimVtx.z(), mPicoDst->event()->ZDCx()) ;
}
// _________________________________________________________

int StPicoHFLambdaCMaker::fillControlHistos() {
  // fill control plots for single particles

  int const centrality = mRefmultCorrUtil->getCentralityBin9() ;
  float const centralityWeight = mRefmultCorrUtil->getWeight();
  static_cast<TH1D*>(mSinglePartList->FindObject("centrality"))->Fill(centrality);
  static_cast<TH1D*>(mSinglePartList->FindObject("centralityCorrection"))->Fill(centrality, centralityWeight);

  fillSingleParticleHistos(StHFCuts::kProton);
  fillSingleParticleHistos(StHFCuts::kPion);
  fillSingleParticleHistos(StHFCuts::kKaon);
}

// _________________________________________________________
inline float StPicoHFLambdaCMaker::getBetaInvDiff(float mom, float beta, float mass) {
  if(beta <= 0 || beta != beta) // if beta is NaN or less than 0
    return std::numeric_limits<float>::quiet_NaN();

  float const theoreticalBetaInv = sqrt( mass*mass + mom*mom )/mom;
  return 1./beta - theoreticalBetaInv;
}

// _________________________________________________________
inline bool StPicoHFLambdaCMaker::isApproxHybridTOFHadron(StPicoTrack const * const trk, int pidFlag) const {
  // TOF beta cut without corrections implemented to save time. 1.2 times beta cut is used.

  float const TOFbetaCut = mHFCuts->getTOFDeltaOneOverBetaMax(pidFlag);
  mHFCuts->setCutTOFDeltaOneOverBeta(1.6*TOFbetaCut, pidFlag);
  bool const isTOFhadron = mHFCuts->isHybridTOFHadron(trk, mHFCuts->getTofBetaBase(trk), pidFlag, mPrimVtx);
  mHFCuts->setCutTOFDeltaOneOverBeta(TOFbetaCut, pidFlag);
  return isTOFhadron;
}

// _________________________________________________________
bool StPicoHFLambdaCMaker::isHadron(StPicoTrack const * const trk, int pidFlag) const {
  // -- is good hadron
  //    -> used for initial filling of vectors only

  // double eta = trk->gMom(mPrimVtx,mBField).pseudoRapidity();  
  // return (mHFCuts->isGoodTrack(trk) && mHFCuts->cutMinDcaToPrimVertex(trk, pidFlag) && mHFCuts->isTPCHadron(trk, pidFlag) && abs(eta) < 1.);
  bool isGoodTOF = true;
  if (mDecayChannel == StPicoHFLambdaCMaker::kPionKaonProton)
  {
    if(pidFlag == StHFCuts::kPion)
      isGoodTOF = mHFCuts->isHybridTOFHadron(trk, mHFCuts->getTofBeta(trk), pidFlag, mPrimVtx);
    else
      isGoodTOF = mHFCuts->isTOFHadron(trk, mHFCuts->getTofBeta(trk), pidFlag, mPrimVtx);
  }
  else
    isGoodTOF = isApproxHybridTOFHadron(trk, pidFlag);

  return (mHFCuts->isGoodTrack(trk) && mHFCuts->cutMinDcaToPrimVertex(trk, pidFlag) 
	  && mHFCuts->isTPCHadron(trk, pidFlag)
	  && isGoodTOF);
}
  
// _________________________________________________________
bool StPicoHFLambdaCMaker::isPion(StPicoTrack const * const trk) const {
  // -- is good pion 
  //    -> used for initial filling of vectors only

  return isHadron(trk, StPicoCutsBase::kPion);
}

// _________________________________________________________
bool StPicoHFLambdaCMaker::isKaon(StPicoTrack const * const trk) const {
  // -- is good kaon 
  //    -> used for initial filling of vectors only

  return isHadron(trk, StPicoCutsBase::kKaon);
}

// _________________________________________________________
bool StPicoHFLambdaCMaker::isProton(StPicoTrack const * const trk) const {
  // -- good proton
  //    -> used for initial filling of vectors only
  
  return isHadron(trk, StPicoCutsBase::kProton);
}

