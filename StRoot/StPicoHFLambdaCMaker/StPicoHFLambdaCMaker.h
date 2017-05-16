#ifndef StPicoHFLambdaCMaker_h
#define StPicoHFLambdaCMaker_h

#include "StPicoHFMaker/StPicoHFMaker.h"
#include "StRefMultCorr/StRefMultCorr.h"

/* **************************************************
 *  Class for HF LambdaC analysis
 * --------------------------------------------------
 * 
 *  For more info look also in the .h files in StPicoHFMaker/
 *     StPicoHFMaker/StPicoHFMaker.h      <-- Base Class for analysis
 *     StPicoHFMaker/StPicoHFEvent.h      <-- Holds candidates for one event (written to Tree)
 *     StPicoHFMaker/StHFCuts.h           <-- Cuts, can be set in run macro
 *     StPicoHFMaker/StHFPair.h           <-- Holds a pair candidate of a two body decay
 *     StPicoHFMaker/StHFTriplet.h        <-- Holds a triplet of a three body decay
 *
 *  Usage:
 *   - Implement
 *        InitHF()
 *        MakeHF()
 *        ClearHF()
 *        FinishHF()
 *
 *  - Do not ovewrite Init, Make, Clear, Finish which are inhertited from StPicoHFMaker via StMaker 

 *  - Set StHFCuts class via setHFBaseCuts(...) in run macro
 *
 *  - Set use mode of StPicoHFMaker class  via setMakerMode(...)
 *     use enum of StPicoHFMaker::eMakerMode
 *      StPicoHFMaker::kAnalyze - don't write candidate trees, just fill histograms
 *      StPicoHFMaker::kWrite   - write candidate trees
 *      StPicoHFMaker::kRead    - read candidate trees and fill histograms
 *
 *  - Set decay mode of analysis via setDecayMode(...)
 *     use enum of StPicoHFEvent::eHFEventMode (see there for more info)
 *      StPicoHFEvent::kTwoParticleDecay,
 *      StPicoHFEvent::kThreeParticleDecay
 *      StPicoHFEvent::kTwoAndTwoParticleDecay
 *
 *  - Implement these track selection methods used to fill vectors for 'good' identified particles
 *      (methods from StHFCuts utility class can/should be used)
 *       isPion
 *       isKaon
 *       isProton
 *
 *  --------------------------------------------------
 *  Authors: 
 *            Jochen Thaeder (jmthader@lbl.gov)
 *            Miroslav Simko (msimko@bnl.gov)
 *
 * **************************************************
 */

class TNtuple;

class StPicoDst;
class StPicoDstMaker;
class StPicoEvent;
class StPicoTrack;
class StPicoHFEvent;

class StHFPair;
class StHFTriplet;
class StHFCuts;
class StRefMultCorr;

class StPicoHFLambdaCMaker : public StPicoHFMaker 
{
 public:
  StPicoHFLambdaCMaker(char const* name, StPicoDstMaker* picoMaker, char const* outputBaseFileName,  
		       char const* inputHFListHFtree);
  virtual ~StPicoHFLambdaCMaker();
  
  virtual Int_t InitHF();
  virtual Int_t MakeHF();
  //    virtual Int_t ClearHF();
  virtual Int_t FinishHF();
  
  void setDecayChannel(unsigned int u) { mDecayChannel = u; }

  enum eDecayChannel {kPionKaonProton, kProtonK0short, kLambdaPion};

  void setRefMutCorr(StRefMultCorr* gRefMultCorr) { mRefmultCorrUtil = gRefMultCorr; }
  StRefMultCorr* getRefMultCorr() { return mRefmultCorrUtil; }
  void fillSingleParticleHists(bool shouldWe = true) { mFillParticleHistos = shouldWe; }

protected:
  virtual bool isHadron(StPicoTrack const*, int pidFlag) const;
  virtual bool isPion(StPicoTrack const*) const;
  virtual bool isKaon(StPicoTrack const*) const;
  virtual bool isProton(StPicoTrack const*) const;


private:
  int createCandidates();
  int analyzeCandidates();
  void fillControlHistos();
  void fillSingleParticleHistos(int pidFlag);
  void calculateCentrality();

  // -- private members --------------------------

  unsigned int mDecayChannel;

  TNtuple*  mNtupleSecondary;
  TNtuple*  mNtupleTertiary;
  TList*    mSinglePartList;

  StRefMultCorr* mRefmultCorrUtil ;

  bool mFillParticleHistos;

  int mRunNumber;
  long nProcessed;
  long nTriplets;

  inline float getBetaInvDiff(float mom, float beta, float mass);
  inline bool isApproxHybridTOFHadron(StPicoTrack const * const, int pidFlag) const;

  ClassDef(StPicoHFLambdaCMaker, 1)
};

#endif
