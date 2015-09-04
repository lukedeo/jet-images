#ifndef  MIAnalysis_H
#define  MIAnalysis_H

#include <vector>
#include <math.h>
#include <string>

#include "fastjet/ClusterSequence.hh"
#include "fastjet/PseudoJet.hh"  
#include "fastjet/tools/Filter.hh"
#include "fastjet/Selector.hh"

#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TParticle.h"

#include "MITools.h"
#include "myFastJetBase.h"
#include "Pythia8/Pythia.h"

#include "TH2F.h"

using namespace std;
using namespace fastjet;

class MIAnalysis
{
    public:
        MIAnalysis(int imagesize = 25);
        ~MIAnalysis();
        
        void Begin();
        void AnalyzeEvent(int iEvt, Pythia8::Pythia *pythia8,  
            Pythia8::Pythia *pythia_MB, int NPV, int pixels, float range);

        void End();
        void DeclareBranches();
        void ResetBranches();

        void Debug(int debug)
        {
            fDebug = debug;
        }

        void SetOutName(const string &outname)
        {
            fOutName = outname;
        }
    private:
        int  ftest;
        int  fDebug;
        string fOutName;

        TFile *tF;
        TTree *tT;
        MITools *tool;

        // Tree Vars ---------------------------------------
        int fTEventNumber;
        int fTNPV;

        void SetupInt(int & val, TString name);
        void SetupFloat(float & val, TString name);

        vector<TString> names;
        vector<float> pts;
        vector<float> ms;
        vector<float> etas;
        vector<float> nsub21s;
        vector<float> nsub32s;
        vector<int>   nsubs;

        TH2F* detector;

        int MaxN;

        int fTNFilled;

        float fTLeadingEta;
        float fTLeadingPhi;
        float fTLeadingPt;
        float fTLeadingM;
        
	float fTLeadingEta_nopix;
        float fTLeadingPhi_nopix;
        float fTLeadingPt_nopix;
        float fTLeadingM_nopix;

        float fTSubLeadingEta;
        float fTSubLeadingPhi;
	
	float fTPCEta;
	float fTPCPhi;

        //float fTRotationAngle;

        float fTTau1;
        float fTTau2;
        float fTTau3;

        float fTTau21;
        float fTTau32;

	float fTTau1_nopix;
        float fTTau2_nopix;
        float fTTau3_nopix;

	float fTTau21_nopix;
	float fTTau32_nopix;

        float fTdeltaR;

        // float fTTau21old;
        // float fTTau32old;

        float *fTIntensity;
        // float *fTRotatedIntensity;
        // float *fTLocalDensity;
        // float *fTGlobalDensity;

        // float fTPt [MaxN];
        // double fTEta [MaxN];
        // double fTPhi [MaxN];

        // float fTIntensity[MaxN];
        // float fTRotatedIntensity[MaxN];
        // int  fTPixx[MaxN];
        // int  fTPixy[MaxN]; 


 
       
    
};

#endif

