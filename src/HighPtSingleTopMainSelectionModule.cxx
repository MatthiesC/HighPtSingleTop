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

    bool debug;

    unique_ptr<AnalysisModule> sf_lumi, sf_pileup, sf_muon_trig, sf_muon_id, sf_muon_iso, sf_toptag, sf_deepjet;
    unique_ptr<AnalysisModule> scale_variation, primarylep, hadronictop, toptaggedjet, btaggedjets, nontopak4jets, wboson, pseudotop, SingleTopGen_tWchProd;
    unique_ptr<DNNSetup> dnn_setup;

    unique_ptr<Selection> slct_trigger, slct_0toptag, slct_1toptag, slct_tW_merged3, slct_tW_merged2, slct_tW_merged1, slct_tW_merged0, slct_tW_TopToHad, slct_tW_WToTau, slct_WJetsHeavy, slct_oneijet, slct_noxjet, slct_1bxjet;

    unique_ptr<AndHists> hist_presel, hist_trigger, hist_1toptag, hist_0toptag, hist_1toptag_btagsf, hist_0toptag_btagsf;
    unique_ptr<Hists> hist_decaymatch, hist_decaymatch_Pt0to300, hist_decaymatch_Pt300toInf, hist_decaymatch_Pt300to400, hist_decaymatch_Pt0to400, hist_decaymatch_Pt400toInf;
    unique_ptr<BinnedDNNHists> hist_dnn, hist_dnn_noxjet_YES, hist_dnn_noxjet_NO, hist_dnn_0bxjet, hist_dnn_1bxjet, hist_dnn_0toptag;

    bool is_data, is_mc, is_muon, is_elec, using_hotvr;
    string dataset_version;

    TopJetId StandardHOTVRTopTagID;

    vector<Event::Handle<double>> h_dnn_inputs;

    unique_ptr<lwt::LightweightNeuralNetwork> NeuralNetwork;
    vector<string> dnn_config_inputNames;
    string dnn_config_outputName;
    vector<Event::Handle<double>> m_input_handles;
    Event::Handle<double> h_dnn_output_val;
  };


  HighPtSingleTopMainSelectionModule::HighPtSingleTopMainSelectionModule(Context & ctx) {

    //------//
    // KEYS //
    //------//

    debug = false;

    is_data = ctx.get("dataset_type") == "DATA";
    is_mc   = ctx.get("dataset_type") == "MC";
    is_muon = ctx.get("analysis_channel") == "MUON";
    is_elec = ctx.get("analysis_channel") == "ELECTRON";

    dataset_version = ctx.get("dataset_version");
    using_hotvr = ctx.get("TopJetCollection") == "hotvrPuppi"; // if false, we use AK8 Puppi

    string syst_pileup       = ctx.get("SystDirection_Pileup", "nominal");
    string syst_muon_trigger = ctx.get("SystDirection_MuonTrig", "nominal");
    string syst_muon_id      = ctx.get("SystDirection_MuonId", "nominal");
    string syst_muon_iso     = ctx.get("SystDirection_MuonIso", "nominal");
    string syst_toptag       = ctx.get("SystDirection_TopTagSF", "nominal");
    string syst_btag         = ctx.get("SystDirection_BTagSF", "nominal");

    string neural_net_filepath = ctx.get("NeuralNetFile");


    //---------------------//
    // KINEMATIC VARIABLES //
    //---------------------//

    // HOTVR t-tagging criteria
    double hotvr_fpt_max     = 0.8;
    double hotvr_jetmass_min = 140;
    double hotvr_jetmass_max = 220;
    double hotvr_mpair_min   = 50;
    double hotvr_tau32_max   = 0.56;


    //-----------------//
    // IDENTIFICATIONS //
    //-----------------//

    StandardHOTVRTopTagID = AndId<TopJet>(HOTVRTopTag(hotvr_fpt_max, hotvr_jetmass_min, hotvr_jetmass_max, hotvr_mpair_min), Tau32Groomed(hotvr_tau32_max));
    BTag::algo btag_algo = BTag::DEEPJET;
    BTag::wp btag_workingpoint = BTag::WP_MEDIUM; // working point needed by some histogram classes ("analysis b-tag workingpoint"); should be removed at some point
    JetId BJetID = BTag(btag_algo, btag_workingpoint);


    //---------------//
    // SCALE FACTORS //
    //---------------//

    sf_lumi.reset(new MCLumiWeight(ctx));
    sf_pileup.reset(new MCPileupReweight(ctx, syst_pileup));
    // only 2016 muon scale factors linked here, TODO later: distinguish between years and ele/muon
    // checkout Alex' genius: /nfs/dust/cms/user/froehlia/CMSSW_10_2_10/src/UHH2/BstarToTW/src/BstarToTWSystematics.cxx
    sf_muon_trig.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/matthies/uhh2-102X-v2/CMSSW_10_2_16/src/UHH2/common/data/2016/MuonTrigger_EfficienciesAndSF_average_RunBtoH.root", "IsoMu24_OR_IsoTkMu24_PtEtaBins", 0.5, "muon_trigger", true, syst_muon_trigger));
    sf_muon_id.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/matthies/uhh2-102X-v2/CMSSW_10_2_16/src/UHH2/common/data/2016/MuonID_EfficienciesAndSF_average_RunBtoH.root", "NUM_TightID_DEN_genTracks_eta_pt", 1, "muon_tightID", true, syst_muon_id));
    sf_muon_iso.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/matthies/uhh2-102X-v2/CMSSW_10_2_16/src/UHH2/common/data/2016/MuonIso_EfficienciesAndSF_average_RunBtoH.root", "NUM_TightRelIso_DEN_TightIDandIPCut_eta_pt", 1, "muon_isolation", true, syst_muon_iso));
    scale_variation.reset(new MCScaleVariation(ctx));
    sf_toptag.reset(new HOTVRScaleFactor(ctx, StandardHOTVRTopTagID, syst_toptag));
    sf_deepjet.reset(new MCBTagDiscriminantReweighting(ctx, btag_algo, "jets", syst_btag));


    //---------------//
    // MISCELLANEOUS //
    //---------------//

    primarylep.reset(new PrimaryLepton(ctx));
    hadronictop.reset(new HadronicTop(ctx));
    toptaggedjet.reset(new TopTaggedJet(ctx, StandardHOTVRTopTagID));
    btaggedjets.reset(new BTaggedJets(ctx, btag_algo, btag_workingpoint));
    nontopak4jets.reset(new NonTopAK4Jets(ctx, btag_algo, btag_workingpoint, (using_hotvr ? 1.5 : 0.8)));
    wboson.reset(new WBosonLeptonic(ctx));
    pseudotop.reset(new PseudoTopLeptonic(ctx, true));

    SingleTopGen_tWchProd.reset(new SingleTopGen_tWchProducer(ctx, "h_GENtW"));

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


    //------------//
    // SELECTIONS //
    //------------//

    slct_trigger.reset(new HighPtSingleTopTriggerSelection(ctx));
    slct_0toptag.reset(new NTopJetSelection(0, 0, StandardHOTVRTopTagID));
    slct_1toptag.reset(new NTopJetSelection(1, 1, StandardHOTVRTopTagID));
    slct_tW_merged3.reset(new MergeScenarioSelection(ctx, 3));
    slct_tW_merged2.reset(new MergeScenarioSelection(ctx, 2));
    slct_tW_merged1.reset(new MergeScenarioSelection(ctx, 1));
    slct_tW_merged0.reset(new MergeScenarioSelection(ctx, 0));
    slct_tW_TopToHad.reset(new tWgenSelection(ctx, "TopToHad", is_muon));
    slct_tW_WToTau.reset(new tWgenSelection(ctx, "WToTau", is_muon));
    slct_WJetsHeavy.reset(new WJetsGenSelection(ctx, "HF"));
    slct_oneijet.reset(new NObjectsSelection(ctx, 1, -1, "TopInJets"));
    slct_noxjet.reset(new NObjectsSelection(ctx, 0, 0, "TopExJets"));
    slct_1bxjet.reset(new NObjectsSelection(ctx, 1, -1, "TopExBJetsTight"));


    //------------//
    // HISTOGRAMS //
    //------------//

    hist_presel.reset(new AndHists(ctx, "1_PreSel"));

    hist_trigger.reset(new AndHists(ctx, "2_Trigger"));

    hist_1toptag.reset(new AndHists(ctx, "3_1TopTag"));
    hist_1toptag->add_hist(new TopTagHists(ctx, "3_1TopTag_TopTagHists_Full"));
    hist_1toptag->add_hist(new TopTagHists(ctx, "3_1TopTag_TopTagHists_Pt0to400", 0, 400));
    hist_1toptag->add_hist(new TopTagHists(ctx, "3_1TopTag_TopTagHists_Pt400toInf", 400));
    hist_0toptag.reset(new AndHists(ctx, "3_0TopTag"));
    hist_0toptag->add_hist(new TopTagHists(ctx, "3_0TopTag_TopTagHists_Full"));
    hist_0toptag->add_hist(new TopTagHists(ctx, "3_0TopTag_TopTagHists_Pt0to400", 0, 400));
    hist_0toptag->add_hist(new TopTagHists(ctx, "3_0TopTag_TopTagHists_Pt400toInf", 400));

    hist_1toptag_btagsf.reset(new AndHists(ctx, "4_1TopTag_BTagSF"));
    hist_1toptag_btagsf->add_hist(new TopTagHists(ctx, "4_1TopTag_BTagSF_TopTagHists_Full"));
    hist_1toptag_btagsf->add_hist(new TopTagHists(ctx, "4_1TopTag_BTagSF_TopTagHists_Pt0to400", 0, 400));
    hist_1toptag_btagsf->add_hist(new TopTagHists(ctx, "4_1TopTag_BTagSF_TopTagHists_Pt400toInf", 400));
    hist_0toptag_btagsf.reset(new AndHists(ctx, "4_0TopTag_BTagSF"));
    hist_0toptag_btagsf->add_hist(new TopTagHists(ctx, "4_0TopTag_BTagSF_TopTagHists_Full"));
    hist_0toptag_btagsf->add_hist(new TopTagHists(ctx, "4_0TopTag_BTagSF_TopTagHists_Pt0to400", 0, 400));
    hist_0toptag_btagsf->add_hist(new TopTagHists(ctx, "4_0TopTag_BTagSF_TopTagHists_Pt400toInf", 400));

    hist_decaymatch.reset(new MatchHists(ctx, "MatchHists_Full"));
    hist_decaymatch_Pt0to400.reset(new MatchHists(ctx, "MatchHists_Pt0to400", 0, 400));
    hist_decaymatch_Pt400toInf.reset(new MatchHists(ctx, "MatchHists_Pt400toInf", 400));

    hist_dnn.reset(new BinnedDNNHists(ctx, "DNNHists_1TopTag", dnn_config_inputNames, dnn_setup->inputs_info()));
    hist_dnn_noxjet_YES.reset(new BinnedDNNHists(ctx, "DNNHists_NoXJet_YES", dnn_config_inputNames, dnn_setup->inputs_info()));
    hist_dnn_noxjet_NO.reset(new BinnedDNNHists(ctx, "DNNHists_NoXJet_NO", dnn_config_inputNames, dnn_setup->inputs_info()));
    hist_dnn_0bxjet.reset(new BinnedDNNHists(ctx, "DNNHists_0bXJet", dnn_config_inputNames, dnn_setup->inputs_info()));
    hist_dnn_1bxjet.reset(new BinnedDNNHists(ctx, "DNNHists_1bXJet", dnn_config_inputNames, dnn_setup->inputs_info()));
    hist_dnn_0toptag.reset(new BinnedDNNHists(ctx, "DNNHists_0TopTag", dnn_config_inputNames, dnn_setup->inputs_info()));
}


  //------------//
  // EVENT LOOP //
  //------------//

  bool HighPtSingleTopMainSelectionModule::process(Event & event) {

    // Split up WJets into heavy flavour and light jets
    if(debug) cout << "Split up WJets into heavy flavour and light jets" << endl;
    if((dataset_version.find("WJetsHeavy") == 0) && !slct_WJetsHeavy->passes(event)) return false;
    if((dataset_version.find("WJetsLight") == 0) && slct_WJetsHeavy->passes(event)) return false;

    // Split up tW background into TopToHadAndWToTau and Else
    if(debug) cout << "Split up tW background into TopToHadAndWToTau and Else" << endl;
    if(dataset_version.find("ST_tW") == 0) {
      SingleTopGen_tWchProd->process(event);
      bool is_TopToHadAndWToTau = slct_tW_TopToHad->passes(event) && slct_tW_WToTau->passes(event);
      if((dataset_version.find("ST_tW_bkg_TopToHadAndWToTau") == 0 || dataset_version.find("ST_tW_DS_bkg_TopToHadAndWToTau") == 0) && !is_TopToHadAndWToTau) return false;
      if((dataset_version.find("ST_tW_bkg_Else") == 0 || dataset_version.find("ST_tW_DS_bkg_Else") == 0) && is_TopToHadAndWToTau) return false;
    }

    // Identify primary lepton
    if(debug) cout << "Identify primary lepton" << endl;
    primarylep->process(event);

    // After preselection: lumi, PU, and lepton scale factors
    if(debug) cout << "After preselection" << endl;
    scale_variation->process(event);
    sf_lumi->process(event);
    sf_pileup->process(event);
    if(is_muon) {
      sf_muon_id->process(event);
      sf_muon_iso->process(event);
    }
    else if(is_elec) {
      // TODO: electron id, reco sf
    }
    hist_presel->fill(event);

    // Trigger paths and scale factors
    if(debug) cout << "Trigger paths" << endl;
    if(!slct_trigger->passes(event)) return false;
    if(is_muon) {
      sf_muon_trig->process(event);
    }
    else if(is_elec) {
      // TODO: electron trigger sf
    }
    hist_trigger->fill(event);

    // Require exactly one HOTVR t-tag
    if(debug) cout << "Require exactly one HOTVR t-tag" << endl;
    bool _1toptag = slct_1toptag->passes(event);
    //if(!slct_1toptag->passes(event)) return false;
    //if(!(slct_1toptag->passes(event) && slct_0toptag->passes(event))) return false; // make sure to reject events with 2 or more t tags
    if(_1toptag) {
      hadronictop->process(event);
      sf_toptag->process(event);
    }

    // Tag some objects
    if(debug) cout << "Tag some objects" << endl;
    toptaggedjet->process(event); // in case, we are in the 0toptag validation region, use leading HOTVR jet as "t jet"
    btaggedjets->process(event);
    nontopak4jets->process(event);
    wboson->process(event);
    if(!slct_oneijet->passes(event)) return false; // filter very few events which do not have one AK4 jet overlapping with t jet
    pseudotop->process(event); // needs to come after oneijet seletion! Else, no leptonic top quark hypothesis can be built

    if(_1toptag) hist_1toptag->fill(event);
    else hist_0toptag->fill(event);

    // Apply reweighting of DeepJet distributions
    if(debug) cout << "Apply reweighting of DeepJet distributions" << endl;
    sf_deepjet->process(event);
    if(_1toptag) hist_1toptag_btagsf->fill(event);
    else hist_0toptag_btagsf->fill(event);

    // Split tW signal into 3-merged, 2-merged, 1-merged, 0-merged (== how many top decay products ended up inside t-tagged HOTVR jet)
    if(debug) cout << "Split tW signal into 3-merged, 2-merged, 1-merged, 0-merged (== how many top decay products ended up inside t-tagged HOTVR jet)" << endl;
    if(_1toptag && dataset_version.find("ST_tW") == 0) {
      // GenProducer already processed at beginning of module!
      if((dataset_version.find("ST_tW_merged3") == 0 || dataset_version.find("ST_tW_DS_merged3") == 0) && !slct_tW_merged3->passes(event)) return false;
      if((dataset_version.find("ST_tW_merged2") == 0 || dataset_version.find("ST_tW_DS_merged2") == 0) && !slct_tW_merged2->passes(event)) return false;
      if((dataset_version.find("ST_tW_merged1") == 0 || dataset_version.find("ST_tW_DS_merged1") == 0) && !slct_tW_merged1->passes(event)) return false;
      if((dataset_version.find("ST_tW_merged0") == 0 || dataset_version.find("ST_tW_DS_merged0") == 0) && !slct_tW_merged0->passes(event)) return false;
      hist_decaymatch->fill(event);
      hist_decaymatch_Pt0to400->fill(event);
      hist_decaymatch_Pt400toInf->fill(event);
    }

    // Set event handles used as input for DNN training
    if(debug) cout << "Set event handles used as input for DNN training" << endl;
    dnn_setup->process(event);

    // Application of a trained DNN
    if(debug) cout << "Application of a trained DNN" << endl;
    map<string, double> inputs_map;
    for(uint i = 0; i < dnn_config_inputNames.size(); i++) {
      inputs_map[dnn_config_inputNames.at(i)] = (double)event.get(m_input_handles.at(i));
    }
    auto dnn_output_vals = NeuralNetwork->compute(inputs_map);
    event.set(h_dnn_output_val, (double)dnn_output_vals[dnn_config_outputName]);

    // Histograms of DNN inputs and DNN output
    if(debug) cout << "Histograms of DNN inputs and DNN output" << endl;
    if(_1toptag) {
      hist_dnn->fill(event);
      if(slct_noxjet->passes(event)) hist_dnn_noxjet_YES->fill(event); // well-defined tW LO, suppressing interference effects of tW NLO / ttbar LO
      else {
	      hist_dnn_noxjet_NO->fill(event);
	      if(slct_1bxjet->passes(event)) hist_dnn_1bxjet->fill(event);
	      else hist_dnn_0bxjet->fill(event);
      }
    }
    else { // Fill validation region DNN plots
      hist_dnn_0toptag->fill(event);
    }

    if(!_1toptag) return false; // Finally through away events without t-tagged jet

    // Place analysis routines into a new Module!!!
    // End of main selection
    return true;
  }


  UHH2_REGISTER_ANALYSIS_MODULE(HighPtSingleTopMainSelectionModule)
}
