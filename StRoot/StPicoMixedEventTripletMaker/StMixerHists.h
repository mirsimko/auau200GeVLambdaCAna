#ifndef StMixerHists__h
#define StMixerHists__h

/* **************************************************
 *
 * Histogram manager
 *
 * **************************************************
 *
 *  Initial Authors:  
 *            Michael Lomnitz (mrlomnitz@lbl.gov)
 *            Mustafa Mustafa (mmustafa@lbl.gov)
 *  Other authors:
 *         ** Miroslav Simko  (msimko@bnl.gov)
 *
 *  ** Code Maintainer
 *
 * **************************************************
 */

#include "StarClassLibrary/StThreeVectorF.hh"
#include "TH2F.h"

#include "StMixerTriplet.h"

class StMixerHists
{
 public:
  StMixerHists(char* fileBaseName);
  ~StMixerHists();

  void fillSameEvt(const StThreeVectorF& vtx);
  void fillMixedEvt(const StThreeVectorF& vtx);
  void fillSameEvtTriplet(StMixerTriplet const* const, int charge);
  void fillMixedEvtTriplet(StMixerTriplet const* const, int charge);
  void closeFile();
 private:
  TH2F* mSE_Vtx;
  TH2F* mME_Vtx;
  TH2F* mSE_LS;
  TH2F* mSE_US;
  TH2F* mME_LS;
  TH2F* mME_US;
};
inline void StMixerHists::fillSameEvt(const StThreeVectorF& vtx)
{
  mSE_Vtx->Fill(vtx.x(), vtx.y());
  return;
}
inline void StMixerHists::fillMixedEvt(const StThreeVectorF& vtx)
{
  mME_Vtx->Fill(vtx.x(), vtx.y());
  return;
}
inline void StMixerHists::fillSameEvtTriplet(StMixerTriplet const* const triplet, int charge)
{
  if(charge == 5 || charge == 8 ) // binary b011 for LC+ or b100 for LC-
    mSE_US-> Fill(triplet->pt(),triplet->m());
  else
    mSE_LS-> Fill(triplet->pt(),triplet->m());
  return;
}
inline void StMixerHists::fillMixedEvtTriplet(StMixerTriplet const* const triplet, int charge)
{
  if(charge == 5 || charge == 8 ) // binary b011 for LC+ or b100 for LC-
    mME_US-> Fill(triplet->pt(),triplet->m());
  else
    mME_LS-> Fill(triplet->pt(),triplet->m());
  return;
}
#endif
