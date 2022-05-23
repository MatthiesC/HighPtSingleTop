#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/MuonIds.h"
#include "UHH2/common/include/ElectronIds.h"
#include "UHH2/common/include/ObjectIdUtils.h"
#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/PrimaryLepton.h"
#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/common/include/CleaningModules.h"

#include "UHH2/HighPtSingleTop/include/AnalysisRegions.h"
#include "UHH2/HighPtSingleTop/include/Constants.h"
#include "UHH2/HighPtSingleTop/include/DNNApplication.h"
#include "UHH2/HighPtSingleTop/include/DNNInputs.h"
#include "UHH2/HighPtSingleTop/include/DNNHists.h"
#include "UHH2/HighPtSingleTop/include/LeptonicHemisphereHists.h"
#include "UHH2/HighPtSingleTop/include/MatchingHists.h"
#include "UHH2/HighPtSingleTop/include/Utils.h"

#include "UHH2/LegacyTopTagging/include/AK8Hists.h"
#include "UHH2/LegacyTopTagging/include/AndHists.h"
#include "UHH2/LegacyTopTagging/include/Constants.h"
#include "UHH2/LegacyTopTagging/include/HOTVRHists.h"
#include "UHH2/LegacyTopTagging/include/LeptonScaleFactors.h"
#include "UHH2/LegacyTopTagging/include/JetMETCorrections.h"
#include "UHH2/LegacyTopTagging/include/TopJetCorrections.h"
#include "UHH2/LegacyTopTagging/include/Utils.h"


using namespace std;
using namespace uhh2;
using namespace uhh2::btw;
using namespace uhh2::ltt;


namespace uhh2 { namespace btw {

class HighPtSingleTopMainSelectionModule: public AnalysisModule {
public:
  explicit HighPtSingleTopMainSelectionModule(Context & ctx);
  virtual bool process(Event & event) override;

private:
  const bool debug;
  unsigned long long i_event = 0;
  const Channel fChannel;
  const Event::Handle<ERegion> fHandle_Region;

  /*
  Kinematic variables:
  */

  const double muon_eta_max = 2.4;
  const double muon_lowpt_pt_min = 30.0;
  const double muon_highpt_pt_min = 55.0;

  const double electron_eta_max = 2.4;
  const double electron_lowpt_pt_min = 30.0;
  const double electron_highpt_pt_min = 120.0;

  const double met_min = 50.0;

  const double hotvr_pt_min = 200.;
  const double hotvr_eta_max = 2.5;
  const double hotvr_dr_lep_min = kDeltaRLeptonicHemisphere;

  const double ak8_pt_min = 200.;
  const double ak8_eta_max = 2.5;
  const double ak8_dr_lep_min = kDeltaRLeptonicHemisphere;

  const double ak4_pt_min = 30.;
  const double ak4_eta_max = 5.0; // keep forward jets; will define different handles for central (= b-taggable) and forward jets
  // const double ak4_eta_max = 2.5;
  const double ak4_dr_lep_min = 0.4;

  /*
  All modules etc. in chronological order as used in event loop:
  */

  bool is_tW;
  bool is_tW_dnnSig;
  bool is_tW_dnnBkg;
  bool is_tW_trueSig;
  bool is_tW_trueBkg;

  unique_ptr<AnalysisModule> prod_SingleTopGen_tWch;
  unique_ptr<AnalysisModule> classify_tW_DNN;
  unique_ptr<Selection> slct_tW_Sig_DNN;
  unique_ptr<AnalysisModule> genleveldef;

  const Muon::Selector muonIDselector_lowpt = Muon::Selector::CutBasedIdTight;
  const Muon::Selector muonISOselector_lowpt = Muon::Selector::PFIsoTight;
  const MuonId muonID_lowpt = AndId<Muon>(MuonID(muonIDselector_lowpt), MuonID(muonISOselector_lowpt), PtEtaCut(muon_lowpt_pt_min, muon_eta_max));
  const Muon::Selector muonIDselector_highpt = Muon::Selector::CutBasedIdGlobalHighPt;
  const Muon::Selector muonISOselector_highpt = Muon::Selector::TkIsoLoose;
  const MuonId muonID_highpt = AndId<Muon>(MuonID(muonIDselector_highpt), MuonID(muonISOselector_highpt), PtEtaCut(muon_highpt_pt_min, muon_eta_max));

