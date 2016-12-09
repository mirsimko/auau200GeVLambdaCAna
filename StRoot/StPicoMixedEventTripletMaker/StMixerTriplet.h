#ifndef StMixerTriplet_hh
#define StMixerTriplet_hh

/* **************************************************
 *  Generic class calculating and storing pairs in Event Mixing
 *  Allows to combine:
 *  - two particles, using
 *      StMixerTriplet(StPicoTrack const * particle1, StPicoTrack const * particle2, ...
 *
 * **************************************************
 *
 *  Initial Authors: 
 *         ** Michael Lomnitz (mrlomnitz@lbl.gov)
 *            Mustafa Mustafa (mmustafa@lbl.gov)
 *
 *  ** Code Maintainer 
 *
 * **************************************************
 */

#include "TObject.h"
#include "StarClassLibrary/StLorentzVectorF.hh"
#include "StarClassLibrary/StThreeVectorF.hh"

class StMixerTrack;

class StMixerTriplet : public TObject
{
 public:
  StMixerTriplet();
  StMixerTriplet(StMixerTriplet const*);

  StMixerTriplet(StMixerTrack const&  particle1, StMixerTrack const& particle2, StMixerTrack const& particle3,
	   float p1MassHypo, float p2MassHypo, float p3MassHypo,
	   StThreeVectorF const& vtx1, StThreeVectorF const& vtx2, StThreeVectorF const& vtx3,
	   float bField);

  ~StMixerTriplet() {;}
  

  StLorentzVectorF const & lorentzVector() const;
  StThreeVectorF const & decayVertex() const;
  float m()    const;
  float pt()   const;
  float eta()  const;
  float phi()  const;
  float pointingAngle() const;
  float decayLength() const;
  float particle1Dca() const;
  float particle2Dca() const;
  float particle3Dca() const;
  StThreeVectorF const & particle1Mom() const;
  StThreeVectorF const & particle2Mom() const;
  StThreeVectorF const & particle3Mom() const;
  float dcaDaughters12() const;
  float dcaDaughters23() const;
  float dcaDaughters31() const;
  float v0x() const;
  float v0y() const;
  float v0z() const;
  float px() const;
  float py() const;
  float pz() const;

 private:
  StMixerTriplet(StMixerTriplet const &);
  StMixerTriplet& operator=(StMixerTriplet const &);
  StLorentzVectorF mLorentzVector; 
  StThreeVectorF   mDecayVertex; 

  float mPointingAngle;
  float mDecayLength;
  float mParticle1Dca;
  float mParticle2Dca;
  float mParticle3Dca;

  StThreeVectorF mParticle1Mom;
  StThreeVectorF mParticle2Mom;
  StThreeVectorF mParticle3Mom;

  float mDcaDaughters12;
  float mDcaDaughters23;
  float mDcaDaughters31;

  ClassDef(StMixerTriplet,1)
};
inline StLorentzVectorF const& StMixerTriplet::lorentzVector() const { return mLorentzVector;}
inline float StMixerTriplet::m()    const { return mLorentzVector.m();}
inline float StMixerTriplet::pt()   const { return mLorentzVector.perp();}
inline float StMixerTriplet::eta()  const { return mLorentzVector.pseudoRapidity();}
inline float StMixerTriplet::phi()  const { return mLorentzVector.phi();}
inline float StMixerTriplet::px()   const { return mLorentzVector.px();}
inline float StMixerTriplet::py()   const { return mLorentzVector.py();}
inline float StMixerTriplet::pz()   const { return mLorentzVector.pz();}
inline float StMixerTriplet::pointingAngle() const { return mPointingAngle;}
inline float StMixerTriplet::decayLength()   const { return mDecayLength;}
inline float StMixerTriplet::particle1Dca()  const { return mParticle1Dca;}
inline float StMixerTriplet::particle2Dca()  const { return mParticle2Dca;}
inline StThreeVectorF const & StMixerTriplet::particle1Mom() const { return mParticle1Mom; }
inline StThreeVectorF const & StMixerTriplet::particle2Mom() const { return mParticle2Mom; }
inline StThreeVectorF const & StMixerTriplet::particle3Mom() const { return mParticle3Mom; }
inline float StMixerTriplet::dcaDaughters12() const { return mDcaDaughters12;}
inline float StMixerTriplet::dcaDaughters23() const { return mDcaDaughters23;}
inline float StMixerTriplet::dcaDaughters31() const { return mDcaDaughters31;}
inline StThreeVectorF const & StMixerTriplet::decayVertex() const { return mDecayVertex;}
inline float StMixerTriplet::v0x() const { return mDecayVertex.x();}
inline float StMixerTriplet::v0y() const { return mDecayVertex.y();}
inline float StMixerTriplet::v0z() const { return mDecayVertex.z();}
#endif

