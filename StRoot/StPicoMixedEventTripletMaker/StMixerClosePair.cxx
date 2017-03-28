#include <limits>
#include <cmath>

#include "StarClassLibrary/StPhysicalHelixD.hh"
#include "StarClassLibrary/SystemOfUnits.h"

#include "StMixerClosePair.h"
#include "StPicoHFMaker/StHFClosePair.h"
#include "StMixerTrack.h"

ClassImp(StMixerClosePair)

// _________________________________________________________
StMixerClosePair::StMixerClosePair(const StMixerTrack &  particle1, const StMixerTrack & particle2,
			       float p1MassHypo, float p2MassHypo,
			       const StThreeVectorF & vtx1, const StThreeVectorF & vtx2,
			       float bField) :
  StHFClosePair(),
  mParticle1Mom(StThreeVectorF()),
  mParticle2Mom(StThreeVectorF())
{
  // -- Create pair out of 2 tracks
  //     prefixes code:
  //      p1 means particle 1
  //      p2 means particle 2
  //      pair means particle1-particle2  pair

  if (vtx1 == vtx2) // for the same event
  {
    // see if the particles are the same
    if(!particle1 || !particle2 || mParticle1Idx == mParticle2Idx ||  particle1->id() == particle2->id()) {
      mParticle1Idx = std::numeric_limits<unsigned short>::max();
      mParticle2Idx = std::numeric_limits<unsigned short>::max();
      return;
    }
  }

  StThreeVectorF dVtx12 = vtx1 - vtx2;
  mP1Helix = new StPhysicalHelixD( particle1->dcaGeometry().helix());
  mP2Helix = new StPhysicalHelixD( particle2->dcaGeometry().helix());
  if (!mP1Helix || !mP2Helix)
  {
    cerr << "StHFClosePair::StHFClosePair(...): Helixes not initiated" << endl;
    mParticle1Idx = std::numeric_limits<unsigned short>::max();
    mParticle2Idx = std::numeric_limits<unsigned short>::max();
    return;
  }
  // -- move origins of helices to the primary vertex origin
  mP1Helix->moveOrigin(mP1Helix->pathLength(vtx));
  mP2Helix->moveOrigin(mP2Helix->pathLength(vtx));

  // -- use straight lines approximation to get point of DCA of particle1-particle2 pair
  StThreeVectorF const p1Mom = mP1Helix->momentum(bField * kilogauss);
  StThreeVectorF const p2Mom = mP2Helix->momentum(bField * kilogauss);
  mP1StraightLine = new StPhysicalHelixD(p1Mom, mP1Helix->origin(), 0, particle1->charge());
  mP2StraightLine = new StPhysicalHelixD(p2Mom, mP2Helix->origin(), 0, particle2->charge());

  if(!mP1StraightLine || !mP2StraightLine)
  {
    cerr << "StHFClosePair::StHFClosePair(...): StraightLines not initiated" << endl;
    mParticle1Idx = std::numeric_limits<unsigned short>::max();
    mParticle2Idx = std::numeric_limits<unsigned short>::max();
    return;
  }
  pair<double, double> const ss = (useStraightLine) ? mP1StraightLine->pathLengths(*mP2StraightLine) : mP1Helix->pathLengths(*mP2Helix);
  mP1AtDcaToP2 = (useStraightLine) ? mP1StraightLine->at(ss.first) : mP1Helix->at(ss.first);
  mP2AtDcaToP1 = (useStraightLine) ? mP2StraightLine->at(ss.second) : mP2Helix->at(ss.second);

  // -- calculate DCA of particle1 to particle2 at their DCA
  mDcaDaughters = (mP1AtDcaToP2 - mP2AtDcaToP1).mag();

  // -- single part DCAs
  mParticle1Dca = (mP1Helix->origin() - vtx).mag();
  mParticle2Dca = (mP2Helix->origin() - vtx).mag();
  // -----------------------------------------------------------------------
  // start of the mixer implementation

  StPhysicalHelixD p1Helix(particle1.gMom(), particle1.origin(),bField*kilogauss, particle1.charge());
  StPhysicalHelixD p2Helix(particle2.gMom(), particle2.origin() + dVtx12, bField*kilogauss,  particle2.charge());

  // -- move origins of helices to the primary vertex origin
  p1Helix.moveOrigin(p1Helix.pathLength(vtx1));
  p2Helix.moveOrigin(p2Helix.pathLength(vtx1));

  // -- use straight lines approximation to get point of DCA of particle1-particle2 pair
  StThreeVectorF const p1Mom = p1Helix.momentum(bField * kilogauss);
  StThreeVectorF const p2Mom = p2Helix.momentum(bField * kilogauss);

  StPhysicalHelixD const p1StraightLine(p1Mom, p1Helix.origin(), 0, particle1.charge());
  StPhysicalHelixD const p2StraightLine(p2Mom, p2Helix.origin(), 0, particle2.charge());

  pair<double, double> const ss12 = p1StraightLine.pathLengths(p2StraightLine);
  StThreeVectorF const p1AtDcaToP2 = p1StraightLine.at(ss12.first);
  StThreeVectorF const p2AtDcaToP1 = p2StraightLine.at(ss12.second);

  // -- calculate DCA of particle1 to particle2 at their DCA
  mDcaDaughters12 = (p1AtDcaToP2 - p2AtDcaToP1).mag();

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