  // can be increased from wp90 to wp80 later...
  const Electron::tag electronIDtag_lowpt = Electron::tag::mvaEleID_Fall17_iso_V2_wp90;
  const ElectronId electronID_lowpt = AndId<Electron>(ElectronTagID(electronIDtag_lowpt), PtEtaCut(electron_lowpt_pt_min, electron_eta_max));
  const Electron::tag electronIDtag_highpt = Electron::tag::mvaEleID_Fall17_iso_V2_wp90;
  const ElectronId electronID_highpt = AndId<Electron>(ElectronTagID(electronIDtag_highpt), PtEtaCut(electron_highpt_pt_min, electron_eta_max));

  unique_ptr<Selection> slct_muon_lowpt;
  unique_ptr<Selection> slct_muon_highpt;
  unique_ptr<Selection> slct_elec_lowpt;
  unique_ptr<Selection> slct_elec_highpt;

  unique_ptr<AnalysisModule> sf_muon_id_highpt;
  unique_ptr<AnalysisModule> sf_muon_id_lowpt;
  unique_ptr<AnalysisModule> sf_muon_id_dummy;
  unique_ptr<AnalysisModule> sf_muon_iso_highpt;
  unique_ptr<AnalysisModule> sf_muon_iso_lowpt;
  unique_ptr<AnalysisModule> sf_muon_iso_dummy;
  unique_ptr<AnalysisModule> sf_elec_id_highpt;
  unique_ptr<AnalysisModule> sf_elec_id_lowpt;
  unique_ptr<AnalysisModule> sf_elec_id_dummy;
  unique_ptr<AnalysisModule> sf_elec_reco;
  unique_ptr<AnalysisModule> sf_elec_reco_dummy;

  unique_ptr<AnalysisModule> primlep;
  unique_ptr<AnalysisModule> scale_variation;
  unique_ptr<AnalysisModule> ps_variation;
  unique_ptr<AnalysisModule> sf_lumi;
  unique_ptr<AnalysisModule> sf_pileup;
  unique_ptr<AnalysisModule> sf_prefire;
  unique_ptr<AnalysisModule> weight_trickery;

  unique_ptr<ltt::JetMETCorrections> jetmet_corrections_puppi;
  unique_ptr<AnalysisModule> cleaner_ak4puppi;
  unique_ptr<ltt::JetMETCorrections> jetmet_corrections_chs;

  unique_ptr<ltt::TopJetCorrections> corrections_hotvr;
  unique_ptr<AnalysisModule> cleaner_hotvr;
  unique_ptr<ltt::TopJetCorrections> corrections_ak8;
  unique_ptr<AnalysisModule> cleaner_ak8;

  unique_ptr<AnalysisModule> object_pt_sorter;
  const BTag::algo btagAlgo = BTag::algo::DEEPJET;
  const BTag::wp btagWP = BTag::wp::WP_LOOSE;
  unique_ptr<AnalysisModule> puppichs_matching;
  unique_ptr<AnalysisModule> sf_btagging;
  unique_ptr<AnalysisModule> sf_btag_njet;

  unique_ptr<Selection> slct_met;
  unique_ptr<Selection> slct_metfilter;

  unique_ptr<OrSelection> slct_1largejet;

  unique_ptr<Selection> slct_1ak4jet;

  unique_ptr<ltt::HEM2018Selection> slct_hem2018;

  unique_ptr<ltt::AndHists> hist_presel;

  unique_ptr<Selection> slct_trigger_highpt;
  unique_ptr<Selection> slct_trigger_lowpt;

  unique_ptr<AnalysisModule> sf_muon_trigger_highpt;
  unique_ptr<AnalysisModule> sf_muon_trigger_lowpt;
  unique_ptr<AnalysisModule> sf_muon_trigger_dummy;

  unique_ptr<ltt::AndHists> hist_trigger;

  unique_ptr<AnalysisModule> fatjet_tagger;
  unique_ptr<AnalysisModule> setter_analysis_regions;
  unique_ptr<AnalysisModule> classify_tW_TrueDecay;
  unique_ptr<Selection> slct_tW_Sig_TrueDecay;
  unique_ptr<AnalysisModule> setter_toptag;
  unique_ptr<AnalysisModule> setter_wtag;
  unique_ptr<AnalysisModule> reco_leptonichemisphere;

