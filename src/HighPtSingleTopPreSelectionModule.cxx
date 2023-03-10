#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/AdditionalSelections.h"
#include "UHH2/common/include/LumiSelection.h"
#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/ObjectIdUtils.h"
#include "UHH2/common/include/MuonIds.h"
#include "UHH2/common/include/ElectronIds.h"
#include "UHH2/common/include/PrimaryLepton.h"
#include "UHH2/common/include/Utils.h"

#include "UHH2/HighPtSingleTop/include/Constants.h"
#include "UHH2/HighPtSingleTop/include/Utils.h"

#include "UHH2/LegacyTopTagging/include/AndHists.h"
#include "UHH2/LegacyTopTagging/include/Constants.h"
#include "UHH2/LegacyTopTagging/include/RochesterCorrections.h"
#include "UHH2/LegacyTopTagging/include/LeptonScaleFactors.h"
#include "UHH2/LegacyTopTagging/include/JetMETCorrections.h"
#include "UHH2/LegacyTopTagging/include/TopJetCorrections.h"
#include "UHH2/LegacyTopTagging/include/Utils.h"


using namespace std;
using namespace uhh2;
using namespace uhh2::btw;
using namespace uhh2::ltt;


namespace uhh2 { namespace btw {


//------------------//
// CLASS DEFINITION //
//------------------//

class HighPtSingleTopPreSelectionModule: public AnalysisModule {
public:
  explicit HighPtSingleTopPreSelectionModule(Context & ctx);
  virtual bool process(Event & event) override;

private:
  const bool debug;
  unsigned long long i_event = 0;
  const bool is_mc;
  const bool is_singlemuon;
  const bool is_tW;
  const bool is_tW_incl;
  const bool is_extra_syst;
  enum class JECVariation {
    nominal,
    jes_up,
    jes_down,
    jer_up,
    jer_down,
  };

  map<JECVariation, string> kJECVariationToString;

  /*
  Kinematic variables:
  */

  const double muon_eta_max = 2.4;
  const double muon_veto_pt_min = 30.0;
  const double muon_lowpt_pt_min = 30.0;
  const double muon_highpt_pt_min = 55.0;

  const double electron_eta_max = 2.4;
  const double electron_veto_pt_min = 30.0;
  const double electron_lowpt_pt_min = 30.0;
  const double electron_highpt_pt_min = 120.0;

  const double met_min = 50.0;

  const double hotvr_pt_min = 200.;
  const double hotvr_eta_max = 2.5;
  const double hotvr_dr_lep_min = kDeltaRLeptonicHemisphere;

  const double ak8_pt_min = 200.;
  const double ak8_eta_max = 2.5;
  const double ak8_dr_lep_min = kDeltaRLeptonicHemisphere;

  /*
  All modules etc. in chronological order as used in event loop:
  */

  unique_ptr<AnalysisModule> prod_SingleTopGen_tWch;
  uhh2::Event::Handle<ltt::SingleTopGen_tWch> fHandle_GENtW;
  unique_ptr<AnalysisModule> genleveldef;
  unique_ptr<Selection> slct_matrix;

  unique_ptr<Selection> slct_lumi;
  unique_ptr<AnalysisModule> sf_lumi;
  unique_ptr<ltt::AndHists> hist_lumi;

  unique_ptr<AnalysisModule> clnr_pv;
  unique_ptr<Selection> slct_pv;
  unique_ptr<AnalysisModule> sf_pileup;
  unique_ptr<ltt::AndHists> hist_pv;

  unique_ptr<AnalysisModule> rochester;
  unique_ptr<ltt::AndHists> hist_rochester;

  // isolation requirements are not used here; can be added later...
  const MuonId muonID_veto = AndId<Muon>(MuonID(Muon::Selector::CutBasedIdLoose), PtEtaCut(muon_veto_pt_min, muon_eta_max));
  const ElectronId electronID_veto = AndId<Electron>(ElectronTagID(Electron::tag::mvaEleID_Fall17_noIso_V2_wpLoose), PtEtaCut(electron_veto_pt_min, electron_eta_max));

