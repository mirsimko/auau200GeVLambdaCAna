
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

#include "macros/loadSharedHFLibraries.C"

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
			   const Char_t *productionBasePath = "/project/projectdirs/starprod/picodsts/Run14/AuAu/200GeV/physics2/P15ic",
			   const unsigned int decayChannel = 0 /* kPionKaonProton */) { 
  // -- Check STAR Library. Please set SL_version to the original star library used in the production 
  //    from http://www.star.bnl.gov/devcgi/dbProdOptionRetrv.pl
  string SL_version = "SL15c";
  string env_SL = getenv ("STAR");
  if (env_SL.find(SL_version)==string::npos) {
      cout<<"Environment Star Library does not match the requested library in runPicoHFLambdaCMaker.C. Exiting..."<<endl;
      exit(1);
  }
  
  //  bool useMC = true;


  Int_t nEvents = 100000000;

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
    
    hfCuts->setCutPrimaryDCAtoVtxMax(999.0);  // DCA to check for TOF usage

    hfCuts->setCutPionPtRange(0., 999.);
    hfCuts->setCutTPCNSigmaPion(3);
    hfCuts->setCutTOFDeltaOneOverBetaPion(0.04);
    hfCuts->setCutPionPtotRangeHybridTOF(0., 999.);
    hfCuts->setPiDcaCut(0.005);

    hfCuts->setCutProtonPtRange(0., 999.);
    hfCuts->setCutTPCNSigmaProton(3);
    hfCuts->setCutTOFDeltaOneOverBetaProton(0.04);
    hfCuts->setCutProtonPtotRangeHybridTOF(0., 999.);

    hfCuts->setCutKaonPtRange(5., 999.);        // turn off kaons
    hfCuts->setCutTPCNSigmaKaon(0.);            // turn off kaons
    hfCuts->setCutTOFDeltaOneOverBetaKaon(0.);  // turn off kaons
    hfCuts->setCutKaonPtotRangeHybridTOF(0.3, 999.);

    // -- pi


    // -- Ks0
    float dcaDaughtersMax = 0.03;    // maximum  (100 um)
    float decayLengthMin  = 0.1;     // 1 mm minimum  (cT 2.68 cm)
    float decayLengthMax  = 300;
    float cosThetaMin     = 0.;    // minimum  >> dca2vtx = cosTheta * decaylength = 0.98*0.1 
    float minMass         = 0.4;
    float maxMass         = 0.6;
    hfCuts->setCutTertiaryPair(dcaDaughtersMax, decayLengthMin, decayLengthMax, cosThetaMin, minMass, maxMass);

    // -- LambdaC
    dcaDaughtersMax = 0.03;   // maximum (200 um)
    decayLengthMin  = 0.003;  // minimum (30 um)
    decayLengthMax  = 300.; 
    cosThetaMin     = 0.;   // minimum
    minMass         = 2.0;
    maxMass         = 2.5;
    hfCuts->setCutSecondaryPair(dcaDaughtersMax, decayLengthMin, decayLengthMax, cosThetaMin, minMass, maxMass);
  }
  // == == == == == == == == == == == == == == == == == == == == == == == == == == == == == 

  // -- Lc -> pi+ + L , L -> p + pi-
  else if (decayChannel == StPicoHFLambdaCMaker::kLambdaPion) {
    picoHFLambdaCMaker->setDecayMode(StPicoHFEvent::kTwoAndTwoParticleDecay);
    
    hfCuts->setCutPrimaryDCAtoVtxMax(100.0);  // DCA to check for TOF usage

    hfCuts->setCutPionPtRange(0.3, 999.);
    hfCuts->setCutTPCNSigmaPion(3);
    hfCuts->setCutTOFDeltaOneOverBetaPion(0.04);
    hfCuts->setCutPionPtotRangeHybridTOF(0.3, 999.);

    hfCuts->setCutProtonPtRange(0.3, 999.);
    hfCuts->setCutTPCNSigmaProton(3);
    hfCuts->setCutTOFDeltaOneOverBetaProton(0.04);
    hfCuts->setCutProtonPtotRangeHybridTOF(0.3, 999.);

    hfCuts->setCutKaonPtRange(0.3, 999.);
    hfCuts->setCutTPCNSigmaKaon(0.);            // turn off kaons
    hfCuts->setCutTOFDeltaOneOverBetaKaon(0.);  // turn off kaons
    hfCuts->setCutKaonPtotRangeHybridTOF(0.3, 999.);

    // -- Lambda
    float dcaDaughtersMax = 0.02;    // maximum  (100 um)
    float decayLengthMin  = 0.;      // minimum  (cT )
    float decayLengthMax  = 300; 
    float cosThetaMin     = 0.98;   // minimum
    float minMass         = 1.08;
    float maxMass         = 1.15;
    hfCuts->setCutTertiaryPair(dcaDaughtersMax, decayLengthMin, decayLengthMax, cosThetaMin, minMass, maxMass);

    // -- LambdaC
    dcaDaughtersMax = 0.02;    // 200 um maximum (80 um)
    decayLengthMin  = 0.003;   // minimum (30 um)
    decayLengthMax  = 300.; 
    cosThetaMin     = 0.98;   // minimum
    minMass         = 2.0;
    maxMass         = 2.5;
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
    hfCuts->setCutPionPtotRangeTOF(0.3, 999.);
      
    hfCuts->setCutProtonPtRange(0.3, 999.);
    hfCuts->setCutTPCNSigmaProton(3);
    hfCuts->setCutTOFDeltaOneOverBetaProton(0.04);
    hfCuts->setCutProtonPtotRangeTOF(0.5, 999.);

    hfCuts->setCutKaonPtRange(0.3, 999.);
    hfCuts->setCutTPCNSigmaKaon(3);           
    hfCuts->setCutTOFDeltaOneOverBetaKaon(0.04);
    hfCuts->setCutKaonPtotRangeTOF(0.5, 999.);

    // -- LambdaC
    float dcaDaughtersMax = 0.02;  // maximum
    float decayLengthMin  = 0.003;  // minimum
    float decayLengthMax  = 300.;
    float cosThetaMin     = 0.98;   // minimum
    float minMass         = 2.0;
    float maxMass         = 2.5;
    hfCuts->setCutSecondaryTriplet(dcaDaughtersMax,dcaDaughtersMax,dcaDaughtersMax, 
				   decayLengthMin, decayLengthMax, cosThetaMin, minMass, maxMass);
  }

  // ========================================================================================

  // ========================================================================================

  std::clock_t start = std::clock(); // getting starting time 
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
  double duration = (double) (std::clock() - start) / (double) CLOCKS_PER_SEC;
  cout << "****************************************** " << endl;
  cout << "total number of events  " << nEvents << endl;
  cout << "****************************************** " << endl;
  cout << "Time needed " << duration << " s" << endl;
  cout << "****************************************** " << endl;
  
  delete chain;

  // -- clean up if in read mode
  if (makerMode == StPicoHFMaker::kRead)
    gSystem->Exec(Form("rm -f %s", sInputFile.Data()));
}

