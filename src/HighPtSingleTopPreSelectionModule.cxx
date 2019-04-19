#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/LumiSelection.h"
#include "UHH2/common/include/AdditionalSelections.h"
#include "UHH2/common/include/TriggerSelection.h"
#include "UHH2/common/include/ObjectIdUtils.h"
#include "UHH2/common/include/ElectronIds.h"
#include "UHH2/common/include/MuonIds.h"
#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/PrimaryLepton.h"

#include "UHH2/HighPtSingleTop/include/AndHists.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopModules.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopSelections.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopHists.h"
#include "UHH2/HighPtSingleTop/include/SingleTopGen_tWch.h"


using namespace std;
using namespace uhh2;

namespace uhh2 {

  class HighPtSingleTopPreSelectionModule: public AnalysisModule {
  public:
    
    explicit HighPtSingleTopPreSelectionModule(Context & ctx);
    virtual bool process(Event & event) override;
    
  private:
    
    std::unique_ptr<AnalysisModule> object_setup, clnr_muon, clnr_elec;
    std::unique_ptr<AnalysisModule> primarylep;
    std::unique_ptr<AnalysisModule> SingleTopGen_tWchProd;

    std::unique_ptr<Selection> slct_lumi, slct_mttbarGenCut;
    std::unique_ptr<Selection> trig_IsoMu24, trig_IsoTkMu24, trig_Ele27, trig_Ele115, trig_Pho175;
    std::unique_ptr<Selection> slct_1muon, slct_0muon, slct_1elec, slct_0elec;
    std::unique_ptr<Selection> slct_met, slct_1jet, slct_1hotvr, slct_tWgenSignal;
    
    std::unique_ptr<AndHists> hist_nocuts, hist_trigger, hist_cleaning, hist_1lepton, hist_met, hist_1jet, hist_1hotvr;

    bool is_data, is_mc, is_muon, is_elec;
    string dataset_version;

    MuonId muonID, muonID_veto;
    double muonPt_min, muonEta_max, muonIso_max, muonPt_min_veto, muonEta_max_veto;
    ElectronId elecID, elecID_veto;
    double elecPt_min, elecEta_max, elecPt_min_veto, elecEta_max_veto;
    JetId jetID;
    double jetPt_min, jetEta_max;
    TopJetId hotvrID;
    double hotvrPt_min, hotvrEta_max;
    double met_min;
  };


