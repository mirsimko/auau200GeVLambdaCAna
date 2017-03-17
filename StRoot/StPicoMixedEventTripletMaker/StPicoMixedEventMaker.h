#ifndef StPicoMixedEventMaker_h
#define StPicoMixedEventMaker_h

#include "StMaker.h"

/* **************************************************
 *
 *  Base class for Mixed Event cosntructions
 *  Template implemented for D0 recosntruction. User should use a 
 *  Mixer per category in Event Mixing and define event buffer size (10 by default).
 *  For different decays changes must be made to StPicoEventMixer class
 * 
 *
 * **************************************************
 *
 *  Initial Authors:
 *        **  Michael Lomnitz  (mrlomnitz@lbl.gov)
 *            Mustafa Mustafa  (mmustafa@lbl.gov)
 *
 *  ** Code Maintainer
 * 
 *
 * **************************************************  
 */

class TTree;
class TFile;
class TChain;

class StPicoDst;
class StPicoDstMaker;
class StPicoEvent;
class StPicoTrack;
class StRefMultCorr;

class StPicoEventMixer;

class StPicoMixedEventMaker : public StMaker 
{
  public:
  StPicoMixedEventMaker(char const* name, StPicoDstMaker* picoMaker, StRefMultCorr* grefmultCorrUtil, StHFCuts* hfCuts,
			char const* outputBaseFileName,  
			char const* inputHFListHFtree);
    ~StPicoMixedEventMaker();
    Int_t Init();
    Int_t Make();
    Int_t Finish();
    void  Clear(Option_t* opt="");

    Int_t SetCategories();

 private:
    int categorize(StPicoDst const*);
    StPicoDst*      mPicoDst;
    StPicoDstMaker* mPicoDstMaker;      
    StPicoEvent*    mPicoEvent;         
    StRefMultCorr*  mGRefMultCorrUtil;
    StHFCuts*	    mHFCuts;

    StPicoEventMixer* mPicoEventMixer[10][9];

    TString         mOuputFileBaseName; 
    TString         mInputFileName;     

    int             mEventCounter;

    bool loadEventPlaneCorr(int const runId);
                                        
    TTree*          mTree;
    TFile*          mOutputFileTree; 

    ClassDef(StPicoMixedEventMaker, 0)
};
#endif
