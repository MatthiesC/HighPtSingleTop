#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/JetHists.h"
#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/PrimaryLepton.h"
#include "UHH2/common/include/TopJetIds.h"

#include "UHH2/HighPtSingleTop/include/AndHists.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopHists.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopSelections.h"
#include "UHH2/HighPtSingleTop/include/DNNSetup.h"

#include "UHH2/HOTVR/include/HOTVRHists.h"
#include "UHH2/HOTVR/include/HOTVRIds.h"
#include "UHH2/HOTVR/include/HOTVRScaleFactor.h"
#include "UHH2/HOTVR/include/HadronicTop.h"


using namespace std;
using namespace uhh2;

namespace uhh2 {

  class HighPtSingleTopMainSelectionModule: public AnalysisModule {
  public:
    
    explicit HighPtSingleTopMainSelectionModule(Context & ctx);
    virtual bool process(Event & event) override;
    
  private:
    
    unique_ptr<AnalysisModule> sf_lumi, sf_pileup, sf_muon_trig, sf_muon_id, sf_muon_iso, sf_muon_trk, sf_toptag;
    unique_ptr<AnalysisModule> scale_variation, primarylep, hadronictop, dnn_setup;

    unique_ptr<Selection> slct_deltaRcut, slct_1toptag;
    
    unique_ptr<AndHists> hist_noweights, hist_lumipuweights, hist_leptonsf, hist_deltaRcut, hist_1toptag;
 
    bool is_data, is_mc, is_muon, is_elec, bTopPtReweighting;
    string dataset_version;
    string syst_pileup, syst_muon_trigger, syst_muon_id, syst_muon_iso, syst_muon_trk, syst_hotvr_toptag, syst_btag;

    double hotvr_fpt_max, hotvr_jetmass_min, hotvr_jetmass_max, hotvr_mpair_min, hotvr_tau32_max;
    double deltaR_lepton_nextjet_min;

    TopJetId StandardHOTVRTopTagID;
 
    vector<Event::Handle<float>> h_dnn_inputs;
    Event::Handle<float> h_event_weight, h_toptag_pt;
  };