  const Muon::Selector muonIDselector_lowpt = Muon::Selector::CutBasedIdTight;
  const MuonId muonID_lowpt = AndId<Muon>(MuonID(muonIDselector_lowpt), PtEtaCut(muon_lowpt_pt_min, muon_eta_max));
  const Muon::Selector muonIDselector_highpt = Muon::Selector::CutBasedIdGlobalHighPt;
  const MuonId muonID_highpt = AndId<Muon>(MuonID(muonIDselector_highpt), PtEtaCut(muon_highpt_pt_min, muon_eta_max));

  // can be increased from wp90 to wp80 later...
  const Electron::tag electronIDtag_lowpt = Electron::tag::mvaEleID_Fall17_noIso_V2_wp90;
  const ElectronId electronID_lowpt = AndId<Electron>(ElectronTagID(electronIDtag_lowpt), PtEtaCut(electron_lowpt_pt_min, electron_eta_max));
  const Electron::tag electronIDtag_highpt = Electron::tag::mvaEleID_Fall17_noIso_V2_wp90;
  const ElectronId electronID_highpt = AndId<Electron>(ElectronTagID(electronIDtag_highpt), PtEtaCut(electron_highpt_pt_min, electron_eta_max));

  unique_ptr<AnalysisModule> sf_muon_id_highpt;
  unique_ptr<AnalysisModule> sf_muon_id_lowpt;
  unique_ptr<AnalysisModule> sf_muon_id_dummy;
  unique_ptr<AnalysisModule> sf_elec_id_highpt;
  unique_ptr<AnalysisModule> sf_elec_id_lowpt;
  unique_ptr<AnalysisModule> sf_elec_id_dummy;
  unique_ptr<AnalysisModule> sf_elec_reco;
  unique_ptr<AnalysisModule> sf_elec_reco_dummy;

  unique_ptr<AnalysisModule> primlep;
  unique_ptr<ltt::AndHists> hist_1lepton;

  Event::Handle<vector<Jet>> h_ak4chs_orig;
  Event::Handle<MET> h_metchs_orig;
  Event::Handle<vector<Jet>> h_ak4puppi_orig;
  Event::Handle<MET> h_metpuppi_orig;
  Event::Handle<vector<TopJet>> h_ak8puppi_orig;
  Event::Handle<vector<TopJet>> h_hotvrpuppi_orig;

  map<JECVariation, Event::Handle<vector<Jet>>> h_ak4puppi_jecvariations;
  map<JECVariation, Event::Handle<MET>> h_metpuppi_jecvariations;
  map<JECVariation, unique_ptr<ltt::JetMETCorrections>> jetmet_corrections_puppi;
  map<JECVariation, Event::Handle<vector<Jet>>> h_ak4chs_jecvariations;
  map<JECVariation, Event::Handle<MET>> h_metchs_jecvariations;
  map<JECVariation, unique_ptr<ltt::JetMETCorrections>> jetmet_corrections_chs;
  unique_ptr<ltt::AndHists> hist_jetmet_corrections;

  unique_ptr<OrSelection> slct_met;
  unique_ptr<ltt::AndHists> hist_metcut;

  map<JECVariation, Event::Handle<vector<TopJet>>> h_ak8puppi_jecvariations;
  map<JECVariation, unique_ptr<ltt::TopJetCorrections>> corrections_ak8;
  map<JECVariation, unique_ptr<TopJetCleaner>> cleaner_ak8;
  map<JECVariation, Event::Handle<vector<TopJet>>> h_hotvrpuppi_jecvariations;
  map<JECVariation, unique_ptr<ltt::TopJetCorrections>> corrections_hotvr;
  map<JECVariation, unique_ptr<TopJetCleaner>> cleaner_hotvr;
  unique_ptr<ltt::AndHists> hist_largejet_correction;

  unique_ptr<OrSelection> slct_1largejet;
  unique_ptr<ltt::AndHists> hist_1largejet;

