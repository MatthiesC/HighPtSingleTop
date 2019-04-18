#include "UHH2/HighPtSingleTop/include/HighPtSingleTopSelections.h"
#include "UHH2/HighPtSingleTop/include/SingleTopGen_tWch.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"

#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/FlavorParticle.h"

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

MTWSelection::MTWSelection(Context & ctx, double mtw_min_):
  h_primlepton(ctx.get_handle<FlavorParticle>("PrimaryLepton")),
  mtw_min(mtw_min_) {}

bool MTWSelection::passes(const Event & event) {

  assert(event.met);

  const auto lepton = event.get(h_primlepton);
  auto mtw = calcMTW(lepton, event);

  return (mtw > mtw_min);
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
