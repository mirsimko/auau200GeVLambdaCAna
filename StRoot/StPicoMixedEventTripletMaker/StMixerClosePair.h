#ifndef StMixerClosePair_hh
#define StMixerClosePair_hh

/* **************************************************
 *  ClosePairClass for storing pairs for a future calculation of a triplet.
 *  Some topological cuts can be performed on the pair to save computaion time.
 *
 *  Make sure that this does not happen (no virtual destructiors in the base class):
 *  StMixerClosePair *ClosePair = new StMixerClosePair(...);
 *  StHFClosePair *tr = ClosePair;
 *  delete tr;
 *
 *
 * **************************************************
 *
 *  Initial Authors: 
 *            Michael Lomnitz (mrlomnitz@lbl.gov)
 *            Mustafa Mustafa (mmustafa@lbl.gov)
 *          **Miroslav Simko  (msimko@bnl.gov)
 *
 *  ** Code Maintainer 
 *
 * **************************************************
 */

#include "TObject.h"
#include "StarClassLibrary/StLorentzVectorF.hh"
#include "StarClassLibrary/StThreeVectorF.hh"
#include "StMixerTrack.h"
#include "StPicoHFMaker/StHFClosePair.h"

class StMixerTrack;
class StHFClosePair;

class StMixerClosePair : public StHFClosePair
{
 public:
  StMixerClosePair(StMixerTrack const &  particle1, StMixerTrack const & particle2,
		 float p1MassHypo, float p2MassHypo,
		 StThreeVectorF const & vtx1, StThreeVectorF const & vtx2,
		 float bField);

  ~StMixerClosePair() {;}
  

  StThreeVectorF const & particle1Mom() const;
  StThreeVectorF const & particle2Mom() const;
  StThreeVectorF const & particle3Mom() const;

 private:
  StMixerClosePair(StMixerClosePair const &);
  StMixerClosePair& operator=(StMixerClosePair const &);
  StThreeVectorF mParticle1Mom;
  StThreeVectorF mParticle2Mom;
  ClassDef(StMixerClosePair,1)
};
inline StThreeVectorF const & StMixerClosePair::particle1Mom() const { return mParticle1Mom; }
inline StThreeVectorF const & StMixerClosePair::particle2Mom() const { return mParticle2Mom; }
inline StThreeVectorF const & StMixerClosePair::particle3Mom() const { return mParticle3Mom; }
#endif
