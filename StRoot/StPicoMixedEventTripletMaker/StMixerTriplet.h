#ifndef StMixerTriplet_hh
#define StMixerTriplet_hh

/* **************************************************
 *  Generic class calculating and storing triplets in Event Mixing wrapped arround the StHFTriplet class
 *  Allows to combine:
 *  - three particles, using
 *      StMixerTriplet(StPicoTrack const * particle1, StPicoTrack const * particle2, ...
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
#include "StPicoHFMaker/StHFTriplet.h"

class StMixerTrack;
class StHFTriplet;

class StMixerTriplet : public StHFTriplet
{
 public:
  StMixerTriplet(StMixerTrack const&  particle1, StMixerTrack const& particle2, StMixerTrack const& particle3,
	   float p1MassHypo, float p2MassHypo, float p3MassHypo,
	   StThreeVectorF const& vtx1, StThreeVectorF const& vtx2, StThreeVectorF const& vtx3,
	   float bField);

  ~StMixerTriplet() {;}
  

  StThreeVectorF const & particle1Mom() const;
  StThreeVectorF const & particle2Mom() const;
  StThreeVectorF const & particle3Mom() const;

 private:
  StMixerTriplet(StMixerTriplet const &);
  StMixerTriplet& operator=(StMixerTriplet const &);
  StThreeVectorF mParticle1Mom;
  StThreeVectorF mParticle2Mom;
  StThreeVectorF mParticle3Mom;
  ClassDef(StMixerTriplet,1)
};
inline StThreeVectorF const & StMixerTriplet::particle1Mom() const { return mParticle1Mom; }
inline StThreeVectorF const & StMixerTriplet::particle2Mom() const { return mParticle2Mom; }
inline StThreeVectorF const & StMixerTriplet::particle3Mom() const { return mParticle3Mom; }
#endif
