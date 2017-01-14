#include <limits>
#include <cmath>

#include "StarClassLibrary/StPhysicalHelixD.hh"
#include "StarClassLibrary/SystemOfUnits.h"

#include "StMixerTriplet.h"
#include "StPicoHFMaker/StHFTriplet.h"
#include "StMixerTrack.h"

ClassImp(StMixerTriplet)

// _________________________________________________________
StMixerTriplet::StMixerTriplet(const StMixerTrack &  particle1, const StMixerTrack & particle2, const StMixerTrack & particle3,
			       float p1MassHypo, float p2MassHypo, float p3MassHypo,
			       const StThreeVectorF & vtx1, const StThreeVectorF & vtx2, const StThreeVectorF & vtx3,
			       float bField) :
  StHFTriplet(),
  mParticle1Mom(StThreeVectorF()),
  mParticle2Mom(StThreeVectorF()),
  mParticle3Mom(StThreeVectorF())
{
  // -- Create pair out of 2 tracks
  //     prefixes code:
  //      p1 means particle 1
  //      p2 means particle 2
  //      pair means particle1-particle2  pair

  StThreeVectorF dVtx12 = vtx1 - vtx2;
  StThreeVectorF dVtx13 = vtx1 - vtx3;

  StPhysicalHelixD p1Helix(particle1.gMom(), particle1.origin(),bField*kilogauss, particle1.charge());
  StPhysicalHelixD p2Helix(particle2.gMom(), particle2.origin() + dVtx12, bField*kilogauss,  particle2.charge());
  StPhysicalHelixD p3Helix(particle3.gMom(), particle3.origin() + dVtx13, bField*kilogauss, particle3.charge());

  // -- move origins of helices to the primary vertex origin
  p1Helix.moveOrigin(p1Helix.pathLength(vtx1));
  p2Helix.moveOrigin(p2Helix.pathLength(vtx1));
  p3Helix.moveOrigin(p3Helix.pathLength(vtx1));

  // -- use straight lines approximation to get point of DCA of particle1-particle2 pair
  StThreeVectorF const p1Mom = p1Helix.momentum(bField * kilogauss);
  StThreeVectorF const p2Mom = p2Helix.momentum(bField * kilogauss);
  StThreeVectorF const p3Mom = p3Helix.momentum(bField * kilogauss);

  StPhysicalHelixD const p1StraightLine(p1Mom, p1Helix.origin(), 0, particle1.charge());
  StPhysicalHelixD const p2StraightLine(p2Mom, p2Helix.origin(), 0, particle2.charge());
  StPhysicalHelixD const p3StraightLine(p3Mom, p3Helix.origin(), 0, particle3.charge());

  pair<double, double> const ss12 = p1StraightLine.pathLengths(p2StraightLine);
  StThreeVectorF const p1AtDcaToP2 = p1StraightLine.at(ss12.first);
  StThreeVectorF const p2AtDcaToP1 = p2StraightLine.at(ss12.second);

  pair<double, double> const ss23 = p2StraightLine.pathLengths(p3StraightLine);
  StThreeVectorF const p2AtDcaToP3 = p2StraightLine.at(ss23.first);
  StThreeVectorF const p3AtDcaToP2 = p3StraightLine.at(ss23.second);

  pair<double, double> const ss31 = p3StraightLine.pathLengths(p1StraightLine);
  StThreeVectorF const p3AtDcaToP1 = p3StraightLine.at(ss31.first);
  StThreeVectorF const p1AtDcaToP3 = p1StraightLine.at(ss31.second);

  // -- calculate DCA of particle1 to particle2 at their DCA
  mDcaDaughters12 = (p1AtDcaToP2 - p2AtDcaToP1).mag();
  mDcaDaughters23 = (p2AtDcaToP3 - p3AtDcaToP2).mag();
  mDcaDaughters31 = (p3AtDcaToP1 - p1AtDcaToP3).mag();

  // -- calculate decay vertex (secondary)
  mDecayVertex = (p1AtDcaToP2 + p2AtDcaToP1 + p2AtDcaToP3 + p3AtDcaToP2 + p3AtDcaToP1 + p1AtDcaToP3) / 6.;

  // -- constructing mother daughter four momentum. Need helix (not straight line) for each daughter
  double const p1AtV0 = p1Helix.pathLength( mDecayVertex );
  StThreeVectorF const p1MomAtDca = p1Helix.momentumAt(p1AtV0 ,  bField * kilogauss);

  double const p2AtV0 = p2Helix.pathLength( mDecayVertex );
  StThreeVectorF const p2MomAtDca = p2Helix.momentumAt(p2AtV0 ,  bField * kilogauss);

  double const p3AtV0 = p3Helix.pathLength( mDecayVertex );
  StThreeVectorF const p3MomAtDca = p3Helix.momentumAt(p3AtV0 ,  bField * kilogauss);

  StLorentzVectorF const p1FourMom(p1MomAtDca, p1MomAtDca.massHypothesis(p1MassHypo));
  StLorentzVectorF const p2FourMom(p2MomAtDca, p2MomAtDca.massHypothesis(p2MassHypo));
  StLorentzVectorF const p3FourMom(p3MomAtDca, p3MomAtDca.massHypothesis(p3MassHypo));

  mLorentzVector = p1FourMom + p2FourMom + p3FourMom;

  // -- calculate pointing angle and decay length with respect to primary vertex
  StThreeVectorF const vtxToV0 = mDecayVertex - vtx1;
  mPointingAngle = vtxToV0.angle(mLorentzVector.vect());
  mDecayLength = vtxToV0.mag();

  // -- calculate DCA of tracks to primary vertex
  mParticle1Dca = (p1Helix.origin() - vtx1).mag();
  mParticle2Dca = (p2Helix.origin() - vtx1).mag();
  mParticle3Dca = (p3Helix.origin() - vtx1).mag();
}
