#ifndef _cutsConsts_h_
#define _cutsConsts_h_

// # Original simulations (not using protons DCA and using old data driven MC)
  // ***********************************************
  // The original set of cuts:
  // ***********************************************
  // float MdLength = 0.003 + 0.004* index[0];
  // float MdcaDaughters = 0.02 - 0.004*index[1];
  // float MmaxVdist = 0.05 - 0.01*index[2];
  // float MpPt = 0.3 + 0.2*index[3];
  // float MpiPt = 0.3 + 0.2*index[4];
  // float MkPt = 0.3 + 0.2*index[5];
  // float McosTheta = 0.9825 + 0.0025*index[6];
  // ***********************************************

  // ***********************************************
  // second iteration:
  // ***********************************************
  // float MdLength = 0.012 + 0.004* index[0];
  // float MdcaDaughters = 0.018 - 0.002*index[1];
  // float MmaxVdist = 0.04 - 0.005*index[2];
  // float MpPt = 0.3 + 0.1*index[3];
  // float MpiPt = 0.3 + 0.1*index[4];
  // float MkPt = 0.3 + 0.1*index[5];
  // float McosTheta = 0.98 + 0.0025*index[6];
  // ***********************************************

  // ***********************************************
  // third iteration:
  // ***********************************************
  // float MdLength = 0.02 + 0.002* index[0];
  // float MdcaDaughters = 0.010 - 0.002*index[1];
  // float MmaxVdist = 0.028 - 0.002*index[2];
  // float MpPt = 0.6 + 0.1*index[3];
  // float MpiPt = 0.5 + 0.05*index[4];
  // float MkPt = 0.5 + 0.05*index[5];
  // float McosTheta = 0.9875 + 0.0025*index[6];
  // ***********************************************

  // ***********************************************
  // fourth iteration:
  // ***********************************************
  // float MdLength = 0.028 + 0.002* index[0];
  // float MdcaDaughters = 0.010 - 0.001*index[1];
  // float MmaxVdist = 0.025 - 0.001*index[2];
  // float MpPt = 1. + 0.1*index[3];
  // float MpiPt = 0.7 + 0.05*index[4];
  // float MkPt = 0.7 + 0.05*index[5];
  // float McosTheta = 0.995 + 0.001*index[6];
  // ***********************************************

  // ***********************************************
  // fifth iteration:
  // ***********************************************
  // float MdLength = 0.036 + 0.002* index[0];
  // float MdcaDaughters = 0.006 - 0.001*index[1];
  // float MmaxVdist = 0.021 - 0.001*index[2];
  // float MpPt = 1.4 + 0.1*index[3];
  // float MpiPt = 0.75 + 0.025*index[4];
  // float MkPt = 0.9 + 0.05*index[5];
  // float McosTheta = 0.998 + 0.0002*index[6];
  // ***********************************************

  // ***********************************************
  // sixth iteration:
  // ***********************************************
  // float MdLength = 0.028 + 0.002* index[0];
  // float MdcaDaughters = 0.006 - 0.0005*index[1];
  // float MmaxVdist = 0.025 - 0.001*index[2];
  // float MpPt = 1.8 + 0.1*index[3];
  // float MpiPt = 0.75 + 0.0125*index[4];
  // float MkPt = 1. + 0.025*index[5];
  // float McosTheta = 0.9988 + 0.0002*index[6];
  // ***********************************************

  // ***********************************************
  // seventh iteration:
  // ***********************************************
  // float MdLength = 0.034 + 0.001* index[0];
  // float MdcaDaughters = 0.0055 - 0.00025*index[1];
  // float MmaxVdist = 0.022 - 0.0005*index[2];
  // float MpPt = 1.7 + 0.05*index[3];
  // float MpiPt = 0.775 + 0.0125*index[4];
  // float MkPt = 1.0375 + 0.0125*index[5];
  // float McosTheta = 0.9986 + 0.0001*index[6];
  // ***********************************************