  HighPtSingleTopMainSelectionModule::HighPtSingleTopMainSelectionModule(Context & ctx) {

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

    syst_pileup = ctx.get("SystDirection_Pileup", "nominal");
    syst_muon_trigger = ctx.get("SystDirection_MuonTrig", "nominal");
    syst_muon_id = ctx.get("SystDirection_MuonId", "nominal");
    syst_muon_iso = ctx.get("SystDirection_MuonIso", "nominal");
    syst_muon_trk = ctx.get("SystDirection_MuonTrk", "nominal");
    syst_hotvr_toptag = ctx.get("SystDirection_HOTVRTopTagSF", "nominal");


    //---------------------//
    // KINEMATIC VARIABLES //
    //---------------------//

    // t-tagging criteria
    hotvr_fpt_max     = 0.8;
    hotvr_jetmass_min = 140;
    hotvr_jetmass_max = 220;
    hotvr_mpair_min   = 50;
    hotvr_tau32_max   = 0.56;

    deltaR_lepton_nextjet_min = 0.4; // minimum R-distance between primary lepton and next AK4 jet


    //-----------------//
    // IDENTIFICATIONS //
    //-----------------//

    StandardHOTVRTopTagID = AndId<TopJet>(HOTVRTopTag(hotvr_fpt_max, hotvr_jetmass_min, hotvr_jetmass_max, hotvr_mpair_min), Tau32Groomed(hotvr_tau32_max));


    //----------------//
    // EVENT CLEANING //
    //----------------//


    //---------------//
    // SCALE FACTORS //
    //---------------//

    sf_lumi.reset(new MCLumiWeight(ctx));
    sf_pileup.reset(new MCPileupReweight(ctx, syst_pileup));
    sf_muon_trig.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/matthies/Analysis_80x_v5/CMSSW_8_0_24_patch1/src/UHH2/common/data/MuonTrigger_EfficienciesAndSF_average_RunBtoH.root", "IsoMu24_OR_IsoTkMu24_PtEtaBins", 0.5, "trigger", true, syst_muon_trigger));
    sf_muon_id.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/matthies/Analysis_80x_v5/CMSSW_8_0_24_patch1/src/UHH2/common/data/MuonID_EfficienciesAndSF_average_RunBtoH.root", "MC_NUM_TightID_DEN_genTracks_PAR_pt_eta", 1, "tightID", true, syst_muon_id));
    sf_muon_iso.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/matthies/Analysis_80x_v5/CMSSW_8_0_24_patch1/src/UHH2/common/data/MuonIso_EfficienciesAndSF_average_RunBtoH.root", "TightISO_TightID_pt_eta", 1, "iso", true, syst_muon_iso));
    sf_muon_trk.reset(new MCMuonTrkScaleFactor(ctx, "/nfs/dust/cms/user/matthies/Analysis_80x_v5/CMSSW_8_0_24_patch1/src/UHH2/common/data/Tracking_EfficienciesAndSF_BCDEFGH.root", 1, "track", syst_muon_trk));
    scale_variation.reset(new MCScaleVariation(ctx));
    sf_toptag.reset(new HOTVRScaleFactor(ctx, StandardHOTVRTopTagID, syst_hotvr_toptag));


    //---------------//
    // MISCELLANEOUS //
    //---------------//

    primarylep.reset(new PrimaryLepton(ctx));
    hadronictop.reset(new HadronicTop(ctx));
    dnn_setup.reset(new DNNSetup(ctx, h_dnn_inputs, 3, 8));
    h_event_weight = ctx.declare_event_output<float>("DNN_EventWeight");
    h_toptag_pt = ctx.declare_event_output<float>("DNN_TopTagPt");


    //------------//
    // SELECTIONS //
    //------------//

    slct_deltaRcut.reset(new DeltaRCut(ctx, deltaR_lepton_nextjet_min));
    slct_1toptag.reset(new NTopJetSelection(1, 1, StandardHOTVRTopTagID));


    //------------//
    // HISTOGRAMS //
    //------------//

    hist_noweights.reset(new AndHists(ctx, "0_NoWeights"));
    hist_noweights->add_hist(new HighPtSingleTopHists(ctx, "0_NoWeights_CustomHists"));
    hist_lumipuweights.reset(new AndHists(ctx, "1_LumiAndPileupWeights"));
    hist_lumipuweights->add_hist(new HighPtSingleTopHists(ctx, "1_LumiAndPileupWeights_CustomHists"));
    hist_leptonsf.reset(new AndHists(ctx, "2_LeptonScaleFactors"));
    hist_leptonsf->add_hist(new HighPtSingleTopHists(ctx, "2_LeptonScaleFactors_CustomHists"));
    hist_deltaRcut.reset(new AndHists(ctx, "3_DeltaRCut"));
    hist_deltaRcut->add_hist(new HighPtSingleTopHists(ctx, "3_DeltaRCut_CustomHists"));
    hist_1toptag.reset(new AndHists(ctx, "4_OneTopTag"));
    hist_1toptag->add_hist(new HighPtSingleTopHists(ctx, "4_OneTopTag_CustomHists"));
    hist_1toptag->add_hist(new HOTVRHists(ctx, "4_OneTopTag_HOTVRTopTag", StandardHOTVRTopTagID));
}


  //------------//
  // EVENT LOOP //
  //------------//

  bool HighPtSingleTopMainSelectionModule::process(Event & event) {

    // Some space for preselection optimization
    // ...


    // Scale variations
    scale_variation->process(event);

    // Apply scale factors
    hist_noweights->fill(event);
    sf_lumi->process(event);
    sf_pileup->process(event);
    hist_lumipuweights->fill(event);
    if(is_muon) {
      sf_muon_trig->process(event);
      sf_muon_id->process(event);
      sf_muon_iso->process(event);
      sf_muon_trk->process(event);
    }
    else if(is_elec) {
      // electron trigger, id, reco
    }
    hist_leptonsf->fill(event);

    // Apply cut on DeltaR(lepton, jet) to reject QCD
    primarylep->process(event);
    if(!slct_deltaRcut->passes(event)) return false;
    hist_deltaRcut->fill(event);

    // Require exactly one HOTVR t-tag
    if(!slct_1toptag->passes(event)) return false;
    hadronictop->process(event);
    sf_toptag->process(event);
    hist_1toptag->fill(event);

    // DNN setup
    dnn_setup->process(event);
    event.set(h_event_weight, event.weight);
    TopJet toptaggedjet;
    for(auto t : *event.topjets) { if(StandardHOTVRTopTagID(t, event)) toptaggedjet = t; }
    event.set(h_toptag_pt, toptaggedjet.v4().Pt());

    // Place analysis routines into a new Module!!!
    // End of main selection
    return true;
  }


  UHH2_REGISTER_ANALYSIS_MODULE(HighPtSingleTopMainSelectionModule)
}
