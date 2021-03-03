#include "UHH2/HighPtSingleTop/include/HighPtSingleTopSelections.h"
#include "UHH2/HighPtSingleTop/include/SingleTopGen_tWch.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"

#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/FlavorParticle.h"

#include "UHH2/common/include/Utils.h"
#include "UHH2/common/include/TopJetIds.h"
#include "UHH2/common/include/TriggerSelection.h"

#include <stdexcept>

using namespace std;
using namespace uhh2;


//-----------------------------//
// Missing transverse momentum //
//-----------------------------//

METSelection::METSelection(double met_min_) {
  met_min = met_min_;
}

bool METSelection::passes(const Event & event) {
  assert(event.met);
  return (event.met->pt() > met_min);
}


//-------------------------//
// Transverse W boson mass //
//-------------------------//

MTWSelection::MTWSelection(Context & ctx, double mtw_min_, string primlep_name_):
  h_primlepton(ctx.get_handle<FlavorParticle>(primlep_name_)),
  mtw_min(mtw_min_) {}

bool MTWSelection::passes(const Event & event) {

  assert(event.met);

  const FlavorParticle & lepton = event.get(h_primlepton);
  auto mtw = calcMTW(lepton, event);

  return (mtw > mtw_min);
}


//--------------------------------------//
// DeltaR(primary lepton, next AK4 jet) //
//--------------------------------------//

DeltaRLeptonNextJetCut::DeltaRLeptonNextJetCut(Context & ctx, double deltaR_min_, string primlep_name_):
  h_primlepton(ctx.get_handle<FlavorParticle>(primlep_name_)),
  deltaR_min(deltaR_min_) {}

bool DeltaRLeptonNextJetCut::passes(const Event & event) {

  const FlavorParticle & lepton = event.get(h_primlepton);
  const auto jets = *event.jets;

  return deltaR_min < uhh2::deltaR(lepton.v4(), (nextJet(lepton, jets))->v4());
}


//------------------------------------------------//
// DeltaPhi(primary lepton, top-tagged HOTVR jet) //
//------------------------------------------------//

DeltaPhiTopLeptonCut::DeltaPhiTopLeptonCut(Context & ctx, double & deltaPhi_min_, TopJetId & topJetId_, string primlep_name_):
  h_primlepton(ctx.get_handle<FlavorParticle>(primlep_name_)),
  deltaPhi_min(deltaPhi_min_),
  topJetId(topJetId_) {}

bool DeltaPhiTopLeptonCut::passes(const Event & event) {

  const FlavorParticle & lepton = event.get(h_primlepton);

  TopJet toptaggedjet;
  for(auto t : *event.topjets) { if(topJetId(t, event)) toptaggedjet = t; }

  return deltaPhi_min < uhh2::deltaPhi(lepton.v4(), toptaggedjet.v4());
}


//-----------------------------------------------------//
// Select t(had)W(lep) events on MC truth parton level //
//-----------------------------------------------------//

tWgenSignalSelection::tWgenSignalSelection(Context & ctx, bool is_muon_):
  h_GENtW(ctx.get_handle<SingleTopGen_tWch>("h_GENtW")),
  is_muon(is_muon_) {}

bool tWgenSignalSelection::passes(const Event & event) {

  const auto & GENtW = event.get(h_GENtW);

  if(is_muon) {
    if(
      (GENtW.IsTopHadronicDecay() && GENtW.IsAssToMuonDecay()) ||
      (GENtW.IsAssHadronicDecay() && GENtW.IsTopToMuonDecay())
    ) return true;
    else return false;
  }
  else{
    if(
      (GENtW.IsTopHadronicDecay() && GENtW.IsAssToElectronDecay()) ||
      (GENtW.IsAssHadronicDecay() && GENtW.IsTopToElectronDecay())
    ) return true;
    else return false;
  }

  return false;
}


//--------------------------------------------------------------------//
// Select tW events based on a specific decay channel given by string //
//--------------------------------------------------------------------//

tWgenSelection::tWgenSelection(Context & ctx, string decay_):
  h_GENtW(ctx.get_handle<SingleTopGen_tWch>("h_GENtW")),
  m_decay(decay_) {

  cout << "tWgenSelection: CAVEAT, cannot check which particles a tau lepton decays into! Generator info missing." << endl;
}

