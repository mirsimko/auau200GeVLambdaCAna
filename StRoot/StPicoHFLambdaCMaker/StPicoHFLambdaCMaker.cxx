#include <vector>

#include "TNtuple.h"
#include "TClonesArray.h"

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
#include "StPicoHFMaker/StHFTriplet.h"

#include "StPicoHFLambdaCMaker.h"

ClassImp(StPicoHFLambdaCMaker)

// _________________________________________________________
StPicoHFLambdaCMaker::StPicoHFLambdaCMaker(char const* name, StPicoDstMaker* picoMaker, char const* outputBaseFileName,  
					   char const* inputHFListHFtree = "") :
  StPicoHFMaker(name, picoMaker, outputBaseFileName, inputHFListHFtree),
  mDecayChannel(kPionKaonProton), mNtupleSecondary(NULL), mNtupleTertiary(NULL) {
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
      mNtupleSecondary = new TNtuple("secondary", "secondary", "p1pt:p2pt:p3pt:charges:m:pt:eta:phi:cosPntAngle:dLength:p1Dca:p2Dca:p3Dca:cosThetaStar:dcaDaugthers12:dcaDaugthers23:dcaDaugthers31");
  }
  
  return kStOK;
}

// _________________________________________________________
int StPicoHFLambdaCMaker::FinishHF() {
  
  if (isMakerMode() != StPicoHFMaker::kWrite) {
    mNtupleSecondary->Write();
    
    if (isDecayMode() == StPicoHFEvent::kTwoAndTwoParticleDecay)
      mNtupleTertiary->Write();
  }
  
  return kStOK;
}

// _________________________________________________________
int StPicoHFLambdaCMaker::MakeHF() {

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

  return kStOK;
}

