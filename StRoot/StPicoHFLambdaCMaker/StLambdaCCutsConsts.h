#ifndef _STLAMBDACCUTSCONSTS_H_
#define _STLAMBDACCUTSCONSTS_H_

// Header file for storage of global variables
//
// For later consideration: 
// Should be moved to a config file ... a config manager should be written


namespace LambdaCthreePartDecayReadConsts
{
  extern const float piDcaMin = 0.0065;
  extern const float kDcaMin = 0.0096;
  extern const float pDcaMin = 0.0075;
  // extern const float piDcaMin = std::numeric_limits<float>::lowest();
  // extern const float kDcaMin =  std::numeric_limits<float>::lowest();
  // extern const float pDcaMin =  std::numeric_limits<float>::lowest();

  extern const float minDaughtersPt = 0.5;

  extern const float piTpcNSigma = 3.;
  extern const float KTpcNSigma  = 2.;
  extern const float pTpcNSigma  = 2.;

  extern const float tofDeltaOneOverBeta = 0.03;

  // -- LambdaC
  extern const float LCdcaDaughtersMax = 0.005;   // maximum 300um
  extern const float LCdecayLengthMin  = 0.0234;  // minimum  60um
  extern const float LCdecayLengthMax  = 9999.;
  extern const float LCcosThetaMin     = 0.995;   // minimum
  extern const float LCminMass         = 0.;
  extern const float LCmaxMass         = 3.;
  extern const float LCdcaToPv         = 0.01;
}



#endif
