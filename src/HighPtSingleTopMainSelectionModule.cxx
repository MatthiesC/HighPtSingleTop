#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/ObjectIdUtils.h"
#include "UHH2/common/include/PrimaryLepton.h"
#include "UHH2/common/include/TopJetIds.h"

#include "UHH2/HOTVR/include/HOTVRIds.h"
#include "UHH2/HOTVR/include/HadronicTop.h"

#include "UHH2/HighPtSingleTop/include/Ak8Corrections.h"
#include "UHH2/HighPtSingleTop/include/MyAk8Hists.h"
#include "UHH2/HighPtSingleTop/include/AndHists.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopSelections.h"
#include "UHH2/HighPtSingleTop/include/TaggedJets.h"
#include "UHH2/HighPtSingleTop/include/BTagHists.h"
#include "UHH2/HighPtSingleTop/include/DNNSetup.h"
#include "UHH2/HighPtSingleTop/include/DNNApplication.h"
#include "UHH2/HighPtSingleTop/include/DNNHists.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopHists.h"
#include "UHH2/HighPtSingleTop/include/MatchHists.h"
#include "UHH2/HighPtSingleTop/include/TopTagHists.h"
#include "UHH2/HighPtSingleTop/include/WTagHists.h"
#include "UHH2/HighPtSingleTop/include/ReconstructionAlgorithms.h"
#include "UHH2/HighPtSingleTop/include/LeptonAndTriggerScaleFactors.h"
#include "UHH2/HighPtSingleTop/include/HcalAndEcalModules.h"
#include "UHH2/HighPtSingleTop/include/TaggingScaleFactors.h"
#include "UHH2/HighPtSingleTop/include/TheoryCorrections.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"
#include "UHH2/HighPtSingleTop/include/VariablesOfInterest.h"


using namespace std;
using namespace uhh2;

namespace uhh2 {

  class HighPtSingleTopMainSelectionModule: public AnalysisModule {
  public:

    explicit HighPtSingleTopMainSelectionModule(Context & ctx);
    virtual bool process(Event & event) override;

  private:
    // --- Meta / miscellaneous
    bool debug;
    bool is_muo, is_ele;
    bool is_QCDsideband;
    bool empty_output_tree;
    string dataset_version;
    bool is_tW, is_tW_Bkg, is_tW_Sig;
    bool apply_DNNs;

    Event::Handle<GenericRegion> h_generic_region;
    Event::Handle<Region> h_region;

    // --- Scale factors
    unique_ptr<AnalysisModule> scale_variation;
    unique_ptr<AnalysisModule> sf_lumi, sf_pileup, sf_lepton, sf_trigger, sf_prefiring;
    // unique_ptr<AnalysisModule> sf_toppt;
    unique_ptr<AnalysisModule> sf_vjets;
    unique_ptr<AnalysisModule> sf_btagging;
    unique_ptr<MyHOTVRScaleFactor> sf_toptag;
    // unique_ptr<DeepAK8ScaleFactor> sf_wtag;

    // --- Event handle setters
    unique_ptr<AnalysisModule> SingleTopGen_tWchProd;
    unique_ptr<AnalysisModule> handle_primarylep, handle_ak8jets, handle_btaggedjets, handle_toptaggedjets, handle_wtaggedjets, handle_wboson;
    unique_ptr<AnalysisModule> handle_toptaggedjet, handle_ak4InExJets_top, handle_hadronictop;
    unique_ptr<AnalysisModule> handle_wtaggedjet, handle_ak4InExJets_W;
    unique_ptr<AnalysisModule> handle_pseudotop;
    unique_ptr<AnalysisModule> handle_voi;
    unique_ptr<DNNSetup> dnn_setup;
    unique_ptr<DNNApplication> dnn_app_ttag, dnn_app_wtag;

    // --- Selections
    unique_ptr<Selection> slct_tW_Sig;
    unique_ptr<Selection> slct_1ak4jet;
    unique_ptr<Selection> slct_trigger;
    unique_ptr<HEMIssueSelection> slct_hemissue;
    unique_ptr<Selection> slct_0btag, slct_1btag, slct_0toptag, slct_1toptag, slct_0wtag, slct_1wtag;

