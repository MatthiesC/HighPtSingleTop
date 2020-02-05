#include <iostream>
#include <fstream>
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

#include "UHH2/HOTVR/include/HOTVRHists.h"
#include "UHH2/HOTVR/include/HOTVRIds.h"
#include "UHH2/HOTVR/include/HOTVRScaleFactor.h"
#include "UHH2/HOTVR/include/HadronicTop.h"

#include "UHH2/HighPtSingleTop/include/AndHists.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopHists.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopSelections.h"
#include "UHH2/HighPtSingleTop/include/TaggedJets.h"
#include "UHH2/HighPtSingleTop/include/DNNSetup.h"
#include "UHH2/HighPtSingleTop/include/MatchHists.h"
#include "UHH2/HighPtSingleTop/include/TopTagHists.h"
#include "UHH2/HighPtSingleTop/include/ReconstructionAlgorithms.h"

#include "lwtnn/LightweightNeuralNetwork.hh"
#include "lwtnn/parse_json.hh"


using namespace std;
using namespace uhh2;

namespace uhh2 {

  class HighPtSingleTopMainSelectionModule: public AnalysisModule {
  public:
    
    explicit HighPtSingleTopMainSelectionModule(Context & ctx);
    virtual bool process(Event & event) override;
    
  private:
    
    unique_ptr<AnalysisModule> sf_lumi, sf_pileup, sf_muon_trig, sf_muon_id, sf_muon_iso, sf_toptag, sf_btag;
    unique_ptr<AnalysisModule> scale_variation, primarylep, hadronictop, toptaggedjet, btaggedjets, nontopak4jets, wboson, pseudotop, SingleTopGen_tWchProd, dnn_setup;

    unique_ptr<Selection> slct_trigger, slct_1toptag, slct_tW_merged3, slct_tW_merged2, slct_tW_merged1, slct_tW_merged0, slct_tW_TopToHad, slct_tW_WToTau, slct_WJetsHeavy;

    unique_ptr<AndHists> hist_presel, hist_trigger, hist_1toptag, hist_btagsf; 
    unique_ptr<Hists> hist_btag_mc_efficiency, hist_decaymatch, hist_decaymatch_Pt0to300, hist_decaymatch_Pt300toInf, hist_decaymatch_Pt300to400, hist_decaymatch_Pt0to400, hist_decaymatch_Pt400toInf;
    unique_ptr<BinnedDNNHists> hist_dnn;

    bool is_data, is_mc, is_muon, is_elec;
    string dataset_version;
    string syst_pileup, syst_muon_trigger, syst_muon_id, syst_muon_iso, syst_muon_trk, syst_hotvr_toptag, syst_btag;

    double hotvr_fpt_max, hotvr_jetmass_min, hotvr_jetmass_max, hotvr_mpair_min, hotvr_tau32_max;

    TopJetId StandardHOTVRTopTagID;
    JetId BJetID;

    vector<Event::Handle<float>> h_dnn_inputs;
    Event::Handle<float> h_event_weight, h_toptag_pt;

    unique_ptr<lwt::LightweightNeuralNetwork> NeuralNetwork;
    vector<string> dnn_config_inputNames;
    string dnn_config_outputName;
    vector<Event::Handle<float>> m_input_handles;
    Event::Handle<double> h_dnn_output_val;
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
    syst_hotvr_toptag = ctx.get("SystDirection_HOTVRTopTagSF", "nominal");
    syst_btag = ctx.get("SystDirection_BTagSF", "nominal");

    string neural_net_filepath = ctx.get("NeuralNetFile");


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

    StandardHOTVRTopTagID = AndId<TopJet>(HOTVRTopTag(hotvr_fpt_max, hotvr_jetmass_min, hotvr_jetmass_max, hotvr_mpair_min), Tau32Groomed(hotvr_tau32_max));
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
    // checkout Alex' genius: /nfs/dust/cms/user/froehlia/CMSSW_10_2_10/src/UHH2/BstarToTW/src/BstarToTWSystematics.cxx
    sf_muon_trig.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/UHH2/common/data/2016/MuonTrigger_EfficienciesAndSF_average_RunBtoH.root", "IsoMu24_OR_IsoTkMu24_PtEtaBins", 0.5, "muon_trigger", true, syst_muon_trigger));
    sf_muon_id.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/UHH2/common/data/2016/MuonID_EfficienciesAndSF_average_RunBtoH.root", "NUM_TightID_DEN_genTracks_eta_pt", 1, "muon_tightID", true, syst_muon_id));
    sf_muon_iso.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/UHH2/common/data/2016/MuonIso_EfficienciesAndSF_average_RunBtoH.root", "NUM_TightRelIso_DEN_TightIDandIPCut_eta_pt", 1, "muon_isolation", true, syst_muon_iso));
    scale_variation.reset(new MCScaleVariation(ctx));
    sf_toptag.reset(new HOTVRScaleFactor(ctx, StandardHOTVRTopTagID, syst_hotvr_toptag));
    sf_btag.reset(new MCBTagScaleFactor(ctx, btag_algo, btag_workingpoint, "jets", syst_btag)); 