  unique_ptr<AnalysisModule> sf_toppt;
  unique_ptr<ltt::AndHists> hist_topptSF;

  unique_ptr<AnalysisModule> sf_vjets;
  unique_ptr<ltt::AndHists> hist_vjetsSF;

  unique_ptr<DNNInputs> dnn_inputs;
  map<ERegion_heavyTags, unique_ptr<DNNApplication>> dnn_application;

  unique_ptr<AnalysisRegionHists> hist_analysis_regions;
  map<ERegion, unique_ptr<ltt::AndHists>> hist_region;

  // unique_ptr<Selection> slct_top_mass;
  // map<ERegion, unique_ptr<ltt::AndHists>> hist_region_withMTopCut;

  Event::Handle<float> fHandle_weight;
};


HighPtSingleTopMainSelectionModule::HighPtSingleTopMainSelectionModule(Context & ctx):
  debug(string2bool(ctx.get("debug"))),
  fChannel(extract_channel(ctx)),
  fHandle_Region(ctx.get_handle<ERegion>(kHandleName_Region))
  // fHandle_Region_heavyTags(ctx.get_handle<ERegion_heavyTags>(kHandleName_Region_heavyTags))
{
  ctx.undeclare_all_event_output(); // throw away all output trees (jet collections etc.) which are not needed in further steps of the analysis
  // empty_output_tree = string2bool(ctx.get("EmptyOutputTree")); // handy to not have output trees for systematics files, reduces root file size
  // is_QCDsideband = string2bool(ctx.get("QCD_sideband"));

  unsigned int i_hist(0);

  const string dataset_version = ctx.get("dataset_version");
  is_tW = dataset_version.find("ST_tW") == 0;
  is_tW_dnnSig = is_tW && dataset_version.find("_dnnSig") != string::npos;
  is_tW_dnnBkg = is_tW && dataset_version.find("_dnnBkg") != string::npos;
  is_tW_trueSig = is_tW && dataset_version.find("_trueSig") != string::npos;
  is_tW_trueBkg = is_tW && dataset_version.find("_trueBkg") != string::npos;

  prod_SingleTopGen_tWch.reset(new ltt::SingleTopGen_tWchProducer(ctx, kHandleName_SingleTopGen_tWch));
  classify_tW_DNN.reset(new btw::TWClassification_DNN(ctx));
  slct_tW_Sig_DNN.reset(new btw::TWSignalSelection_DNN(ctx));
  genleveldef.reset(new btw::GenLevelDefinitions(ctx));

  slct_muon_lowpt.reset(new NMuonSelection(1, 1, muonID_lowpt));
  slct_muon_highpt.reset(new NMuonSelection(1, 1, muonID_highpt));
  slct_elec_lowpt.reset(new NElectronSelection(1, 1, electronID_lowpt));
  slct_elec_highpt.reset(new NElectronSelection(1, 1, electronID_highpt));

  sf_muon_id_highpt.reset(new ltt::MuonIdScaleFactors(ctx, muonIDselector_highpt));
  sf_muon_id_lowpt.reset(new ltt::MuonIdScaleFactors(ctx, muonIDselector_lowpt));
  sf_muon_id_dummy.reset(new ltt::MuonIdScaleFactors(ctx, boost::none, boost::none, boost::none, boost::none, true));
  sf_muon_iso_highpt.reset(new ltt::MuonIsoScaleFactors(ctx, muonISOselector_highpt, muonIDselector_highpt));
  sf_muon_iso_lowpt.reset(new ltt::MuonIsoScaleFactors(ctx, muonISOselector_lowpt, muonIDselector_lowpt));
  sf_muon_iso_dummy.reset(new ltt::MuonIsoScaleFactors(ctx, boost::none, boost::none, boost::none, boost::none, boost::none, true));
  sf_elec_id_highpt.reset(new ltt::ElectronIdScaleFactors(ctx, electronIDtag_highpt));
  sf_elec_id_lowpt.reset(new ltt::ElectronIdScaleFactors(ctx, electronIDtag_lowpt));
  sf_elec_id_dummy.reset(new ltt::ElectronIdScaleFactors(ctx, boost::none, boost::none, boost::none, boost::none, true));
  sf_elec_reco.reset(new ltt::ElectronRecoScaleFactors(ctx));
  sf_elec_reco_dummy.reset(new ltt::ElectronRecoScaleFactors(ctx, boost::none, boost::none, boost::none, boost::none, true));

  primlep.reset(new PrimaryLepton(ctx));
  scale_variation.reset(new MCScaleVariation(ctx));
  ps_variation.reset(new ltt::PartonShowerVariation(ctx));
  sf_lumi.reset(new MCLumiWeight(ctx));
  sf_pileup.reset(new MCPileupReweight(ctx, ctx.get("SystDirection_Pileup", "nominal")));
  sf_prefire.reset(new ltt::PrefiringWeights(ctx));
  weight_trickery.reset(new ltt::WeightTrickery(ctx, kHandleName_SingleTopGen_tWch, false));

  const JetId jetID = AndId<Jet>(PtEtaCut(ak4_pt_min, ak4_eta_max), ltt::NoLeptonInJet("all", ak4_dr_lep_min)); // JetPFID is already part of JetMETCorrections

  const string met_name = ctx.get("METName");
  const bool puppi_met = (met_name == kCollectionName_METPUPPI);
  jetmet_corrections_puppi.reset(new ltt::JetMETCorrections(boost::none, boost::none, puppi_met ? boost::none : (boost::optional<std::string>)kCollectionName_METPUPPI));
  jetmet_corrections_puppi->init(ctx);
  cleaner_ak4puppi.reset(new JetCleaner(ctx, jetID));
  jetmet_corrections_chs.reset(new ltt::JetMETCorrections(kCollectionName_AK4CHS, boost::none, puppi_met ? (boost::optional<std::string>)kCollectionName_METCHS : boost::none));
  jetmet_corrections_chs->init(ctx);

  const TopJetId hotvrID = AndId<TopJet>(JetPFID(JetPFID::WP_TIGHT_PUPPI), PtEtaCut(hotvr_pt_min, hotvr_eta_max), ltt::NoLeptonInJet("all", hotvr_dr_lep_min));
  const TopJetId ak8ID = AndId<TopJet>(JetPFID(JetPFID::WP_TIGHT_PUPPI), PtEtaCut(ak8_pt_min, ak8_eta_max), ltt::NoLeptonInJet("all", ak8_dr_lep_min));

  corrections_hotvr.reset(new ltt::TopJetCorrections());
  corrections_hotvr->switch_topjet_corrections(false);
  corrections_hotvr->switch_subjet_corrections(true);
  corrections_hotvr->switch_rebuilding_topjets_from_subjets(true);
  corrections_hotvr->init(ctx);
  cleaner_hotvr.reset(new TopJetCleaner(ctx, hotvrID));
  corrections_ak8.reset(new ltt::TopJetCorrections(kCollectionName_AK8_rec, kCollectionName_AK8_gen));
  corrections_ak8->init(ctx);
  cleaner_ak8.reset(new TopJetCleaner(ctx, ak8ID, kCollectionName_AK8_rec));

  object_pt_sorter.reset(new ltt::ObjectPtSorter(ctx));
  puppichs_matching.reset(new ltt::MatchPuppiToCHSAndSetBTagHandles(ctx, btagAlgo, btagWP));
  sf_btagging.reset(new MCBTagDiscriminantReweighting(ctx, btagAlgo, kHandleName_pairedCHSjets));
  sf_btag_njet.reset(new ltt::BTagNJetScaleFactor(ctx));

  slct_met.reset(new ltt::METSelection(ctx, met_min));
  slct_metfilter.reset(new ltt::METFilterSelection(ctx));

  slct_1largejet.reset(new OrSelection());
  slct_1largejet->add(make_shared<NTopJetSelection>(1, -1));
  slct_1largejet->add(make_shared<NTopJetSelection>(1, -1, boost::none, ctx.get_handle<vector<TopJet>>(kCollectionName_AK8_rec)));

  slct_1ak4jet.reset(new NJetSelection(1, -1, boost::none, ctx.get_handle<vector<Jet>>(kHandleName_pairedPUPPIjets)));

  slct_hem2018.reset(new ltt::HEM2018Selection(ctx));

  hist_presel.reset(new ltt::AndHists(ctx, to_string(i_hist++)+"_Presel", true, true));

  slct_trigger_highpt.reset(new BTWTriggerSelection(ctx, false));
  slct_trigger_lowpt.reset(new BTWTriggerSelection(ctx, true));

  sf_muon_trigger_highpt.reset(new ltt::MuonTriggerScaleFactors(ctx, true));
  sf_muon_trigger_lowpt.reset(new ltt::MuonTriggerScaleFactors(ctx, false));
  sf_muon_trigger_dummy.reset(new ltt::MuonTriggerScaleFactors(ctx, boost::none, boost::none, boost::none, boost::none, boost::none, true));

  fatjet_tagger.reset(new FatJetTagger(ctx, WTag::algo::ParticleNet, WTag::wp::WP_DUMMY));
  setter_analysis_regions.reset(new AnalysisRegionSetter(ctx, string2bool(ctx.get("bVetoLike"))));
  classify_tW_TrueDecay.reset(new btw::TWClassification_TrueDecay(ctx));
  slct_tW_Sig_TrueDecay.reset(new btw::TWSignalSelection_TrueDecay(ctx));
  setter_toptag.reset(new TopTagSetter(ctx));
  setter_wtag.reset(new WTagSetter(ctx));
  reco_leptonichemisphere.reset(new LeptonicHemisphereReco(ctx));

  hist_trigger.reset(new ltt::AndHists(ctx, to_string(i_hist++)+"_Trigger", true, true));
  hist_trigger->add_hist(new btw::LeptonicHemisphereHists(ctx, hist_trigger->dirname()+"_LeptHemi"));

  sf_toppt.reset(new ltt::TopPtReweighting(ctx, string2bool(ctx.get("apply_TopPtReweighting"))));
  hist_topptSF.reset(new ltt::AndHists(ctx, to_string(i_hist++)+"_TopPtSF", true, true));
  hist_topptSF->add_hist(new btw::LeptonicHemisphereHists(ctx, hist_topptSF->dirname()+"_LeptHemi"));

  sf_vjets.reset(new ltt::VJetsReweighting(ctx));
  hist_vjetsSF.reset(new ltt::AndHists(ctx, to_string(i_hist++)+"_VjetsSF", true, true));
  hist_vjetsSF->add_hist(new btw::LeptonicHemisphereHists(ctx, hist_vjetsSF->dirname()+"_LeptHemi"));

  dnn_inputs.reset(new btw::DNNInputs(ctx));
  for(const ERegion_heavyTags & region_heavyTags : kRelevantRegions_heavyTags) {
    dnn_application[region_heavyTags].reset(new btw::DNNApplication(ctx, region_heavyTags));
  }

  hist_analysis_regions.reset(new AnalysisRegionHists(ctx, to_string(i_hist)+"_AnalysisRegions"));
  for(const ERegion & region : kRelevantRegions) {
    hist_region[region].reset(new ltt::AndHists(ctx, to_string(i_hist)+"_Region_"+kRegions.at(region).name, false, true)); // manually add AK8 and HOTVR hists to use argument replacing leading jet with tagged jet
    hist_region[region]->add_hist(new ltt::HOTVRHists(ctx, hist_region[region]->dirname()+"_HOTVR", "", "", kRegions.at(region).region_heavyTags==ERegion_heavyTags::_1t ? kHandleName_TheTopJet : ""));
    hist_region[region]->add_hist(new ltt::AK8Hists(ctx, hist_region[region]->dirname()+"_AK8", kCollectionName_AK8_rec, kCollectionName_AK8_gen, kRegions.at(region).region_heavyTags==ERegion_heavyTags::_0t1W ? kHandleName_TheWJet : ""));
    hist_region[region]->add_hist(new btw::LeptonicHemisphereHists(ctx, hist_region[region]->dirname()+"_LeptHemi"));
    hist_region[region]->add_hist(new btw::MatchingHists(ctx, hist_region[region]->dirname()+"_Matching", is_tW));
    hist_region[region]->add_hist(new btw::DNNHists(ctx, hist_region[region]->dirname()+"_DNN", dnn_inputs, dnn_application.at(kRegions.at(region).region_heavyTags)));
  }
  i_hist++;

  // slct_top_mass.reset(new LeptonicTopQuarkMassSelection(ctx, 100., 230.)); // only acts on _0t1W regions
  // for(const ERegion & region : kRelevantRegions) {
  //   hist_region_withMTopCut[region].reset(new ltt::AndHists(ctx, to_string(i_hist)+"_Region_"+kRegions.at(region).name, false, true)); // manually add AK8 and HOTVR hists to use argument replacing leading jet with tagged jet
  //   hist_region_withMTopCut[region]->add_hist(new ltt::HOTVRHists(ctx, hist_region_withMTopCut[region]->dirname()+"_HOTVR", "", "", kRegions.at(region).region_heavyTags==ERegion_heavyTags::_1t ? kHandleName_TheTopJet : ""));
  //   hist_region_withMTopCut[region]->add_hist(new ltt::AK8Hists(ctx, hist_region_withMTopCut[region]->dirname()+"_AK8", kCollectionName_AK8_rec, kCollectionName_AK8_gen, kRegions.at(region).region_heavyTags==ERegion_heavyTags::_0t1W ? kHandleName_TheWJet : ""));
  //   hist_region_withMTopCut[region]->add_hist(new btw::LeptonicHemisphereHists(ctx, hist_region_withMTopCut[region]->dirname()+"_LeptHemi"));
  //   hist_region_withMTopCut[region]->add_hist(new btw::MatchingHists(ctx, hist_region_withMTopCut[region]->dirname()+"_Matching", is_tW));
  // }
  // i_hist++;

  fHandle_weight = ctx.declare_event_output<float>("weight");
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
    cout << "i_event = " << to_string(i_event++) << endl;
    cout << endl;
  }