bool tWgenSelection::passes(const Event & event) {

  const auto & GENtW = event.get(h_GENtW);

  if(m_decay == "TopToHad") {
    if(!GENtW.IsTopHadronicDecay()) return false;
  }
  else if(m_decay == "TopToEle") {
    if(!GENtW.IsTopToElectronDecay()) return false;
  }
  else if(m_decay == "TopToMuo") {
    if(!GENtW.IsTopToMuonDecay()) return false;
  }
  else if(m_decay == "TopToTau") {
    if(!GENtW.IsTopToTauonDecay()) return false;
  }
  else if(m_decay == "WToHad") {
    if(!GENtW.IsAssHadronicDecay()) return false;
  }
  else if(m_decay == "WToEle") {
    if(!GENtW.IsAssToElectronDecay()) return false;
  }
  else if(m_decay == "WToMuo") {
    if(!GENtW.IsAssToMuonDecay()) return false;
  }
  else if(m_decay == "WToTau") {
    if(!GENtW.IsAssToTauonDecay()) return false;
  }
  //else if(m_decay == "LO") {
  //  if(!GENtW.IsBottomGluonProcess()) return false;
  //}
  else if(m_decay == "GenBJet") {
    if(!GENtW.HasAssociatedBottom()) return false;
    if(!(abs(GENtW.bAss().v4().Eta()) < 2.4)) return false;
    if(!(GENtW.bAss().v4().Pt() > 30)) return false;
    bool near_genjet = false;
    for(auto gj : *event.genjets) {
      if(deltaR(gj.v4(), GENtW.bAss().v4()) < 0.1) near_genjet = true;
    }
    if(!near_genjet) return false;
  }

  return true;
}


//---------------------------------------//
// Generator selection for W+jets events //
//---------------------------------------//

WJetsGenSelection::WJetsGenSelection(Context &ctx, string jets_type):
  m_jets_type(jets_type) {

  m_is_wjets = ctx.get("dataset_version").find("WJets") == 0;
}


bool WJetsGenSelection::passes(const Event &event) {

  assert(event.genparticles);

  if(!m_is_wjets) return true;

  uint n_cQuarks = 0;
  uint n_bQuarks = 0;

  for(GenParticle gp : *event.genparticles) {
    if(gp.index() > 1) { // exclude initial-state partons from consideration
      if(abs(gp.pdgId()) == 4) ++n_cQuarks;
      else if(abs(gp.pdgId()) == 5) ++n_bQuarks;
    }
  }

  if(m_jets_type == "HF") { // W + heavy flavour (HF)
    return (n_cQuarks + n_bQuarks > 0);
  }

  return true;
}


//--------------------------------------------------//
// Cleaner for jets overlapping with primary lepton //
//--------------------------------------------------//

// NB: The standard jet-lepton cleaning in UHH2 looks at all leptons (wihtout ID criteria and so on...) and also requires jet.muonMultiplicity > 0.

JetLeptonOverlapRemoval::JetLeptonOverlapRemoval(Context & ctx, double & deltaR_min_, string primlep_name_):
  h_primlepton(ctx.get_handle<FlavorParticle>(primlep_name_)),
  deltaR_min(deltaR_min_) {}

bool JetLeptonOverlapRemoval::process(Event & event) {

  std::vector<Jet> result;
  const FlavorParticle & lepton = event.get(h_primlepton);

  for(const auto & jet : *event.jets) {
    if(uhh2::deltaR(jet, lepton) > deltaR_min) {
      result.push_back(jet);
    }
   }

  std::swap(result, *event.jets);

  return true;
}


//------------------------------------------------------------//
// Selection based on how many quarks are merged to HOTVR jet //
//------------------------------------------------------------//

MergeScenarioSelection::MergeScenarioSelection(Context & ctx, unsigned int number_of_merged_quarks, string toptaggedjet_name_):
  h_toptaggedjet(ctx.get_handle<TopJet>(toptaggedjet_name_)),
  h_GENtW(ctx.get_handle<SingleTopGen_tWch>("h_GENtW")),
  m_mergedQuarks(number_of_merged_quarks) {}

bool MergeScenarioSelection::passes(const Event & event) {

  const auto & GENtW = event.get(h_GENtW);
  const auto & topjet = event.get(h_toptaggedjet);

  if(!GENtW.IsTopHadronicDecay()) throw runtime_error("MergeScenarioSelection: Cannot use this selection class on event which has no hadronic top quark decay!");

  vector<GenParticle> top_decays;
  top_decays.push_back(GENtW.bTop());
  top_decays.push_back(GENtW.WTopDecay1());
  top_decays.push_back(GENtW.WTopDecay2());

  // Definition of HOTVR radius
  double jet_radius = min(1.5, max(0.1, 600.0 / (topjet.pt() * topjet.JEC_factor_raw()) ));

  unsigned int mergedQuarks = 0;

  for(GenParticle gp : top_decays) {
    if(jet_radius > uhh2::deltaR(gp.v4(), topjet.v4())) ++mergedQuarks;
  }

  return mergedQuarks == m_mergedQuarks;
}


MyNJetSelection::MyNJetSelection(Context & ctx, int n_min, int n_max, string objects_handle_name) {

  h_objects = ctx.get_handle<vector<Jet>>(objects_handle_name);
  m_n_min = n_min;
  m_n_max = n_max;

  if(m_n_max < -1) throw runtime_error("MyNJetSelection: n_max must be an integer >= -1");
  if(m_n_min < 0) throw runtime_error("MyNJetSelection: n_min must be an integer >= 0");
  if(m_n_max < m_n_min && m_n_max != -1) throw runtime_error("MyNJetSelection: n_max >= n_min must be fulfilled");
}

