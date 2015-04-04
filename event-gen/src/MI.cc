#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

#include "TString.h"
#include "TSystem.h"
#include "TError.h"
//#include "TPythia8.h"
#include "TClonesArray.h"
#include "TParticle.h"
#include "TDatabasePDG.h"

#include "fastjet/PseudoJet.hh"  
#include "fastjet/ClusterSequence.hh"
#include "fastjet/Selector.hh"

#include "Pythia8/Pythia.h"

#include "MITools.h"
#include "MIAnalysis.h"

#include "boost/program_options.hpp"


using std::cout;
using std::endl;
using std::string;
using std::map;
using namespace std;
namespace po = boost::program_options;

int getSeed(int seed)
{                                                                                                                                                                                                                                              
    if (seed > -1)
    {
        return seed;
    }
    int timeSeed = time(NULL);                                                                                                                                                                                                                                         
    return abs(((timeSeed*181)*((getpid()-83)*359))%104729);                                                                                                                                                                                                           
}  

int main(int argc, char* argv[])
{
    // argument parsing  ------------------------
    cout << "Called as: ";

    for(int ii = 0; ii < argc; ++ii)
    {
        cout << argv[ii] << " ";
    }
    cout << endl;

    // agruments 
    string outName   = "MI.root";
    int    pileup    = 0;
    int    nEvents   = 0;
    int    fDebug    = 0;
    float  pThatmin  = 100;
    float  pThatmax  = 500;
    float  boson_mass= 1500;
    int    proc      = 1;
    int    seed      = -1;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("NEvents",   po::value<int>(&nEvents)->default_value(10) ,    "Number of Events ")
        ("Debug",     po::value<int>(&fDebug) ->default_value(0) ,     "Debug flag")
        ("Pileup",    po::value<int>(&pileup)->default_value(0), "Number of Additional Interactions")
        ("OutFile",   po::value<string>(&outName)->default_value("test.root"), "output file name")
        ("Proc",      po::value<int>(&proc)->default_value(2), "Process: 1=ZprimeTottbar, 2=WprimeToWZ_lept, 3=WprimeToWZ_had, 4=QCD")
        ("Seed",      po::value<int>(&seed)->default_value(-1), "seed. -1 means random seed")
        ("pThatMin",  po::value<float>(&pThatmin)->default_value(100), "pThatMin for QCD")
        ("pThatMax",  po::value<float>(&pThatmax)->default_value(500), "pThatMax for QCD")
        ("BosonMass", po::value<float>(&boson_mass)->default_value(800), "Z' or W' mass in GeV");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") > 0)
    {
        cout << desc << "\n";
        return 1;
    }


    //seed 
    seed = getSeed(seed);

    // Configure and initialize pythia
    Pythia8::Pythia* pythia8 = new Pythia8::Pythia();

    pythia8->readString("Random:setSeed = on"); 
    std::stringstream ss; ss << "Random:seed = " << seed;
    cout << ss.str() << endl; 
    pythia8->readString(ss.str());

    if(proc == 1)
    {
        std::stringstream bosonmass_str; bosonmass_str<< "32:m0=" << boson_mass ;
        pythia8->readString(bosonmass_str.str());
        pythia8->readString("NewGaugeBoson:ffbar2gmZZprime= on");
        pythia8->readString("Zprime:gmZmode=3");
        pythia8->readString("32:onMode = off");
        pythia8->readString("32:onIfAny = 6");
        pythia8->readString("24:onMode = off");
        pythia8->readString("24:onIfAny = 1 2 3 4");
        pythia8->init();
    }
    else if(proc == 2)
    {
        std::stringstream bosonmass_str; bosonmass_str<< "34:m0=" << boson_mass ;
        pythia8->readString(bosonmass_str.str());
        pythia8->readString("NewGaugeBoson:ffbar2Wprime = on");
        pythia8->readString("Wprime:coup2WZ=1");
        pythia8->readString("34:onMode = off");
        pythia8->readString("34:onIfAny = 23 24");
        pythia8->readString("24:onMode = off");
        pythia8->readString("24:onIfAny = 1 2 3 4");
        pythia8->readString("23:onMode = off");
        pythia8->readString("23:onIfAny = 12");
        pythia8->init();
    }
    else if(proc == 3)
    {
        std::stringstream bosonmass_str; bosonmass_str<< "34:m0=" << boson_mass ;
        pythia8->readString(bosonmass_str.str());
        pythia8->readString("NewGaugeBoson:ffbar2Wprime = on");
        pythia8->readString("Wprime:coup2WZ=1");
        pythia8->readString("34:onMode = off");
        pythia8->readString("34:onIfAny = 23 24");
        pythia8->readString("24:onMode = off");
        pythia8->readString("24:onIfAny = 11 12");
        pythia8->readString("23:onMode = off");
        pythia8->readString("23:onIfAny = 1 2 3 4 5");
        pythia8->init();
    }
    else if(proc == 4)
    { 
        pythia8->readString("HardQCD:all = on");
        std::stringstream ptHatMin;
        std::stringstream ptHatMax;
        ptHatMin << "PhaseSpace:pTHatMin  =" << pThatmin;
        ptHatMax << "PhaseSpace:pTHatMax  =" << pThatmax;
        pythia8->readString(ptHatMin.str());
        pythia8->readString(ptHatMax.str());
        pythia8->init();
    }
    else 
    {
        throw std::invalid_argument("received invalid 'process'");
    }

    //Setup the pileup
    Pythia8::Pythia* pythia_MB = new Pythia8::Pythia();
    pythia_MB->readString("Random:setSeed = on");   
    ss.clear(); ss.str(""); ss << "Random:seed = " << seed+1; 
    cout << ss.str() << endl; 
    pythia_MB->readString(ss.str());
    pythia_MB->readString("SoftQCD:nonDiffractive = on");
    pythia_MB->readString("HardQCD:all = off");
    pythia_MB->readString("PhaseSpace:pTHatMin  = .1");
    pythia_MB->readString("PhaseSpace:pTHatMax  = 20000");
    pythia_MB->init();

    MIAnalysis * analysis = new MIAnalysis();
    analysis->SetOutName(outName);
    analysis->Begin();
    analysis->Debug(fDebug);

    std::cout << pileup << " is the number of pileu pevents " << std::endl;

    // Event loop
    for (Int_t iev = 0; iev < nEvents; iev++) 
    {
        if (iev%10==0)
        {
            std::cout << iev << std::endl;
        }
        analysis->AnalyzeEvent(iev, pythia8, pythia_MB, pileup);
    }

    analysis->End();

    // that was it
    delete pythia8;
    delete analysis;

    return 0;
}