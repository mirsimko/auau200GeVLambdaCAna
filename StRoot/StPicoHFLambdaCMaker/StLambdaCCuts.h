#ifndef _STLAMBDACCUTS_H_
#define _STLAMBDACCUTS_H_

#include "StPicoHFMaker/StHFCuts.h"
#include <iostream>

namespace threePartDecay
{
  void setLambdaCCuts(StHFCuts * hfCuts)
  {
    if(!hfCuts)
    {
      using namespace std;
      cerr << "threePartDecay::setCuts(...): no StHFCuts; exitting..." << endl;
      return;
    }

    hfCuts->setCutPrimaryDCAtoVtxMax(std::numeric_limits<float>::max());    // DCA to check for TOF usage

    hfCuts->setCutPtRange(0.5, 999., StPicoCutsBase::kPion);
    hfCuts->setCutEtaMax(1.,StPicoCutsBase::kPion);
    hfCuts->setCutDcaMin(0.0065, StPicoCutsBase::kPion);          // minimum 50um
    hfCuts->setCutTPCNSigma(3., StPicoCutsBase::kPion);
    hfCuts->setCutTOFDeltaOneOverBeta(0.03, StPicoCutsBase::kPion);
    // hfCuts->setCutPtotRangeHybridTOF(0., 0., StPicoCutsBase::kPion); // TOF is not used for pions

    hfCuts->setCutPtRange(0.5, 999., StPicoCutsBase::kProton);
    hfCuts->setCutEtaMax(1.,StPicoCutsBase::kProton);
    hfCuts->setCutDcaMin(0.0075, StPicoCutsBase::kProton);        // minimum 50um
    hfCuts->setCutTPCNSigma(2, StPicoCutsBase::kProton);
    hfCuts->setCutTOFDeltaOneOverBeta(0.03, StPicoCutsBase::kProton);
    // hfCuts->setCutPtotRangeTOF(0., 0., StPicoCutsBase::kProton);

    hfCuts->setCutPtRange(0.5, 999., StPicoCutsBase::kKaon);
    hfCuts->setCutEtaMax(1.,StPicoCutsBase::kKaon);
    hfCuts->setCutDcaMin(0.0096, StPicoCutsBase::kKaon);          // minimum 50um
    hfCuts->setCutTPCNSigma(2, StPicoCutsBase::kKaon);
    hfCuts->setCutTOFDeltaOneOverBeta(0.03, StPicoCutsBase::kKaon);
    // hfCuts->setCutPtotRangeTOF(0., 0., StPicoCutsBase::kKaon);

    // -- LambdaC
    float LCdcaDaughtersMax = 0.005;   // maximum 300um
    float LCdecayLengthMin  = 0.0234;  // minimum  60um
    float LCdecayLengthMax  = 9999.;
    float LCcosThetaMin     = 0.995;   // minimum
    float LCminMass         = 2.1;
    float LCmaxMass         = 2.5;
    float LCdcaToPv         = 0.01;
      
    hfCuts->setCutSecondaryTriplet(LCdcaDaughtersMax, LCdcaDaughtersMax, LCdcaDaughtersMax,
				   LCdecayLengthMin, LCdecayLengthMax, LCcosThetaMin, LCminMass, LCmaxMass);
    hfCuts->setCutSecondaryTripletDcaToPvMax(LCdcaToPv);

  }
}
#endif