    //---------------//
    // MISCELLANEOUS //
    //---------------//

    primarylep.reset(new PrimaryLepton(ctx));
    hadronictop.reset(new HadronicTop(ctx));
    toptaggedjet.reset(new TopTaggedJet(ctx, StandardHOTVRTopTagID));
    btaggedjets.reset(new BTaggedJets(ctx, btag_algo, btag_workingpoint));
    nontopak4jets.reset(new NonTopAK4Jets(ctx, btag_algo, btag_workingpoint));
    wboson.reset(new WBosonLeptonic(ctx));
    pseudotop.reset(new PseudoTopLeptonic(ctx, true));

    SingleTopGen_tWchProd.reset(new SingleTopGen_tWchProducer(ctx, "h_GENtW"));

    dnn_setup.reset(new DNNSetup(ctx, h_dnn_inputs, 3, 8, StandardHOTVRTopTagID, BJetID, 0.));
    h_event_weight = ctx.declare_event_output<float>("DNN_EventWeight");
    h_toptag_pt = ctx.declare_event_output<float>("DNN_TopTagPt");

    ifstream neural_net_file(neural_net_filepath);
    auto dnn_config = lwt::parse_json(neural_net_file);
    NeuralNetwork.reset(new lwt::LightweightNeuralNetwork(dnn_config.inputs, dnn_config.layers, dnn_config.outputs));
    cout << "Number of used DNN inputs: " << dnn_config.inputs.size() << endl;
    for(uint i = 0; i < dnn_config.inputs.size(); i++) {
      const auto & input = dnn_config.inputs.at(i);
      cout << "input.name: " << input.name << endl;
      dnn_config_inputNames.push_back(input.name);
    }
    for(uint i = 0; i < dnn_config.outputs.size(); i++) {
      const auto & output = dnn_config.outputs.at(i);
      cout << "output.name: " << output << endl;
      dnn_config_outputName = output;
    }
    for(uint i = 0; i < dnn_config_inputNames.size(); i++) {
      m_input_handles.push_back(ctx.get_handle<float>(dnn_config_inputNames.at(i)));
    }
    h_dnn_output_val = ctx.declare_event_output<double>("DNN_Output");


    //------------//
    // SELECTIONS //
    //------------//

    slct_trigger.reset(new HighPtSingleTopTriggerSelection(ctx));
    slct_1toptag.reset(new NTopJetSelection(1, 1, StandardHOTVRTopTagID));
    slct_tW_merged3.reset(new MergeScenarioSelection(ctx, 3));
    slct_tW_merged2.reset(new MergeScenarioSelection(ctx, 2));
    slct_tW_merged1.reset(new MergeScenarioSelection(ctx, 1));
    slct_tW_merged0.reset(new MergeScenarioSelection(ctx, 0));
    slct_tW_TopToHad.reset(new tWgenSelection(ctx, "TopToHad", is_muon));
    slct_tW_WToTau.reset(new tWgenSelection(ctx, "WToTau", is_muon));
    slct_WJetsHeavy.reset(new WJetsGenSelection(ctx, "HF"));


    //------------//
    // HISTOGRAMS //
    //------------//

    hist_presel.reset(new AndHists(ctx, "1_PreSel"));

    hist_trigger.reset(new AndHists(ctx, "2_Trigger"));

    hist_1toptag.reset(new AndHists(ctx, "3_OneTopTag"));
    hist_1toptag->add_hist(new TopTagHists(ctx, "3_OneTopTag_TopTagHists_Full"));
    hist_1toptag->add_hist(new TopTagHists(ctx, "3_OneTopTag_TopTagHists_Pt0to300", 0, 300));
    hist_1toptag->add_hist(new TopTagHists(ctx, "3_OneTopTag_TopTagHists_Pt300toInf", 300));

    hist_btag_mc_efficiency.reset(new BTagMCEfficiencyHists(ctx, "BTagMCEfficiency", BJetID, "jets"));

    hist_btagsf.reset(new AndHists(ctx, "4_BTagSF"));
    hist_btagsf->add_hist(new TopTagHists(ctx, "4_BTagSF_TopTagHists_Full"));
    hist_btagsf->add_hist(new TopTagHists(ctx, "4_BTagSF_TopTagHists_Pt0to300", 0, 300));
    hist_btagsf->add_hist(new TopTagHists(ctx, "4_BTagSF_TopTagHists_Pt300toInf", 300));

