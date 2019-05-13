// @(#)root/tmva $Id$
/**********************************************************************************
 * Project   : TMVA - a ROOT-integrated toolkit for multivariate data analysis    *
 * Package   : TMVA                                                               *
 * Root Macro: TMVAClassification                                                 *
 *                                                                                *
 * This macro provides examples for the training and testing of the               *
 * TMVA classifiers.                                                              *
 *                                                                                *
 * As input data is used a toy-MC sample consisting of four Gaussian-distributed  *
 * and linearly correlated input variables.                                       *
 *                                                                                *
 * The methods to be used can be switched on and off by means of booleans, or     *
 * via the prompt command, for example:                                           *
 *                                                                                *
 *    root -l ./TMVAClassification.C\(\"Fisher,Likelihood\"\)                     *
 *                                                                                *
 * (note that the backslashes are mandatory)                                      *
 * If no method given, a default set of classifiers is used.                      *
 *                                                                                *
 * The output file "TMVA.root" can be analysed with the use of dedicated          *
 * macros (simply say: root -l <macro.C>), which can be conveniently              *
 * invoked through a GUI that will appear at the end of the run of this macro.    *
 * Launch the GUI via the command:                                                *
 *                                                                                *
 *    root -l ./TMVAGui.C                                                         *
 *                                                                                *
 * You can also compile and run the example with the following commands           *
 *                                                                                *
 *    make                                                                        *
 *    ./TMVAClassification <Methods>                                              *
 *                                                                                *
 * where: <Methods> = "method1 method2"                                           *
 *        are the TMVA classifier names                                           *
 *                                                                                *
 * example:                                                                       *
 *    ./TMVAClassification Fisher LikelihoodPCA BDT                               *
 *                                                                                *
 * If no method given, a default set is of classifiers is used                    *
 **********************************************************************************/

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#include "TMVA/TMVAGui.h"