// _________________________________________________________
int StPicoHFLambdaCMaker::createCandidates() {
  // create candidate pairs/ triplet and fill them in arrays (in StPicoHFEvent)

  // cout << " N pions    : " << mIdxPicoPions.size()  << endl;
  // cout << " N kaons    : " << mIdxPicoKaons.size()  << endl;
  // cout << " N protons  : " << mIdxPicoProtons.size()  << endl;
  
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
	  if ( ! mHFCuts->isHybridTOFProton(proton, mHFCuts->getTofBeta(proton, lambdaC.lorentzVector(), lambdaC.decayVertex())) )
	    continue;
	  
	  // -- check if both pions are still good pions
	  StPicoTrack const *pion1 = mPicoDst->track(k0Short->particle1Idx());	  
	  StPicoTrack const *pion2 = mPicoDst->track(k0Short->particle2Idx());	  
	  
	  if ( ! mHFCuts->isHybridTOFPion(pion1, mHFCuts->getTofBeta(pion1, lambdaC.lorentzVector(), lambdaC.decayVertex(), 
								     k0Short->lorentzVector(), k0Short->decayVertex())) ||
	       ! mHFCuts->isHybridTOFPion(pion2, mHFCuts->getTofBeta(pion2, lambdaC.lorentzVector(), lambdaC.decayVertex(), 
								     k0Short->lorentzVector(), k0Short->decayVertex())) )
	    continue;

	  mPicoHFEvent->addHFSecondaryVertexPair(&lambdaC);

	} // for (unsigned int idxProton = 0; idxProton < mIdxPicoProtons.size(); ++idxProton) {	  
      } // for (unsigned int idxK0Short = 0; idxK0Short <  mPicoHFEvent->nHFTertiaryVertices(); ++idxK0Short) {
    } //  if (mPicoHFEvent->nHFTertiaryVertices() > 0) {

    //    cout << "      N K0Shorts : " << mPicoHFEvent->nHFTertiaryVertices() << endl;
    //    cout << "      N Lambda_C : " << mPicoHFEvent->nHFSecondaryVertices() << endl;

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
	  if ( ! mHFCuts->isHybridTOFPion(pion, mHFCuts->getTofBeta(pion, lambdaC.lorentzVector(), lambdaC.decayVertex())) )
	    continue;

	  // -- check if both lambda daughthers (proton + pi-) are still good 
	  StPicoTrack const *proton1 = mPicoDst->track(lambda->particle1Idx());	  
	  StPicoTrack const *pion2   = mPicoDst->track(lambda->particle2Idx());	  

	  if ( ! mHFCuts->isHybridTOFProton(proton1, mHFCuts->getTofBeta(proton1, lambdaC.lorentzVector(), lambdaC.decayVertex(), 
									 lambda->lorentzVector(), lambda->decayVertex())) ||
	       ! mHFCuts->isHybridTOFPion(pion2, mHFCuts->getTofBeta(pion2, lambdaC.lorentzVector(), lambdaC.decayVertex(), 
								     lambda->lorentzVector(), lambda->decayVertex())) )
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

    for (unsigned short idxProton = 0; idxProton < mIdxPicoProtons.size(); ++idxProton) {
      StPicoTrack const *proton = mPicoDst->track(mIdxPicoProtons[idxProton]);
      
      for (unsigned short idxKaon = 0; idxKaon < mIdxPicoKaons.size(); ++idxKaon) {
	StPicoTrack const *kaon = mPicoDst->track(mIdxPicoKaons[idxKaon]);
	
	if (mIdxPicoKaons[idxKaon] == mIdxPicoProtons[idxProton]) 
	  continue;
	
	StHFPair tmpProtonKaon(kaon, proton, 
			       mHFCuts->getHypotheticalMass(StHFCuts::kKaon), mHFCuts->getHypotheticalMass(StHFCuts::kProton), 
			       mIdxPicoKaons[idxKaon], mIdxPicoProtons[idxProton], mPrimVtx, mBField);
	if (!mHFCuts->isClosePair(tmpProtonKaon)) 
	  continue;
	
  	for (unsigned short idxPion = 0; idxPion < mIdxPicoPions.size(); ++idxPion) {
	  StPicoTrack const *pion = mPicoDst->track(mIdxPicoPions[idxPion]);
	  
	  if (mIdxPicoProtons[idxProton] == mIdxPicoPions[idxPion] || mIdxPicoKaons[idxKaon] == mIdxPicoPions[idxPion]) 
	    continue;
	  
	  StHFTriplet lambdaC(kaon, proton, pion, 
			      mHFCuts->getHypotheticalMass(StHFCuts::kKaon), mHFCuts->getHypotheticalMass(StHFCuts::kProton), 
			      mHFCuts->getHypotheticalMass(StHFCuts::kPion), 
			      mIdxPicoKaons[idxKaon], mIdxPicoProtons[idxProton], mIdxPicoPions[idxPion], mPrimVtx, mBField);
	  if (!mHFCuts->isGoodSecondaryVertexTriplet(lambdaC)) 
	    continue;
	  
	  // -- get corrected TOF beta
	  // ----------------------------

	  // -- check if all particles are still good
	  if ( ! mHFCuts->isHybridTOFProton(proton, mHFCuts->getTofBeta(proton, lambdaC.lorentzVector(), lambdaC.decayVertex())) ||
	       ! mHFCuts->isHybridTOFKaon(  kaon,   mHFCuts->getTofBeta(kaon,   lambdaC.lorentzVector(), lambdaC.decayVertex())) ||
	       ! mHFCuts->isHybridTOFPion(  pion,   mHFCuts->getTofBeta(pion,   lambdaC.lorentzVector(), lambdaC.decayVertex())) )
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
      
      // if (!isPion(pion1) || !isPion(pion1))
      // 	continue;

      // if (!mHFCuts->isGoodTertiaryVertexPair(k0Short)) 
      // 	continue;

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

      // if (!isProton(proton) || !isPion(pion1) || !isPion(pion1))
      // 	continue;
      
      
      // if (!mHFCuts->isGoodTertiaryVertexPair(k0Short)) 
      // 	continue;

      // if (!mHFCuts->isGoodSecondaryVertexPair(lambdaC)) 
      // 	continue;
     
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
      
      // if (!isPion(pion1) || !isPion(pion1))
      // 	continue;

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

      StPicoTrack const* pion1 = mPicoDst->track(lambdaC->particle1Idx());
      StPicoTrack const* proton  = mPicoDst->track(lambda->particle1Idx());
      StPicoTrack const* pion2  = mPicoDst->track(lambda->particle2Idx());

      // if (!isProton(proton) || !isPion(pion1) || !isPion(pion1))
      // 	continue;
      
      
      // if (!mHFCuts->isGoodTertiaryVertexPair(k0Short)) 
      // 	continue;

      // if (!mHFCuts->isGoodSecondaryVertexPair(lambdaC)) 
      // 	continue;
     
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

      StPicoTrack const* proton = mPicoDst->track(lambdaC->particle1Idx());
      StPicoTrack const* kaon   = mPicoDst->track(lambdaC->particle2Idx());
      StPicoTrack const* pion   = mPicoDst->track(lambdaC->particle3Idx());

      // if (!isProton(proton) || !isKaon(kaon) || !isPion(pion))
      // 	continue;
      
      // JMT - recalculate topological cuts with updated secondary vertex
      
      if (!mHFCuts->isGoodSecondaryVertexTriplet(lambdaC)) 
	continue;
     
      float isCorrectSign = (kaon->charge() != pion->charge() && pion->charge() == proton->charge()) ? 1. : -1.;

      float aSecondary[] = {proton->gPt(), kaon->gPt(), pion->gPt(), 
			    isCorrectSign,
			    lambdaC->m(), lambdaC->pt(), lambdaC->eta(), lambdaC->phi(), 
			    lambdaC->pointingAngle(), lambdaC->decayLength(), 
			    lambdaC->particle1Dca(), lambdaC->particle2Dca(), lambdaC->particle3Dca(),
			    lambdaC->cosThetaStar(),
			    lambdaC->dcaDaughters12(), lambdaC->dcaDaughters23(), lambdaC->dcaDaughters31()};

      mNtupleSecondary->Fill(aSecondary);
      
    } // for (unsigned int idxLambdaC = 0; idxLambdaC <  mPicoHFEvent->nHFSecondaryVertices(); ++idxLambdaC) {
  } // else  if (mDecayChannel == StPicoHFLambdaCMaker::kPionKaonProton) {

 return kStOK;
}
  
// _________________________________________________________
bool StPicoHFLambdaCMaker::isPion(StPicoTrack const * const trk) const {
  // -- is good pion 
  //    -> used for initial filling of vectors only
  return (mHFCuts->isGoodTrack(trk) && mHFCuts->isTPCPion(trk) && mHFCuts->isHybridTOFPion(trk));
}

// _________________________________________________________
bool StPicoHFLambdaCMaker::isKaon(StPicoTrack const * const trk) const {
  // -- is good kaon 
  //    -> used for initial filling of vectors only
  return (mHFCuts->isGoodTrack(trk) && mHFCuts->isTPCKaon(trk) && mHFCuts->isHybridTOFKaon(trk));
} 

// _________________________________________________________
bool StPicoHFLambdaCMaker::isProton(StPicoTrack const * const trk) const {
  // -- good proton
  //    -> used for initial filling of vectors only
  return (mHFCuts->isGoodTrack(trk) && mHFCuts->isTPCProton(trk) && mHFCuts->isHybridTOFProton(trk));
}

