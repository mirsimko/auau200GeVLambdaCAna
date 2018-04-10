#ifndef _STLAMBDACCUTS_H_
#define _STLAMBDACCUTS_H_

/***************************************************************
 * This is a header file with cuts constants for the Lambda_c analysis
 * Cuts are given dirrectly to the StHFCuts class via non-member functiuons
 *
 * Author:    Miroslav Simko  (simko@ujf.cas.cz)
 ***************************************************************/

#include "StPicoHFMaker/StHFCuts.h"
#include "StLambdaCCutsConsts.h"
#include <iostream>
#include <limits>

namespace StLambdaCCuts
{
  //___________________________________________________________________
  void setEventCuts(StHFCuts * hfCuts)
  {
    cout << "Setting event cuts ..." << endl;

    if(!hfCuts)
    {
      using namespace std;
      cerr << "setEventCuts(...): no StHFCuts; exitting..." << endl;
      return;
    }

    hfCuts->setCutVzMax(6.);
    hfCuts->setCutVzVpdVzMax(3.);

    hfCuts->setCutNHitsFitMin(20); 
    hfCuts->setCutRequireHFT(true);
    hfCuts->setCutNHitsFitnHitsMax(0.52);
  }
  //___________________________________________________________________

  namespace threePartDecayRead
  {
    // using namespace openCuts;
    using namespace LambdaCthreePartDecayReadConsts;

    void setLambdaCCuts(StHFCuts * hfCuts)
    {
      if(!hfCuts)
      {
	using namespace std;
	cerr << "threePartDecay::setCuts(...): no StHFCuts; exitting..." << endl;
	return;
      }

      hfCuts->setCutPrimaryDCAtoVtxMax(std::numeric_limits<float>::max());    // DCA to check for TOF usage

      hfCuts->setCutPtRange(minDaughtersPt, 999., StPicoCutsBase::kPion);
      hfCuts->setCutEtaMax(1.,StPicoCutsBase::kPion);
      hfCuts->setCutDcaMin(piDcaMin, StPicoCutsBase::kPion);          // minimum 50um
      hfCuts->setCutTPCNSigma(piTpcNSigma, StPicoCutsBase::kPion);
      hfCuts->setCutTOFDeltaOneOverBeta(tofDeltaOneOverBeta, StPicoCutsBase::kPion);
      // hfCuts->setCutPtotRangeHybridTOF(0., 0., StPicoCutsBase::kPion); // TOF is not used for pions

      hfCuts->setCutPtRange(minDaughtersPt, 999., StPicoCutsBase::kProton);
      hfCuts->setCutEtaMax(1.,StPicoCutsBase::kProton);
      hfCuts->setCutDcaMin(pDcaMin, StPicoCutsBase::kProton);        // minimum 50um
      hfCuts->setCutTPCNSigma(pTpcNSigma, StPicoCutsBase::kProton);
      hfCuts->setCutTOFDeltaOneOverBeta(tofDeltaOneOverBeta, StPicoCutsBase::kProton);
      // hfCuts->setCutPtotRangeTOF(0., 0., StPicoCutsBase::kProton);

      hfCuts->setCutPtRange(minDaughtersPt, 999., StPicoCutsBase::kKaon);
      hfCuts->setCutEtaMax(1.,StPicoCutsBase::kKaon);
      hfCuts->setCutDcaMin(kDcaMin, StPicoCutsBase::kKaon);          // minimum 50um
      hfCuts->setCutTPCNSigma(KTpcNSigma, StPicoCutsBase::kKaon);
      hfCuts->setCutTOFDeltaOneOverBeta(tofDeltaOneOverBeta, StPicoCutsBase::kKaon);
      // hfCuts->setCutPtotRangeTOF(0., 0., StPicoCutsBase::kKaon);


      hfCuts->setCutSecondaryTriplet(LCdcaDaughtersMax, LCdcaDaughtersMax, LCdcaDaughtersMax,
	  LCdecayLengthMin, LCdecayLengthMax, LCcosThetaMin, LCminMass, LCmaxMass);
      hfCuts->setCutSecondaryTripletDcaToPvMax(LCdcaToPv);

      hfCuts->setCutSecondaryTripletPtMin(3.);
    }
  } // namespace threePartDecayRead
  //___________________________________________________________________