bool MyNJetSelection::passes(const Event & event) {

  const auto objects = event.get(h_objects);

  if(m_n_max == -1) {
    return objects.size() >= (uint)m_n_min;
  }
  else {
    return objects.size() >= (uint)m_n_min && objects.size() <= (uint)m_n_max;
  }
}


MyNTopJetSelection::MyNTopJetSelection(Context & ctx, int n_min, int n_max, string objects_handle_name) {

  h_objects = ctx.get_handle<vector<TopJet>>(objects_handle_name);
  m_n_min = n_min;
  m_n_max = n_max;

  if(m_n_max < -1) throw runtime_error("MyNTopJetSelection: n_max must be an integer >= -1");
  if(m_n_min < 0) throw runtime_error("MyNTopJetSelection: n_min must be an integer >= 0");
  if(m_n_max < m_n_min && m_n_max != -1) throw runtime_error("MyNTopJetSelection: n_max >= n_min must be fulfilled");
}

bool MyNTopJetSelection::passes(const Event & event) {

  const auto objects = event.get(h_objects);

  if(m_n_max == -1) {
    return objects.size() >= (uint)m_n_min;
  }
  else {
    return objects.size() >= (uint)m_n_min && objects.size() <= (uint)m_n_max;
  }
}

// TODO: Merge this class with the one above...
// MyNTopJetsSelection::MyNTopJetsSelection(Context & ctx, int n_min, int n_max, string objects_handle_name) {
//
//   h_objects = ctx.get_handle<vector<TopJet>>(objects_handle_name);
//   m_n_min = n_min;
//   m_n_max = n_max;
//
//   if(m_n_max < -1) throw runtime_error("MyNTopJetsSelection: n_max must be an integer >= -1");
//   if(m_n_min < 0) throw runtime_error("MyNTopJetsSelection: n_min must be an integer >= 0");
//   if(m_n_max < m_n_min && m_n_max != -1) throw runtime_error("MyNTopJetsSelection: n_max >= n_min must be fulfilled");
// }
//
// bool MyNTopJetsSelection::passes(const Event & event) {
//
//   const auto objects = event.get(h_objects);
//
//   if(m_n_max == -1) {
//     return objects.size() >= (uint)m_n_min;
//   }
//   else {
//     return objects.size() >= (uint)m_n_min && objects.size() <= (uint)m_n_max;
//   }
// }


HighPtSingleTopTriggerSelection::HighPtSingleTopTriggerSelection(Context & ctx) {

  year = extract_year(ctx);
  TString dataset_version = ctx.get("dataset_version");
  b_electron_stream = dataset_version.Contains("SingleElectron");

  is_ele = ctx.get("analysis_channel") == "ele";
  is_muo = ctx.get("analysis_channel") == "muo";

  trig_isomu24.reset(new TriggerSelection("HLT_IsoMu24_v*"));
  trig_isotkmu24.reset(new TriggerSelection("HLT_IsoTkMu24_v*"));
  trig_isomu27.reset(new TriggerSelection("HLT_IsoMu27_v*"));

  trig_ele27.reset(new TriggerSelection("HLT_Ele27_WPTight_Gsf_v*"));
  trig_ele32.reset(new TriggerSelection("HLT_Ele32_WPTight_Gsf_v*"));
  trig_ele35.reset(new TriggerSelection("HLT_Ele35_WPTight_Gsf_v*"));

  trig_photon175.reset(new TriggerSelection("HLT_Photon175_v*"));
  trig_photon200.reset(new TriggerSelection("HLT_Photon200_v*"));
}

bool HighPtSingleTopTriggerSelection::passes(const Event & event) {

  if(year == Year::is2016v3) {
    if(is_ele) {
      if(event.isRealData) {
        if(b_electron_stream) return trig_ele27->passes(event);
        else return (!trig_ele27->passes(event) && trig_photon175->passes(event)); // only select events without ele27 trigger, since they are already in SingleElectron
      }
      else return (trig_ele27->passes(event) || trig_photon175->passes(event));
    }
    else if(is_muo) return (trig_isomu24->passes(event) || trig_isotkmu24->passes(event));
    else return false;
  }

  else if(year == Year::is2017v2) {
    if(is_ele) {
      if(event.isRealData) {
        if(b_electron_stream) return trig_ele35->passes(event);
        else return (!trig_ele35->passes(event) && trig_photon200->passes(event)); // only select events without ele35 trigger, since they are already in SingleElectron
      }
      else return (trig_ele35->passes(event) || trig_photon200->passes(event));
    }
    else if(is_muo) return (trig_isomu27->passes(event));
    else return false;
  }

  else if(year == Year::is2018) {
    // no need for differentiation between photon and electron stream since EGamma dataset
    if(is_ele) return (trig_ele32->passes(event) || trig_photon200->passes(event));
    else if(is_muo) return (trig_isomu24->passes(event));
    else return false;
  }

  else return false;
}