    // --- Histogram collections
    unique_ptr<AndHists> hist_lumiSF, hist_1ak4jet, hist_pileupSF, hist_leptonSF, hist_prefiringSF, hist_trigger, hist_triggerSF, hist_hemissue;
    // unique_ptr<AndHists> hist_topptSF;
    unique_ptr<AndHists> hist_vjetsSF;
    unique_ptr<Hists> hist_btagging_preSF_deepcsv, hist_btagging_preSF_deepjet;
    unique_ptr<AndHists> hist_btaggingSF;
    unique_ptr<Hists> hist_btagging_postSF_deepcsv, hist_btagging_postSF_deepjet;

    unique_ptr<AndHists> hist_TopTag_Begin, hist_TopTag_End, hist_TopTag0b, hist_TopTag1b, hist_TopTag2b;;
    unique_ptr<AndHists> hist_WTag_Begin, hist_WTag_End, hist_WTag0b, hist_WTag1b, hist_WTag2b;
    unique_ptr<AndHists> hist_Veto, hist_Veto0b, hist_Veto1b, hist_Veto2b;
    unique_ptr<AndHists> hist_0b, hist_1b, hist_2b;
    unique_ptr<Hists> hist_regions;

    unique_ptr<MatchHists> hist_Matching_TopTag, hist_Matching_WTag;

    unique_ptr<DNNHists> hist_dnn_TopTag, hist_dnn_WTag;
    unique_ptr<DNNHists> hist_dnn_TopTag0b, hist_dnn_WTag0b;
    unique_ptr<DNNHists> hist_dnn_TopTag1b, hist_dnn_WTag1b;
    unique_ptr<DNNHists> hist_dnn_TopTag2b, hist_dnn_WTag2b;
  };