  HighPtSingleTopPreSelectionModule::HighPtSingleTopPreSelectionModule(Context & ctx){

    //------//
    // KEYS //
    //------//
    
    is_data = ctx.get("dataset_type") == "DATA";
    is_mc   = ctx.get("dataset_type") == "MC";
    is_muon = ctx.get("analysis_channel") == "MUON";
    is_elec = ctx.get("analysis_channel") == "ELECTRON";

    if(!(is_data || is_mc)) throw runtime_error("HighPtSingleTopPreSelectionModule: Dataset is labeled as neither DATA nor MC. Please check the XML config file!");
    if(!(is_muon || is_elec)) throw runtime_error("HighPtSingleTopPreSelectionModule: Analysis channel ( ELECTRON / MUON ) not correctly given. Please check the XML config file!");

    dataset_version = ctx.get("dataset_version");


    //---------------------//
    // KINEMATIC VARIABLES //
    //---------------------//

    muonPt_min  = 50.0;
    muonEta_max =  2.4;
    muonIso_max =  0.15;

    elecPt_min  = 50.0;
    elecEta_max =  2.4;

    met_min = 30.0;


    //-----------------//
    // IDENTIFICATIONS //
    //-----------------//

    muonID = AndId<Muon>(MuonIDTight(), PtEtaCut(muonPt_min, muonEta_max), MuonIso(muonIso_max));
    elecID = AndId<Electron>(ElectronID_Spring16_tight, PtEtaCut(elecPt_min, elecEta_max));


    //----------------//
    // EVENT CLEANING //
    //----------------//

    object_setup.reset(new ObjectSetup(ctx)); // includes jet corrections
    clnr_muon.reset(new MuonCleaner(muonID));
    clnr_elec.reset(new ElectronCleaner(elecID));


    //------------//
    // SELECTIONS //
    //------------//

    slct_lumi.reset(new LumiSelection(ctx));
    slct_mttbarGenCut.reset(new MttbarGenSelection(0, 700));
    // For recommendations, see https://twiki.cern.ch/twiki/bin/view/CMS/SingleTopTWRun2
    trig_IsoMu24.reset(new TriggerSelection("HLT_IsoMu24_v*"));
    trig_IsoTkMu24.reset(new TriggerSelection("HLT_IsoTkMu24_v*"));
    trig_Ele27.reset(new TriggerSelection("HLT_Ele27_WPTight_Gsf_v*"));
    trig_Ele115.reset(new TriggerSelection("HLT_Ele115_CaloIdVT_GsfTrkIdT_v*"));
    trig_Pho175.reset(new TriggerSelection("HLT_Photon175_v*"));
    slct_1muon.reset(new NMuonSelection(1, 1));
    slct_0muon.reset(new NMuonSelection(0, 0));
    slct_1elec.reset(new NElectronSelection(1, 1));
    slct_0elec.reset(new NElectronSelection(0, 0));
    slct_met.reset(new METSelection(met_min));
    slct_1jet.reset(new NJetSelection(1, -1));
    slct_1hotvr.reset(new NTopJetSelection(1, -1));
    slct_tWgenSignal.reset(new tWgenSignalSelection(ctx, is_muon));


    //------------//
    // HISTOGRAMS //
    //------------//

    hist_nocuts.reset(new AndHists(ctx, "0_NoCuts"));
    hist_trigger.reset(new AndHists(ctx, "1_Trigger"));
    hist_cleaning.reset(new AndHists(ctx, "2_Cleaning"));
    hist_1lepton.reset(new AndHists(ctx, "3_OneLepton"));
    hist_met.reset(new AndHists(ctx, "4_MET"));
    hist_1jet.reset(new AndHists(ctx, "5_OneJet"));
    hist_1hotvr.reset(new AndHists(ctx, "6_OneHotvr"));


    //---------------//
    // MISCELLANEOUS //
    //---------------//

    primarylep.reset(new PrimaryLepton(ctx));
    SingleTopGen_tWchProd.reset(new SingleTopGen_tWchProducer(ctx, "h_GENtW"));
  }


  //------------//
  // EVENT LOOP //
  //------------//

  bool HighPtSingleTopPreSelectionModule::process(Event & event) {
    
    // Luminosity selection
    if(event.isRealData && !slct_lumi->passes(event)) return false;
    hist_nocuts->fill(event);

    // Mttbar gencut
    if(dataset_version == "TTbarM0to700" && !slct_mttbarGenCut->passes(event)) return false;

    // Trigger paths
    if(is_muon) {
      if(!(trig_IsoMu24->passes(event))) return false;
    }
    else if(is_elec) {
      if(!(trig_Ele27->passes(event) || trig_Pho175->passes(event))) return false;
    }
    hist_trigger->fill(event);

    // Initial event cleaning and jet corrections
    if(!object_setup->process(event)) return false;
    hist_cleaning->fill(event);

    // Single-lepton selection and veto on additional leptons
    if(is_muon) {
      if(!(slct_1muon->passes(event) && slct_0elec->passes(event))) return false;
      clnr_muon->process(event);
      if(!slct_1muon->passes(event)) return false;
    }
    else if(is_elec) {
      if(!(slct_1elec->passes(event) && slct_0muon->passes(event))) return false;
      clnr_elec->process(event);
      if(!slct_1elec->passes(event)) return false;
    }
    primarylep->process(event);
    hist_1lepton->fill(event);

    // MET selection
    if(!slct_met->passes(event)) return false;
    hist_met->fill(event);

    // At least one AK4 jet
    if(!slct_1jet->passes(event)) return false;
    hist_1jet->fill(event);

    // At least one HOTVR jet
    if(!slct_1hotvr->passes(event)) return false;
    hist_1hotvr->fill(event);

    // Split up tW samples into SIGNAL and OTHER depending on MC truth info
    if(dataset_version.find("ST_tW") == 0) {
      SingleTopGen_tWchProd->process(event);
      if(dataset_version.find("ST_tW_signal") == 0 && !slct_tWgenSignal->passes(event)) return false;
      else if(dataset_version.find("ST_tW_other") == 0 && slct_tWgenSignal->passes(event)) return false;
    }

    // Place additional selections into a new Module!!!
    // End of preselection
    return true;
  }


  UHH2_REGISTER_ANALYSIS_MODULE(HighPtSingleTopPreSelectionModule)
}
