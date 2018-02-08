#ifndef _STLAMBDACCUTSCONSTS_H_
#define _STLAMBDACCUTSCONSTS_H_

namespace LambdaCthreePartDecayReadConsts
{
  const float piDcaMin = 0.0065;
  const float kDcaMin = 0.0096;
  const float pDcaMin = 0.0075;
  // const float piDcaMin = std::numeric_limits<float>::lowest();
  // const float kDcaMin =  std::numeric_limits<float>::lowest();
  // const float pDcaMin =  std::numeric_limits<float>::lowest();

  const float minDaughtersPt = 0.5;

  const float piTpcNSigma = 3.;
  const float KTpcNSigma  = 2.;
  const float pTpcNSigma  = 2.;

  const float tofDeltaOneOverBeta = 0.03;

  // -- LambdaC
  const float LCdcaDaughtersMax = 0.005;   // maximum 300um
  const float LCdecayLengthMin  = 0.0234;  // minimum  60um
  const float LCdecayLengthMax  = 9999.;
  const float LCcosThetaMin     = 0.995;   // minimum
  const float LCminMass         = 0.;
  const float LCmaxMass         = 3.;
  const float LCdcaToPv         = 0.01;
}



#endif
