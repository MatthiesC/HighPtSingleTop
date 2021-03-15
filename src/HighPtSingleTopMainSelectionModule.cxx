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

    bool debug, empty_output_tree;

    unique_ptr<AnalysisModule> sf_lumi, sf_pileup, sf_lepton, sf_trigger, sf_prefiring, sf_btagging, scale_variation;
    // unique_ptr<AnalysisModule> sf_toppt;
    unique_ptr<AnalysisModule> sf_vjets;
    unique_ptr<MyHOTVRScaleFactor> sf_toptag;
    // unique_ptr<DeepAK8ScaleFactor> sf_wtag;
    unique_ptr<AnalysisModule> handle_primarylep, handle_hadronictop, handle_toptaggedjet, handle_wtaggedjet, handle_btaggedjets, handle_toptaggedjets, handle_ak4InExJets_top, handle_ak4InExJets_W, handle_wboson, handle_pseudotop, SingleTopGen_tWchProd;
    unique_ptr<Ak8Corrections> ak8corrections;
    unique_ptr<AnalysisModule> ak8cleaning, handle_ak8jets, handle_wtaggedjets;
    unique_ptr<DNNSetup> dnn_setup;
    unique_ptr<DNNApplication> dnn_app_ttag, dnn_app_wtag;

    unique_ptr<Selection> slct_WJetsHeavy;
    unique_ptr<Selection> slct_tW_Sig;
    unique_ptr<Selection> slct_1jet;
    // unique_ptr<Selection> slct_tW_TopToHad, slct_tW_TopToEle, slct_tW_TopToMuo, slct_tW_TopToTau, slct_tW_WToHad, slct_tW_WToEle, slct_tW_WToMuo, slct_tW_WToTau;
    unique_ptr<HEMIssueSelection> slct_hemissue;
    unique_ptr<Selection> slct_trigger, slct_0toptag, slct_1toptag, slct_oneAk8jet, slct_oneAk4jet, slct_bJetVeto;
    unique_ptr<Selection> slct_0btag, slct_1btag, slct_0wtag, slct_1wtag, slct_oneijet_top, slct_onexjet_W;
    // unique_ptr<Selection> slct_tW_merged3, slct_tW_merged2, slct_tW_merged1, slct_tW_merged0, slct_oneijet, slct_noxjet, slct_1bxjet;

    unique_ptr<AndHists> hist_presel_noweights, hist_presel_lumiSF, hist_presel_pileupSF, hist_presel_leptonSF, hist_presel_prefiringSF, hist_hemissue, hist_trigger, hist_triggerSF;
    // unique_ptr<AndHists> hist_topptSF;
    unique_ptr<Hists> hist_btagging_preSF_deepcsv, hist_btagging_preSF_deepjet;
    unique_ptr<Hists> hist_btagging_postSF_deepcsv, hist_btagging_postSF_deepjet;
    unique_ptr<AndHists> hist_btaggingSF;
    unique_ptr<AndHists> hist_vjetsSF;
    unique_ptr<Hists> hist_ak8_preCorr, hist_ak8_postCorr, hist_ak8_postCleaning;
    unique_ptr<AndHists> hist_TopTag_Begin, hist_TopTag_HotvrSF, hist_TopTag_End;
    unique_ptr<AndHists> hist_WTag_Begin, hist_WTag_DeepAk8SF, hist_WTag_End;
    unique_ptr<AndHists> hist_Veto_Begin, hist_Veto_Ak8Cut, hist_Veto_Ak4Cut, hist_Veto_End;

    unique_ptr<AndHists> hist_TopTag0b, hist_TopTag1b, hist_TopTag2b;
    unique_ptr<AndHists> hist_WTag0b, hist_WTag1b, hist_WTag2b;
    unique_ptr<AndHists> hist_Veto0b, hist_Veto1b, hist_Veto2b;
    unique_ptr<AndHists> hist_0b, hist_1b, hist_2b;
    unique_ptr<RegionHist> hist_regions;

    unique_ptr<AnalysisModule> handle_voi;

    //unique_ptr<Hists> hist_count_TopHadWLep_before, hist_count_TopHadWLep_after, hist_count_TopLepWHad_before, hist_count_TopLepWHad_after, hist_count_Veto_before, hist_count_Veto_after;
    // unique_ptr<Hists> hist_decaymatch, hist_decaymatch_Pt0to300, hist_decaymatch_Pt300toInf, hist_decaymatch_Pt300to400, hist_decaymatch_Pt0to400, hist_decaymatch_Pt400toInf;
    // unique_ptr<BinnedDNNHists> hist_dnn_ttag;
    unique_ptr<MatchHists> hist_Matching_TopTag, hist_Matching_WTag;
    unique_ptr<DNNHists> hist_dnn_TopTag, hist_dnn_WTag, hist_dnn_VetoTopTag, hist_dnn_VetoWTag;
    unique_ptr<DNNHists> hist_dnn_TopTag0b, hist_dnn_WTag0b, hist_dnn_VetoTopTag0b, hist_dnn_VetoWTag0b;
    unique_ptr<DNNHists> hist_dnn_TopTag1b, hist_dnn_WTag1b, hist_dnn_VetoTopTag1b, hist_dnn_VetoWTag1b;
    unique_ptr<DNNHists> hist_dnn_TopTag2b, hist_dnn_WTag2b, hist_dnn_VetoTopTag2b, hist_dnn_VetoWTag2b;
    // unique_ptr<BinnedDNNHists> hist_dnn_binned_TopTag, hist_dnn_binned_WTag, hist_dnn_binned_VetoTopTag, hist_dnn_binned_VetoWTag;

    string dataset_version;

    bool is_QCDsideband;

    Event::Handle<int> h_which_region;

    bool is_muo, is_ele;
    bool is_WJetsHeavy, is_WJetsLight;
    bool is_tW, is_tW_Bkg, is_tW_Sig;
    bool apply_DNNs;
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
    is_WJetsHeavy = dataset_version.find("WJetsHeavy") == 0;
    is_WJetsLight = dataset_version.find("WJetsLight") == 0;
    is_tW = dataset_version.find("ST_tW") == 0;
    is_tW_Sig = is_tW && dataset_version.find("_Sig") != string::npos;
    is_tW_Bkg = is_tW && dataset_version.find("_Bkg") != string::npos;

    string syst_pileup = ctx.get("SystDirection_Pileup", "nominal");
    string syst_toptag = ctx.get("SystDirection_HOTVRTopTagSF", "nominal");

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

    // Ak8 cleaning criteria
    double ak8_pt_min = 200.0;
    double ak8_eta_max = 2.4;
    double ak8_deltaRlepton_min = 0.8;


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

    sf_lumi.reset(new MCLumiWeight(ctx));
    sf_pileup.reset(new MCPileupReweight(ctx, syst_pileup));
    sf_lepton.reset(new LeptonScaleFactors(ctx));
    sf_trigger.reset(new TriggerScaleFactors(ctx));
    sf_prefiring.reset(new PrefiringWeights(ctx));
    scale_variation.reset(new MCScaleVariation(ctx));
    // sf_toppt.reset(new TopPtReweighting(ctx, 0.0615, -0.0005));
    sf_vjets.reset(new VJetsReweighting(ctx));
    sf_toptag.reset(new MyHOTVRScaleFactor(ctx, StandardHOTVRTopTagID));
    // sf_wtag.reset(new DeepAK8ScaleFactor(ctx, "W", false, wtag_workingpoint)); // false = don't use mass-decorrelated (MD) but nominal DeepAK8
    sf_btagging.reset(new MCBTagDiscriminantReweighting(ctx, btag_algo, "jets", syst_btag));


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
    handle_toptaggedjets.reset(new TopTaggedJets(ctx, StandardHOTVRTopTagID));
    handle_wboson.reset(new WBosonLeptonic(ctx));
    handle_pseudotop.reset(new PseudoTopLeptonic(ctx, true, "WBosonLeptonic", "BJets")); // true = use b jets (if there are no b jets, all ak4 jets will be used instead to avoid crashes; important for Veto region)

    handle_toptaggedjet.reset(new TopTaggedJet(ctx));
    handle_ak4InExJets_top.reset(new InExAK4Jets(ctx, btag_algo, btag_workingpoint, "_Top", "TopTaggedJet", true));
    handle_hadronictop.reset(new HadronicTop(ctx));

    handle_wtaggedjet.reset(new WTaggedJet(ctx));
    handle_ak4InExJets_W.reset(new InExAK4Jets(ctx, btag_algo, btag_workingpoint, "_W", "WTaggedJet", false));

    h_which_region = ctx.declare_event_output<int>("which_region"); // need to declare this event output before initializing DNNSetup class

    handle_voi.reset(new VariablesOfInterest(ctx));

    dnn_setup.reset(new DNNSetup(ctx, btag_algo));
    if(apply_DNNs) {
      dnn_app_ttag.reset(new DNNApplication(ctx, "Top"));
      dnn_app_wtag.reset(new DNNApplication(ctx, "W"));
    }


    //------------//
    // SELECTIONS //
    //------------//

    slct_WJetsHeavy.reset(new WJetsGenSelection(ctx, "HF"));

    slct_tW_Sig.reset(new tWgenSignalSelection(ctx, is_muo));
    // slct_tW_TopToHad.reset(new tWgenSelection(ctx, "TopToHad"));
    // slct_tW_TopToEle.reset(new tWgenSelection(ctx, "TopToEle"));
    // slct_tW_TopToMuo.reset(new tWgenSelection(ctx, "TopToMuo"));
    // slct_tW_TopToTau.reset(new tWgenSelection(ctx, "TopToTau"));
    // slct_tW_WToHad.reset(new tWgenSelection(ctx, "WToHad"));
    // slct_tW_WToEle.reset(new tWgenSelection(ctx, "WToEle"));
    // slct_tW_WToMuo.reset(new tWgenSelection(ctx, "WToMuo"));
    // slct_tW_WToTau.reset(new tWgenSelection(ctx, "WToTau"));

    slct_1jet.reset(new NJetSelection(1, -1));

    slct_trigger.reset(new HighPtSingleTopTriggerSelection(ctx));
    slct_hemissue.reset(new HEMIssueSelection(ctx));

    slct_0btag.reset(new NJetSelection(0, 0, BJetID));
    slct_1btag.reset(new NJetSelection(1, 1, BJetID));
    slct_0toptag.reset(new NTopJetSelection(0, 0, StandardHOTVRTopTagID));
    slct_1toptag.reset(new NTopJetSelection(1, 1, StandardHOTVRTopTagID));
    slct_0wtag.reset(new MyNTopJetSelection(ctx, 0, 0, "WJets"));
    slct_1wtag.reset(new MyNTopJetSelection(ctx, 1, 1, "WJets"));

    slct_oneijet_top.reset(new MyNJetSelection(ctx, 1, -1, "InBJets_Top")); // InJets_Top
    slct_onexjet_W.reset(new MyNJetSelection(ctx, 1, -1, "ExBJets_W")); // "ExJets_W"
    slct_oneAk8jet.reset(new MyNTopJetSelection(ctx, 1, -1, "Ak8Jets"));
    slct_oneAk4jet.reset(new NJetSelection(1, -1));
    slct_bJetVeto.reset(new MyNJetSelection(ctx, 0, 0, "BJets"));


    // slct_tW_merged3.reset(new MergeScenarioSelection(ctx, 3));
    // slct_tW_merged2.reset(new MergeScenarioSelection(ctx, 2));
    // slct_tW_merged1.reset(new MergeScenarioSelection(ctx, 1));
    // slct_tW_merged0.reset(new MergeScenarioSelection(ctx, 0));






    //------------//
    // HISTOGRAMS //
    //------------//

    // Control distributions after last module's preselection; lumi, PU, and lepton id/reco scale factors are applied
    hist_presel_noweights.reset(new AndHists(ctx, "1_PreSel_0_noweights"));
    hist_presel_lumiSF.reset(new AndHists(ctx, "1_PreSel_1_lumiSF"));
    hist_presel_pileupSF.reset(new AndHists(ctx, "1_PreSel_2_pileupSF"));
    hist_presel_leptonSF.reset(new AndHists(ctx, "1_PreSel_3_leptonSF"));
    hist_presel_prefiringSF.reset(new AndHists(ctx, "1_PreSel_4_prefiringSF"));

    // Control distributions after trigger selection and respective scale factors
    hist_trigger.reset(new AndHists(ctx, "2_Trigger"));
    hist_triggerSF.reset(new AndHists(ctx, "2_TriggerSF"));

    // Control distributions or AK8 jets: JEC+JER and cleaning
    hist_ak8_preCorr.reset(new MyAk8Hists(ctx, "2_Ak8Setup_PreCorr", ctx.get("Ak8recCollection"))); // AK8 handles not yet set, use additional branch directly
    hist_ak8_postCorr.reset(new MyAk8Hists(ctx, "2_Ak8Setup_PostCorr", ctx.get("Ak8recCollection"))); // AK8 handles not yet set, use additional branch directly
    hist_ak8_postCleaning.reset(new MyAk8Hists(ctx, "2_Ak8Setup_PostCleaning"));

    hist_btagging_preSF_deepcsv.reset(new BTagHists(ctx, "2_BTaggingPreSF_DeepCSV", BTag::algo::DEEPCSV));
    hist_btagging_preSF_deepjet.reset(new BTagHists(ctx, "2_BTaggingPreSF_DeepJet", BTag::algo::DEEPJET));
    hist_btaggingSF.reset(new AndHists(ctx, "2_BTaggingPostSF"));
    // hist_btaggingSF->add_BTagHists(ctx);
    hist_btagging_postSF_deepcsv.reset(new BTagHists(ctx, "2_BTaggingPostSF_DeepCSV", BTag::algo::DEEPCSV));
    hist_btagging_postSF_deepjet.reset(new BTagHists(ctx, "2_BTaggingPostSF_DeepJet", BTag::algo::DEEPJET));

    hist_hemissue.reset(new AndHists(ctx, "2_HEM"));
    hist_hemissue->add_Ak8Hists(ctx);
    hist_hemissue->add_BTagHists(ctx);

    // hist_topptSF.reset(new AndHists(ctx, "2_TopPtSF"));
    // hist_topptSF->add_Ak8Hists(ctx);
    // hist_topptSF->add_BTagHists(ctx);

    hist_vjetsSF.reset(new AndHists(ctx, "2_VJetsSF"));
    hist_vjetsSF->add_Ak8Hists(ctx);
    hist_vjetsSF->add_BTagHists(ctx);

    hist_TopTag_Begin.reset(new AndHists(ctx, "3_TopTag_Begin"));
    hist_TopTag_Begin->add_TopTagHists(ctx);
    hist_TopTag_Begin->add_Ak8Hists(ctx);
    hist_TopTag_Begin->add_TaggedJetsHists(ctx, "TopTaggedJet", "_Top");
    hist_TopTag_Begin->add_BTagHists(ctx);
    // hist_TopTag_HotvrSF.reset(new AndHists(ctx, "3_TopTag_HotvrSF"));
    // hist_TopTag_HotvrSF->add_TopTagHists(ctx);
    // hist_TopTag_HotvrSF->add_Ak8Hists(ctx);
    // hist_TopTag_HotvrSF->add_TaggedJetsHists(ctx, "TopTaggedJet", "_Top");
    hist_TopTag_End.reset(new AndHists(ctx, "3_TopTag_End"));
    hist_TopTag_End->add_TopTagHists(ctx);
    hist_TopTag_End->add_Ak8Hists(ctx);
    hist_TopTag_End->add_TaggedJetsHists(ctx, "TopTaggedJet", "_Top");
    hist_TopTag_End->add_BTagHists(ctx);

    hist_WTag_Begin.reset(new AndHists(ctx, "3_WTag_Begin"));
    hist_WTag_Begin->add_WTagHists(ctx);
    hist_WTag_Begin->add_Ak8Hists(ctx);
    hist_WTag_Begin->add_TaggedJetsHists(ctx, "WTaggedJet", "_W");
    hist_WTag_Begin->add_BTagHists(ctx);
    // hist_WTag_DeepAk8SF.reset(new AndHists(ctx, "3_WTag_DeepAk8SF"));
    // hist_WTag_DeepAk8SF->add_WTagHists(ctx);
    // hist_WTag_DeepAk8SF->add_Ak8Hists(ctx);
    // hist_WTag_DeepAk8SF->add_TaggedJetsHists(ctx, "WTaggedJet", "_W");
    hist_WTag_End.reset(new AndHists(ctx, "3_WTag_End"));
    hist_WTag_End->add_WTagHists(ctx);
    hist_WTag_End->add_Ak8Hists(ctx);
    hist_WTag_End->add_TaggedJetsHists(ctx, "WTaggedJet", "_W");
    hist_WTag_End->add_BTagHists(ctx);

    hist_Veto_Begin.reset(new AndHists(ctx, "3_Veto_Begin"));
    hist_Veto_Begin->add_Ak8Hists(ctx);
    hist_Veto_Begin->add_BTagHists(ctx);
    // hist_Veto_Ak8Cut.reset(new AndHists(ctx, "3_Veto_Ak8Cut"));
    // hist_Veto_Ak8Cut->add_TopTagHists(ctx);
    // hist_Veto_Ak8Cut->add_WTagHists(ctx);
    // hist_Veto_Ak8Cut->add_Ak8Hists(ctx);
    // hist_Veto_Ak4Cut.reset(new AndHists(ctx, "3_Veto_Ak4Cut"));
    // hist_Veto_Ak4Cut->add_TopTagHists(ctx);
    // hist_Veto_Ak4Cut->add_WTagHists(ctx);
    // hist_Veto_Ak4Cut->add_Ak8Hists(ctx);
    // hist_Veto_Ak4Cut->add_TaggedJetsHists(ctx, "TopTaggedJet", "_Top");
    // hist_Veto_Ak4Cut->add_TaggedJetsHists(ctx, "WTaggedJet", "_W");
    hist_Veto_End.reset(new AndHists(ctx, "3_Veto_End"));
    hist_Veto_End->add_TopTagHists(ctx);
    hist_Veto_End->add_WTagHists(ctx);
    hist_Veto_End->add_Ak8Hists(ctx);
    hist_Veto_End->add_TaggedJetsHists(ctx, "TopTaggedJet", "_Top");
    hist_Veto_End->add_TaggedJetsHists(ctx, "WTaggedJet", "_W");
    hist_Veto_End->add_BTagHists(ctx);


    hist_TopTag0b.reset(new AndHists(ctx, "3_TopTag0b"));
    hist_TopTag0b->add_TopTagHists(ctx);
    hist_TopTag0b->add_Ak8Hists(ctx);
    hist_TopTag0b->add_TaggedJetsHists(ctx, "TopTaggedJet", "_Top");
    hist_TopTag0b->add_BTagHists(ctx);

    hist_TopTag1b.reset(new AndHists(ctx, "3_TopTag1b"));
    hist_TopTag1b->add_TopTagHists(ctx);
    hist_TopTag1b->add_Ak8Hists(ctx);
    hist_TopTag1b->add_TaggedJetsHists(ctx, "TopTaggedJet", "_Top");
    hist_TopTag1b->add_BTagHists(ctx);

    hist_TopTag2b.reset(new AndHists(ctx, "3_TopTag2b"));
    hist_TopTag2b->add_TopTagHists(ctx);
    hist_TopTag2b->add_Ak8Hists(ctx);
    hist_TopTag2b->add_TaggedJetsHists(ctx, "TopTaggedJet", "_Top");
    hist_TopTag2b->add_BTagHists(ctx);


    hist_WTag0b.reset(new AndHists(ctx, "3_WTag0b"));
    hist_WTag0b->add_WTagHists(ctx);
    hist_WTag0b->add_Ak8Hists(ctx);
    hist_WTag0b->add_TaggedJetsHists(ctx, "WTaggedJet", "_W");
    hist_WTag0b->add_BTagHists(ctx);

    hist_WTag1b.reset(new AndHists(ctx, "3_WTag1b"));
    hist_WTag1b->add_WTagHists(ctx);
    hist_WTag1b->add_Ak8Hists(ctx);
    hist_WTag1b->add_TaggedJetsHists(ctx, "WTaggedJet", "_W");
    hist_WTag1b->add_BTagHists(ctx);

    hist_WTag2b.reset(new AndHists(ctx, "3_WTag2b"));
    hist_WTag2b->add_WTagHists(ctx);
    hist_WTag2b->add_Ak8Hists(ctx);
    hist_WTag2b->add_TaggedJetsHists(ctx, "WTaggedJet", "_W");
    hist_WTag2b->add_BTagHists(ctx);


    hist_Veto0b.reset(new AndHists(ctx, "3_Veto0b"));
    hist_Veto0b->add_TopTagHists(ctx);
    hist_Veto0b->add_WTagHists(ctx);
    hist_Veto0b->add_Ak8Hists(ctx);
    hist_Veto0b->add_TaggedJetsHists(ctx, "TopTaggedJet", "_Top");
    hist_Veto0b->add_TaggedJetsHists(ctx, "WTaggedJet", "_W");
    hist_Veto0b->add_BTagHists(ctx);

    hist_Veto1b.reset(new AndHists(ctx, "3_Veto1b"));
    hist_Veto1b->add_TopTagHists(ctx);
    hist_Veto1b->add_WTagHists(ctx);
    hist_Veto1b->add_Ak8Hists(ctx);
    hist_Veto1b->add_TaggedJetsHists(ctx, "TopTaggedJet", "_Top");
    hist_Veto1b->add_TaggedJetsHists(ctx, "WTaggedJet", "_W");
    hist_Veto1b->add_BTagHists(ctx);

    hist_Veto2b.reset(new AndHists(ctx, "3_Veto2b"));
    hist_Veto2b->add_TopTagHists(ctx);
    hist_Veto2b->add_WTagHists(ctx);
    hist_Veto2b->add_Ak8Hists(ctx);
    hist_Veto2b->add_TaggedJetsHists(ctx, "TopTaggedJet", "_Top");
    hist_Veto2b->add_TaggedJetsHists(ctx, "WTaggedJet", "_W");
    hist_Veto2b->add_BTagHists(ctx);


    hist_0b.reset(new AndHists(ctx, "3_0b"));
    hist_0b->add_Ak8Hists(ctx);
    hist_0b->add_BTagHists(ctx);

    hist_1b.reset(new AndHists(ctx, "3_1b"));
    hist_1b->add_Ak8Hists(ctx);
    hist_1b->add_BTagHists(ctx);

    hist_2b.reset(new AndHists(ctx, "3_2b"));
    hist_2b->add_BTagHists(ctx);
    hist_2b->add_Ak8Hists(ctx);

    hist_regions.reset(new RegionHist(ctx, "AnalysisRegions"));


    hist_Matching_TopTag.reset(new MatchHists(ctx, "Matching_TopTag", "Top"));
    hist_Matching_WTag.reset(new MatchHists(ctx, "Matching_WTag", "W"));

    // hist_decaymatch.reset(new MatchHists(ctx, "MatchHists_Full"));
    // hist_decaymatch_Pt0to400.reset(new MatchHists(ctx, "MatchHists_Pt0to400", 0, 400));
    // hist_decaymatch_Pt400toInf.reset(new MatchHists(ctx, "MatchHists_Pt400toInf", 400));

    // hist_dnn.reset(new BinnedDNNHists(ctx, "DNNHists_TopTag", dnn_config_inputNames, dnn_setup->inputs_info()));

    // hist_dnn_binned_TopTag.reset(new BinnedDNNHists(ctx, "BinnedDNNHists_TopTag", dnn_setup->get_input_names_ttag(), dnn_setup->get_inputs_info_ttag(), dnn_app_ttag->get_output_names()));
    // hist_dnn_binned_WTag.reset(new BinnedDNNHists(ctx, "BinnedDNNHists_WTag", dnn_setup->get_input_names_wtag(), dnn_setup->get_inputs_info_wtag(), dnn_app_wtag->get_output_names()));
    // hist_dnn_binned_VetoTopTag.reset(new BinnedDNNHists(ctx, "BinnedDNNHists_VetoTopTag", dnn_setup->get_input_names_ttag(), dnn_setup->get_inputs_info_ttag(), dnn_app_ttag->get_output_names())); // For the Veto region, it makes no sense to look at the pT of the t jet/pseudtop or W jet/leptonic W boson
    // hist_dnn_binned_VetoWTag.reset(new BinnedDNNHists(ctx, "BinnedDNNHists_VetoWTag", dnn_setup->get_input_names_wtag(), dnn_setup->get_inputs_info_wtag(), dnn_app_wtag->get_output_names()));

    if(apply_DNNs) {
      hist_dnn_TopTag.reset(new DNNHists(ctx, "DNNHists_TopTag", dnn_setup->get_input_names_ttag(), dnn_setup->get_inputs_info_ttag(), dnn_app_ttag->get_output_names()));
      hist_dnn_TopTag0b.reset(new DNNHists(ctx, "DNNHists_TopTag0b", dnn_setup->get_input_names_ttag(), dnn_setup->get_inputs_info_ttag(), dnn_app_ttag->get_output_names()));
      hist_dnn_TopTag1b.reset(new DNNHists(ctx, "DNNHists_TopTag1b", dnn_setup->get_input_names_ttag(), dnn_setup->get_inputs_info_ttag(), dnn_app_ttag->get_output_names()));
      hist_dnn_TopTag2b.reset(new DNNHists(ctx, "DNNHists_TopTag2b", dnn_setup->get_input_names_ttag(), dnn_setup->get_inputs_info_ttag(), dnn_app_ttag->get_output_names()));

      hist_dnn_WTag.reset(new DNNHists(ctx, "DNNHists_WTag", dnn_setup->get_input_names_wtag(), dnn_setup->get_inputs_info_wtag(), dnn_app_wtag->get_output_names()));
      hist_dnn_WTag0b.reset(new DNNHists(ctx, "DNNHists_WTag0b", dnn_setup->get_input_names_wtag(), dnn_setup->get_inputs_info_wtag(), dnn_app_wtag->get_output_names()));
      hist_dnn_WTag1b.reset(new DNNHists(ctx, "DNNHists_WTag1b", dnn_setup->get_input_names_wtag(), dnn_setup->get_inputs_info_wtag(), dnn_app_wtag->get_output_names()));
      hist_dnn_WTag2b.reset(new DNNHists(ctx, "DNNHists_WTag2b", dnn_setup->get_input_names_wtag(), dnn_setup->get_inputs_info_wtag(), dnn_app_wtag->get_output_names()));

      hist_dnn_VetoTopTag.reset(new DNNHists(ctx, "DNNHists_VetoTopTag", dnn_setup->get_input_names_ttag(), dnn_setup->get_inputs_info_ttag(), dnn_app_ttag->get_output_names()));
      hist_dnn_VetoTopTag0b.reset(new DNNHists(ctx, "DNNHists_VetoTopTag0b", dnn_setup->get_input_names_ttag(), dnn_setup->get_inputs_info_ttag(), dnn_app_ttag->get_output_names()));
      hist_dnn_VetoTopTag1b.reset(new DNNHists(ctx, "DNNHists_VetoTopTag1b", dnn_setup->get_input_names_ttag(), dnn_setup->get_inputs_info_ttag(), dnn_app_ttag->get_output_names()));
      hist_dnn_VetoTopTag2b.reset(new DNNHists(ctx, "DNNHists_VetoTopTag2b", dnn_setup->get_input_names_ttag(), dnn_setup->get_inputs_info_ttag(), dnn_app_ttag->get_output_names()));

      hist_dnn_VetoWTag.reset(new DNNHists(ctx, "DNNHists_VetoWTag", dnn_setup->get_input_names_wtag(), dnn_setup->get_inputs_info_wtag(), dnn_app_wtag->get_output_names()));
      hist_dnn_VetoWTag0b.reset(new DNNHists(ctx, "DNNHists_VetoWTag0b", dnn_setup->get_input_names_wtag(), dnn_setup->get_inputs_info_wtag(), dnn_app_wtag->get_output_names()));
      hist_dnn_VetoWTag1b.reset(new DNNHists(ctx, "DNNHists_VetoWTag1b", dnn_setup->get_input_names_wtag(), dnn_setup->get_inputs_info_wtag(), dnn_app_wtag->get_output_names()));
      hist_dnn_VetoWTag2b.reset(new DNNHists(ctx, "DNNHists_VetoWTag2b", dnn_setup->get_input_names_wtag(), dnn_setup->get_inputs_info_wtag(), dnn_app_wtag->get_output_names()));
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

    // Split up W+jets and tW samples

    if(debug) cout << "Split up WJets into heavy flavor and light jets" << endl;
    if(is_WJetsHeavy && !slct_WJetsHeavy->passes(event)) return false;
    if(is_WJetsLight && slct_WJetsHeavy->passes(event)) return false;

    if(debug) cout << "Split up tW samples into decay channels" << endl;
    if(is_tW) {

      SingleTopGen_tWchProd->process(event);

      if(is_tW_Sig && !slct_tW_Sig->passes(event)) return false;
      if(is_tW_Bkg && slct_tW_Sig->passes(event)) return false;

      // bool is_TopToHad = slct_tW_TopToHad->passes(event);
      // bool is_TopToEle = slct_tW_TopToEle->passes(event);
      // bool is_TopToMuo = slct_tW_TopToMuo->passes(event);
      // bool is_TopToTau = slct_tW_TopToTau->passes(event);
      //
      // bool is_WToHad = slct_tW_WToHad->passes(event);
      // bool is_WToEle = slct_tW_WToEle->passes(event);
      // bool is_WToMuo = slct_tW_WToMuo->passes(event);
      // bool is_WToTau = slct_tW_WToTau->passes(event);
      //
      // if(dataset_version.find("TopToHad_WToHad") != string::npos && !(is_TopToHad && is_WToHad)) return false; // not included in NoFullyHadronic tW samples
      // if(dataset_version.find("TopToHad_WToEle") != string::npos && !(is_TopToHad && is_WToEle)) return false;
      // if(dataset_version.find("TopToHad_WToMuo") != string::npos && !(is_TopToHad && is_WToMuo)) return false;
      // if(dataset_version.find("TopToHad_WToTau") != string::npos && !(is_TopToHad && is_WToTau)) return false;
      //
      // if(dataset_version.find("TopToEle_WToHad") != string::npos && !(is_TopToEle && is_WToHad)) return false;
      // if(dataset_version.find("TopToEle_WToEle") != string::npos && !(is_TopToEle && is_WToEle)) return false;
      // if(dataset_version.find("TopToEle_WToMuo") != string::npos && !(is_TopToEle && is_WToMuo)) return false;
      // if(dataset_version.find("TopToEle_WToTau") != string::npos && !(is_TopToEle && is_WToTau)) return false;
      //
      // if(dataset_version.find("TopToMuo_WToHad") != string::npos && !(is_TopToMuo && is_WToHad)) return false;
      // if(dataset_version.find("TopToMuo_WToEle") != string::npos && !(is_TopToMuo && is_WToEle)) return false;
      // if(dataset_version.find("TopToMuo_WToMuo") != string::npos && !(is_TopToMuo && is_WToMuo)) return false;
      // if(dataset_version.find("TopToMuo_WToTau") != string::npos && !(is_TopToMuo && is_WToTau)) return false;
      //
      // if(dataset_version.find("TopToTau_WToHad") != string::npos && !(is_TopToTau && is_WToHad)) return false;
      // if(dataset_version.find("TopToTau_WToEle") != string::npos && !(is_TopToTau && is_WToEle)) return false;
      // if(dataset_version.find("TopToTau_WToMuo") != string::npos && !(is_TopToTau && is_WToMuo)) return false;
      // if(dataset_version.find("TopToTau_WToTau") != string::npos && !(is_TopToTau && is_WToTau)) return false;
    }

    // This is where the fun begins...

    if(!slct_1jet->passes(event)) return false; // require at least one AK4 jet for computational reasons (dR(lepton, jet) etc.), reject ca. 0.05 % of events in real data (tested in 2017 muo, RunB)

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
    hist_presel_noweights->fill(event);
    sf_lumi->process(event);
    hist_presel_lumiSF->fill(event);
    sf_pileup->process(event);
    hist_presel_pileupSF->fill(event);
    if(!is_QCDsideband) sf_lepton->process(event);
    hist_presel_leptonSF->fill(event);
    sf_prefiring->process(event);
    hist_presel_prefiringSF->fill(event);

    if(debug) cout << "Select trigger paths and apply trigger scale factors" << endl;
    if(!slct_trigger->passes(event)) return false;
    hist_trigger->fill(event);
    sf_trigger->process(event);
    hist_triggerSF->fill(event);

    if(debug) cout << "Apply corrections to AK8 jets and clean them" << endl;
    hist_ak8_preCorr->fill(event);
    ak8corrections->process(event); // don't sort AK8 jets after correcting them to have consistent preCorr and postCorr histograms
    hist_ak8_postCorr->fill(event);
    ak8cleaning->process(event); // clean AK8 jets and sort them by pt
    handle_ak8jets->process(event);
    hist_ak8_postCleaning->fill(event);
    handle_wtaggedjets->process(event);

    if(debug) cout << "Set handles for b-tagged AK4 jets, t-tagged HOTVR jets, and leptonic W boson hypothesis" << endl;
    handle_btaggedjets->process(event);
    handle_toptaggedjets->process(event);
    handle_wboson->process(event); // the leptonic one!

    if(debug) cout << "Handle HEM15/16 issue for 2018" << endl;
    if(slct_hemissue->passes(event)) {
      if(event.isRealData) return false;
      else event.weight *= slct_hemissue->MCWeight();
    }
    hist_hemissue->fill(event);

    if(debug) cout << "Reweight b-tagging distributions for AK4 jets" << endl;
    hist_btagging_preSF_deepcsv->fill(event);
    hist_btagging_preSF_deepjet->fill(event);
    sf_btagging->process(event);
    hist_btaggingSF->fill(event);
    hist_btagging_postSF_deepcsv->fill(event);
    hist_btagging_postSF_deepjet->fill(event);

    // if(debug) cout << "Apply top-pt reweighting for ttbar events" << endl;
    // sf_toppt->process(event);
    // hist_topptSF->fill(event);

    if(debug) cout << "Apply (N)NLO QCD/EWK corrections to V+jets samples" << endl;
    sf_vjets->process(event);
    hist_vjetsSF->fill(event);

    if(debug) cout << "Set some booleans for analysis regions" << endl;
    bool b_0btag = slct_0btag->passes(event);
    bool b_1btag = slct_1btag->passes(event);
    bool b_1toptag = slct_1toptag->passes(event);
    bool b_0toptag = slct_0toptag->passes(event);
    bool b_1wtag = slct_1wtag->passes(event);
    bool b_0wtag = slct_0wtag->passes(event);
    bool is_TopTagRegion(false);
    bool is_WTagRegion(false);
    bool is_VetoRegion(false);
    bool is_ElseRegion(false);
    int region(0);

    // Caveat: The order of analysis modules in the following if-statements is crucial and should be changed with care only!

    if(b_1toptag) { // don't veto w-tags since this might hurt the signal efficiency
      if(debug) cout << "Event belongs to 'TopTag' region." << endl;
      // sf_wtag->process_dummy(event); // Need to call event.set() since all scale factor weights are stored into output tree. Else, an error occurs

      if(debug) cout << "SR t(had)W(lep): Set handles for top tag and AK4 jets inside/outside top jet" << endl;
      handle_toptaggedjet->process(event);
      handle_ak4InExJets_top->process(event);
      if(debug) cout << "SR t(had)W(lep): Fill initial control histograms" << endl;
      hist_TopTag_Begin->fill(event);
      if(debug) cout << "SR t(had)W(lep): Apply HOTVR top-tagging scale factors" << endl;
      handle_hadronictop->process(event);
      sf_toptag->process(event);
      // if(debug) cout << "SR t(had)W(lep): Fill control histograms after HOTVR scale factors" << endl;
      // hist_TopTag_HotvrSF->fill(event);
      // if(debug) cout << "SR t(had)W(lep): Throw away very few events which do not have at least one AK4 jet within top jet" << endl;
      // if(!slct_oneijet_top->passes(event)) return false; // Need to have at least one AK4 jet for pseudotop and also makes sense to have at least one AK4 jet inside HOTVR jet
      if(debug) cout << "SR t(had)W(lep): Fill final control histograms" << endl;
      hist_TopTag_End->fill(event);

      if(is_tW) hist_Matching_TopTag->fill(event);

      is_TopTagRegion = true;

      if(b_0btag) {
        if(debug) cout << "0 b-tags" << endl;
        hist_TopTag0b->fill(event);
        region = 1;
      }
      else if(b_1btag) {
        if(debug) cout << "1 b-tag" << endl;
        hist_TopTag1b->fill(event);
        region = 4;
      }
      else {
        if(debug) cout << "2 or more b-tags" << endl;
        hist_TopTag2b->fill(event);
        region = 7;
      }
    }

    else if(b_0toptag && b_1wtag) {
      if(debug) cout << "Event belongs to 'WTag' region." << endl;
      sf_toptag->process_dummy(event);

      if(debug) cout << "SR t(lep)W(had): Set handles for W tag and AK4 jets inside/outside W jet" << endl;
      handle_wtaggedjet->process(event);
      handle_ak4InExJets_W->process(event);
      if(debug) cout << "SR t(lep)W(had): Fill initial control histograms" << endl;
      hist_WTag_Begin->fill(event);
      // if(debug) cout << "SR t(lep)W(had): Apply DeepAK8 W-tagging scale factors" << endl;
      // sf_wtag->process(event); // TODO: need to check those weird scale factors ...
      // if(debug) cout << "SR t(had)W(lep): Fill control histograms after DeepAK8 scale factors" << endl;
      // hist_WTag_DeepAk8SF->fill(event);
      // if(debug) cout << "SR t(lep)W(had): Require at least one AK4 jet outside W jet as potential candidate for the b jet from leptonic top quark" << endl;
      // if(!slct_onexjet_W->passes(event)) return false; // Need to have at least one AK4 jet for pseudotop and also makes sense due to decay mode (additional AK4 jet potentially represents b jet from leptonic top quark)
      if(debug) cout << "SR t(lep)W(had): Fill final control histograms" << endl;
      hist_WTag_End->fill(event);

      if(is_tW) hist_Matching_WTag->fill(event);

      is_WTagRegion = true;

      if(b_0btag) {
        if(debug) cout << "0 b-tags" << endl;
        hist_WTag0b->fill(event);
        region = 2;
      }
      else if(b_1btag) {
        if(debug) cout << "1 b-tag" << endl;
        hist_WTag1b->fill(event);
        region = 5;
      }
      else {
        if(debug) cout << "2 or more b-tags" << endl;
        hist_WTag2b->fill(event);
        region = 8;
      }
    }

    else if(b_0toptag && b_0wtag) {
      if(debug) cout << "Event belongs to 'Veto' region." << endl;
      sf_toptag->process_dummy(event);
      // sf_wtag->process_dummy(event);

      if(debug) cout << "VR: Fill initial control histograms" << endl;
      hist_Veto_Begin->fill(event);
      if(debug) cout << "VR: Require at least one AK8 jet" << endl;
      if(!slct_oneAk8jet->passes(event)) return false; // Need to have at least one AK8 jet as W-tag substitute. For the t-tag substitute, we already required one HOTVR jet during the preselection...
      if(debug) cout << "VR: Set handles" << endl;
      handle_toptaggedjet->process(event); // leading HOTVR jet
      handle_wtaggedjet->process(event); // leading AK8 jet
      handle_ak4InExJets_top->process(event);
      handle_ak4InExJets_W->process(event);
      // if(debug) cout << "VR: Fill control histograms after AK8 / before AK4 cut" << endl;
      // hist_Veto_Ak8Cut->fill(event);
      // if(debug) cout << "VR: Require at least one AK4 jet" << endl;
      // if(!slct_oneAk4jet->passes(event)) return false; // Need at least one AK4 jet for pseudotop
      // if(debug) cout << "VR: Fill control histograms after AK4 / before b jets cut" << endl;
      // hist_Veto_Ak4Cut->fill(event);
      // if(debug) cout << "VR: Veto loose b jets" << endl; // In order to ensure that the Veto region does not have too much signal in it, veto b jets! Needed to validate the data/MC agreement of DNN inputs
      // if(!slct_bJetVeto->passes(event)) return false;
      if(debug) cout << "VR: Fill final control histograms" << endl;
      hist_Veto_End->fill(event);

      is_VetoRegion = true;

      if(b_0btag) {
        if(debug) cout << "0 b-tags" << endl;
        hist_Veto0b->fill(event);
        region = 3;
      }
      else if(b_1btag) {
        if(debug) cout << "1 b-tag" << endl;
        hist_Veto1b->fill(event);
        region = 6;
      }
      else {
        if(debug) cout << "2 or more b-tags" << endl;
        hist_Veto2b->fill(event);
        region = 9;
      }
    }

    else {
      if(debug) cout << "Event belongs to 'Else' region." << endl;

      is_ElseRegion = true;
      region = 10;
    }

    event.set(h_which_region, region);

    if(debug) cout << "Fill region histogram" << endl;
    hist_regions->fill(event);

    if(is_ElseRegion) return false;

    if(debug) cout << "Fill histograms based on number of b-tags" << endl;
    if(region <= 3) hist_0b->fill(event);
    else if(region <= 6) hist_1b->fill(event);
    else if(region <= 9) hist_2b->fill(event);

    if(debug) cout << "Set handle for leptonic pseudo top" << endl; // Events w/o AK4 jet have already been discarded at this point
    handle_pseudotop->process(event);

    if(debug) cout << "Setup handles for variables of interest (VOI)" << endl;
    handle_voi->process(event);

    // DNN-related code starts here...

    if(debug) cout << "Setting up input handles for the DNNs" << endl;
    dnn_setup->process(event);

    if(apply_DNNs) {
      if(debug) cout << "Calculate DNN outputs and fill DNN histograms" << endl;
      if(is_TopTagRegion) {
        dnn_app_ttag->process(event);
        dnn_app_wtag->process_dummy(event);

        hist_dnn_TopTag->fill(event);
        if(b_0btag) {
          hist_dnn_TopTag0b->fill(event);
        }
        else if(b_1btag) {
          hist_dnn_TopTag1b->fill(event);
        }
        else {
          hist_dnn_TopTag2b->fill(event);
        }
      }

      else if(is_WTagRegion) {
        dnn_app_wtag->process(event);
        dnn_app_ttag->process_dummy(event);

        hist_dnn_WTag->fill(event);
        if(b_0btag) {
          hist_dnn_WTag0b->fill(event);
        }
        else if(b_1btag) {
          hist_dnn_WTag1b->fill(event);
        }
        else {
          hist_dnn_WTag2b->fill(event);
        }
      }

      else if(is_VetoRegion) {
        dnn_app_ttag->process(event);
        dnn_app_wtag->process(event);

        hist_dnn_VetoTopTag->fill(event);
        hist_dnn_VetoWTag->fill(event);
        if(b_0btag) {
          hist_dnn_VetoTopTag0b->fill(event);
          hist_dnn_VetoWTag0b->fill(event);
        }
        else if(b_1btag) {
          hist_dnn_VetoTopTag1b->fill(event);
          hist_dnn_VetoWTag1b->fill(event);
        }
        else {
          hist_dnn_VetoTopTag2b->fill(event);
          hist_dnn_VetoWTag2b->fill(event);
        }
      }
    }
    else {
      if(debug) cout << "Skipping DNN application." << endl;
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
    // else { // Fill Veto region DNN plots
    //   hist_dnn_0toptag->fill(event);
    // }
    //
    // if(!_1toptag) return false; // Finally through away events without t-tagged jet


    // End of Module
    if(debug) cout << "End of module reached. Yay!" << endl;
    bool keep_event(true);
    if(empty_output_tree || !((is_TopTagRegion || is_WTagRegion) && !b_0btag)) keep_event = false;
    return keep_event;
  }


  UHH2_REGISTER_ANALYSIS_MODULE(HighPtSingleTopMainSelectionModule)
}