    hist_decaymatch.reset(new MatchHists(ctx, "MatchHists_Full"));
    hist_decaymatch_Pt0to300.reset(new MatchHists(ctx, "MatchHists_Pt0to300", 0, 300));
    hist_decaymatch_Pt300toInf.reset(new MatchHists(ctx, "MatchHists_Pt300toInf", 300));
    hist_decaymatch_Pt300to400.reset(new MatchHists(ctx, "MatchHists_Pt300to400", 300, 400));
    hist_decaymatch_Pt0to400.reset(new MatchHists(ctx, "MatchHists_Pt0to400", 0, 400));
    hist_decaymatch_Pt400toInf.reset(new MatchHists(ctx, "MatchHists_Pt400toInf", 400));

    hist_dnn.reset(new BinnedDNNHists(ctx, "DNNHists"));
}


  //------------//
  // EVENT LOOP //
  //------------//

  bool HighPtSingleTopMainSelectionModule::process(Event & event) {

    // Split up WJets into heavy flavour and light jets
    if((dataset_version.find("WJetsHeavy") == 0) && !slct_WJetsHeavy->passes(event)) return false;
    if((dataset_version.find("WJetsLight") == 0) && slct_WJetsHeavy->passes(event)) return false;

    // Split up tW background into TopToHadAndWToTau and Else
    if(dataset_version.find("ST_tW") == 0) {
      SingleTopGen_tWchProd->process(event);
      bool is_TopToHadAndWToTau = slct_tW_TopToHad->passes(event) && slct_tW_WToTau->passes(event);
      if(dataset_version.find("ST_tW_bkg_TopToHadAndWToTau") == 0 && !is_TopToHadAndWToTau) return false;
      if(dataset_version.find("ST_tW_bkg_Else") == 0 && is_TopToHadAndWToTau) return false;
    }

    // Identify primary lepton
    primarylep->process(event);

    // After preselection
    scale_variation->process(event);
    sf_lumi->process(event);
    sf_pileup->process(event);
    if(is_muon) {
      sf_muon_id->process(event);
      sf_muon_iso->process(event);
    }
    else if(is_elec) {
      // electron id, reco
    }
    hist_presel->fill(event);

    // Trigger paths
    if(!slct_trigger->passes(event)) return false;
    if(is_muon) {
      sf_muon_trig->process(event);
    }
    else if(is_elec) {
      // electron trigger sf
    }
    hist_trigger->fill(event);

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

    // Tag some objects
    btaggedjets->process(event);
    nontopak4jets->process(event);
    wboson->process(event);
    pseudotop->process(event);

    // Split tW signal into 3-merged, 2-merged, 1-merged, 0-merged (== how many top decay products ended up inside t-tagged HOTVR jet)
    if(dataset_version.find("ST_tW") == 0) {
      // GenProducer already processed at beginning of module!
      if(dataset_version.find("ST_tW_merged3") == 0 && !slct_tW_merged3->passes(event)) return false;
      if(dataset_version.find("ST_tW_merged2") == 0 && !slct_tW_merged2->passes(event)) return false;
      if(dataset_version.find("ST_tW_merged1") == 0 && !slct_tW_merged1->passes(event)) return false;
      if(dataset_version.find("ST_tW_merged0") == 0 && !slct_tW_merged0->passes(event)) return false;
      hist_decaymatch->fill(event);
      hist_decaymatch_Pt0to300->fill(event);
      hist_decaymatch_Pt300toInf->fill(event);
      hist_decaymatch_Pt300to400->fill(event);
      hist_decaymatch_Pt0to400->fill(event);
      hist_decaymatch_Pt400toInf->fill(event);
    }

    // Set event handles used as input for DNN training
    dnn_setup->process(event);
    event.set(h_event_weight, event.weight);
    TopJet toptaggedjet;
    for(auto t : *event.topjets) { if(StandardHOTVRTopTagID(t, event)) toptaggedjet = t; }
    event.set(h_toptag_pt, toptaggedjet.v4().Pt());

    // Application of a trained DNN
    map<string, double> inputs_map;
    for(uint i = 0; i < dnn_config_inputNames.size(); i++) {
      inputs_map[dnn_config_inputNames.at(i)] = (double)event.get(m_input_handles.at(i));
    }
    auto dnn_output_vals = NeuralNetwork->compute(inputs_map);
    event.set(h_dnn_output_val, (double)dnn_output_vals[dnn_config_outputName]);

    // Histograms of DNN inputs and DNN output
    hist_dnn->fill(event);

    // Place analysis routines into a new Module!!!
    // End of main selection
    return true;
  }


  UHH2_REGISTER_ANALYSIS_MODULE(HighPtSingleTopMainSelectionModule)
}
