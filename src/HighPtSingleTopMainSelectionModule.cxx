#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/JetHists.h"
#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/ObjectIdUtils.h"
#include "UHH2/common/include/PrimaryLepton.h"
#include "UHH2/common/include/TopJetIds.h"

#include "UHH2/HighPtSingleTop/include/AndHists.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopHists.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopSelections.h"
#include "UHH2/HighPtSingleTop/include/TaggedJets.h"
#include "UHH2/HighPtSingleTop/include/DNNSetup.h"
#include "UHH2/HighPtSingleTop/include/MatchHists.h"
#include "UHH2/HighPtSingleTop/include/TopTagHists.h"
#include "UHH2/HighPtSingleTop/include/ReconstructionAlgorithms.h"
#include "UHH2/HighPtSingleTop/include/DNNHists.h"

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
    
    unique_ptr<AnalysisModule> sf_lumi, sf_pileup, sf_muon_trig, sf_muon_id, sf_muon_iso, sf_muon_trk, sf_toptag, sf_btag;
    unique_ptr<AnalysisModule> scale_variation, primarylep, hadronictop, toptaggedjet, btaggedjets, wboson, pseudotop, SingleTopGen_tWchProd, dnn_setup;

    unique_ptr<Selection> slct_1toptag, slct_tW_merged3, slct_tW_merged2, slct_tW_merged1, slct_tW_merged0, slct_tW_TopToHad, slct_tW_WToTau;
    
    //unique_ptr<AndHists> hist_noweights, hist_lumipuweights;
    unique_ptr<AndHists> hist_leptonsf, hist_1toptag, hist_btagsf;
 
    unique_ptr<Hists> hist_btag_mc_efficiency, hist_decaymatch, hist_decaymatch_Pt0to300, hist_decaymatch_Pt300toInf, hist_decaymatch_Pt300to400, hist_decaymatch_Pt0to400, hist_decaymatch_Pt400toInf, hist_dnn, hist_dnn_Pt0to400, hist_dnn_Pt400toInf;
    bool is_data, is_mc, is_muon, is_elec;
    string dataset_version;
    string syst_pileup, syst_muon_trigger, syst_muon_id, syst_muon_iso, syst_muon_trk, syst_hotvr_toptag, syst_btag;

    double hotvr_fpt_max, hotvr_jetmass_min, hotvr_jetmass_max, hotvr_mpair_min, hotvr_tau32_max;

    TopJetId StandardHOTVRTopTagID;
    JetId BJetID;
 
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
    syst_btag = ctx.get("SystDirection_BTagSF", "nominal");


    //---------------------//
    // KINEMATIC VARIABLES //
    //---------------------//

    // t-tagging criteria
    hotvr_fpt_max     = 0.8;
    hotvr_jetmass_min = 140;
    hotvr_jetmass_max = 220;
    hotvr_mpair_min   = 50;
    hotvr_tau32_max   = 0.56;


    //-----------------//
    // IDENTIFICATIONS //
    //-----------------//

    StandardHOTVRTopTagID = AndId<TopJet>(HOTVRTopTag(hotvr_fpt_max, hotvr_jetmass_min, hotvr_jetmass_max, hotvr_mpair_min), Tau32Groomed(hotvr_tau32_max));//, PtEtaCut(600, 2.5)); // ATTENTION!!!!!!!!!!! Cut on pT!!!!!!
    BTag::algo btag_algo = BTag::DEEPJET;
    BTag::wp btag_workingpoint = BTag::WP_MEDIUM;
    BJetID = BTag(btag_algo, btag_workingpoint);


    //----------------//
    // EVENT CLEANING //
    //----------------//


    //---------------//
    // SCALE FACTORS //
    //---------------//

    sf_lumi.reset(new MCLumiWeight(ctx));
    sf_pileup.reset(new MCPileupReweight(ctx, syst_pileup));
    // only 2016 muon scale factors linked here, TODO later: distinguish between years and ele/muon
    sf_muon_trig.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/UHH2/common/data/2016/MuonTrigger_EfficienciesAndSF_average_RunBtoH.root", "IsoMu24_OR_IsoTkMu24_PtEtaBins", 0.5, "trigger", true, syst_muon_trigger));
    sf_muon_id.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/UHH2/common/data/2016/MuonID_EfficienciesAndSF_average_RunBtoH.root", "MC_NUM_TightID_DEN_genTracks_PAR_pt_eta", 1, "tightID", true, syst_muon_id));
    sf_muon_iso.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/UHH2/common/data/2016/MuonIso_EfficienciesAndSF_average_RunBtoH.root", "TightISO_TightID_pt_eta", 1, "iso", true, syst_muon_iso));
    sf_muon_trk.reset(new MCMuonTrkScaleFactor(ctx, "/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/UHH2/common/data/2016/Tracking_EfficienciesAndSF_BCDEFGH.root", 1, "track", syst_muon_trk));
    scale_variation.reset(new MCScaleVariation(ctx));
    sf_toptag.reset(new HOTVRScaleFactor(ctx, StandardHOTVRTopTagID, syst_hotvr_toptag));
    sf_btag.reset(new MCBTagScaleFactor(ctx, btag_algo, btag_workingpoint, "jets", syst_btag)); // can have more arguments, see MCWeight.h


    //---------------//
    // MISCELLANEOUS //
    //---------------//

    primarylep.reset(new PrimaryLepton(ctx));
    hadronictop.reset(new HadronicTop(ctx));
    toptaggedjet.reset(new TopTaggedJet(ctx, StandardHOTVRTopTagID));
    btaggedjets.reset(new BTaggedJets(ctx, btag_algo, btag_workingpoint));
    wboson.reset(new WBosonLeptonic(ctx));
    pseudotop.reset(new PseudoTopLeptonic(ctx));
    SingleTopGen_tWchProd.reset(new SingleTopGen_tWchProducer(ctx, "h_GENtW"));
    dnn_setup.reset(new DNNSetup(ctx, h_dnn_inputs, 3, 8, StandardHOTVRTopTagID, BJetID, 0.));
    h_event_weight = ctx.declare_event_output<float>("DNN_EventWeight");
    h_toptag_pt = ctx.declare_event_output<float>("DNN_TopTagPt");


    //------------//
    // SELECTIONS //
    //------------//

    slct_1toptag.reset(new NTopJetSelection(1, 1, StandardHOTVRTopTagID));
    slct_tW_merged3.reset(new MergeScenarioSelection(ctx, 3));
    slct_tW_merged2.reset(new MergeScenarioSelection(ctx, 2));
    slct_tW_merged1.reset(new MergeScenarioSelection(ctx, 1));
    slct_tW_merged0.reset(new MergeScenarioSelection(ctx, 0));
    slct_tW_TopToHad.reset(new tWgenSelection(ctx, "TopToHad", is_muon));
    slct_tW_WToTau.reset(new tWgenSelection(ctx, "WToTau", is_muon));


    //------------//
    // HISTOGRAMS //
    //------------//

    //hist_noweights.reset(new AndHists(ctx, "0_NoWeights"));
    //hist_noweights->add_hist(new HighPtSingleTopHists(ctx, "0_NoWeights_CustomHists"));

    //hist_lumipuweights.reset(new AndHists(ctx, "1_LumiAndPileupWeights"));
    //hist_lumipuweights->add_hist(new HighPtSingleTopHists(ctx, "1_LumiAndPileupWeights_CustomHists"));

    hist_leptonsf.reset(new AndHists(ctx, "2_LeptonScaleFactors"));
    hist_leptonsf->add_hist(new HighPtSingleTopHists(ctx, "2_LeptonScaleFactors_CustomHists"));

    hist_1toptag.reset(new AndHists(ctx, "3_OneTopTag"));
    hist_1toptag->add_hist(new HighPtSingleTopHists(ctx, "3_OneTopTag_CustomHists"));
    hist_1toptag->add_hist(new HOTVRHists(ctx, "3_OneTopTag_HOTVRTopTag", StandardHOTVRTopTagID));
    hist_1toptag->add_hist(new TopTagHists(ctx, "3_OneTopTag_TopTagHists_Full"));
    hist_1toptag->add_hist(new TopTagHists(ctx, "3_OneTopTag_TopTagHists_Pt0to300", 0, 300));
    hist_1toptag->add_hist(new TopTagHists(ctx, "3_OneTopTag_TopTagHists_Pt300toInf", 300));

    hist_btag_mc_efficiency.reset(new BTagMCEfficiencyHists(ctx, "BTagMCEfficiency", BJetID, "jets"));

    hist_btagsf.reset(new AndHists(ctx, "4_BTagScaleFactors"));
    hist_btagsf->add_hist(new HighPtSingleTopHists(ctx, "4_BTagScaleFactors_CustomHists"));
    hist_btagsf->add_hist(new HOTVRHists(ctx, "4_BTagScaleFactors_HOTVRTopTag", StandardHOTVRTopTagID));
    hist_btagsf->add_hist(new TopTagHists(ctx, "4_BTagScaleFactors_TopTagHists_Full"));
    hist_btagsf->add_hist(new TopTagHists(ctx, "4_BTagScaleFactors_TopTagHists_Pt0to300", 0, 300));
    hist_btagsf->add_hist(new TopTagHists(ctx, "4_BTagScaleFactors_TopTagHists_Pt300toInf", 300));

    hist_decaymatch.reset(new MatchHists(ctx, "MatchHists_Full"));
    hist_decaymatch_Pt0to300.reset(new MatchHists(ctx, "MatchHists_Pt0to300", 0, 300));
    hist_decaymatch_Pt300toInf.reset(new MatchHists(ctx, "MatchHists_Pt300toInf", 300));
    hist_decaymatch_Pt300to400.reset(new MatchHists(ctx, "MatchHists_Pt300to400", 300, 400));
    hist_decaymatch_Pt0to400.reset(new MatchHists(ctx, "MatchHists_Pt0to400", 0, 400));
    hist_decaymatch_Pt400toInf.reset(new MatchHists(ctx, "MatchHists_Pt400toInf", 400));

    hist_dnn.reset(new DNNHists(ctx, "DNNHists_Full"));
    hist_dnn_Pt0to400.reset(new DNNHists(ctx, "DNNHists_Pt0to400", 0, 400));
    hist_dnn_Pt400toInf.reset(new DNNHists(ctx, "DNNHists_Pt400toInf", 400));
}


  //------------//
  // EVENT LOOP //
  //------------//

  bool HighPtSingleTopMainSelectionModule::process(Event & event) {

    // Scale variations
    scale_variation->process(event);

    // Apply scale factors
    //hist_noweights->fill(event);
    sf_lumi->process(event);
    sf_pileup->process(event);
    //hist_lumipuweights->fill(event);
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
    primarylep->process(event);

    // Require exactly one HOTVR t-tag
    if(!slct_1toptag->passes(event)) return false;
    hadronictop->process(event);
    sf_toptag->process(event);
    toptaggedjet->process(event);
    hist_1toptag->fill(event);

    // Apply b-tag scale factors
    hist_btag_mc_efficiency->fill(event);
    sf_btag->process(event);
    hist_btagsf->fill(event);

    btaggedjets->process(event);
    wboson->process(event);
    pseudotop->process(event);

    // Split tW signal into 3-merged, 2-merged, 1-merged, 0-merged (== how many top decay products ended up inside t-tagged HOTVR jet)
    if(dataset_version.find("ST_tW") == 0) {
      SingleTopGen_tWchProd->process(event);
      if(dataset_version.find("ST_tW_merged3") == 0 && !slct_tW_merged3->passes(event)) return false;
      if(dataset_version.find("ST_tW_merged2") == 0 && !slct_tW_merged2->passes(event)) return false;
      if(dataset_version.find("ST_tW_merged1") == 0 && !slct_tW_merged1->passes(event)) return false;
      if(dataset_version.find("ST_tW_merged0") == 0 && !slct_tW_merged0->passes(event)) return false;
      // now, also check if we have signal-like decay but with intermediate tau leptons (however, not sure if tau actually decays into required lepton)
      bool is_TopToHadAndWToTau = slct_tW_TopToHad->passes(event) && slct_tW_WToTau->passes(event);
      if(dataset_version.find("ST_tW_bkg_TopToHadAndWToTau") == 0 && !is_TopToHadAndWToTau) return false;
      if(dataset_version.find("ST_tW_bkg_Else") == 0 && is_TopToHadAndWToTau) return false;
      hist_decaymatch->fill(event);
      hist_decaymatch_Pt0to300->fill(event);
      hist_decaymatch_Pt300toInf->fill(event);
      hist_decaymatch_Pt300to400->fill(event);
      hist_decaymatch_Pt0to400->fill(event);
      hist_decaymatch_Pt400toInf->fill(event);
    }

    // DNN setup
    hist_dnn->fill(event);
    hist_dnn_Pt0to400->fill(event);
    hist_dnn_Pt400toInf->fill(event);
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
