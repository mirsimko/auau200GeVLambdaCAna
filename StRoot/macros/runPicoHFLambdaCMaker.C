
/* **************************************************
 *   Run LambdaC Maker in different modes
 * --------------------------------------------------
 * run as :
 *  root -l -b -q StRoot/macros/loadSharedHFLibraries.C StRoot/macros/runPicoHFLambdaCMaker.C++
 *   or
 *  root -l -b -q StRoot/macros/runPicoHFLambdaCMaker.C
 *
 * -------------------------------------------------- 
 *  - Different modes to use the  class
 *    - StPicoHFMaker::kAnalyze - don't write candidate trees, just fill histograms
 *        inputFile : fileList of PicoDst files or single picoDst file
 *        outputFile: baseName for outfile 
 *    - StPicoHFMaker::kWrite   - write candidate trees
 *        inputFile : path to single picoDist file
 *        outputFile: baseName for outfile 
 *    - StPicoHFMaker::kRead    - read candidate trees and fill histograms
 *        inputFile : fileList of PicoDst files
 *        outputFile: baseName for outfile 
 *
 * --------------------------------------------------
 *  Authors:  Jochen Thaeder  (jmthader@lbl.gov)
 *	      Miroslav Simko  (msimko@bnl.gov)
 *
 * **************************************************
 */

#ifndef __CINT__
#include "TROOT.h"
#include "TSystem.h"
#include "TChain.h"

#include "StMaker.h"
#include "StChain.h"
#include "TStopwatch.h"

#include "StPicoDstMaker/StPicoDstMaker.h"
#include "StPicoHFMaker/StPicoHFEvent.h"
#include "StPicoHFMaker/StHFCuts.h"
#include "StRefMultCorr/StRefMultCorr.h"
#include "StRefMultCorr/CentralityMaker.h"

#include "StPicoHFLambdaCMaker/StPicoHFLambdaCMaker.h"

#include "macros/loadSharedHFLibraries.C"

#include <limits>
#include <iostream>
#include <ctime>
#include <cstdio>

using namespace std;

#else
class StChain;

#endif

StChain *chain;