  if(debug) cout << "Initial stuff after preselection" << endl;
  if(fChannel == Channel::isMuo) {
    if(event.muons->size() != 1) return false;
  }
  else if(fChannel == Channel::isEle) {
    if(event.electrons->size() != 1) return false;
  }
  if(is_tW) {
    prod_SingleTopGen_tWch->process(event);
    genleveldef->process(event);
  }
  classify_tW_DNN->process(event);
  const bool passes_tW_Sig_DNN = slct_tW_Sig_DNN->passes(event);
  if(is_tW_dnnSig && !passes_tW_Sig_DNN) return false;
  else if(is_tW_dnnBkg && passes_tW_Sig_DNN) return false;

  bool lowpt(false);
  if(fChannel == Channel::isMuo) {
    const Muon *muon = &event.muons->at(0);
    if(muon->pt() < muon_highpt_pt_min) {
      if(!slct_muon_lowpt->passes(event)) return false;
      sf_muon_id_lowpt->process(event);
      sf_muon_iso_lowpt->process(event);
      lowpt = true;
    }
    else {
      if(!slct_muon_highpt->passes(event)) return false;
      sf_muon_id_highpt->process(event);
      sf_muon_iso_highpt->process(event);
      lowpt = false;
    }
    sf_elec_id_dummy->process(event);
    sf_elec_reco_dummy->process(event);
  }
  else if(fChannel == Channel::isEle) {
    const Electron *electron = &event.electrons->at(0);
    const float abseta_sc = fabs(electron->supercluster_eta());
    if(abseta_sc > 1.4442 && abseta_sc < 1.566) return false; // gap between ECAL barrel and endcap
    if(electron->pt() < electron_highpt_pt_min) {
      if(!slct_elec_lowpt->passes(event)) return false;
      sf_elec_id_lowpt->process(event);
      lowpt = true;
    }
    else {
      if(!slct_elec_highpt->passes(event)) return false;
      sf_elec_id_highpt->process(event);
      lowpt = false;
    }
    sf_elec_reco->process(event);
    sf_muon_id_dummy->process(event);
    sf_muon_iso_dummy->process(event);
  }
  primlep->process(event);
  scale_variation->process(event);
  ps_variation->process(event);
  sf_lumi->process(event);
  sf_pileup->process(event);
  sf_prefire->process(event);
  weight_trickery->process(event);