  // a little more open cuts for run16 testing
  namespace threePartDecayReadRun16
  {
    void setLambdaCCuts(StHFCuts * hfCuts)
    {
      cout << "Setting Lc cuts ..." << endl;
      if(!hfCuts)
      {
	using namespace std;
	cerr << "threePartDecay::setCuts(...): no StHFCuts; exitting..." << endl;
	return;
      }

      hfCuts->setCutPrimaryDCAtoVtxMax(std::numeric_limits<float>::max());    // DCA to check for TOF usage

      // const float piDcaMin = 0.;
      // const float kDcaMin =  0.;
      // const float pDcaMin =  0.;
      const float piDcaMin = 0.006;
      const float kDcaMin = 0.009;
      const float pDcaMin = 0.007;

      hfCuts->setCutPtRange(0.5, 999., StPicoCutsBase::kPion);
      hfCuts->setCutEtaMax(1.,StPicoCutsBase::kPion);
      hfCuts->setCutDcaMin(piDcaMin, StPicoCutsBase::kPion);          // minimum 50um
      hfCuts->setCutTPCNSigma(3., StPicoCutsBase::kPion);
      hfCuts->setCutTOFDeltaOneOverBeta(0.03, StPicoCutsBase::kPion);
      // hfCuts->setCutPtotRangeHybridTOF(0., 0., StPicoCutsBase::kPion); // TOF is not used for pions

      hfCuts->setCutPtRange(0.5, 999., StPicoCutsBase::kProton);
      hfCuts->setCutEtaMax(1.,StPicoCutsBase::kProton);
      hfCuts->setCutDcaMin(pDcaMin, StPicoCutsBase::kProton);        // minimum 50um
      hfCuts->setCutTPCNSigma(2, StPicoCutsBase::kProton);
      hfCuts->setCutTOFDeltaOneOverBeta(0.03, StPicoCutsBase::kProton);
      // hfCuts->setCutPtotRangeTOF(0., 0., StPicoCutsBase::kProton);

      hfCuts->setCutPtRange(0.5, 999., StPicoCutsBase::kKaon);
      hfCuts->setCutEtaMax(1.,StPicoCutsBase::kKaon);
      hfCuts->setCutDcaMin(kDcaMin, StPicoCutsBase::kKaon);          // minimum 50um
      hfCuts->setCutTPCNSigma(2, StPicoCutsBase::kKaon);
      hfCuts->setCutTOFDeltaOneOverBeta(0.03, StPicoCutsBase::kKaon);
      // hfCuts->setCutPtotRangeTOF(0., 0., StPicoCutsBase::kKaon);

      // -- LambdaC
      const float LCdcaDaughtersMax = 0.0055;   // maximum 300um
      const float LCdecayLengthMin  = 0.02;  // minimum  60um
      const float LCdecayLengthMax  = 9999.;
      const float LCcosThetaMin     = 0.994;   // minimum
      const float LCminMass         = 2.1;
      const float LCmaxMass         = 2.5;
      const float LCdcaToPv         = 0.01;
	
      hfCuts->setCutSecondaryTriplet(LCdcaDaughtersMax, LCdcaDaughtersMax, LCdcaDaughtersMax,
				     LCdecayLengthMin, LCdecayLengthMax, LCcosThetaMin, LCminMass, LCmaxMass);
      hfCuts->setCutSecondaryTripletDcaToPvMax(LCdcaToPv);

      hfCuts->setCutSecondaryTripletPtMin(3.);
    }
  }
  //___________________________________________________________________
}
#endif
