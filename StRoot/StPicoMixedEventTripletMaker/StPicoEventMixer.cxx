#include <limits>

#include "TTree.h"
#include "TH2F.h"

#include "StPicoEventMixer.h"
#include "StPicoDstMaker/StPicoEvent.h"
#include "StPicoDstMaker/StPicoTrack.h"
#include "StPicoDstMaker/StPicoDst.h"

#include "StPicoMixedEventMaker.h"
#include "StMixerEvent.h"
#include "StMixerTriplet.h"
#include "StMixerHists.h"
#include "StPicoHFMaker/StHFCuts.h"
#include "StPicoHFMaker/StHFClosePair.h"
#include "StPicoHFMaker/StHFTriplet.h"

//-----------------------------------------------------------
StPicoEventMixer::StPicoEventMixer(char* category):
  mEvents(),
  mHists(NULL), 
  mHFCuts(NULL),
  mEventsBuffer(5), 
  filledBuffer(0)
{
  mHists = new StMixerHists(category);
}
//-----------------------------------------------------------
StPicoEventMixer::~StPicoEventMixer()
{
  delete mHists; 
  for(unsigned int i =0 ; i<mEvents.size() ; i++){
    delete mEvents.at(i);
  }
}
//-----------------------------------------------------------
void StPicoEventMixer::finish() {
  mHists->closeFile();
}
//-----------------------------------------------------------
bool StPicoEventMixer::addPicoEvent(StPicoDst const* const picoDst)
{
  if( !isGoodEvent(picoDst) )
    return false;
  int nTracks = picoDst->numberOfTracks();
  StThreeVectorF pVertex = picoDst->event()->primaryVertex();
  StMixerEvent* event = new StMixerEvent(pVertex, picoDst->event()->bField());
  bool isTpcPi = false;
  bool isTofPi = false;
  bool isTpcP  = false;
  bool isTofP  = false;
  bool isTpcK  = false;
  bool isTofK  = false;
  //Event.setNoTracks( nTracks );
  for( int iTrk = 0; iTrk < nTracks; ++iTrk) {
    StPicoTrack const* trk = picoDst->track(iTrk);

    if(!mHFCuts->isGoodTrack(trk))
      continue;

    const float beta = mHFCuts->getTofBeta(trk);

    bool saveTrack = false;
    if( isTpcPion(trk) && mHFCuts->isHybridTOFPion(trk, beta, pVertex)) {
      isTpcPi = true;
      isTofPi = true;
      event->addPion(event->getNoTracks());
      saveTrack = true;
    }
    if(isTpcKaon(trk) && mHFCuts->isTOFKaon(trk, beta, pVertex)) {
      isTpcK = true;
      isTofK = true;
      saveTrack = true;
      event->addKaon(event->getNoTracks());
    }
    if(isTpcProton(trk) && mHFCuts->isTOFProton(trk, beta, pVertex)) {
      isTpcP = true;
      isTofP = true;
      saveTrack = true;
      event->addProton(event->getNoTracks());
    }
    if(saveTrack == true){
      StMixerTrack mTrack(pVertex, picoDst->event()->bField(), *trk, isTpcPi, isTofPi, isTpcK, isTofK, isTpcP, isTofP);
      event->addTrack(mTrack);
    }
  }
  if ( event->getNoPions() > 0 ||  event->getNoKaons() > 0 || event->getNoProtons() > 0) {
    mEvents.push_back(event);
    filledBuffer+=1;
  }
  else {
    delete event;
    return false;
  }
  //Returns true if need to do mixing, false if buffer has space still
  if ( filledBuffer == mEventsBuffer - 1 )
    return true;
  return false;
}
//-----------------------------------------------------------
void StPicoEventMixer::mixEvents() {
  size_t const nEvent = mEvents.size();
  int const nTracksEvt1 = mEvents.at(0)->getNoProtons();
  if(nTracksEvt1 == 0)
  {
    --filledBuffer;
    return;
  }
  // Go through the event buffer
  for( size_t iEvt2 = 0; iEvt2 < nEvent; ++iEvt2) {
    int const nTracksEvt2 = mEvents.at(iEvt2)->getNoKaons();
    for (size_t iEvt3 = 0; iEvt3 < nEvent; ++iEvt3) {
      if( iEvt2 == 0  && iEvt3 == 0)
	mHists->fillSameEvt(mEvents.at(0)->vertex());
      else
      {
	if(iEvt3 == iEvt2 || iEvt2 == 0 || iEvt3 == 0)
	  continue;

	mHists->fillMixedEvt(mEvents.at(0)->vertex());
      }
      int const nTracksEvt3 = mEvents.at(iEvt3)->getNoPions();

      // evts trk loops
      for(int iTrk1 = 0; iTrk1 < nTracksEvt1; ++iTrk1) {
	StMixerTrack const proton = mEvents.at(0)->protonAt(iTrk1);
	for( int iTrk2 = 0; iTrk2 < nTracksEvt2; ++iTrk2) {
	  // check if the tracks are the same
	  if(iEvt2 == 0)
	  {
	    if(mEvents.at(0)->protonId(iTrk1) == mEvents.at(iEvt2)->kaonId(iTrk2))
	      continue;
	  }

	  StMixerTrack const kaon = mEvents.at(iEvt2)->kaonAt(iTrk2);

	  StMixerClosePair pair(proton, kaon,
				mHFCuts->getHypotheticalMass(StHFCuts::kProton),
				mHFCuts->getHypotheticalMass(StHFCuts::kKaon),
				mEvents.at(0)->vertex(), mEvents.at(iEvt2)->vertex(),
				mEvents.at(0)->field() );

	  // check cuts
	  if (! mHFCuts -> isClosePair(static_cast<StHFClosePair &>(pair)))
	    continue;

	  for( int iTrk3 = 0; iTrk3 < nTracksEvt3; ++iTrk3) {
	    StMixerTrack const pion = mEvents.at(iEvt3)->pionAt(iTrk3);

	    // check if any of te tracks are the same
	    if(iEvt2 == 0 && iEvt3 == 0) {
	      StMixerEvent * event = mEvents.at(0);
	      if(event->pionId(iTrk3) == event->kaonId(iTrk2) || event->pionId(iTrk3) == event->protonId(iTrk1) || event->kaonId(iTrk2) == event->protonId(iTrk1)) {
		continue;
	      }
	    }

	    StMixerTriplet triplet(pair, pion,
				   mHFCuts->getHypotheticalMass(StHFCuts::kPion), 
				   mEvents.at(0)->vertex(), mEvents.at(iEvt3)->vertex(),
				   mEvents.at(0)->field() );

	    // check cuts
	    if(!mHFCuts->isGoodSecondaryVertexTriplet(static_cast<StHFTriplet &>(triplet) ) )
	      continue;

	    int signBits = kaon.charge() > 0;
	    signBits <<=1;
	    signBits += (proton.charge() > 0);
	    signBits <<=1;
	    signBits += (pion.charge() > 0);

	    if(iEvt2 == 0 && iEvt2 == 0)
	      mHists->fillSameEvtTriplet(&triplet, signBits );
	    else
	      mHists->fillMixedEvtTriplet(&triplet, signBits);
	  } //first event track loop 
	} //second event track loop
      } // the third event track loop
    } //loop over the third events
  } // loop over the second Events
  --filledBuffer;
  delete mEvents.at(0);
  mEvents.erase(mEvents.begin());
}
// _________________________________________________________
bool StPicoEventMixer::isMixerPion(StMixerTrack const& track) {
  short info = track.getTrackInfo();
  //TPC pion
  if( (info & (1 << kPionTPCbit)) >> kPionTPCbit != 1) return false;
  //TOF pion
  if( (info & (1 << kPionTOFbit)) >> kPionTOFbit != 1) return false;
  return true;
}
// _________________________________________________________
bool StPicoEventMixer::isMixerKaon(StMixerTrack const& track) {
  short info = track.getTrackInfo();
  //TPC Kaon
  if( (info & (1 << kKaonTPCbit)) >> kKaonTPCbit != 1) return false;
  //TOF Kaon
  if( (info & (1 << kKaonTOFbit)) >> kKaonTOFbit != 1) return false;
  return true;
}
//-----------------------------------------------------------
bool StPicoEventMixer::isMixerProton(StMixerTrack const& track) {
  short info = track.getTrackInfo();
  //TPC Proton
  if( (info & (1 << kProtonTPCbit)) >> kProtonTPCbit != 1) return false;
  //TOF Proton
  if( (info & (1 << kProtonTOFbit)) >> kProtonTOFbit != 1) return false;
  return true;
}
//-----------------------------------------------------------
bool StPicoEventMixer::isGoodEvent(StPicoDst const * const picoDst)
{
  return (mHFCuts->isGoodEvent(picoDst));
}
//-----------------------------------------------------------
bool StPicoEventMixer::isTpcPion(StPicoTrack const * const trk)
{
  return( isTPCHadron(trk, StPicoCutsBase::kPion));
}
//-----------------------------------------------------------
bool StPicoEventMixer::isTpcKaon(StPicoTrack const * const trk)
{
  return( isTPCHadron(trk, StPicoCutsBase::kKaon));
}
//-----------------------------------------------------------
bool StPicoEventMixer::isTpcProton(StPicoTrack const * const trk)
{
  return( isTPCHadron(trk, StPicoCutsBase::kProton));
}
//-----------------------------------------------------------
bool StPicoEventMixer::isTPCHadron(StPicoTrack const * const trk, int pidFlag)
{
  return( mHFCuts->isTPCHadron(trk, pidFlag));
}
//-----------------------------------------------------------
bool StPicoEventMixer::isGoodTrack(StPicoTrack const * const trk)
{
  return (mHFCuts->isGoodTrack(trk));
}
//-----------------------------------------------------------
bool StPicoEventMixer::isGoodTriplet(StMixerTriplet const& triplet)
{
  // int ptIndex = getLcPtIndex(triplet);
  return mHFCuts->isGoodSecondaryVertexTriplet(triplet);
}
//-----------------------------------------------------------------------------
int StPicoEventMixer::getLcPtIndex(StMixerTriplet const& triplet) const
{
  return 0; // so far, we only use one pT index	
}
//-----------------------------------------------------------
bool StPicoEventMixer::isGoodTrigger(StPicoEvent const * const mPicoEvent) const 
{
  return mHFCuts->isGoodTrigger(mPicoEvent);
}