  if(debug) cout << "JetMET corrections, pt sorting, and PUPPI-CHS matching" << endl;
  jetmet_corrections_puppi->process(event);
  cleaner_ak4puppi->process(event);
  jetmet_corrections_chs->process(event);

  corrections_hotvr->process(event); // needs to come already here because of subsequent object pt sorter
  cleaner_hotvr->process(event);
  corrections_ak8->process(event);
  cleaner_ak8->process(event);

  object_pt_sorter->process(event); // needs to come after jet corrections but before PUPPI-CHS matching
  puppichs_matching->process(event);
  sf_btagging->process(event);
  sf_btag_njet->process(event);
  // hist_jetmet_corrections->fill(event);

  if(debug) cout << "MET selection and MET filters" << endl;
  if(!slct_met->passes(event)) return false;
  if(!slct_metfilter->passes(event)) return false;
  // hist_metcut->fill(event);

  if(debug) cout << "At least one large-R jet (HOTVR or AK8)" << endl;
  if(!slct_1largejet->passes(event)) return false;
  // hist_1largejet->fill(event);

  if(debug) cout << "Select at least one AK4 jet" << endl;
  if(!slct_1ak4jet->passes(event)) return false; // Require at least one AK4 jet for computational reasons (dR(lepton, jet) etc.); this rejects only \mathcal{O}(0.01\%) of events in real data (tested in pre-UL 2017 muo, RunB)
  // hist_1ak4jet->fill(event);

