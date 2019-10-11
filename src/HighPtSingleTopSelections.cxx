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

DeltaRCut::DeltaRCut(Context & ctx, double deltaR_min_, string primlep_name_):
  h_primlepton(ctx.get_handle<FlavorParticle>(primlep_name_)),
  deltaR_min(deltaR_min_) {}

bool DeltaRCut::passes(const Event & event) {

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

  if(!GENtW.IsTopHadronicDecay()) return false;

  if(is_muon) { // Muon+jets channel
    if(!GENtW.IsAssToMuonDecay()) return false;
  }
  else { // Electron+jets channel
    if(!GENtW.IsAssToElectronDecay()) return false;
  }

  return true;
}



// copied from Alex
HighPtSingleTopTriggerSelection::HighPtSingleTopTriggerSelection(Context &ctx) {
  year = extract_year(ctx);
  is_ele = ctx.get("analysis_channel") == "ELECTRON";
  is_muo = ctx.get("analysis_channel") == "MUON";    

  trig_isomu24.reset(new TriggerSelection("HLT_IsoMu24_v*"));
  trig_isotkmu24.reset(new TriggerSelection("HLT_IsoTkMu24_v*"));
  trig_isomu27.reset(new TriggerSelection("HLT_IsoMu27_v*"));

  trig_ele27.reset(new TriggerSelection("HLT_Ele27_WPTight_Gsf_v*"));
  trig_ele32.reset(new TriggerSelection("HLT_Ele32_WPTight_Gsf_v*"));
  trig_ele35.reset(new TriggerSelection("HLT_Ele35_WPTight_Gsf_v*"));

  trig_photon175.reset(new TriggerSelection("HLT_Photon175_v*"));
  trig_photon200.reset(new TriggerSelection("HLT_Photon200_v*"));

}

bool HighPtSingleTopTriggerSelection::passes(const Event &event) {

  if (year == Year::is2016v3) 
    {
      if (is_ele)
	{
	  return (trig_ele27->passes(event) || trig_photon175->passes(event));
	}
      if (is_muo)
	{
	  return (trig_isomu24->passes(event) || trig_isotkmu24->passes(event));
	}
    }

  else if (year == Year::is2017v2) 
    {
      if (is_ele)
	{
	  return (trig_ele35->passes(event) || trig_photon200->passes(event));
	}
      if (is_muo)
	{
	  return (trig_isomu27->passes(event));
	}
    }
  
  else if (year == Year::is2018)
    {
      if (is_ele)
	{
	  return (trig_ele32->passes(event));
	}
      if (is_muo)
	{
	  return (trig_isomu24->passes(event));
	}
    }

  return false;
}
