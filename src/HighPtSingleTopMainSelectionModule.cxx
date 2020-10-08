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

#include "UHH2/HighPtSingleTop/include/Ak8Corrections.h"
#include "UHH2/HighPtSingleTop/include/MyAk8Hists.h"
#include "UHH2/HighPtSingleTop/include/AndHists.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopHists.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopSelections.h"
#include "UHH2/HighPtSingleTop/include/TaggedJets.h"
#include "UHH2/HighPtSingleTop/include/DNNSetup.h"
#include "UHH2/HighPtSingleTop/include/MatchHists.h"
#include "UHH2/HighPtSingleTop/include/TopTagHists.h"
#include "UHH2/HighPtSingleTop/include/WTagHists.h"
#include "UHH2/HighPtSingleTop/include/ReconstructionAlgorithms.h"
#include "UHH2/HighPtSingleTop/include/LeptonAndTriggerScaleFactors.h"

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

    bool debug, empty_output_tree;

    unique_ptr<AnalysisModule> sf_lumi, sf_pileup, sf_lepton, sf_trigger, sf_toptag, sf_deepjet, scale_variation;
    unique_ptr<AnalysisModule> handle_primarylep, handle_hadronictop, handle_toptaggedjet, handle_wtaggedjet, handle_btaggedjets, handle_ak4InExJets_top, handle_ak4InExJets_W, handle_wboson, handle_pseudotop, SingleTopGen_tWchProd;
    unique_ptr<Ak8Corrections> ak8corrections;
    unique_ptr<AnalysisModule> ak8cleaning, handle_ak8jets, handle_wtaggedjets;
    unique_ptr<DNNSetup> dnn_setup;

    unique_ptr<Selection> slct_WJetsHeavy, slct_tW_TopToHad, slct_tW_TopToEle, slct_tW_TopToMuo, slct_tW_TopToTau, slct_tW_WToHad, slct_tW_WToEle, slct_tW_WToMuo, slct_tW_WToTau;
    unique_ptr<Selection> slct_trigger, slct_0toptag, slct_1toptag, slct_0wtag, slct_1wtag, slct_oneijet_top, slct_onexjet_W, slct_oneAk8jet, slct_oneAk4jet;
    // unique_ptr<Selection> slct_tW_merged3, slct_tW_merged2, slct_tW_merged1, slct_tW_merged0, slct_oneijet, slct_noxjet, slct_1bxjet;

    unique_ptr<AndHists> hist_presel, hist_preselSF, hist_trigger, hist_triggerSF;
    unique_ptr<Hists> hist_ak8_preCorr, hist_ak8_postCorr, hist_ak8_postCleaning;
    unique_ptr<AndHists> hist_TopTag_Begin, hist_TopTag_HotvrSF, hist_TopTag_End;
    unique_ptr<AndHists> hist_WTag_Begin, hist_WTag_DeepAk8SF, hist_WTag_End;
    unique_ptr<AndHists> hist_Validation_Begin, hist_Validation_Ak8Cut, hist_Validation_End;

    //unique_ptr<Hists> hist_count_TopHadWLep_before, hist_count_TopHadWLep_after, hist_count_TopLepWHad_before, hist_count_TopLepWHad_after, hist_count_Validation_before, hist_count_Validation_after;
    // unique_ptr<Hists> hist_decaymatch, hist_decaymatch_Pt0to300, hist_decaymatch_Pt300toInf, hist_decaymatch_Pt300to400, hist_decaymatch_Pt0to400, hist_decaymatch_Pt400toInf;
    unique_ptr<BinnedDNNHists> hist_dnn;
    // unique_ptr<BinnedDNNHists> hist_dnn_noxjet_YES, hist_dnn_noxjet_NO, hist_dnn_0bxjet, hist_dnn_1bxjet, hist_dnn_0toptag;

    string dataset_version;

    TopJetId StandardHOTVRTopTagID;

    vector<Event::Handle<double>> h_dnn_inputs;

    unique_ptr<lwt::LightweightNeuralNetwork> NeuralNetwork;
    vector<string> dnn_config_inputNames;
    string dnn_config_outputName;
    vector<Event::Handle<double>> m_input_handles;
    Event::Handle<double> h_dnn_output_val;
    Event::Handle<int> h_which_region;
  };


  HighPtSingleTopMainSelectionModule::HighPtSingleTopMainSelectionModule(Context & ctx) {

    //------//
    // KEYS //
    //------//

    debug = string2lowercase(ctx.get("Debug", "false")) == "true";

    empty_output_tree = string2lowercase(ctx.get("EmptyOutputTree")) == "true";

    dataset_version = ctx.get("dataset_version");

    string syst_pileup = ctx.get("SystDirection_Pileup");
    string syst_toptag = ctx.get("SystDirection_HOTVRTopTagSF");
    string syst_wtag  = ctx.get("SystDirection_DeepAK8WTagSF");
    string syst_btag  = ctx.get("SystDirection_DeepJetBTagSF");

    string neural_net_filepath  = ctx.get("NeuralNetFile_tTag");


    //---------------------//
    // KINEMATIC VARIABLES //
    //---------------------//

    // HOTVR t-tagging criteria
    double hotvr_fpt_max = 0.8;
    double hotvr_jetmass_min = 140;
    double hotvr_jetmass_max = 220;
    double hotvr_mpair_min = 50;
    double hotvr_tau32_max = 0.56;

    // Ak8 cleaning criteria
    double ak8_pt_min = 200.0;
    double ak8_eta_max = 2.4;
    double ak8_deltaRlepton_min = 0.8;


    //-----------------//
    // IDENTIFICATIONS //
    //-----------------//

    StandardHOTVRTopTagID = AndId<TopJet>(HOTVRTopTag(hotvr_fpt_max, hotvr_jetmass_min, hotvr_jetmass_max, hotvr_mpair_min), Tau32Groomed(hotvr_tau32_max));
    BTag::algo btag_algo = BTag::DEEPJET;
    BTag::wp btag_workingpoint = BTag::WP_MEDIUM; // working point needed by some histogram classes ("analysis b-tag workingpoint"); should be removed at some point
    JetId BJetID = BTag(btag_algo, btag_workingpoint);
    WTaggedJets::wp wtag_workingpoint = WTaggedJets::WP_LOOSE;


    //---------------//
    // SCALE FACTORS //
    //---------------//

    sf_lumi.reset(new MCLumiWeight(ctx));
    sf_pileup.reset(new MCPileupReweight(ctx, syst_pileup));
    sf_lepton.reset(new LeptonScaleFactors(ctx));
    sf_trigger.reset(new TriggerScaleFactors(ctx));
    // only 2016 muon scale factors linked here, TODO later: distinguish between years and ele/muon
    // checkout Alex' genius: /nfs/dust/cms/user/froehlia/CMSSW_10_2_10/src/UHH2/BstarToTW/src/BstarToTWSystematics.cxx
    // sf_muon_trig.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/matthies/uhh2-102X-v2/CMSSW_10_2_16/src/UHH2/common/data/2016/MuonTrigger_EfficienciesAndSF_average_RunBtoH.root", "IsoMu24_OR_IsoTkMu24_PtEtaBins", 0.5, "muon_trigger", true, syst_muon_trigger));
    // sf_muon_id.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/matthies/uhh2-102X-v2/CMSSW_10_2_16/src/UHH2/common/data/2016/MuonID_EfficienciesAndSF_average_RunBtoH.root", "MC_NUM_TightID_DEN_genTracks_PAR_pt_eta", 1, "muon_tightID", true, syst_muon_id));
    // sf_muon_iso.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/matthies/uhh2-102X-v2/CMSSW_10_2_16/src/UHH2/common/data/2016/MuonIso_EfficienciesAndSF_average_RunBtoH.root", "TightISO_TightID_pt_eta", 1, "muon_isolation", true, syst_muon_iso));
    scale_variation.reset(new MCScaleVariation(ctx));
    sf_toptag.reset(new HOTVRScaleFactor(ctx, StandardHOTVRTopTagID, syst_toptag));
    sf_deepjet.reset(new MCBTagDiscriminantReweighting(ctx, btag_algo, "jets", syst_btag));


    //---------------//
    // MISCELLANEOUS //
    //---------------//

    SingleTopGen_tWchProd.reset(new SingleTopGen_tWchProducer(ctx, "h_GENtW"));

    handle_primarylep.reset(new PrimaryLepton(ctx));

    ak8corrections.reset(new Ak8Corrections());
    ak8corrections->init(ctx);
    ak8cleaning.reset(new Ak8Cleaning(ctx, ak8_pt_min, ak8_eta_max, ak8_deltaRlepton_min));

    handle_ak8jets.reset(new Ak8Jets(ctx));
    handle_wtaggedjets.reset(new WTaggedJets(ctx, wtag_workingpoint));
    handle_btaggedjets.reset(new BTaggedJets(ctx, btag_algo, btag_workingpoint));
    handle_wboson.reset(new WBosonLeptonic(ctx));
    handle_pseudotop.reset(new PseudoTopLeptonic(ctx, true)); // true = don't use b jets but all jets

    handle_toptaggedjet.reset(new TopTaggedJet(ctx, StandardHOTVRTopTagID));
    handle_ak4InExJets_top.reset(new InExAK4Jets(ctx, btag_algo, btag_workingpoint, "_Top", "TopTaggedJet", true));
    handle_hadronictop.reset(new HadronicTop(ctx));

    handle_wtaggedjet.reset(new WTaggedJet(ctx));
    handle_ak4InExJets_W.reset(new InExAK4Jets(ctx, btag_algo, btag_workingpoint, "_W", "WTaggedJet", false));

    dnn_setup.reset(new DNNSetup(ctx, h_dnn_inputs, 3, 8, StandardHOTVRTopTagID, BJetID, 0.));

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
      m_input_handles.push_back(ctx.get_handle<double>(dnn_config_inputNames.at(i)));
    }
    h_dnn_output_val = ctx.declare_event_output<double>("DNN_Output");

    h_which_region = ctx.declare_event_output<int>("which_region");


    //------------//
    // SELECTIONS //
    //------------//

    slct_WJetsHeavy.reset(new WJetsGenSelection(ctx, "HF"));

    slct_tW_TopToHad.reset(new tWgenSelection(ctx, "TopToHad"));
    slct_tW_TopToEle.reset(new tWgenSelection(ctx, "TopToEle"));
    slct_tW_TopToMuo.reset(new tWgenSelection(ctx, "TopToMuo"));
    slct_tW_TopToTau.reset(new tWgenSelection(ctx, "TopToTau"));
    slct_tW_WToHad.reset(new tWgenSelection(ctx, "WToHad"));
    slct_tW_WToEle.reset(new tWgenSelection(ctx, "WToEle"));
    slct_tW_WToMuo.reset(new tWgenSelection(ctx, "WToMuo"));
    slct_tW_WToTau.reset(new tWgenSelection(ctx, "WToTau"));

    slct_trigger.reset(new HighPtSingleTopTriggerSelection(ctx));

    slct_0toptag.reset(new NTopJetSelection(0, 0, StandardHOTVRTopTagID));
    slct_1toptag.reset(new NTopJetSelection(1, 1, StandardHOTVRTopTagID));
    slct_0wtag.reset(new MyNTopJetsSelection(ctx, 0, 0, "WJets"));
    slct_1wtag.reset(new MyNTopJetsSelection(ctx, 1, 1, "WJets"));

    slct_oneijet_top.reset(new NObjectsSelection(ctx, 1, -1, "InJets_Top"));
    slct_onexjet_W.reset(new NObjectsSelection(ctx, 1, -1, "ExJets_W"));
    //slct_noxjet.reset(new NObjectsSelection(ctx, 0, 0, "TopExJets"));
    //slct_1bxjet.reset(new NObjectsSelection(ctx, 1, -1, "TopExBJetsTight"));
    slct_oneAk8jet.reset(new MyNTopJetsSelection(ctx, 1, -1, "Ak8Jets"));
    slct_oneAk4jet.reset(new NJetSelection(1, -1));


    // slct_tW_merged3.reset(new MergeScenarioSelection(ctx, 3));
    // slct_tW_merged2.reset(new MergeScenarioSelection(ctx, 2));
    // slct_tW_merged1.reset(new MergeScenarioSelection(ctx, 1));
    // slct_tW_merged0.reset(new MergeScenarioSelection(ctx, 0));






    //------------//
    // HISTOGRAMS //
    //------------//

    // Control distributions after last module's preselection; lumi, PU, and lepton id/reco scale factors are applied
    hist_presel.reset(new AndHists(ctx, "1_PreSel"));
    hist_preselSF.reset(new AndHists(ctx, "1_PreSelSF"));

    // Control distributions after trigger selection and respective scale factors
    hist_trigger.reset(new AndHists(ctx, "2_Trigger"));
    hist_triggerSF.reset(new AndHists(ctx, "2_TriggerSF"));

    // Control distributions or AK8 jets: JEC+JER and cleaning
    hist_ak8_preCorr.reset(new MyAk8Hists(ctx, "2_Ak8Setup_PreCorr", ctx.get("Ak8recCollection"))); // AK8 handles not yet set, use additional branch directly
    hist_ak8_postCorr.reset(new MyAk8Hists(ctx, "2_Ak8Setup_PostCorr", ctx.get("Ak8recCollection"))); // AK8 handles not yet set, use additional branch directly
    hist_ak8_postCleaning.reset(new MyAk8Hists(ctx, "2_Ak8Setup_PostCleaning"));

    hist_TopTag_Begin.reset(new AndHists(ctx, "3_TopTag_Begin"));
    hist_TopTag_Begin->add_TopTagHists(ctx);
    hist_TopTag_HotvrSF.reset(new AndHists(ctx, "3_TopTag_HotvrSF"));
    hist_TopTag_HotvrSF->add_TopTagHists(ctx);
    hist_TopTag_End.reset(new AndHists(ctx, "3_TopTag_End"));
    hist_TopTag_End->add_TopTagHists(ctx);

    hist_WTag_Begin.reset(new AndHists(ctx, "3_WTag_Begin"));
    hist_WTag_Begin->add_WTagHists(ctx);
    hist_WTag_DeepAk8SF.reset(new AndHists(ctx, "3_WTag_DeepAk8SF"));
    hist_WTag_DeepAk8SF->add_WTagHists(ctx);
    hist_WTag_End.reset(new AndHists(ctx, "3_WTag_End"));
    hist_WTag_End->add_WTagHists(ctx);

    hist_Validation_Begin.reset(new AndHists(ctx, "3_Validation_Begin"));
    hist_Validation_Ak8Cut.reset(new AndHists(ctx, "3_Validation_Ak8Cut"));
    hist_Validation_Ak8Cut->add_TopTagHists(ctx);
    hist_Validation_Ak8Cut->add_WTagHists(ctx);
    hist_Validation_End.reset(new AndHists(ctx, "3_Validation_End"));
    hist_Validation_End->add_TopTagHists(ctx);
    hist_Validation_End->add_WTagHists(ctx);

    // hist_decaymatch.reset(new MatchHists(ctx, "MatchHists_Full"));
    // hist_decaymatch_Pt0to400.reset(new MatchHists(ctx, "MatchHists_Pt0to400", 0, 400));
    // hist_decaymatch_Pt400toInf.reset(new MatchHists(ctx, "MatchHists_Pt400toInf", 400));

    hist_dnn.reset(new BinnedDNNHists(ctx, "DNNHists_TopTag", dnn_config_inputNames, dnn_setup->inputs_info()));
    //hist_dnn_noxjet_YES.reset(new BinnedDNNHists(ctx, "DNNHists_NoXJet_YES", dnn_config_inputNames, dnn_setup->inputs_info()));
    //hist_dnn_noxjet_NO.reset(new BinnedDNNHists(ctx, "DNNHists_NoXJet_NO", dnn_config_inputNames, dnn_setup->inputs_info()));
    //hist_dnn_0bxjet.reset(new BinnedDNNHists(ctx, "DNNHists_0bXJet", dnn_config_inputNames, dnn_setup->inputs_info()));
    //hist_dnn_1bxjet.reset(new BinnedDNNHists(ctx, "DNNHists_1bXJet", dnn_config_inputNames, dnn_setup->inputs_info()));
    //hist_dnn_0toptag.reset(new BinnedDNNHists(ctx, "DNNHists_0TopTag", dnn_config_inputNames, dnn_setup->inputs_info()));
}


  //------------//
  // EVENT LOOP //
  //------------//

  bool HighPtSingleTopMainSelectionModule::process(Event & event) {

    if(debug) {
      cout << endl;
      cout << "+-----------+" << endl;
      cout << "| NEW EVENT |" << endl;
      cout << "+-----------+" << endl;
    }

    // Split up W+jets and tW samples

    if(debug) cout << "Split up WJets into heavy flavor and light jets" << endl;
    if((dataset_version.find("WJetsHeavy") == 0) && !slct_WJetsHeavy->passes(event)) return false;
    if((dataset_version.find("WJetsLight") == 0) && slct_WJetsHeavy->passes(event)) return false;

    if(debug) cout << "Split up tW samples into decay channels" << endl;
    if(dataset_version.find("ST_tW") == 0) {

      SingleTopGen_tWchProd->process(event);

      bool is_TopToHad = slct_tW_TopToHad->passes(event);
      bool is_TopToEle = slct_tW_TopToEle->passes(event);
      bool is_TopToMuo = slct_tW_TopToMuo->passes(event);
      bool is_TopToTau = slct_tW_TopToTau->passes(event);

      bool is_WToHad = slct_tW_WToHad->passes(event);
      bool is_WToEle = slct_tW_WToEle->passes(event);
      bool is_WToMuo = slct_tW_WToMuo->passes(event);
      bool is_WToTau = slct_tW_WToTau->passes(event);

      if(dataset_version.find("TopToHad_WToHad") != string::npos && !(is_TopToHad && is_WToHad)) return false; // not included in NoFullyHadronic tW samples
      if(dataset_version.find("TopToHad_WToEle") != string::npos && !(is_TopToHad && is_WToEle)) return false;
      if(dataset_version.find("TopToHad_WToMuo") != string::npos && !(is_TopToHad && is_WToMuo)) return false;
      if(dataset_version.find("TopToHad_WToTau") != string::npos && !(is_TopToHad && is_WToTau)) return false;

      if(dataset_version.find("TopToEle_WToHad") != string::npos && !(is_TopToEle && is_WToHad)) return false;
      if(dataset_version.find("TopToEle_WToEle") != string::npos && !(is_TopToEle && is_WToEle)) return false;
      if(dataset_version.find("TopToEle_WToMuo") != string::npos && !(is_TopToEle && is_WToMuo)) return false;
      if(dataset_version.find("TopToEle_WToTau") != string::npos && !(is_TopToEle && is_WToTau)) return false;

      if(dataset_version.find("TopToMuo_WToHad") != string::npos && !(is_TopToMuo && is_WToHad)) return false;
      if(dataset_version.find("TopToMuo_WToEle") != string::npos && !(is_TopToMuo && is_WToEle)) return false;
      if(dataset_version.find("TopToMuo_WToMuo") != string::npos && !(is_TopToMuo && is_WToMuo)) return false;
      if(dataset_version.find("TopToMuo_WToTau") != string::npos && !(is_TopToMuo && is_WToTau)) return false;

      if(dataset_version.find("TopToTau_WToHad") != string::npos && !(is_TopToTau && is_WToHad)) return false;
      if(dataset_version.find("TopToTau_WToEle") != string::npos && !(is_TopToTau && is_WToEle)) return false;
      if(dataset_version.find("TopToTau_WToMuo") != string::npos && !(is_TopToTau && is_WToMuo)) return false;
      if(dataset_version.find("TopToTau_WToTau") != string::npos && !(is_TopToTau && is_WToTau)) return false;
    }

    // This is where the fun begins...

    if(debug) cout << "Sort jets, topjets, and subjets by pt" << endl;
    sort_by_pt<Jet>(*event.jets);
    sort_by_pt<TopJet>(*event.topjets);
    for(auto & tj : *event.topjets) {
      vector<Jet> sorted_subjets = tj.subjets();
      sort_by_pt<Jet>(sorted_subjets);
      tj.set_subjets(std::move(sorted_subjets));
    }

    if(debug) cout << "Identify primary lepton" << endl;
    handle_primarylep->process(event);

    if(debug) cout << "Apply lumi, pileup, and lepton id/iso/reco scale factors" << endl;
    scale_variation->process(event);
    sf_lumi->process(event);
    sf_pileup->process(event);
    hist_presel->fill(event);
    sf_lepton->process(event);
    hist_preselSF->fill(event);

    if(debug) cout << "Select trigger paths and apply trigger scale factors" << endl;
    if(!slct_trigger->passes(event)) return false;
    hist_trigger->fill(event);
    sf_trigger->process(event);
    hist_triggerSF->fill(event);

    if(debug) cout << "Reweight DeepJet distributions for AK4 jets" << endl;
    sf_deepjet->process(event);

    if(debug) cout << "Apply corrections to AK8 jets and clean them" << endl;
    hist_ak8_preCorr->fill(event);
    ak8corrections->process(event); // don't sort AK8 jets after correcting them
    hist_ak8_postCorr->fill(event);
    ak8cleaning->process(event); // clean AK8 jets and sort them by pt
    handle_ak8jets->process(event);
    hist_ak8_postCleaning->fill(event);
    handle_wtaggedjets->process(event);

    if(debug) cout << "Set handles for b-tagged jets and leptonic W boson hypothesis" << endl;
    handle_btaggedjets->process(event);
    handle_wboson->process(event); // the leptonic one!

    if(debug) cout << "Set some booleans for analysis regions" << endl;
    bool b_1toptag = slct_1toptag->passes(event);
    bool b_0toptag = slct_0toptag->passes(event);
    bool b_1wtag = slct_1wtag->passes(event);
    bool b_0wtag = slct_0wtag->passes(event);
    bool is_TopTagRegion(false);
    bool is_WTagRegion(false);
    bool is_ValidationRegion(false);

    // Caveat: The order of analysis modules in the following if-statements is crucial and should be changed with care only!

    if(b_1toptag) { // don't veto w-tags since this might hurt the signal efficiency
      if(debug) cout << "SR t(had)W(lep):  Set handles for top tag and AK4 jets inside/outside top jet" << endl;
      handle_toptaggedjet->process(event);
      handle_ak4InExJets_top->process(event);
      if(debug) cout << "SR t(had)W(lep):  Fill initial control histograms" << endl;
      hist_TopTag_Begin->fill(event);
      if(debug) cout << "SR t(had)W(lep):  Apply HOTVR top-tagging scale factors" << endl;
      handle_hadronictop->process(event);
      sf_toptag->process(event);
      if(debug) cout << "SR t(had)W(lep):  Fill control histograms after HOTVR scale factors" << endl;
      hist_TopTag_HotvrSF->fill(event);
      if(debug) cout << "SR t(had)W(lep):  Throw away very few events which do not have at least one AK4 jet within top jet" << endl;
      if(!slct_oneijet_top->passes(event)) return false; // Need to have at least one AK4 jet for pseudotop and also makes sense to have at least one AK4 jet inside HOTVR jet
      if(debug) cout << "SR t(had)W(lep):  Fill final control histograms" << endl;
      hist_TopTag_End->fill(event);

      event.set(h_which_region, 1);
      is_TopTagRegion = true;
    }

    else if(b_0toptag && b_1wtag) {
      if(debug) cout << "SR t(lep)W(had):  Set handles for W tag and AK4 jets inside/outside W jet" << endl;
      handle_wtaggedjet->process(event);
      handle_ak4InExJets_W->process(event);
      if(debug) cout << "SR t(lep)W(had):  Fill initial control histograms" << endl;
      hist_WTag_Begin->fill(event);
      if(debug) cout << "SR t(lep)W(had):  Apply DeepAK8 W-tagging scale factors" << endl;
      //TODO: sf_wtag->process(event);
      if(debug) cout << "SR t(had)W(lep):  Fill control histograms after DeepAK8 scale factors" << endl;
      hist_WTag_DeepAk8SF->fill(event);
      if(debug) cout << "SR t(lep)W(had):  Require at least one AK4 jet outside W jet as potential candidate for the b jet from leptonic top quark" << endl;
      if(!slct_onexjet_W->passes(event)) return false; // Need to have at least one AK4 jet for pseudotop and also makes sense due to decay mode (additional AK4 jet potentially represents b jet from leptonic top quark)
      if(debug) cout << "SR t(lep)W(had):  Fill final control histograms" << endl;
      hist_WTag_End->fill(event);

      event.set(h_which_region, 2);
      is_WTagRegion = true;
    }

    else if(b_0toptag && b_0wtag) {
      if(debug) cout << "VR:  Fill initial control histograms" << endl;
      hist_Validation_Begin->fill(event);
      if(debug) cout << "VR:  Require at least one AK8 jet" << endl;
      if(!slct_oneAk8jet->passes(event)) return false; // Need to have at least one AK8 jet as W-tag substitute. For the t-tag substitute, we already required one HOTVR jet during the preselection...
      if(debug) cout << "VR:  Set handles" << endl;
      handle_toptaggedjet->process(event); // leading HOTVR jet
      handle_wtaggedjet->process(event); // leading AK8 jet
      handle_ak4InExJets_top->process(event);
      handle_ak4InExJets_W->process(event);
      if(debug) cout << "VR:  Fill control histograms after AK8 / before AK4 cut" << endl;
      hist_Validation_Ak8Cut->fill(event);
      if(debug) cout << "VR:  Require at least one AK4 jet" << endl;
      if(!slct_oneAk4jet->passes(event)) return false; // Need at least one AK4 jet for pseudotop
      if(debug) cout << "VR:  Fill final control histograms" << endl;
      hist_Validation_End->fill(event);

      event.set(h_which_region, 3);
      is_ValidationRegion = true;
    }

    else {
      event.set(h_which_region, 0);
      return false; // Discard events not belonging to one of the three regions above
    }

    if(debug) cout << "Set handle for leptonic pseudo top" << endl; // Events w/o AK4 jet have already been discarded at this point
    handle_pseudotop->process(event);

    // DNN-related code starts here...

    if(is_TopTagRegion) {
      if(debug) cout << "Set event handles used as input for DNN training" << endl;
      dnn_setup->process(event);
      if(debug) cout << "Application of a trained DNN" << endl;
      map<string, double> inputs_map;
      for(uint i = 0; i < dnn_config_inputNames.size(); i++) {
        inputs_map[dnn_config_inputNames.at(i)] = (double)event.get(m_input_handles.at(i));
      }
      auto dnn_output_vals = NeuralNetwork->compute(inputs_map);
      event.set(h_dnn_output_val, (double)dnn_output_vals[dnn_config_outputName]);
      if(debug) cout << "Histograms of DNN inputs and DNN output" << endl;
      hist_dnn->fill(event);
    }

    else if(is_WTagRegion) {
      // do stuff
    }

    else if(is_ValidationRegion) {
      // do stuff
    }




    // Split tW signal into 3-merged, 2-merged, 1-merged, 0-merged (== how many top decay products ended up inside t-tagged HOTVR jet)
    // if(debug) cout << "Split tW signal into 3-merged, 2-merged, 1-merged, 0-merged (== how many top decay products ended up inside t-tagged HOTVR jet)" << endl;
    // if(_1toptag && dataset_version.find("ST_tW") == 0) {
    //   // GenProducer already processed at beginning of module!
    //   if((dataset_version.find("ST_tW_merged3") == 0 || dataset_version.find("ST_tW_DS_merged3") == 0) && !slct_tW_merged3->passes(event)) return false;
    //   if((dataset_version.find("ST_tW_merged2") == 0 || dataset_version.find("ST_tW_DS_merged2") == 0) && !slct_tW_merged2->passes(event)) return false;
    //   if((dataset_version.find("ST_tW_merged1") == 0 || dataset_version.find("ST_tW_DS_merged1") == 0) && !slct_tW_merged1->passes(event)) return false;
    //   if((dataset_version.find("ST_tW_merged0") == 0 || dataset_version.find("ST_tW_DS_merged0") == 0) && !slct_tW_merged0->passes(event)) return false;
    //   hist_decaymatch->fill(event);
    //   hist_decaymatch_Pt0to400->fill(event);
    //   hist_decaymatch_Pt400toInf->fill(event);
    // }

    // Histograms of DNN inputs and DNN output
    // if(debug) cout << "Histograms of DNN inputs and DNN output" << endl;
    // if(_1toptag) {
    //   hist_dnn->fill(event);
    //   if(slct_noxjet->passes(event)) hist_dnn_noxjet_YES->fill(event); // well-defined tW LO, suppressing interference effects of tW NLO / ttbar LO
    //   else {
	  //     hist_dnn_noxjet_NO->fill(event);
	  //     if(slct_1bxjet->passes(event)) hist_dnn_1bxjet->fill(event);
	  //     else hist_dnn_0bxjet->fill(event);
    //   }
    // }
    // else { // Fill validation region DNN plots
    //   hist_dnn_0toptag->fill(event);
    // }
    //
    // if(!_1toptag) return false; // Finally through away events without t-tagged jet


    // End of Module
    return !empty_output_tree;
  }


  UHH2_REGISTER_ANALYSIS_MODULE(HighPtSingleTopMainSelectionModule)
}