  if(debug) cout << "2018 HEM15/16 issue selection" << endl;
  if(slct_hem2018->passes(event)) {
    if(event.isRealData) return false;
    else event.weight *= (1. - slct_hem2018->GetAffectedLumiFraction());
  }
  // hist_hem2018->fill(event);
  hist_presel->fill(event);

  if(debug) cout << "Trigger selection" << endl;
  bool passes_trigger(false);
  if(lowpt) passes_trigger = slct_trigger_lowpt->passes(event);
  else passes_trigger = slct_trigger_highpt->passes(event);
  if(!passes_trigger) return false;

  if(fChannel == Channel::isMuo) {
    if(lowpt) sf_muon_trigger_lowpt->process(event);
    else sf_muon_trigger_highpt->process(event);
    // sf_elec_trigger_dummy->process(event);
  }
  else if(fChannel == Channel::isEle) {
    // sf_elec_trigger->process(event); // need to differentiate between 2017 Run B and Run C-F
    sf_muon_trigger_dummy->process(event);
  }
  // hist_trigger->fill(event);

  if(debug) cout << "Set fat jet and leptonic W/top handles and define analysis regions" << endl;
  fatjet_tagger->process(event);
  setter_analysis_regions->process(event);
  classify_tW_TrueDecay->process(event); // requires analysis regions
  const bool passes_tW_Sig_TrueDecay = slct_tW_Sig_TrueDecay->passes(event);
  if(is_tW_trueSig && !passes_tW_Sig_TrueDecay) return false;
  else if(is_tW_trueBkg && passes_tW_Sig_TrueDecay) return false;
  setter_toptag->process(event);
  setter_wtag->process(event);
  reco_leptonichemisphere->process(event); // requires analysis regions
  hist_trigger->fill(event);