  HighPtSingleTopMainSelectionModule::HighPtSingleTopMainSelectionModule(Context & ctx) {

    //------//
    // KEYS //
    //------//

    debug = string2bool(ctx.get("Debug"));

    is_muo = extract_channel(ctx) == Channel::isMuo;
    is_ele = extract_channel(ctx) == Channel::isEle;

    is_QCDsideband = string2bool(ctx.get("QCD_sideband"));

    empty_output_tree = string2bool(ctx.get("EmptyOutputTree")); // handy to not have output trees for systematics files, reduces root file size
    ctx.undeclare_all_event_output(); // throw away all output trees (jet collections etc.) which are not needed in further steps of the analysis

    dataset_version = ctx.get("dataset_version");
    is_tW = dataset_version.find("ST_tW") == 0;
    is_tW_Sig = is_tW && dataset_version.find("_Sig") != string::npos;
    is_tW_Bkg = is_tW && dataset_version.find("_Bkg") != string::npos;

    string syst_pileup = ctx.get("SystDirection_PileUp");
    string syst_toptag = ctx.get("SystDirection_HOTVRTopTagSF");

    BTag::algo btag_algo;
    if(ctx.get("BTagAlgorithm") == "DeepJet") {
      btag_algo = BTag::DEEPJET;
      ctx.set("BTagCalibration", ctx.get("BTagCalibration_DeepJet"));
    }
    else if(ctx.get("BTagAlgorithm") == "DeepCSV") {
      btag_algo = BTag::DEEPCSV;
      ctx.set("BTagCalibration", ctx.get("BTagCalibration_DeepCSV"));
    }
    else {
      throw invalid_argument("You need to specify either DeepJet or DeepCSV as b-tagging algorithm in your XML config file.");
    }
    string syst_btag = ctx.get("SystDirection_BTagSF", "nominal");

    apply_DNNs = string2bool(ctx.get("apply_DNNs"));


    //---------------------//
    // KINEMATIC VARIABLES //
    //---------------------//

    // HOTVR t-tagging criteria
    double hotvr_fpt_max = 0.8;
    double hotvr_jetmass_min = 140;
    double hotvr_jetmass_max = 220;
    double hotvr_mpair_min = 50;
    double hotvr_tau32_max = 0.56;


    //-----------------//
    // IDENTIFICATIONS //
    //-----------------//

    TopJetId StandardHOTVRTopTagID = AndId<TopJet>(HOTVRTopTag(hotvr_fpt_max, hotvr_jetmass_min, hotvr_jetmass_max, hotvr_mpair_min), Tau32Groomed(hotvr_tau32_max));

    WTaggedJets::wp wtag_workingpoint = WTaggedJets::WP_LOOSE;

    BTag::wp btag_workingpoint = BTag::WP_LOOSE;
    JetId BJetID = BTag(btag_algo, btag_workingpoint);


    //---------------//
    // SCALE FACTORS //
    //---------------//

    scale_variation.reset(new MCScaleVariation(ctx));
    sf_lumi.reset(new MCLumiWeight(ctx));
    sf_pileup.reset(new MCPileupReweight(ctx, syst_pileup));
    if(!is_QCDsideband) sf_lepton.reset(new LeptonScaleFactors(ctx));
    sf_trigger.reset(new TriggerScaleFactors(ctx));
    sf_prefiring.reset(new PrefiringWeights(ctx));
    // sf_toppt.reset(new TopPtReweighting(ctx, 0.0615, -0.0005));
    sf_vjets.reset(new VJetsReweighting(ctx));
    sf_btagging.reset(new MCBTagDiscriminantReweighting(ctx, btag_algo, "jets", syst_btag));
    sf_toptag.reset(new MyHOTVRScaleFactor(ctx, StandardHOTVRTopTagID));
    // sf_wtag.reset(new DeepAK8ScaleFactor(ctx, "W", false, wtag_workingpoint)); // false = don't use mass-decorrelated (MD) but nominal DeepAK8


    //---------------//
    // MISCELLANEOUS //
    //---------------//

    SingleTopGen_tWchProd.reset(new SingleTopGen_tWchProducer(ctx, "h_GENtW"));

    handle_primarylep.reset(new PrimaryLepton(ctx));
    handle_ak8jets.reset(new Ak8Jets(ctx));
    handle_btaggedjets.reset(new BTaggedJets(ctx, btag_algo, btag_workingpoint));
    handle_toptaggedjets.reset(new TopTaggedJets(ctx, StandardHOTVRTopTagID));
    handle_wtaggedjets.reset(new WTaggedJets(ctx, wtag_workingpoint));
    handle_wboson.reset(new WBosonLeptonic(ctx));
    handle_pseudotop.reset(new PseudoTopLeptonic(ctx, true, "WBosonLeptonic", "BJets")); // true = use b jets (if there are no b jets, all ak4 jets will be used instead to avoid crashes; important for Veto region)

    handle_toptaggedjet.reset(new TopTaggedJet(ctx));
    handle_ak4InExJets_top.reset(new InExAK4Jets(ctx, btag_algo, btag_workingpoint, "_Top", "TopTaggedJet", true));
    handle_hadronictop.reset(new HadronicTop(ctx));

    handle_wtaggedjet.reset(new WTaggedJet(ctx));
    handle_ak4InExJets_W.reset(new InExAK4Jets(ctx, btag_algo, btag_workingpoint, "_W", "WTaggedJet", false));

    h_generic_region = ctx.declare_event_output<GenericRegion>("generic_region");
    h_region = ctx.declare_event_output<Region>("region");

    handle_voi.reset(new VariablesOfInterest(ctx));

    dnn_setup.reset(new DNNSetup(ctx, btag_algo, -10.));
    if(apply_DNNs) {
      dnn_app_ttag.reset(new DNNApplication(ctx, "Top"));
      dnn_app_wtag.reset(new DNNApplication(ctx, "W"));
    }


    //------------//
    // SELECTIONS //
    //------------//

    slct_tW_Sig.reset(new tWgenSignalSelection(ctx, is_muo));

    slct_1ak4jet.reset(new NJetSelection(1, -1));
    slct_trigger.reset(new HighPtSingleTopTriggerSelection(ctx));
    slct_hemissue.reset(new HEMIssueSelection(ctx));

    slct_0btag.reset(new NJetSelection(0, 0, BJetID));
    slct_1btag.reset(new NJetSelection(1, 1, BJetID));
    slct_0toptag.reset(new NTopJetSelection(0, 0, StandardHOTVRTopTagID));
    slct_1toptag.reset(new NTopJetSelection(1, 1, StandardHOTVRTopTagID));
    slct_0wtag.reset(new MyNTopJetSelection(ctx, 0, 0, "WJets"));
    slct_1wtag.reset(new MyNTopJetSelection(ctx, 1, 1, "WJets"));


    //------------//
    // HISTOGRAMS //
    //------------//

    /*
    * PreSelection Level
    */

    hist_lumiSF.reset(new AndHists(ctx, "0_1_PrePreSel_LumiSF"));
    hist_1ak4jet.reset(new AndHists(ctx, "1_1_PreSel_OneAK4Jet"));
    hist_pileupSF.reset(new AndHists(ctx, "1_2_PreSel_PileUpSF"));
    hist_leptonSF.reset(new AndHists(ctx, "1_3_PreSel_LeptonSF"));
    hist_prefiringSF.reset(new AndHists(ctx, "1_4_PreSel_PrefiringSF"));
    hist_prefiringSF->add_Ak8Hists(ctx);
    hist_prefiringSF->add_BTagHists(ctx);

    hist_trigger.reset(new AndHists(ctx, "2_1_Trigger"));
    hist_triggerSF.reset(new AndHists(ctx, "2_2_TriggerSF"));
    hist_triggerSF->add_Ak8Hists(ctx);
    hist_triggerSF->add_BTagHists(ctx);

    hist_hemissue.reset(new AndHists(ctx, "2_3_HEM"));
    hist_hemissue->add_Ak8Hists(ctx);
    hist_hemissue->add_BTagHists(ctx);

    // hist_topptSF.reset(new AndHists(ctx, "2_4_TopPtSF"));
    // hist_topptSF->add_Ak8Hists(ctx);
    // hist_topptSF->add_BTagHists(ctx);

    hist_vjetsSF.reset(new AndHists(ctx, "2_5_VJetsSF"));
    hist_vjetsSF->add_Ak8Hists(ctx);
    hist_vjetsSF->add_BTagHists(ctx);

    hist_btagging_preSF_deepcsv.reset(new BTagHists(ctx, "2_5_BTaggingPreSF_DeepCSV", BTag::algo::DEEPCSV));
    hist_btagging_preSF_deepjet.reset(new BTagHists(ctx, "2_5_BTaggingPreSF_DeepJet", BTag::algo::DEEPJET));
    hist_btaggingSF.reset(new AndHists(ctx, "2_6_BTaggingPostSF"));
    // hist_btaggingSF->add_BTagHists(ctx);
    hist_btagging_postSF_deepcsv.reset(new BTagHists(ctx, "2_6_BTaggingPostSF_DeepCSV", BTag::algo::DEEPCSV));
    hist_btagging_postSF_deepjet.reset(new BTagHists(ctx, "2_6_BTaggingPostSF_DeepJet", BTag::algo::DEEPJET));


    /*
    * TopTagRegion
    */

    hist_TopTag_Begin.reset(new AndHists(ctx, "TopTag_Begin"));
    hist_TopTag_Begin->add_TopTagHists(ctx);
    hist_TopTag_Begin->add_Ak8Hists(ctx);
    hist_TopTag_Begin->add_TaggedJetsHists(ctx, "TopTaggedJet", "_Top");
    hist_TopTag_Begin->add_BTagHists(ctx);
    hist_TopTag_End.reset(new AndHists(ctx, "TopTag_End"));
    hist_TopTag_End->add_TopTagHists(ctx);
    hist_TopTag_End->add_Ak8Hists(ctx);
    hist_TopTag_End->add_TaggedJetsHists(ctx, "TopTaggedJet", "_Top");
    hist_TopTag_End->add_BTagHists(ctx);

    hist_Matching_TopTag.reset(new MatchHists(ctx, "Matching_TopTag", "Top"));

    hist_TopTag0b.reset(new AndHists(ctx, "TopTag0b"));
    hist_TopTag0b->add_TopTagHists(ctx);
    hist_TopTag0b->add_Ak8Hists(ctx);
    hist_TopTag0b->add_TaggedJetsHists(ctx, "TopTaggedJet", "_Top");
    hist_TopTag0b->add_BTagHists(ctx);

    hist_TopTag1b.reset(new AndHists(ctx, "TopTag1b"));
    hist_TopTag1b->add_TopTagHists(ctx);
    hist_TopTag1b->add_Ak8Hists(ctx);
    hist_TopTag1b->add_TaggedJetsHists(ctx, "TopTaggedJet", "_Top");
    hist_TopTag1b->add_BTagHists(ctx);

    hist_TopTag2b.reset(new AndHists(ctx, "TopTag2b"));
    hist_TopTag2b->add_TopTagHists(ctx);
    hist_TopTag2b->add_Ak8Hists(ctx);
    hist_TopTag2b->add_TaggedJetsHists(ctx, "TopTaggedJet", "_Top");
    hist_TopTag2b->add_BTagHists(ctx);


    /*
    * WTagRegion
    */

    hist_WTag_Begin.reset(new AndHists(ctx, "WTag_Begin"));
    hist_WTag_Begin->add_WTagHists(ctx);
    hist_WTag_Begin->add_Ak8Hists(ctx);
    hist_WTag_Begin->add_TaggedJetsHists(ctx, "WTaggedJet", "_W");
    hist_WTag_Begin->add_BTagHists(ctx);
    hist_WTag_End.reset(new AndHists(ctx, "WTag_End"));
    hist_WTag_End->add_WTagHists(ctx);
    hist_WTag_End->add_Ak8Hists(ctx);
    hist_WTag_End->add_TaggedJetsHists(ctx, "WTaggedJet", "_W");
    hist_WTag_End->add_BTagHists(ctx);

    hist_Matching_WTag.reset(new MatchHists(ctx, "Matching_WTag", "W"));

    hist_WTag0b.reset(new AndHists(ctx, "WTag0b"));
    hist_WTag0b->add_WTagHists(ctx);
    hist_WTag0b->add_Ak8Hists(ctx);
    hist_WTag0b->add_TaggedJetsHists(ctx, "WTaggedJet", "_W");
    hist_WTag0b->add_BTagHists(ctx);

    hist_WTag1b.reset(new AndHists(ctx, "WTag1b"));
    hist_WTag1b->add_WTagHists(ctx);
    hist_WTag1b->add_Ak8Hists(ctx);
    hist_WTag1b->add_TaggedJetsHists(ctx, "WTaggedJet", "_W");
    hist_WTag1b->add_BTagHists(ctx);

    hist_WTag2b.reset(new AndHists(ctx, "WTag2b"));
    hist_WTag2b->add_WTagHists(ctx);
    hist_WTag2b->add_Ak8Hists(ctx);
    hist_WTag2b->add_TaggedJetsHists(ctx, "WTaggedJet", "_W");
    hist_WTag2b->add_BTagHists(ctx);


    /*
    * VetoRegion
    */

    hist_Veto.reset(new AndHists(ctx, "Veto"));
    hist_Veto->add_Ak8Hists(ctx);
    hist_Veto->add_BTagHists(ctx);

    hist_Veto0b.reset(new AndHists(ctx, "Veto0b"));
    hist_Veto0b->add_Ak8Hists(ctx);
    hist_Veto0b->add_BTagHists(ctx);

    hist_Veto1b.reset(new AndHists(ctx, "Veto1b"));
    hist_Veto1b->add_Ak8Hists(ctx);
    hist_Veto1b->add_BTagHists(ctx);

    hist_Veto2b.reset(new AndHists(ctx, "Veto2b"));
    hist_Veto2b->add_Ak8Hists(ctx);
    hist_Veto2b->add_BTagHists(ctx);


    /*
    * Miscellaneous
    */

    hist_0b.reset(new AndHists(ctx, "Region0b"));
    hist_0b->add_Ak8Hists(ctx);
    hist_0b->add_BTagHists(ctx);

    hist_1b.reset(new AndHists(ctx, "Region1b"));
    hist_1b->add_Ak8Hists(ctx);
    hist_1b->add_BTagHists(ctx);

    hist_2b.reset(new AndHists(ctx, "Region2b"));
    hist_2b->add_BTagHists(ctx);
    hist_2b->add_Ak8Hists(ctx);

    hist_regions.reset(new RegionHist(ctx, "AnalysisRegions"));

    if(apply_DNNs) {
      hist_dnn_TopTag.reset(new DNNHists(ctx, "DNNHists_TopTag", dnn_setup->get_input_names_ttag(), dnn_setup->get_inputs_info_ttag(), dnn_app_ttag->get_output_names()));
      hist_dnn_TopTag0b.reset(new DNNHists(ctx, "DNNHists_TopTag0b", dnn_setup->get_input_names_ttag(), dnn_setup->get_inputs_info_ttag(), dnn_app_ttag->get_output_names()));
      hist_dnn_TopTag1b.reset(new DNNHists(ctx, "DNNHists_TopTag1b", dnn_setup->get_input_names_ttag(), dnn_setup->get_inputs_info_ttag(), dnn_app_ttag->get_output_names()));
      hist_dnn_TopTag2b.reset(new DNNHists(ctx, "DNNHists_TopTag2b", dnn_setup->get_input_names_ttag(), dnn_setup->get_inputs_info_ttag(), dnn_app_ttag->get_output_names()));

      hist_dnn_WTag.reset(new DNNHists(ctx, "DNNHists_WTag", dnn_setup->get_input_names_wtag(), dnn_setup->get_inputs_info_wtag(), dnn_app_wtag->get_output_names()));
      hist_dnn_WTag0b.reset(new DNNHists(ctx, "DNNHists_WTag0b", dnn_setup->get_input_names_wtag(), dnn_setup->get_inputs_info_wtag(), dnn_app_wtag->get_output_names()));
      hist_dnn_WTag1b.reset(new DNNHists(ctx, "DNNHists_WTag1b", dnn_setup->get_input_names_wtag(), dnn_setup->get_inputs_info_wtag(), dnn_app_wtag->get_output_names()));
      hist_dnn_WTag2b.reset(new DNNHists(ctx, "DNNHists_WTag2b", dnn_setup->get_input_names_wtag(), dnn_setup->get_inputs_info_wtag(), dnn_app_wtag->get_output_names()));
    }
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

    if(debug) cout << "Split up tW samples into decay channels" << endl;
    if(is_tW) {
      SingleTopGen_tWchProd->process(event);

      if(is_tW_Sig && !slct_tW_Sig->passes(event)) return false;
      if(is_tW_Bkg && slct_tW_Sig->passes(event)) return false;
    }

    // This is where the fun begins ...

    if(debug) cout << "Sort jets, topjets, and subjets by pt" << endl;
    sort_by_pt<Jet>(*event.jets); // Just to be sure. AK4 jets should already have been sorted during PreSelectionModule
    sort_by_pt<TopJet>(*event.topjets); // Only sorts HOTVR jets; AK8 jets already have been sorted during PreSelectionModule
    for(auto & tj : *event.topjets) {
      vector<Jet> sorted_subjets = tj.subjets();
      sort_by_pt<Jet>(sorted_subjets);
      tj.set_subjets(std::move(sorted_subjets));
    }

    if(debug) cout << "Set handles" << endl;
    handle_primarylep->process(event);
    handle_ak8jets->process(event);
    handle_btaggedjets->process(event);
    handle_toptaggedjets->process(event);
    handle_wtaggedjets->process(event);
    handle_wboson->process(event); // Reconstruct leptonic W boson; the leptonic top quark will be reconstructed later

    if(debug) cout << "Scale variation and lumi weights" << endl;
    scale_variation->process(event);
    sf_lumi->process(event);
    hist_lumiSF->fill(event);

    if(debug) cout << "Select at least one AK4 jet" << endl;
    if(!slct_1ak4jet->passes(event)) return false; // Require at least one AK4 jet for computational reasons (dR(lepton, jet) etc.); this rejects only \mathcal{O}(0.01\%) of events in real data (tested in 2017 muo, RunB)
    hist_1ak4jet->fill(event);

    if(debug) cout << "Apply pileup, lepton id/iso/reco, and prefiring scale factors" << endl;
    sf_pileup->process(event);
    hist_pileupSF->fill(event);
    if(!is_QCDsideband) sf_lepton->process(event);
    hist_leptonSF->fill(event);
    sf_prefiring->process(event);
    hist_prefiringSF->fill(event);

    if(debug) cout << "Select trigger paths and apply trigger scale factors" << endl;
    if(!slct_trigger->passes(event)) return false;
    hist_trigger->fill(event);
    sf_trigger->process(event);
    hist_triggerSF->fill(event);

    if(debug) cout << "Handle HEM15/16 issue for 2018" << endl;
    if(slct_hemissue->passes(event)) {
      if(event.isRealData) return false;
      else event.weight *= slct_hemissue->MCWeight();
    }
    hist_hemissue->fill(event);

    // if(debug) cout << "Apply top-pt reweighting for ttbar events" << endl;
    // sf_toppt->process(event);
    // hist_topptSF->fill(event);

    if(debug) cout << "Apply (N)NLO QCD/EWK corrections to V+jets samples" << endl;
    sf_vjets->process(event);
    hist_vjetsSF->fill(event);

    if(debug) cout << "Reweight b-tagging distributions for AK4 jets" << endl;
    hist_btagging_preSF_deepcsv->fill(event);
    hist_btagging_preSF_deepjet->fill(event);
    sf_btagging->process(event);
    hist_btaggingSF->fill(event);
    hist_btagging_postSF_deepcsv->fill(event);
    hist_btagging_postSF_deepjet->fill(event);

    if(debug) cout << "Set some booleans for analysis regions" << endl;
    bool b_0btag = slct_0btag->passes(event);
    bool b_1btag = slct_1btag->passes(event);
    bool b_0toptag = slct_0toptag->passes(event);
    bool b_1toptag = slct_1toptag->passes(event);
    bool b_0wtag = slct_0wtag->passes(event);
    bool b_1wtag = slct_1wtag->passes(event);
    GenericRegion generic_region(GenericRegion::not_defined);
    Region region(Region::not_defined);

    if(b_1toptag) { // Don't veto W-tags since this might hurt the signal efficiency
      if(debug) cout << "Event belongs to 'TopTag' region" << endl;
      generic_region = GenericRegion::TopTag;
      // sf_wtag->process_dummy(event); // Need to call event.set() since all scale factor weights are stored into output tree. Else, an error occurs

      if(debug) cout << "TopTagRegion: Set handles" << endl;
      handle_toptaggedjet->process(event);
      handle_ak4InExJets_top->process(event);
      if(debug) cout << "TopTagRegion: Fill initial control histograms" << endl;
      hist_TopTag_Begin->fill(event);
      if(debug) cout << "TopTagRegion: Apply HOTVR t-tagging scale factors" << endl;
      handle_hadronictop->process(event);
      sf_toptag->process(event);
      if(debug) cout << "TopTagRegion: Fill final control histograms" << endl;
      hist_TopTag_End->fill(event);

      if(b_0btag) {
        if(debug) cout << "TopTagRegion: 0 b-tags" << endl;
        hist_TopTag0b->fill(event);
        region = Region::TopTag0b;
      }
      else if(b_1btag) {
        if(debug) cout << "TopTagRegion: 1 b-tag" << endl;
        hist_TopTag1b->fill(event);
        region = Region::TopTag1b;
      }
      else {
        if(debug) cout << "TopTagRegion: 2 or more b-tags" << endl;
        hist_TopTag2b->fill(event);
        region = Region::TopTag2b;
      }

      if(is_tW) hist_Matching_TopTag->fill(event);
    }
    else if(b_0toptag && b_1wtag) {
      if(debug) cout << "Event belongs to 'WTag' region" << endl;
      generic_region = GenericRegion::WTag;
      sf_toptag->process_dummy(event);

      if(debug) cout << "WTagRegion: Set handles" << endl;
      handle_wtaggedjet->process(event);
      handle_ak4InExJets_W->process(event);
      if(debug) cout << "WTagRegion: Fill initial control histograms" << endl;
      hist_WTag_Begin->fill(event);
      // if(debug) cout << "WTagRegion: Apply DeepAK8 W-tagging scale factors" << endl;
      // sf_wtag->process(event); // TODO: Need to check those weird scale factors ...
      if(debug) cout << "WTagRegion: Fill final control histograms" << endl;
      hist_WTag_End->fill(event);

      if(b_0btag) {
        if(debug) cout << "WTagRegion: 0 b-tags" << endl;
        hist_WTag0b->fill(event);
        region = Region::WTag0b;
      }
      else if(b_1btag) {
        if(debug) cout << "WTagRegion: 1 b-tag" << endl;
        hist_WTag1b->fill(event);
        region = Region::WTag1b;
      }
      else {
        if(debug) cout << "WTagRegion: 2 or more b-tags" << endl;
        hist_WTag2b->fill(event);
        region = Region::WTag2b;
      }

      if(is_tW) hist_Matching_WTag->fill(event);
    }
    else if(b_0toptag && b_0wtag) {
      if(debug) cout << "Event belongs to 'Veto' region" << endl;
      generic_region = GenericRegion::Veto;
      // sf_toptag->process_dummy(event); // Needs to be set in case that VetoRegion events are kept in the AnalysisTree
      // sf_wtag->process_dummy(event);

      if(debug) cout << "VetoRegion: Fill control histograms" << endl;
      hist_Veto->fill(event);

      if(b_0btag) {
        if(debug) cout << "VetoRegion: 0 b-tags" << endl;
        hist_Veto0b->fill(event);
        region = Region::Veto0b;
      }
      else if(b_1btag) {
        if(debug) cout << "VetoRegion: 1 b-tag" << endl;
        hist_Veto1b->fill(event);
        region = Region::Veto1b;
      }
      else {
        if(debug) cout << "VetoRegion: 2 or more b-tags" << endl;
        hist_Veto2b->fill(event);
        region = Region::Veto2b;
      }
    }
    else {
      if(debug) cout << "Event belongs to 'Else' region" << endl;
      generic_region = GenericRegion::Else;
      region = Region::Else;
    }

    event.set(h_generic_region, generic_region);
    event.set(h_region, region);

    if(debug) cout << "Fill region histograms" << endl;
    hist_regions->fill(event);

    if(debug) cout << "Fill histograms based on number of b-tags" << endl;
    if(b_0btag) hist_0b->fill(event);
    else if(b_1btag) hist_1b->fill(event);
    else hist_2b->fill(event);

    if(generic_region == GenericRegion::Veto || generic_region == GenericRegion::Else) return false; // We won't need these events anymore for the DNN analysis

    if(debug) cout << "Set handle for leptonic pseudo top" << endl; // Events w/o AK4 jet have already been discarded at this point
    handle_pseudotop->process(event);

    if(debug) cout << "Setup handles for variables of interest (VOI)" << endl;
    handle_voi->process(event);

    // DNN-related code starts here...

    if(debug) cout << "Setting up input handles for the DNNs" << endl;
    dnn_setup->process(event);

    if(apply_DNNs) {
      if(debug) cout << "Calculate DNN outputs and fill DNN histograms" << endl;

      if(generic_region == GenericRegion::TopTag) {
        dnn_app_ttag->process(event);
        dnn_app_wtag->process_dummy(event);

        hist_dnn_TopTag->fill(event);
        if(b_0btag) hist_dnn_TopTag0b->fill(event);
        else if(b_1btag) hist_dnn_TopTag1b->fill(event);
        else hist_dnn_TopTag2b->fill(event);
      }
      else if(generic_region == GenericRegion::WTag) {
        dnn_app_wtag->process(event);
        dnn_app_ttag->process_dummy(event);

        hist_dnn_WTag->fill(event);
        if(b_0btag) hist_dnn_WTag0b->fill(event);
        else if(b_1btag) hist_dnn_WTag1b->fill(event);
        else hist_dnn_WTag2b->fill(event);
      }
    }
    else {
      if(debug) cout << "Skipping DNN application" << endl;
    }

    if(debug) cout << "Decide whether to keep the event in the output tree" << endl;
    bool keep_event(false);
    if(generic_region == GenericRegion::TopTag || generic_region == GenericRegion::WTag) keep_event = true;
    if(empty_output_tree) keep_event = false;

    if(debug) cout << "End of MainSelectionModule" << endl;
    return keep_event;
  }


  UHH2_REGISTER_ANALYSIS_MODULE(HighPtSingleTopMainSelectionModule)
}