  Event::Handle<bool> fHandle_bool_reco_sel;
  Event::Handle<bool> fHandle_bool_matrix_sel;
  Event::Handle<bool> fHandle_bool_parton_sel;
  Event::Handle<bool> fHandle_bool_particle_sel;
};


//-------------//
// CONSTRUCTOR //
//-------------//

HighPtSingleTopPreSelectionModule::HighPtSingleTopPreSelectionModule(Context & ctx):
  debug(string2bool(ctx.get("debug"))),
  is_mc(ctx.get("dataset_type") == "MC"),
  is_singlemuon(!is_mc && ctx.get("dataset_version").find("SingleMuon") != string::npos),
  is_tW(ctx.get("dataset_version").find("ST_tW") == 0),
  is_tW_incl(is_tW && ctx.get("dataset_version").find("inclusiveDecays") != string::npos),
  is_extra_syst(string2bool(ctx.get("extra_syst")))
{
  unsigned int i_hist(0);

  kJECVariationToString[JECVariation::nominal] = "nominal";
  if(is_mc) {
    kJECVariationToString[JECVariation::jes_up] = "jes_up";
    kJECVariationToString[JECVariation::jes_down] = "jes_down";
    kJECVariationToString[JECVariation::jer_up] = "jer_up";
    kJECVariationToString[JECVariation::jer_down] = "jer_down";
  }

  prod_SingleTopGen_tWch.reset(new ltt::SingleTopGen_tWchProducer(ctx, kHandleName_SingleTopGen_tWch));
  fHandle_GENtW = ctx.get_handle<ltt::SingleTopGen_tWch>(kHandleName_SingleTopGen_tWch);
  genleveldef.reset(new btw::GenLevelDefinitions(ctx));
  slct_matrix.reset(new btw::MatrixLevelSelection(ctx, "presel", ltt::Channel::notValid));

  slct_lumi.reset(new LumiSelection(ctx));
  sf_lumi.reset(new MCLumiWeight(ctx));
  hist_lumi.reset(new ltt::AndHists(ctx, to_string(i_hist++)+"_LumiSel"));

  const PrimaryVertexId pvID = StandardPrimaryVertexId();
  clnr_pv.reset(new PrimaryVertexCleaner(pvID));
  slct_pv.reset(new NPVSelection(1, -1));
  sf_pileup.reset(new MCPileupReweight(ctx, ctx.get("SystDirection_Pileup", "nominal")));
  hist_pv.reset(new ltt::AndHists(ctx, to_string(i_hist++)+"_PVSel"));

  rochester.reset(new ltt::RochesterCorrections(ctx));
  hist_rochester.reset(new ltt::AndHists(ctx, to_string(i_hist++)+"_Rochester"));

  sf_muon_id_highpt.reset(new ltt::MuonIdScaleFactors(ctx, muonIDselector_highpt));
  sf_muon_id_lowpt.reset(new ltt::MuonIdScaleFactors(ctx, muonIDselector_lowpt));
  sf_muon_id_dummy.reset(new ltt::MuonIdScaleFactors(ctx, boost::none, boost::none, boost::none, boost::none, true));
  sf_elec_id_highpt.reset(new ltt::ElectronIdScaleFactors(ctx, electronIDtag_highpt));
  sf_elec_id_lowpt.reset(new ltt::ElectronIdScaleFactors(ctx, electronIDtag_lowpt));
  sf_elec_id_dummy.reset(new ltt::ElectronIdScaleFactors(ctx, boost::none, boost::none, boost::none, boost::none, true));
  sf_elec_reco.reset(new ltt::ElectronRecoScaleFactors(ctx));
  sf_elec_reco_dummy.reset(new ltt::ElectronRecoScaleFactors(ctx, boost::none, boost::none, boost::none, boost::none, true));

  primlep.reset(new PrimaryLepton(ctx));
  hist_1lepton.reset(new ltt::AndHists(ctx, to_string(i_hist++)+"_LeptonSel"));

  h_ak4puppi_orig = ctx.get_handle<vector<Jet>>("jets");
  h_metpuppi_orig = ctx.get_handle<MET>("met");
  h_ak4chs_orig = ctx.get_handle<vector<Jet>>(kCollectionName_AK4CHS);
  h_metchs_orig = ctx.get_handle<MET>(kCollectionName_METCHS);
  h_ak8puppi_orig = ctx.get_handle<vector<TopJet>>(kCollectionName_AK8_rec);
  h_hotvrpuppi_orig = ctx.get_handle<vector<TopJet>>("topjets");

  slct_met.reset(new OrSelection());

  const TopJetId hotvrID = AndId<TopJet>(PtEtaCut(hotvr_pt_min, hotvr_eta_max), ltt::NoLeptonInJet("all", hotvr_dr_lep_min));
  const TopJetId ak8ID = AndId<TopJet>(PtEtaCut(ak8_pt_min, ak8_eta_max), ltt::NoLeptonInJet("all", ak8_dr_lep_min));

  slct_1largejet.reset(new OrSelection());

  string pipe_rec_name;
  string pipe_gen_name;
  string pipe_met_name;
  for(const auto & jecvar : kJECVariationToString) {
    switch(jecvar.first) {
      case JECVariation::nominal :
      ctx.set("jecsmear_direction", "nominal");
      ctx.set("jersmear_direction", "nominal");
      break;
      case JECVariation::jes_up :
      ctx.set("jecsmear_direction", "up");
      ctx.set("jersmear_direction", "nominal");
      break;
      case JECVariation::jes_down :
      ctx.set("jecsmear_direction", "down");
      ctx.set("jersmear_direction", "nominal");
      break;
      case JECVariation::jer_up :
      ctx.set("jecsmear_direction", "nominal");
      ctx.set("jersmear_direction", "up");
      break;
      case JECVariation::jer_down :
      ctx.set("jecsmear_direction", "nominal");
      ctx.set("jersmear_direction", "down");
      break;
    }

     // Those pipe names need to include "puppi"/"chs", so JetMETCorrections/TopJetCorrections will find correct PU subtraction
    pipe_rec_name = "ak4puppi_"+jecvar.second;
    pipe_gen_name = "genjets";
    pipe_met_name = "metpuppi_"+jecvar.second;
    h_ak4puppi_jecvariations[jecvar.first] = ctx.get_handle<vector<Jet>>(pipe_rec_name);
    h_metpuppi_jecvariations[jecvar.first] = ctx.get_handle<MET>(pipe_met_name);
    jetmet_corrections_puppi[jecvar.first].reset(new ltt::JetMETCorrections(pipe_rec_name, pipe_gen_name, pipe_met_name));
    jetmet_corrections_puppi[jecvar.first]->init(ctx);
    slct_met->add(make_shared<ltt::METSelection>(ctx, met_min, boost::none, pipe_met_name));

    pipe_rec_name = "ak4chs_"+jecvar.second;
    pipe_gen_name = "genjets";
    pipe_met_name = "metchs_"+jecvar.second;
    h_ak4chs_jecvariations[jecvar.first] = ctx.get_handle<vector<Jet>>(pipe_rec_name);
    h_metchs_jecvariations[jecvar.first] = ctx.get_handle<MET>(pipe_met_name);
    jetmet_corrections_chs[jecvar.first].reset(new ltt::JetMETCorrections(pipe_rec_name, pipe_gen_name, pipe_met_name));
    jetmet_corrections_chs[jecvar.first]->init(ctx);
    slct_met->add(make_shared<ltt::METSelection>(ctx, met_min, boost::none, pipe_met_name));

    pipe_rec_name = "ak8puppi_"+jecvar.second;
    pipe_gen_name = kCollectionName_AK8_gen;
    h_ak8puppi_jecvariations[jecvar.first] = ctx.get_handle<vector<TopJet>>(pipe_rec_name);
    corrections_ak8[jecvar.first].reset(new ltt::TopJetCorrections(pipe_rec_name, pipe_gen_name));
    corrections_ak8[jecvar.first]->init(ctx);
    cleaner_ak8[jecvar.first].reset(new TopJetCleaner(ctx, ak8ID, pipe_rec_name));
    slct_1largejet->add(make_shared<NTopJetSelection>(1, -1, boost::none, h_ak8puppi_jecvariations[jecvar.first]));

    pipe_rec_name = "hotvrpuppi_"+jecvar.second;
    pipe_gen_name = "gentopjets";
    h_hotvrpuppi_jecvariations[jecvar.first] = ctx.get_handle<vector<TopJet>>(pipe_rec_name);
    corrections_hotvr[jecvar.first].reset(new ltt::TopJetCorrections(pipe_rec_name, pipe_gen_name));
    corrections_hotvr[jecvar.first]->switch_topjet_corrections(false);
    corrections_hotvr[jecvar.first]->switch_subjet_corrections(true);
    corrections_hotvr[jecvar.first]->switch_rebuilding_topjets_from_subjets(true);
    corrections_hotvr[jecvar.first]->init(ctx);
    cleaner_hotvr[jecvar.first].reset(new TopJetCleaner(ctx, hotvrID, pipe_rec_name));
    slct_1largejet->add(make_shared<NTopJetSelection>(1, -1, boost::none, h_hotvrpuppi_jecvariations[jecvar.first]));
  }

  hist_jetmet_corrections.reset(new ltt::AndHists(ctx, to_string(i_hist++)+"_AK4METCorr"));

  hist_metcut.reset(new ltt::AndHists(ctx, to_string(i_hist++)+"_METSel", true)); // switch on HOTVR/AK8 hists to have a comparison before/after corrections

  hist_largejet_correction.reset(new ltt::AndHists(ctx, to_string(i_hist++)+"_LargeJetCorr", true));

  hist_1largejet.reset(new ltt::AndHists(ctx, to_string(i_hist++)+"_LargeJetSel", true));

  fHandle_bool_reco_sel = ctx.declare_event_output<bool>(kHandleName_bool_reco_sel);
  fHandle_bool_matrix_sel = ctx.declare_event_output<bool>(kHandleName_bool_matrix_sel);
  fHandle_bool_parton_sel = ctx.declare_event_output<bool>(kHandleName_bool_parton_sel);
  fHandle_bool_particle_sel = ctx.declare_event_output<bool>(kHandleName_bool_particle_sel);
}


//------------//
// EVENT LOOP //
//------------//

bool HighPtSingleTopPreSelectionModule::process(Event & event) {

  if(debug) {
    cout << endl;
    cout << "+-----------+" << endl;
    cout << "| NEW EVENT |" << endl;
    cout << "+-----------+" << endl;
    cout << "i_event = " << to_string(i_event++) << endl;
    cout << endl;
  }

  bool passes_reco_sel(true);
  bool passes_matrix_sel(false);
  bool passes_parton_sel(false);
  bool passes_particle_sel(false);
  SingleTopGen_tWch *GENtW = nullptr;
  if(is_tW) {
    prod_SingleTopGen_tWch->process(event);
    GENtW = &event.get(fHandle_GENtW);
    if(is_tW_incl && is_extra_syst && !(GENtW->IsTopHadronicDecay() && GENtW->IsAssHadronicDecay())) return false; // remove all NoFullyHadronic events from inclusiveDecays sample if using extra_syst
    genleveldef->process(event);
    passes_matrix_sel = slct_matrix->passes(event);
    passes_parton_sel = false; // FIXME
    passes_particle_sel = false; // FIXME
  }

  if(debug) cout << "Lumi selection and scale factor" << endl; // else getting error for some data samples, e.g. "RunSwitcher cannot handle run number 275656 for year 2016"
  if(event.isRealData && !slct_lumi->passes(event)) {
    passes_reco_sel = false;
    if(!is_tW) return false;
  }
  sf_lumi->process(event);
  if(passes_reco_sel) hist_lumi->fill(event);

  if(debug) cout << "Primary vertex selection and PU scale factor" << endl;
  clnr_pv->process(event);
  if(!slct_pv->passes(event)) {
    passes_reco_sel = false;
    if(!is_tW) return false;
  }
  sf_pileup->process(event);
  if(passes_reco_sel) hist_pv->fill(event);

  if(debug) cout << "Rochester corrections for muons" << endl;
  rochester->process(event);
  if(passes_reco_sel) hist_rochester->fill(event);

  if(debug) cout << "Identify and count electrons and muons" << endl;
  unsigned int n_electrons_veto(0);
  unsigned int n_electrons_lowpt(0);
  const Electron (*electron_lowpt)(nullptr);
  unsigned int n_electrons_highpt(0);
  const Electron (*electron_highpt)(nullptr);
  for(const Electron & ele : *event.electrons) {
    if(electronID_veto(ele, event)) ++n_electrons_veto;
    if(electronID_lowpt(ele, event)) {
      ++n_electrons_lowpt;
      electron_lowpt = &ele;
    }
    if(electronID_highpt(ele, event)) {
      ++n_electrons_highpt;
      electron_highpt = &ele;
    }
  }

  unsigned int n_muons_veto(0);
  unsigned int n_muons_lowpt(0);
  const Muon (*muon_lowpt)(nullptr);
  unsigned int n_muons_highpt(0);
  const Muon (*muon_highpt)(nullptr);
  for(const Muon & muo : *event.muons) {
    if(muonID_veto(muo, event)) ++n_muons_veto;
    if(muonID_lowpt(muo, event)) {
      ++n_muons_lowpt;
      muon_lowpt = &muo;
    }
    if(muonID_highpt(muo, event)) {
      ++n_muons_highpt;
      muon_highpt = &muo;
    }
  }

  if(debug) cout << "Check which lepton domain the event belongs to" << endl;
  bool passes_electron_lowpt(false);
  bool passes_electron_highpt(false);
  if(n_electrons_lowpt == 1) {
    unsigned int n_veto = n_muons_veto;
    for(const Electron & ele : *event.electrons) {
      if(electron_lowpt == &ele) continue;
      else if(electronID_veto(ele, event)) ++n_veto;
    }
    if(n_veto == 0 && (electron_lowpt->v4().pt() < electron_highpt_pt_min)) passes_electron_lowpt = true;
  }
  if(n_electrons_highpt == 1) {
    unsigned int n_veto = n_muons_veto;
    for(const Electron & ele : *event.electrons) {
      if(electron_highpt == &ele) continue;
      else if(electronID_veto(ele, event)) ++n_veto;
    }
    if(n_veto == 0) passes_electron_highpt = true;
  }

  bool passes_muon_lowpt(false);
  bool passes_muon_highpt(false);
  if(n_muons_lowpt == 1) {
    unsigned int n_veto = n_electrons_veto;
    for(const Muon & muo : *event.muons) {
      if(muon_lowpt == &muo) continue;
      else if(muonID_veto(muo, event)) ++n_veto;
    }
    if(n_veto == 0 && (muon_lowpt->v4().pt() < muon_highpt_pt_min)) passes_muon_lowpt = true;
  }
  if(n_muons_highpt == 1) {
    unsigned int n_veto = n_electrons_veto;
    for(const Muon & muo : *event.muons) {
      if(muon_highpt == &muo) continue;
      else if(muonID_veto(muo, event)) ++n_veto;
    }
    if(n_veto == 0) passes_muon_highpt = true;
  }

  if(debug) cout << "Keep only events which pass lepton domain criteria" << endl;
  vector<Muon> signal_muon; // will have 1 element in case of muon channel, and 0 in case of electron channel
  vector<Electron> signal_electron; // similar as above
  if(passes_muon_highpt) {
    /*
    These events CANNOT be part of the electron channel since we require 0 veto electrons (wpLoose) which are a strict superset of lowpt and highpt signal electrons (wp90).
    These events also CANNOT be part of lowpt muon domain since we require 0 additional veto muons (looseID) which are a strict superset of lowpt signal muons (tightID);
    it also is not possible that the highpt signal muon is equal to the lowpt signal muon since we cut off the pt spectrum of lowpt muons at the minimum pt value of highpt muons.
    */
    if(muon_highpt == nullptr) cout << "High-pT muon is nullptr" << endl;
    signal_muon.push_back(*muon_highpt);
  }
  else if(passes_muon_lowpt) {
    /*
    Similar to the highpt muon domain, these events CANNOT be part of the electron channel due to the electron veto.
    As explained in the above comment, there cannot be an indecisiveness whether the event belongs to the lowpt or highpt muon domain since we cut off the pt spectrum.
    It is however possible that we have an additional muon that fulfills the HighPtID and fulfills the minimum pt threshold of the highpt muon domain;
    this should happen only very rarely; if it happens, this will not be a problem in terms of ambiguities with the highpt muon domain since the present tightID muon,
    which is unequal to the *additional* HighptID muon, forbids that the event is part of the highpt muon domain since we require 0 additional looseID muons there.
    */
    if(muon_lowpt == nullptr) cout << "Low-pT muon is nullptr" << endl;
    signal_muon.push_back(*muon_lowpt);
  }
  else if(passes_electron_highpt) {
    /*
    There is no overlap with the muon channel since the muon channel requires 0 veto electrons which are a strict superset of signal electrons.
    */
    if(electron_highpt == nullptr) cout << "High-pT electron is nullptr" << endl;
    signal_electron.push_back(*electron_highpt);
  }
  else if(passes_electron_lowpt) {
    /*
    Same as above.
    Also, there is no indecisiveness whether the event belongs to the lowpt or highpt electron domain due to the pt spectrum cut off.
    */
    if(electron_lowpt == nullptr) cout << "Low-pT electron is nullptr" << endl;
    signal_electron.push_back(*electron_lowpt);
  }
  else {
    passes_reco_sel = false;
    if(!is_tW) return false;
  }

  if(debug) cout << "Clean lepton collections, apply lepton scale factors, and set primary lepton" << endl;
  swap(*event.muons, signal_muon);
  swap(*event.electrons, signal_electron);
  if(!is_mc) { // It is possible that SingleMuon does contain events with signal electrons and vice-versa. We reject those events here, which would be rejected at the beginning of the MainSelection regardlessly
    if(is_singlemuon) {
      if(event.muons->size() != 1) return false;
    }
    else {
      if(event.electrons->size() != 1) return false;
    }
  }
  if(passes_muon_highpt) {
    sf_muon_id_highpt->process(event);
    sf_elec_id_dummy->process(event);
    sf_elec_reco_dummy->process(event);
  }
  else if(passes_muon_lowpt) {
    sf_muon_id_lowpt->process(event);
    sf_elec_id_dummy->process(event);
    sf_elec_reco_dummy->process(event);
  }
  else if(passes_electron_highpt) {
    sf_elec_id_highpt->process(event);
    sf_muon_id_dummy->process(event);
    sf_elec_reco->process(event);
  }
  else if(passes_electron_lowpt) {
    sf_elec_id_lowpt->process(event);
    sf_muon_id_dummy->process(event);
    sf_elec_reco->process(event);
  }
  else { // if not passes_reco_sel
    sf_muon_id_dummy->process(event);
    sf_elec_id_dummy->process(event);
    sf_elec_reco_dummy->process(event);
  }
  if(passes_reco_sel) {
    primlep->process(event);
  }
  if(passes_reco_sel) hist_1lepton->fill(event);

  if(debug) cout << "AK4 and MET corrections" << endl;
  if(passes_reco_sel) {
    for(const auto & jecvar : kJECVariationToString) {
      event.set(h_ak4puppi_jecvariations[jecvar.first], event.get(h_ak4puppi_orig));
      event.set(h_metpuppi_jecvariations[jecvar.first], event.get(h_metpuppi_orig));
      jetmet_corrections_puppi[jecvar.first]->process(event);
      event.set(h_ak4chs_jecvariations[jecvar.first], event.get(h_ak4chs_orig));
      event.set(h_metchs_jecvariations[jecvar.first], event.get(h_metchs_orig));
      jetmet_corrections_chs[jecvar.first]->process(event);
    }
  }
  if(passes_reco_sel) hist_jetmet_corrections->fill(event);

  if(debug) cout << "MET selection" << endl;
  if(passes_reco_sel) {
    if(!slct_met->passes(event)) {
      passes_reco_sel = false;
      if(!is_tW) return false;
    }
  }
  if(passes_reco_sel) hist_metcut->fill(event);

  if(debug) cout << "Apply corrections to top jet collections and clean them" << endl;
  if(passes_reco_sel) {
    for(const auto & jecvar : kJECVariationToString) {
      event.set(h_ak8puppi_jecvariations[jecvar.first], event.get(h_ak8puppi_orig));
      corrections_ak8[jecvar.first]->process(event);
      cleaner_ak8[jecvar.first]->process(event);
      event.set(h_hotvrpuppi_jecvariations[jecvar.first], event.get(h_hotvrpuppi_orig));
      corrections_hotvr[jecvar.first]->process(event);
      cleaner_hotvr[jecvar.first]->process(event);
    }
  }
  if(passes_reco_sel) hist_largejet_correction->fill(event);

  if(debug) cout << "At least one large-R jet (HOTVR or AK8)" << endl;
  if(passes_reco_sel) {
    if(!slct_1largejet->passes(event)) {
      passes_reco_sel = false;
      if(!is_tW) return false;
    }
  }
  if(passes_reco_sel) hist_1largejet->fill(event);

  if(debug) cout << "End of PreSelectionModule. Event passed" << endl;
  event.set(fHandle_bool_reco_sel, passes_reco_sel);
  event.set(fHandle_bool_matrix_sel, passes_matrix_sel);
  event.set(fHandle_bool_parton_sel, passes_parton_sel);
  event.set(fHandle_bool_particle_sel, passes_particle_sel);
  return passes_reco_sel || passes_matrix_sel || passes_parton_sel || passes_particle_sel;
  // return true;
}


UHH2_REGISTER_ANALYSIS_MODULE(HighPtSingleTopPreSelectionModule)

}}
