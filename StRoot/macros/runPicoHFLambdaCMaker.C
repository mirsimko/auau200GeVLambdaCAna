
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
 *
 * **************************************************
 */

#ifndef __CINT__
#include "TROOT.h"
#include "TSystem.h"
#include "TChain.h"

#include "StMaker.h"
#include "StChain.h"

#include "StPicoDstMaker/StPicoDstMaker.h"
#include "StPicoHFMaker/StPicoHFEvent.h"
#include "StPicoHFMaker/StHFCuts.h"

#include "StPicoHFLambdaCMaker/StPicoHFLambdaCMaker.h"
#include "StPicoHFMyAnaMaker/StPicoHFMyAnaMaker.h"

#include "loadSharedHFLibraries.C"

#else
class StChain;
#endif

StChain *chain;

void runPicoHFLambdaCMaker(const Char_t *inputFile="test.list", const Char_t *outputFile="outputBaseName",  unsigned int makerMode = 0 /*kAnalyze*/, 
       	                   const Char_t *badRunListFileName = "picoList_bad_MB.list") { 
  // -- Check STAR Library. Please set SL_version to the original star library used in the production 
  //    from http://www.star.bnl.gov/devcgi/dbProdOptionRetrv.pl
  string SL_version = "SL15c";
  string env_SL = getenv ("STAR");
  if (env_SL.find(SL_version)==string::npos) {
      cout<<"Environment Star Library does not match the requested library in runPicoHFLambdaCMaker.C. Exiting..."<<endl;
      exit(1);
  }
  
  Int_t nEvents = 10000000;

#ifdef __CINT__
  gROOT->LoadMacro("loadSharedHFLibraries.C");
  loadSharedHFLibraries();
#endif

  chain = new StChain();

  // ========================================================================================
  unsigned int decayChannel = StPicoHFLambdaCMaker::kLambdaPion;
  //unsigned int decayChannel = StPicoHFLambdaCMaker::kPionKaonProton;
  //unsigned int decayChannel = StPicoHFLambdaCMaker::kProtonK0short;
  // ========================================================================================
  
  cout << "Maker Mode    " << makerMode << endl;
  cout << "Decay Channel " << decayChannel << endl; 

  TString sInputFile(inputFile);
  TString sInputListHF("");  

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
   
   // -- prepare filelist for picoDst from hfTrees
   sInputListHF = sInputFile;
   sInputFile = "tmpPico.list";
   TString command = "sed 's|^.*hfTree|/project/projectdirs/starprod/picodsts/Run14/AuAu/200GeV/physics/P15ic|g' "
     + sInputListHF + " > " + sInputFile;
   gSystem->Exec(command.Data());
   command = "sed -i 's|picoHFtree|picoDst|g' " + sInputFile;
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

  StHFCuts* hfCuts = new StHFCuts("lambdaCBaseCuts");
  picoHFLambdaCMaker->setHFBaseCuts(hfCuts);

  // ---------------------------------------------------
  // -- Set Base cuts for HF analysis

  // -- File name of bad run list
  hfCuts->setBadRunListFileName(badRunListFileName);

  hfCuts->setCutVzMax(6.);
  hfCuts->setCutVzVpdVzMax(3.);
  hfCuts->setCutTriggerWord(0x1F);

  hfCuts->setCutNHitsFitMax(15); 
  hfCuts->setCutRequireHFT(true);
  hfCuts->setCutNHitsFitnHitsMax(0.52);
 
  // ---------------------------------------------------

  // -- Lc -> p + Ks0 , Ks0 -> pi+ + pi-
  if (decayChannel == StPicoHFLambdaCMaker::kProtonK0short) {
    picoHFLambdaCMaker->setDecayMode(StPicoHFEvent::kTwoAndTwoParticleDecay);
    
    hfCuts->setCutPrimaryDCAtoVtxMax(10.0);  // DCA to check for TOF usage

    hfCuts->setCutPionPtRange(0.3, 999.);
    hfCuts->setCutTPCNSigmaPion(3);
    hfCuts->setCutTOFDeltaOneOverBetaPion(0.04);
    hfCuts->setCutPionPtotRangeTOF(0.5, 999.);
      
    hfCuts->setCutProtonPtRange(0.3, 999.);
    hfCuts->setCutTPCNSigmaProton(3);
    hfCuts->setCutTOFDeltaOneOverBetaProton(0.04);
    hfCuts->setCutProtonPtotRangeTOF(0.5, 999.);

    hfCuts->setCutKaonPtRange(5., 999.);        // turn off kaons
    hfCuts->setCutTPCNSigmaKaon(0.);            // turn off kaons
    hfCuts->setCutTOFDeltaOneOverBetaKaon(0.);  // turn off kaons
    hfCuts->setCutKaonPtotRangeTOF(0.5, 999.);

    // -- Ks0
    float dcaDaughtersMax = 0.01;    // maximum  (100 um)
    float decayLengthMin  = 1.5;     // minimum  (cT 2.68 cm)
    float decayLengthMax  = 999999; 
    float cosThetaMin     = 0.98;   // minimum
    float minMass         = 0.4;
    float maxMass         = 0.6;
    hfCuts->setCutTertiaryPair(dcaDaughtersMax, decayLengthMin, decayLengthMax, cosThetaMin, minMass, maxMass);

    // -- LambdaC
    dcaDaughtersMax = 0.02;   // maximum (80 um)
    decayLengthMin  = 0.003;   // minimum (30 um)
    decayLengthMax  = 4.; 
    cosThetaMin     = 0.98;   // minimum
    minMass         = 1.8;
    maxMass         = 2.8;
    hfCuts->setCutSecondaryPair(dcaDaughtersMax, decayLengthMin, decayLengthMax, cosThetaMin, minMass, maxMass);
  }
  // == == == == == == == == == == == == == == == == == == == == == == == == == == == == == 

  // -- Lc -> pi+ + L , L -> p + pi-
  else if (decayChannel == StPicoHFLambdaCMaker::kLambdaPion) {
    picoHFLambdaCMaker->setDecayMode(StPicoHFEvent::kTwoAndTwoParticleDecay);
    
    hfCuts->setCutPrimaryDCAtoVtxMax(10.0);  // DCA to check for TOF usage

    hfCuts->setCutPionPtRange(0.3, 999.);
    hfCuts->setCutTPCNSigmaPion(3);
    hfCuts->setCutTOFDeltaOneOverBetaPion(0.04);
    hfCuts->setCutPionPtotRangeTOF(0.5, 999.);
      
    hfCuts->setCutProtonPtRange(0.3, 999.);
    hfCuts->setCutTPCNSigmaProton(3);
    hfCuts->setCutTOFDeltaOneOverBetaProton(0.04);
    hfCuts->setCutProtonPtotRangeTOF(0.5, 999.);

    hfCuts->setCutKaonPtRange(0.5, 999.);
    hfCuts->setCutTPCNSigmaKaon(0.);            // turn off kaons
    hfCuts->setCutTOFDeltaOneOverBetaKaon(0.);  // turn off kaons
    hfCuts->setCutKaonPtotRangeTOF(0.5, 999.);

    // -- Lambda
    float dcaDaughtersMax = 0.01;    // maximum  (100 um)
    float decayLengthMin  = 5.;      // minimum  (cT )
    float decayLengthMax  = 999999; 
    float cosThetaMin     = 0.991;   // minimum
    float minMass         = 0.9;
    float maxMass         = 1.3;
    hfCuts->setCutTertiaryPair(dcaDaughtersMax, decayLengthMin, decayLengthMax, cosThetaMin, minMass, maxMass);

    // -- LambdaC
    dcaDaughtersMax = 0.008;   // maximum (80 um)
    decayLengthMin  = 0.003;   // minimum (30 um)
    decayLengthMax  = 999999; 
    cosThetaMin     = 0.994;   // minimum
    minMass         = 1.8;
    maxMass         = 2.8;
    hfCuts->setCutSecondaryPair(dcaDaughtersMax, decayLengthMin, decayLengthMax, cosThetaMin, minMass, maxMass);
  }

  // == == == == == == == == == == == == == == == == == == == == == == == == == == == == == 

  // -- Lc -> p + K- + pi+
  else if (decayChannel == StPicoHFLambdaCMaker::kPionKaonProton) {
    picoHFLambdaCMaker->setDecayMode(StPicoHFEvent::kThreeParticleDecay);
    
    hfCuts->setCutPrimaryDCAtoVtxMax(1.0);  // DCA to check for TOF usage

    hfCuts->setCutPionPtRange(0.3, 999.);
    hfCuts->setCutTPCNSigmaPion(3);
    hfCuts->setCutTOFDeltaOneOverBetaPion(0.04);
    hfCuts->setCutPionPtotRangeTOF(0.5, 999.);
      
    hfCuts->setCutProtonPtRange(0.3, 999.);
    hfCuts->setCutTPCNSigmaProton(3);
    hfCuts->setCutTOFDeltaOneOverBetaProton(0.04);
    hfCuts->setCutProtonPtotRangeTOF(0.5, 999.);

    hfCuts->setCutKaonPtRange(0.3, 999.);
    hfCuts->setCutTPCNSigmaKaon(3);           
    hfCuts->setCutTOFDeltaOneOverBetaKaon(0.04);
    hfCuts->setCutKaonPtotRangeTOF(0.5, 999.);

    // -- Lambda
    float dcaDaughtersMax = 0.008;   // maximum
    float decayLengthMin  = 0.003;  // minimum  (30 um)
    float decayLengthMax  = 999999; 
    float cosThetaMin     = 0.996;   // minimum
    float minMass         = 0.9;
    float maxMass         = 1.3;

    // -- LambdaC
    dcaDaughtersMax = 0.008;  // maximum
    decayLengthMin  = 0.003;  // minimum
    decayLengthMax  = 999999; 
    cosThetaMin     = 0.994;   // minimum
    minMass         = 1.8;
    maxMass         = 2.8;
    hfCuts->setCutSecondaryTriplet(dcaDaughtersMax,dcaDaughtersMax,dcaDaughtersMax, 
				   decayLengthMin, decayLengthMax, cosThetaMin, minMass, maxMass);
  }

  // ========================================================================================

  // ========================================================================================

  chain->Init();
  cout << "chain->Init();" << endl;
  int total = picoDstMaker->chain()->GetEntries();
  cout << " Total entries = " << total << endl;
  if(nEvents>total) nEvents = total;

  for (Int_t i=0; i<nEvents; i++) {
    if(i%10000==0)
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
  cout << "****************************************** " << endl;
  cout << "total number of events  " << nEvents << endl;
  cout << "****************************************** " << endl;
  
  delete chain;

  // -- clean up if in read mode
  if (makerMode == StPicoHFMaker::kRead)
    gSystem->Exec(Form("rm -f %s", sInputFile.Data()));
}