int TMVAClassification_BDT_adv( TString myMethodList = "" )
{
   // The explicit loading of the shared libTMVA is done in TMVAlogon.C, defined in .rootrc
   // if you use your private .rootrc, or run from a different directory, please copy the
   // corresponding lines from .rootrc

   // methods to be processed can be given as an argument; use format:
   //
   // mylinux~> root -l TMVAClassification.C\(\"myMethod1,myMethod2,myMethod3\"\)
   //
   // if you like to use a method via the plugin mechanism, we recommend using
   //
   // mylinux~> root -l TMVAClassification.C\(\"P_myMethod\"\)
   // (an example is given for using the BDT as plugin (see below),
   // but of course the real application is when you write your own
   // method based)

   //---------------------------------------------------------------
   // This loads the library
   TMVA::Tools::Instance();

   // Default MVA methods to be trained + tested
   std::map<std::string,int> Use;

   // --- Boosted Decision Trees
   Use["BDT_AdaB_Gini"] = 1;
   Use["BDT_AdaB_CrEn"] = 0;

   Use["BDT_Grad_Gini"] = 0;
   Use["BDT_Grad_CrEn_100Trees_20Cuts"] = 0;

   Use["BDT_Grad_CrEn_100Trees_50Cuts"] = 0;
   Use["BDT_Grad_CrEn_200Trees_20Cuts"] = 0;
   Use["BDT_Grad_CrEn_200Trees_50Cuts"] = 0;
   Use["BDT_Grad_CrEn_200Trees_50Cuts_3Depth"] = 0;



   // ---------------------------------------------------------------

   std::cout << std::endl;
   std::cout << "==> Start TMVAClassification" << std::endl;

   // Select methods (don't look at this code - not of interest)
   if (myMethodList != "") {
      for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

      std::vector<TString> mlist = TMVA::gTools().SplitString( myMethodList, ',' );
      for (UInt_t i=0; i<mlist.size(); i++) {
         std::string regMethod(mlist[i]);

         if (Use.find(regMethod) == Use.end()) {
            std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
            for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";
            std::cout << std::endl;
            return 1;
         }
         Use[regMethod] = 1;
      }
   }

   // --------------------------------------------------------------------------------------------------

   // --- Here the preparation phase begins

   // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
   TString outfileName( "TMVAOutput_BDT_advanced_AdaB_Gini.root" );
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

   // Create the factory object. Later you can choose the methods
   // whose performance you'd like to investigate. The factory is 
   // the only TMVA object you have to interact with
   //
   // The first argument is the base of the name of all the
   // weightfiles in the directory weight/
   //
   // The second argument is the output file for the training results
   // All TMVA output can be suppressed by removing the "!" (not) in
   // front of the "Silent" argument in the option string
   TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification_BDT_adv", outputFile,
                                               "!V:!Silent:Color:DrawProgressBar:AnalysisType=Classification" ); //Transformations=I;D;P;G,D

   // If you wish to modify default settings
   // (please check "src/Config.h" to see all available global options)
   //    (TMVA::gConfig().GetVariablePlotting()).fTimesRMS = 8.0;
   //    (TMVA::gConfig().GetIONames()).fWeightFileDir = "myWeightDirectory";

   // Define the input variables that shall be used for the MVA training
   // note that you may also use variable expressions, such as: "3*var1/var2*abs(var3)"
   // [all types of expressions that can also be parsed by TTree::Draw( "expression" )]
   /*factory->AddVariable( "myvar1 := var1+var2", 'F' );
   factory->AddVariable( "myvar2 := var1-var2", "Expression 2", "", 'F' );
   factory->AddVariable( "var3",                "Variable 3", "units", 'F' );
   factory->AddVariable( "var4",                "Variable 4", "units", 'F' );*/

   // in case of 1-btag region, remove first variable:


   factory->AddVariable("tmva_jet1_pt", "Jet1 pT", "GeV", 'F' );
   factory->AddVariable("tmva_jet2_pt", "Jet2 pT", "GeV", 'F' );
   //factory->AddVariable("tmva_jet3_pt", "Jet3 pT", "GeV", 'F' );
   //factory->AddVariable("tmva_jet4_pt", "Jet4 pT", "GeV", 'F' );
   //factory->AddVariable("tmva_jet5_pt", "Jet5 pT", "GeV", 'F' );
   //factory->AddVariable("tmva_jet6_pt", "Jet6 pT", "GeV", 'F' );
   factory->AddVariable("tmva_jet1_eta", "Jet1 |#eta|", "", 'F' );
   factory->AddVariable("tmva_jet2_eta", "Jet2 |#eta|", "", 'F' );
   //factory->AddVariable("tmva_jet3_eta", "Jet3 |#eta|", "", 'F' );
   //factory->AddVariable("tmva_jet4_eta", "Jet4 |#eta|", "", 'F' );
   //factory->AddVariable("tmva_jet5_eta", "Jet5 |#eta|", "", 'F' );
   //factory->AddVariable("tmva_jet6_eta", "Jet6 |#eta|", "", 'F' );
   factory->AddVariable("tmva_number_ak4", "Number of AK4 jets", "", 'I' );

   factory->AddSpectator("tmva_hotvr1_pt", "HOTVRJet1 pT", "GeV", 'F' );

   //factory->AddVariable("tmva_hotvr2_pt", "HOTVRJet2 pT", "GeV", 'F' );
   //factory->AddVariable("tmva_hotvr3_pt", "HOTVRJet3 pT", "GeV", 'F' );
   //factory->AddVariable("tmva_hotvr4_pt", "HOTVRJet4 pT", "GeV", 'F' );
   //factory->AddVariable("tmva_hotvr1_eta", "HOTVRJet1 |#eta|", "", 'F' );
   //factory->AddVariable("tmva_hotvr2_eta", "HOTVRJet2 |#eta|", "", 'F' );
   //factory->AddVariable("tmva_hotvr3_eta", "HOTVRJet3 |#eta|", "", 'F' );
   //factory->AddVariable("tmva_hotvr4_eta", "HOTVRJet4 |#eta|", "", 'F' );
   factory->AddVariable("tmva_number_hotvr", "Number of HOTVR jets", "", 'I' );
   factory->AddVariable("tmva_jet1_area", "atan(Jet1 radius)", "", 'F' );
   factory->AddVariable("tmva_jet2_area", "atan(Jet2 radius)", "", 'F' );
   //factory->AddVariable("tmva_jet3_area", "atan(Jet3 radius)", "", 'F' );
   //factory->AddVariable("tmva_jet4_area", "atan(Jet4 radius)", "", 'F' );
   //factory->AddVariable("tmva_jet5_area", "atan(Jet5 radius)", "", 'F' );
   //factory->AddVariable("tmva_jet6_area", "atan(Jet6 radius)", "", 'F' );
   //factory->AddVariable("tmva_hotvr1_area", "atan(HOTVRJet1 radius)", "", 'F' );
   //factory->AddVariable("tmva_hotvr2_area", "atan(HOTVRJet2 radius)", "", 'F' );
   //factory->AddVariable("tmva_hotvr3_area", "atan(HOTVRJet3 radius)", "", 'F' );
   //factory->AddVariable("tmva_hotvr4_area", "atan(HOTVRJet4 radius)", "", 'F' );
   factory->AddVariable("tmva_met_pt", "MET pT", "GeV", 'F' );
   factory->AddVariable("tmva_deltaPhi_met_lepton", "#Delta#phi(MET,l)", "", 'F' );
   factory->AddVariable("tmva_deltaPhi_met_ak4", "#Delta#phi(MET,nextJet)", "", 'F' );
   factory->AddVariable("tmva_deltaPhi_met_hotvr1", "#Delta#phi(MET,HOTVRJet1)", "", 'F' );
   factory->AddVariable("tmva_deltaPhi_lepton_ak4", "#Delta#phi(l,nextJet)", "", 'F' );
   factory->AddVariable("tmva_deltaPhi_lepton_hotvr1", "#Delta#phi(l,HOTVRJet1)", "", 'F' );
   factory->AddVariable("tmva_mtw", "MT(l,MET)", "GeV", 'F' );
   factory->AddVariable("tmva_deltaR_lepton_ak4", "#DeltaR(l,nextJet)", "", 'F' );
   factory->AddVariable("tmva_deltaR_lepton_hotvr1", "#DeltaR(l,HOTVRJet1)", "", 'F' );
   factory->AddVariable("tmva_lepton_eta", "l |#eta|", "", 'F' );
   factory->AddVariable("tmva_lepton_pt", "l pT", "GeV", 'F' );
   factory->AddVariable("tmva_deltaR_jet1_ak4", "#DeltaR(Jet1,nextJet)", "", 'F' );
   factory->AddVariable("tmva_deltaPhi_jet1_ak4", "#Delta#phi(Jet1,nextJet)", "", 'F' );
   factory->AddVariable("tmva_deltaR_jet2_ak4", "#DeltaR(Jet2,nextJet)", "", 'F' );
   factory->AddVariable("tmva_deltaPhi_jet2_ak4", "#Delta#phi(Jet2,nextJet)", "", 'F' );

   factory->AddSpectator("tmva_toptag_pt", "t-tag pT", "GeV", 'F');


   // You can add so-called "Spectator variables", which are not used in the MVA training,
   // but will appear in the final "TestTree" produced by TMVA. This TestTree will contain the
   // input variables, the response values of all trained MVAs, and the spectator variables
   /*factory->AddSpectator( "spec1 := var1*2",  "Spectator 1", "units", 'F' );
     factory->AddSpectator( "spec2 := var1*3",  "Spectator 2", "units", 'F' );*/

   // Read training and test data
   
   TString dirname = "/nfs/dust/cms/user/matthies/HighPtSingleTop/80X_v5/MainSelection/Muon/";

   std::vector<TFile*> input;
   
   // Signal samples
   input.push_back( TFile::Open( dirname+"uhh2.AnalysisModuleRunner.MC.ST_tW_signal_T.root" ) );
   input.push_back( TFile::Open( dirname+"uhh2.AnalysisModuleRunner.MC.ST_tW_signal_Tbar.root" ) );
   //input.push_back( TFile::Open( dirname+"uhh2.AnalysisModuleRunner.MC.ST_tW_other_T.root" ) );
   //input.push_back( TFile::Open( dirname+"uhh2.AnalysisModuleRunner.MC.ST_tW_other_Tbar.root" ) );

   // TTbar background samples
   input.push_back( TFile::Open( dirname+"uhh2.AnalysisModuleRunner.MC.TTbarM0to700.root" ) );
   input.push_back( TFile::Open( dirname+"uhh2.AnalysisModuleRunner.MC.TTbarM700to1000.root" ) );
   input.push_back( TFile::Open( dirname+"uhh2.AnalysisModuleRunner.MC.TTbarM1000toInf.root" ) );


   for( int i = 0; i < input.size(); ++i )
     {
       std::cout << "--- TMVAClassification       : Using input file: " << input.at(i)->GetName() << std::endl;
     }

   // --- Register the training and test trees

   // Make sure that the input trees are actually non-empty! Else: Fatal error

   std::vector<TTree*> signal;

   signal.push_back( (TTree*)input.at(0)->Get("AnalysisTree") );
   signal.push_back( (TTree*)input.at(1)->Get("AnalysisTree") );

   std::vector<TTree*> background;

   for( int i = 2; i < input.size(); ++i )
     {
       background.push_back( (TTree*)input.at(i)->Get("AnalysisTree") );
     }

   // global event weights per tree (see below for setting event-wise weights)
   Double_t signalWeight     = 1.0;
   Double_t backgroundWeight = 1.0;
   
   // You can add an arbitrary number of signal or background trees
   factory->AddSignalTree( signal.at(0), signalWeight );
   factory->AddSignalTree( signal.at(1), signalWeight );
   for( int i = 0; i < background.size(); ++i )
     {
       factory->AddBackgroundTree( background.at(i), backgroundWeight );
     }

   // To give different trees for training and testing, do as follows:
   //    factory->AddSignalTree( signalTrainingTree, signalTrainWeight, "Training" );
   //    factory->AddSignalTree( signalTestTree,     signalTestWeight,  "Test" );
   
   // Use the following code instead of the above two or four lines to add signal and background
   // training and test events "by hand"
   // NOTE that in this case one should not give expressions (such as "var1+var2") in the input
   //      variable definition, but simply compute the expression before adding the event
   //
   //     // --- begin ----------------------------------------------------------
   //     std::vector<Double_t> vars( 4 ); // vector has size of number of input variables
   //     Float_t  treevars[4], weight;
   //     
   //     // Signal
   //     for (UInt_t ivar=0; ivar<4; ivar++) signal->SetBranchAddress( Form( "var%i", ivar+1 ), &(treevars[ivar]) );
   //     for (UInt_t i=0; i<signal->GetEntries(); i++) {
   //        signal->GetEntry(i);
   //        for (UInt_t ivar=0; ivar<4; ivar++) vars[ivar] = treevars[ivar];
   //        // add training and test events; here: first half is training, second is testing
   //        // note that the weight can also be event-wise
   //        if (i < signal->GetEntries()/2.0) factory->AddSignalTrainingEvent( vars, signalWeight );
   //        else                              factory->AddSignalTestEvent    ( vars, signalWeight );
   //     }
   //   
   //     // Background (has event weights)
   //     background->SetBranchAddress( "weight", &weight );
   //     for (UInt_t ivar=0; ivar<4; ivar++) background->SetBranchAddress( Form( "var%i", ivar+1 ), &(treevars[ivar]) );
   //     for (UInt_t i=0; i<background->GetEntries(); i++) {
   //        background->GetEntry(i);
   //        for (UInt_t ivar=0; ivar<4; ivar++) vars[ivar] = treevars[ivar];
   //        // add training and test events; here: first half is training, second is testing
   //        // note that the weight can also be event-wise
   //        if (i < background->GetEntries()/2) factory->AddBackgroundTrainingEvent( vars, backgroundWeight*weight );
   //        else                                factory->AddBackgroundTestEvent    ( vars, backgroundWeight*weight );
   //     }
         // --- end ------------------------------------------------------------
   //
   // --- end of tree registration 

   // Set individual event weights (the variables must exist in the original TTree)
   //    for signal    : factory->SetSignalWeightExpression    ("weight1*weight2");
   //    for background: factory->SetBackgroundWeightExpression("weight1*weight2");
   factory->SetSignalWeightExpression( "tmva_eventweight" );
   factory->SetBackgroundWeightExpression( "tmva_eventweight" );

   // Apply additional cuts on the signal and background samples (can be different)
   TCut mycuts = "tmva_number_ak4>=2"; // for example: TCut mycuts = "abs(var1)<0.5 && abs(var2-0.5)<1";
   TCut mycutb = "tmva_number_ak4>=2"; // for example: TCut mycutb = "abs(var1)<0.5";

   // Tell the factory how to use the training and testing events
   //
   // If no numbers of events are given, half of the events in the tree are used 
   // for training, and the other half for testing:
   //    factory->PrepareTrainingAndTestTree( mycut, "SplitMode=random:!V" );
   // To also specify the number of testing events, use:
   //    factory->PrepareTrainingAndTestTree( mycut,
   //                                         "NSigTrain=3000:NBkgTrain=3000:NSigTest=3000:NBkgTest=3000:SplitMode=Random:!V" );
   /*factory->PrepareTrainingAndTestTree( mycuts, mycutb,
     "nTrain_Signal=1000:nTrain_Background=1000:SplitMode=Random:NormMode=NumEvents:!V" );*/
 factory->PrepareTrainingAndTestTree( mycuts, mycutb,
				      "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:SplitSeed=100:NormMode=None:!V" ); // Find a value for SplitSeed that avoids Negative Bins
   // ---- Book MVA methods
   //
   // Please lookup the various method configuration options in the corresponding cxx files, eg:
   // src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html
   // it is possible to preset ranges in the option string in which the cut optimisation should be done:
   // "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable

 
 if (Use["BDT_AdaB_Gini"])
   factory->BookMethod( TMVA::Types::kBDT, "BDT_AdaB_Gini",
			"!H:!V:NTrees=2000:MinNodeSize=1.0%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.1:SeparationType=GiniIndex:nCuts=20" );
 if (Use["BDT_AdaB_CrEn"])
   factory->BookMethod( TMVA::Types::kBDT, "BDT_AdaB_CrEn",
			"!H:!V:NTrees=100:MinNodeSize=1.0%:MaxDepth=5:BoostType=AdaBoost:AdaBoostBeta=0.1:SeparationType=CrossEntropy:nCuts=20" );

 if (Use["BDT_Grad_Gini"])
   factory->BookMethod( TMVA::Types::kBDT, "BDT_Grad_Gini",
			"!H:!V:NTrees=100:MinNodeSize=1.0%:MaxDepth=5:BoostType=Grad:SeparationType=GiniIndex:nCuts=20" );
 if (Use["BDT_Grad_CrEn_100Trees_20Cuts"])
   factory->BookMethod( TMVA::Types::kBDT, "BDT_Grad_CrEn_100Trees_20Cuts",
			"!H:!V:NTrees=100:MinNodeSize=1.0%:MaxDepth=5:BoostType=Grad:SeparationType=CrossEntropy:nCuts=20" );

 if (Use["BDT_Grad_CrEn_100Trees_50Cuts"])
   factory->BookMethod( TMVA::Types::kBDT, "BDT_Grad_CrEn_100Trees_50Cuts",
			"!H:!V:NTrees=100:MinNodeSize=1.0%:MaxDepth=5:BoostType=Grad:SeparationType=CrossEntropy:nCuts=50" );
 if (Use["BDT_Grad_CrEn_200Trees_20Cuts"])
   factory->BookMethod( TMVA::Types::kBDT, "BDT_Grad_CrEn_200Trees_20Cuts",
			"!H:!V:NTrees=200:MinNodeSize=1.0%:MaxDepth=5:BoostType=Grad:SeparationType=CrossEntropy:nCuts=20" );
 if (Use["BDT_Grad_CrEn_200Trees_50Cuts"])
   factory->BookMethod( TMVA::Types::kBDT, "BDT_Grad_CrEn_200Trees_50Cuts",
			"!H:!V:NTrees=200:MinNodeSize=1.0%:MaxDepth=5:BoostType=Grad:SeparationType=CrossEntropy:nCuts=50" );
 if (Use["BDT_Grad_CrEn_200Trees_50Cuts_3Depth"])
   factory->BookMethod( TMVA::Types::kBDT, "BDT_Grad_CrEn_200Trees_50Cuts_3Depth",
			"!H:!V:NTrees=200:MinNodeSize=1.0%:MaxDepth=3:BoostType=Grad:SeparationType=CrossEntropy:nCuts=50" );



   // --------------------------------------------------------------------------------------------------

   // ---- Now you can optimize the setting (configuration) of the MVAs using the set of training events

   // ---- STILL EXPERIMENTAL and only implemented for BDT's ! 
   // factory->OptimizeAllMethods("SigEffAt001","Scan");
   // factory->OptimizeAllMethods("ROCIntegral","FitGA");

   // --------------------------------------------------------------------------------------------------

   // ---- Now you can tell the factory to train, test, and evaluate the MVAs

   // Train MVAs using the set of training events
   factory->TrainAllMethods();

   // ---- Evaluate all MVAs using the set of test events
   factory->TestAllMethods();

   // ----- Evaluate and compare performance of all configured MVAs
   factory->EvaluateAllMethods();

   // --------------------------------------------------------------

   // Save the output
   outputFile->Close();

   std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
   std::cout << "==> TMVAClassification is done!" << std::endl;

   delete factory;

   // Launch the GUI for the root macros
   if (!gROOT->IsBatch()) TMVA::TMVAGui( outfileName );

   return 0;
}

int main( int argc, char** argv )
{
   // Select methods (don't look at this code - not of interest)
   TString methodList; 
   for (int i=1; i<argc; i++) {
      TString regMethod(argv[i]);
      if(regMethod=="-b" || regMethod=="--batch") continue;
      if (!methodList.IsNull()) methodList += TString(","); 
      methodList += regMethod;
   }
   return TMVAClassification_BDT_adv(methodList); 
}