  if(debug) cout << "Apply top-pt reweighting for ttbar events" << endl;
  sf_toppt->process(event);
  hist_topptSF->fill(event);

  if(debug) cout << "Apply (N)NLO QCD/EWK corrections to V+jets samples" << endl;
  sf_vjets->process(event);
  hist_vjetsSF->fill(event);

  if(debug) cout << "Histograms in individual analysis regions" << endl;
  hist_analysis_regions->fill(event);
  const ERegion region = event.get(fHandle_Region);
  // const ERegion_heavyTags region_heavyTags = event.get(fHandle_Region_heavyTags);
  if(!(kRelevantRegions.find(region) != kRelevantRegions.end())) return false;
  dnn_inputs->process(event);
  for(const ERegion_heavyTags & region_heavyTags : kRelevantRegions_heavyTags) {
    dnn_application[region_heavyTags]->process(event);
  }
  hist_region[region]->fill(event);
  // if(kRelevantRegions.find(region) != kRelevantRegions.end()) hist_region[region]->fill(event);
  // if(!slct_top_mass->passes(event)) return false;
  // if(kRelevantRegions.find(region) != kRelevantRegions.end()) hist_region_withMTopCut[region]->fill(event);

  if(debug) cout << "End of MainSelectionModule. Event passed" << endl;
  event.set(fHandle_weight, event.weight);
  return true;
}


UHH2_REGISTER_ANALYSIS_MODULE(HighPtSingleTopMainSelectionModule)

}}