namespace firstIter
{
  const float DLstart = 0.003;
  const float DLinc = 0.01;
  const float dcaDaughtersStart = 0.02;
  const float dcaDaughtersInc = 0.005;
  const float maxVdistStart = 0.022;
  const float maxVdistInc = 0.0005;
  const float pPtStart = 0.2;
  const float pPtInc = 0.3;
  const float piPtStart = 0.2;
  const float piPtInc = 0.3;
  const float KPtStart = 0.2;
  const float KPtInc = 0.3;
  const float cosThetaStart = 0.98;
  const float cosThetaInc = 0.004;
}
// Max significance
// ii  jj	kk  ll	mm  nn	oo  dLength dcaD  Vdist	pPt piPt  kPt cos(t)  significance  nSim  nBKG	ratio
// 3 2 0 4	2 2 4 0.033 0.01  0.022	1.4 0.8	0.8 0.996 0.0979951 0.629 40.5705 0.0155039


namespace secondIter
{
  const float DLstart = 0.02;
  const float DLinc = 0.005;
  const float dcaDaughtersStart = 0.015;
  const float dcaDaughtersInc = 0.0025;
  const float maxVdistStart = 0.022;
  const float maxVdistInc = 0.005;
  const float pPtStart = 1.;
  const float pPtInc = 0.3;
  const float piPtStart = 0.5;
  const float piPtInc = 0.15;
  const float KPtStart = 0.5;
  const float KPtInc = 0.15;
  const float cosThetaStart = 0.99;
  const float cosThetaInc = 0.002;
}

// Max significance
// ii  jj	kk  ll	mm  nn	oo  dLength dcaD  Vdist	pPt piPt  kPt cos(t)  significance  nSim  nBKG	ratio
// 4 3 0 3	4 4 4 0.04  0.0075  0.022 1.9 1.1 1.1 0.998 0.108365  0.097 0.704243  0.137737


namespace thirdIter
{
  const float DLstart = 0.035;
  const float DLinc = 0.005;
  const float dcaDaughtersStart = 0.01;
  const float dcaDaughtersInc = 0.00125;
  const float maxVdistStart = 0.03;
  const float maxVdistInc = 0.005;
  const float pPtStart = 1.6;
  const float pPtInc = 0.3;
  const float piPtStart = 0.95;
  const float piPtInc = 0.15;
  const float KPtStart = 0.95;
  const float KPtInc = 0.15;
  const float cosThetaStart = 0.996;
  const float cosThetaInc = 0.0008;
}

// Max significance
// ii  jj	kk  ll	mm  nn	oo  dLength dcaD  Vdist	pPt piPt  kPt cos(t)  significance  nSim  nBKG	ratio
// 1 4 3 1	1 4 2 0.04  0.005 0.015	1.9 1.1	1.55  0.9976  0.164317	0.027 0	inf


// ********************************************
// most likely overtrained
// ********************************************
//
// namespace fourthIter
// {
//   const float DLstart = 0.0375;
//   const float DLinc = 0.0025;
//   const float dcaDaughtersStart = 0.00725;
//   const float dcaDaughtersInc = 0.00125;
//   const float maxVdistStart = 0.02;
//   const float maxVdistInc = 0.0025;
//   const float pPtStart = 1.6;
//   const float pPtInc = 0.15;
//   const float piPtStart = 0.95;
//   const float piPtInc = 0.075;
//   const float KPtStart = 1.25;
//   const float KPtInc = 0.075;
//   const float cosThetaStart = 0.9968;
//   const float cosThetaInc = 0.0004;
// }

// Max significance
// ii  jj	kk  ll	mm  nn	oo  dLength dcaD  Vdist	pPt piPt  kPt cos(t)  significance  nSim  nBKG	ratio
// 1 2 1 1	1 4 2 0.04  0.0075  0.025 1.9 1.1 1.55	0.9976	0.127288  0.079	0.306193  0.258007

namespace fourthIter
{
  const float DLstart = 0.035;
  const float DLinc = 0.0025;
  const float dcaDaughtersStart = 0.00625;
  const float dcaDaughtersInc = 0.00125;
  const float maxVdistStart = 0.02;
  const float maxVdistInc = 0.0025;
  const float pPtStart = 1.6;
  const float pPtInc = 0.15;
  const float piPtStart = 0.95;
  const float piPtInc = 0.075;
  const float KPtStart = 1.475;
  const float KPtInc = 0.075;
  const float cosThetaStart = 0.9968;
  const float cosThetaInc = 0.0004;
}

#endif // #ifndef _cutsConsts_h_