void runPicoHFLambdaCMaker(const Char_t *inputFile="test.list", const Char_t *outputFile="outputBaseName", const unsigned int makerMode = 1 /*kWrite*/, 
       	                   const Char_t *badRunListFileName = "picoList_bad_MB.list", const Char_t *treeName = "picoHFtree",
			   const Char_t *productionBasePath = "/project/projectdirs/starprod/picodsts/Run14/AuAu/200GeV/physics2/P16id",
			   const unsigned int decayChannel = 0 /* kPionKaonProton */) { 
  // -- Check STAR Library. Please set SL_version to the original star library used in the production 
  //    from http://www.star.bnl.gov/devcgi/dbProdOptionRetrv.pl
  string SL_version = "SL16d";
  string env_SL = getenv ("STAR");
  if (env_SL.find(SL_version)==string::npos) {
      cout<<"Environment Star Library does not match the requested library in runPicoHFLambdaCMaker.C. Exiting..."<<endl;
      exit(1);
  }
  
  Int_t nEvents = 1e9;

#ifdef __CINT__
  gROOT->LoadMacro("loadSharedHFLibraries.C");
  loadSharedHFLibraries();

#endif

  chain = new StChain();

  // ========================================================================================
  //  decayChannel = StPicoHFLambdaCMaker::kPionKaonProton;   -  0
  //  decayChannel = StPicoHFLambdaCMaker::kProtonK0short;    -  1
  //  decayChannel = StPicoHFLambdaCMaker::kLambdaPion;       -  2
  // ========================================================================================
  //  makerMode = StPicoHFMaker::kAnalyze;		      -  0
  //  makerMode = StPicoHFMaker::kWrite;    	 	      -  1
  //  makerMode = StPicoHFMaker::kRead;     	 	      -  2
  // ========================================================================================
  
  cout << "Maker Mode    " << makerMode << endl;
  cout << "TreeName      " << treeName << endl; 
  cout << "Decay Channel " << decayChannel << endl; 

  TString sInputFile(inputFile);
  TString sInputListHF("");  
  TString sProductionBasePath(productionBasePath);
  TString sTreeName(treeName);

  if (makerMode == StPicoHFMaker::kAnalyze) {
    if (!sInputFile.Contains(".list") && !sInputFile.Contains("picoDst.root")) {
      cout << "No input list or picoDst root file provided! Exiting..." << endl;
      exit(1);
    }
  }
  else if (makerMode == StPicoHFMaker::kWrite) {
    if (!sInputFile.Contains("picoDst.root")) {
      cout << "No input picoDst root file provided! Exiting..." << endl;
      exit(1);
    }
  }
  else if (makerMode == StPicoHFMaker::kRead) {
   if (!sInputFile.Contains(".list")) {
      cout << "No input list provided! Exiting..." << endl;
      exit(1);
   }
   
   // -- prepare filelist for picoDst from trees
   sInputListHF = sInputFile;
   sInputFile = "tmpPico.list";

   TString command = "sed 's|" + sTreeName + ".root|picoDst.root|g' " + sInputListHF + " > " + sInputFile;
   cout << "COMMAND : " << command << endl; 
   gSystem->Exec(command.Data());

   command = "sed -i 's|^.*" + sTreeName + "|" + sProductionBasePath + "|g' " + sInputFile; // + " > " + sInputFile;
   cout << "COMMAND : " << command << endl; 
   gSystem->Exec(command.Data());
  }
  else {
    cout << "Unknown makerMode! Exiting..." << endl;
    exit(1);
  }

  StPicoDstMaker* picoDstMaker = new StPicoDstMaker(0, sInputFile, "picoDstMaker");
  StPicoHFLambdaCMaker* picoHFLambdaCMaker = new StPicoHFLambdaCMaker("picoHFLambdaCMaker", picoDstMaker, outputFile, sInputListHF);
  picoHFLambdaCMaker->setMakerMode(makerMode);
  picoHFLambdaCMaker->setDecayChannel(decayChannel);
  picoHFLambdaCMaker->setTreeName(treeName);

  StHFCuts* hfCuts = new StHFCuts("lambdaCBaseCuts");
  picoHFLambdaCMaker->setHFBaseCuts(hfCuts);


  // set refmultCorr ... required for the centrality data
  StRefMultCorr* grefmultCorrUtil = CentralityMaker::instance()->getgRefMultCorr() ;
  picoHFLambdaCMaker->setRefMutCorr(grefmultCorrUtil);
  if(!picoHFLambdaCMaker->getRefMultCorr())
  {
    cerr << "RefMultCorr not initiated ... terminating" << endl;
    return;
  }
  // ---------------------------------------------------
  // -- Set Base cuts for HF analysis

  // -- File name of bad run list
  hfCuts->setBadRunListFileName(badRunListFileName);

  hfCuts->setCutVzMax(6.);
  hfCuts->setCutVzVpdVzMax(3.);

  hfCuts->addTriggerId(450050);    // vpdmb-5-p-nobsmd-hlt 
  hfCuts->addTriggerId(450060);    // vpdmb-5-p-nobsmd-hlt 
  hfCuts->addTriggerId(450005);    // vpdmb-5-p-nobsmd 
  hfCuts->addTriggerId(450015);    // vpdmb-5-p-nobsmd 
  hfCuts->addTriggerId(450025);    // vpdmb-5-p-nobsmd 

  hfCuts->setCutNHitsFitMin(20); 
  hfCuts->setCutRequireHFT(true);
  hfCuts->setCutNHitsFitnHitsMax(0.52);

  // >>>>>>>>>>>>>>>>>>>>>>------------------------------<<<<<<<<<<<<<<<<<<<<<<
  // -- Lc -> p + Ks0 , Ks0 -> pi+ + pi-
  // >>>>>>>>>>>>>>>>>>>>>>------------------------------<<<<<<<<<<<<<<<<<<<<<<
  if (decayChannel == StPicoHFLambdaCMaker::kProtonK0short) {
    picoHFLambdaCMaker->setDecayMode(StPicoHFEvent::kTwoAndTwoParticleDecay);

    if (makerMode == StPicoHFMaker::kWrite) { // <--- Tree creation cuts
      hfCuts->setCutPrimaryDCAtoVtxMax(999.0);  // DCA to check for TOF usage

      hfCuts->setCutPtRange(0., 999., StPicoCutsBase::kPion);
      hfCuts->setCutDcaMin(0.1, StPicoCutsBase::kPion);            // minimum 1mm
      hfCuts->setCutDcaMinTertiary(0.1, StPicoCutsBase::kPion);    // minimum 1mm
      hfCuts->setCutTPCNSigma(3, StPicoCutsBase::kPion);
      
      hfCuts->setCutPtRange(0.3, 999., StPicoCutsBase::kProton);
      hfCuts->setCutDcaMin(0.005, StPicoCutsBase::kProton);        // minimum 50um
      hfCuts->setCutTPCNSigma(3, StPicoCutsBase::kProton);

      hfCuts->setCutTPCNSigma(0., StPicoCutsBase::kKaon);          // turn off kaons
      
      // -- Ks0 - very open cuts
      float dcaDaughtersMax = 0.03;   // maximum  100um
      float decayLengthMin  = 0.;     // open   (cT 2.68 cm)
      float decayLengthMax  = 300;
      float cosThetaMin     = 0.;     // minimum  >> dca2vtx = cosTheta * decaylength = 0.98*0.1 
      float minMass         = 0.4;
      float maxMass         = 0.6;
      float dcaToPvMax 	    = 0.1;
      hfCuts->setCutTertiaryPair(dcaDaughtersMax, decayLengthMin, decayLengthMax, cosThetaMin, minMass, maxMass);
      hfCuts->setCutTertiaryPairDcaToPvMax(dcaToPvMax);
      
      // -- LambdaC
      float LCdcaDaughtersMax = 0.03;   // maximum 300um 
      float LCdecayLengthMin  = 0.003;  // minimum  30um
      float LCdecayLengthMax  = 300.; 
      float LCcosThetaMin     = 0.95;   // minimum
      float LCminMass         = 2.0;
      float LCmaxMass         = 2.5;
      hfCuts->setCutSecondaryPair(LCdcaDaughtersMax, LCdecayLengthMin, LCdecayLengthMax, LCcosThetaMin, LCminMass, LCmaxMass);
    }
    else if (makerMode == StPicoHFMaker::kRead) {
      hfCuts->setCutPrimaryDCAtoVtxMax(999.0);       // DCA to check for TOF usage

      hfCuts->setCutPtRange(0., 999., StPicoCutsBase::kPion);
      hfCuts->setCutDcaMin(0.1, StPicoCutsBase::kPion);            // minimum 1mm
      hfCuts->setCutDcaMinTertiary(0.1, StPicoCutsBase::kPion);    // minimum 1mm
      hfCuts->setCutTPCNSigma(2.5, StPicoCutsBase::kPion);
      hfCuts->setCutTOFDeltaOneOverBeta(0.04, StPicoCutsBase::kPion);
      //      hfCuts->setCutPtotRangeHybridTOF(0., 999., StPicoCutsBase::kPion);

      hfCuts->setCutPtRange(0.3, 999., StPicoCutsBase::kProton);
      hfCuts->setCutDcaMin(0.005, StPicoCutsBase::kProton);        // minimum 50um
      hfCuts->setCutTPCNSigma(2.5, StPicoCutsBase::kProton);
      hfCuts->setCutTOFDeltaOneOverBeta(0.04, StPicoCutsBase::kProton);
      //    hfCuts->setCutPtotRangeHybridTOF(0., 999., StPicoCutsBase::kProton);

      hfCuts->setCutTPCNSigma(0., StPicoCutsBase::kKaon);          // turn off kaons
      
      // -- Ks0 - very open cuts
      float dcaDaughtersMax = 0.03;   // maximum  100um
      float decayLengthMin  = 0.;     // open   (cT 2.68 cm)
      float decayLengthMax  = 300;
      float cosThetaMin     = 0.;     // minimum  >> dca2vtx = cosTheta * decaylength = 0.98*0.1 
      float minMass         = 0.4;
      float maxMass         = 0.6;
      float dcaToPvMax 	    = 0.1;
      hfCuts->setCutTertiaryPair(dcaDaughtersMax, decayLengthMin, decayLengthMax, cosThetaMin, minMass, maxMass);
      hfCuts->setCutTertiaryPairDcaToPvMax(dcaToPvMax);
      
      // -- LambdaC
      float LCdcaDaughtersMax = 0.03;   // maximum 300um 
      float LCdecayLengthMin  = 0.003;  // minimum  30um
      float LCdecayLengthMax  = 300.; 
      float LCcosThetaMin     = 0.95;   // minimum
      float LCminMass         = 2.0;
      float LCmaxMass         = 2.5;
      hfCuts->setCutSecondaryPair(LCdcaDaughtersMax, LCdecayLengthMin, LCdecayLengthMax, LCcosThetaMin, LCminMass, LCmaxMass);
    }
  }

  // >>>>>>>>>>>>>>>>>>>>>>------------------------------<<<<<<<<<<<<<<<<<<<<<<
  // -- Lc -> pi+ + L , L -> p + pi-
  // >>>>>>>>>>>>>>>>>>>>>>------------------------------<<<<<<<<<<<<<<<<<<<<<<
  else if (decayChannel == StPicoHFLambdaCMaker::kLambdaPion) {
    picoHFLambdaCMaker->setDecayMode(StPicoHFEvent::kTwoAndTwoParticleDecay);
    
    if (makerMode == StPicoHFMaker::kWrite) { // <--- Tree creation cuts
      hfCuts->setCutPrimaryDCAtoVtxMax(100.0);  // DCA to check for TOF usage

      hfCuts->setCutPtRange(0.3, 999., StPicoCutsBase::kPion);
      hfCuts->setCutDcaMin(0.005, StPicoCutsBase::kPion);          // minimum 50um
      hfCuts->setCutDcaMinTertiary(0.1, StPicoCutsBase::kPion);    // minimum 1mm
      hfCuts->setCutTPCNSigma(3, StPicoCutsBase::kPion);

      hfCuts->setCutPtRange(0.3, 999., StPicoCutsBase::kProton);
      hfCuts->setCutDcaMin(0.1, StPicoCutsBase::kProton);          // minimum 50um
      hfCuts->setCutTPCNSigma(3, StPicoCutsBase::kProton);

      hfCuts->setCutTPCNSigma(0., StPicoCutsBase::kKaon);          // turn off kaons
            
      // -- Lambda
      float dcaDaughtersMax = 0.02;   // maximum 200um
      float decayLengthMin  = 0.;     // minimum (cT)
      float decayLengthMax  = 300; 
      float cosThetaMin     = 0.95;   // minimum
      float minMass         = 1.08;
      float maxMass         = 1.15;
      hfCuts->setCutTertiaryPair(dcaDaughtersMax, decayLengthMin, decayLengthMax, cosThetaMin, minMass, maxMass);

      // -- LambdaC
      float LCdcaDaughtersMax = 0.03;   // maximum 300um 
      float LCdecayLengthMin  = 0.003;  // minimum  30um
      float LCdecayLengthMax  = 300.; 
      float LCcosThetaMin     = 0.95;   // minimum
      float LCminMass         = 2.0;
      float LCmaxMass         = 2.5;
      hfCuts->setCutSecondaryPair(LCdcaDaughtersMax, LCdecayLengthMin, LCdecayLengthMax, LCcosThetaMin, LCminMass, LCmaxMass);
    }
    else if (makerMode == StPicoHFMaker::kRead) {
      hfCuts->setCutPrimaryDCAtoVtxMax(100.0);  // DCA to check for TOF usage

      hfCuts->setCutPtRange(0.3, 999., StPicoCutsBase::kPion);
      hfCuts->setCutDcaMin(0.005, StPicoCutsBase::kPion);          // minimum 50um
      hfCuts->setCutDcaMinTertiary(0.1, StPicoCutsBase::kPion);    // minimum 1mm
      hfCuts->setCutTPCNSigma(2.5, StPicoCutsBase::kPion);
      hfCuts->setCutTOFDeltaOneOverBeta(0.04, StPicoCutsBase::kPion);
      //    hfCuts->setCutPtotRangeHybridTOF(0., 999., StPicoCutsBase::kPion);

      hfCuts->setCutPtRange(0.3, 999., StPicoCutsBase::kProton);
      hfCuts->setCutDcaMin(0.1, StPicoCutsBase::kProton);          // minimum 1mm
      hfCuts->setCutTPCNSigma(2.5, StPicoCutsBase::kProton);
      hfCuts->setCutTOFDeltaOneOverBeta(0.04, StPicoCutsBase::kProton);
      //    hfCuts->setCutPtotRangeHybridTOF(0., 999., StPicoCutsBase::kProton);

      hfCuts->setCutTPCNSigma(0., StPicoCutsBase::kKaon);          // turn off kaons
      
      // -- Lambda
      float dcaDaughtersMax = 0.02;   // maximum 200um
      float decayLengthMin  = 0.;     // minimum (cT)
      float decayLengthMax  = 300; 
      float cosThetaMin     = 0.95;   // minimum
      float minMass         = 1.08;
      float maxMass         = 1.15;
      hfCuts->setCutTertiaryPair(dcaDaughtersMax, decayLengthMin, decayLengthMax, cosThetaMin, minMass, maxMass);

      // -- LambdaC
      float LCdcaDaughtersMax = 0.03;   // maximum 300um 
      float LCdecayLengthMin  = 0.003;  // minimum  30um
      float LCdecayLengthMax  = 300.; 
      float LCcosThetaMin     = 0.95;   // minimum
      float LCminMass         = 2.0;
      float LCmaxMass         = 2.5;
      hfCuts->setCutSecondaryPair(LCdcaDaughtersMax, LCdecayLengthMin, LCdecayLengthMax, LCcosThetaMin, LCminMass, LCmaxMass);
    }
  }

  // >>>>>>>>>>>>>>>>>>>>>>------------------------------<<<<<<<<<<<<<<<<<<<<<<
  // -- Lc -> p + K- + pi+
  // >>>>>>>>>>>>>>>>>>>>>>------------------------------<<<<<<<<<<<<<<<<<<<<<<
  else if (decayChannel == StPicoHFLambdaCMaker::kPionKaonProton) {
    picoHFLambdaCMaker->setDecayMode(StPicoHFEvent::kThreeParticleDecay);

    if (makerMode == StPicoHFMaker::kWrite) { // <--- Tree creation cuts
      hfCuts->setCutPrimaryDCAtoVtxMax(1.0);    // DCA to check for TOF usage

      hfCuts->setCutPtRange(0.3, 999., StPicoCutsBase::kPion);
      hfCuts->setCutEtaMax(1.,StPicoCutsBase::kPion);
      hfCuts->setCutDcaMin(0.005, StPicoCutsBase::kPion);          // minimum 50um
      hfCuts->setCutTPCNSigma(3, StPicoCutsBase::kPion);
      hfCuts->setCutPtotRangeHybridTOF(0., 0., StPicoCutsBase::kPion); // we do not cut on TOF

      hfCuts->setCutPtRange(0.3, 999., StPicoCutsBase::kProton);
      hfCuts->setCutEtaMax(1.,StPicoCutsBase::kProton);
      hfCuts->setCutDcaMin(0.005, StPicoCutsBase::kProton);        // minimum 50um
      hfCuts->setCutTPCNSigma(3, StPicoCutsBase::kProton);
      // hfCuts->setCutTOFDeltaOneOverBeta(0.04, StPicoCutsBase::kProton);
      hfCuts->setCutPtotRangeTOF(0., 0., StPicoCutsBase::kProton); // we do not cut on TOF

      hfCuts->setCutPtRange(0.3, 999., StPicoCutsBase::kKaon);
      hfCuts->setCutEtaMax(1.,StPicoCutsBase::kKaon);
      hfCuts->setCutDcaMin(0.005, StPicoCutsBase::kKaon);          // minimum 50um
      hfCuts->setCutTPCNSigma(2, StPicoCutsBase::kKaon);
      // hfCuts->setCutTOFDeltaOneOverBeta(0.04, StPicoCutsBase::kKaon);
      hfCuts->setCutPtotRangeTOF(0., 0., StPicoCutsBase::kKaon); // we do not cut on TOF

      // -- LambdaC
      float LCdcaDaughtersMax = 0.01;   // maximum 300um 
      float LCdecayLengthMin  = 0.005;  // minimum  30um
      float LCdecayLengthMax  = 300.;
      float LCcosThetaMin     = 0.95;   // minimum
      float LCminMass         = 1.6;
      float LCmaxMass         = 2.6;
      hfCuts->setCutSecondaryTriplet(LCdcaDaughtersMax, LCdcaDaughtersMax, LCdcaDaughtersMax, 
				     LCdecayLengthMin, LCdecayLengthMax, LCcosThetaMin, LCminMass, LCmaxMass);
    }
    else if (makerMode == StPicoHFMaker::kRead || makerMode == StPicoHFMaker::kAnalyze) {
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
  // making sure that StRefMultCorr is initiated (it is only used for the p,K,pi channel when analyzing candidates)
  // ========================================================================================

  // ========================================================================================

  TStopwatch totalTime; // getting starting time 
  chain->Init();
  cout << "chain->Init();" << endl;
  int total = picoDstMaker->chain()->GetEntries();
  cout << " Total entries = " << total << endl;
  if(nEvents>total) nEvents = total;

  for (Int_t i=0; i<nEvents; i++) {
    if(i%1000==0)
      cout << "Working on eventNumber " << i << endl;
    
    chain->Clear();
    int iret = chain->Make(i);
    
    if (iret) { cout << "Bad return code!" << iret << endl; break;}
    
    total++;
  }
  
  cout << "****************************************** " << endl;
  cout << "Work done... now its time to close up shop!"<< endl;
  cout << "****************************************** " << endl;
  chain->Finish();
  totalTime.Stop();
  cout << "****************************************** " << endl;
  cout << "total number of events  " << nEvents << endl;
  cout << "****************************************** " << endl;
  cout << "Time needed " << totalTime.RealTime() << " s" << endl;
  cout << "****************************************** " << endl;
  
  delete chain;

  // -- clean up if in read mode
  if (makerMode == StPicoHFMaker::kRead)
    gSystem->Exec(Form("rm -f %s", sInputFile.Data()));
}

