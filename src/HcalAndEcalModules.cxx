#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/HighPtSingleTop/include/HcalAndEcalModules.h"

using namespace std;
using namespace uhh2;
using namespace btw;

namespace uhh2 { namespace btw {

PrefiringWeights::PrefiringWeights(Context & ctx) {

  year = extract_year(ctx);
  string syst_direction_ = ctx.get("SystDirection_Prefiring", "nominal");

  if(syst_direction_ == "up") {
    syst_direction = 1;
  }
  else if(syst_direction_ == "down") {
    syst_direction = -1;
  }
  else {
    syst_direction = 0;
  }
}

bool PrefiringWeights::process(Event & event) {

  if(!event.isRealData && (year == Year::is2016v3 || year == Year::is2017v2)) {
    if(syst_direction == 1) {
      event.weight *= event.prefiringWeightUp;
    }
    else if(syst_direction == -1) {
      event.weight *= event.prefiringWeightDown;
    }
    else {
      event.weight *= event.prefiringWeight;
    }
  }

  return true;
}


// HEMIssueSelection::HEMIssueSelection(Context & ctx, const string & h_name_ak8jets) {
//
//   year = extract_year(ctx);
//   h_ak8jets = ctx.get_handle<vector<TopJet>>(h_name_ak8jets);
// }
//
// // return true for events which are affected by the HEM issue
// bool HEMIssueSelection::passes(const Event & event) {
//
//   if(year == Year::is2018 && ((event.isRealData && event.run >= m_runnumber) || !event.isRealData)) {
//     for(const Electron & e : *event.electrons) {
//       if(e.v4().Phi() > m_phi.first && e.v4().Phi() < m_phi.second && e.v4().Eta() > m_eta.first && e.v4().Eta() < m_eta.second) return true;
//     }
//     for(const Jet & j : *event.jets) {
//       if(j.v4().Phi() > m_phi.first && j.v4().Phi() < m_phi.second && j.v4().Eta() > m_eta.first && j.v4().Eta() < m_eta.second) return true;
//     }
//     for(const TopJet & t : *event.topjets) {
//       if(t.v4().Phi() > m_phi.first && t.v4().Phi() < m_phi.second && t.v4().Eta() > m_eta.first && t.v4().Eta() < m_eta.second) return true;
//     }
//     for(const TopJet & a : event.get(h_ak8jets)) {
//       if(a.v4().Phi() > m_phi.first && a.v4().Phi() < m_phi.second && a.v4().Eta() > m_eta.first && a.v4().Eta() < m_eta.second) return true;
//     }
//   }
//
//   return false;
// }


HEMIssueSelection::HEMIssueSelection(Context & ctx, const string & h_name_wtaggedjets, const string & h_name_toptaggedjets) {

  year = extract_year(ctx);
  h_wtaggedjets = ctx.get_handle<vector<TopJet>>(h_name_wtaggedjets);
  h_toptaggedjets = ctx.get_handle<vector<TopJet>>(h_name_toptaggedjets);
}

// return true for events which are affected by the HEM issue
bool HEMIssueSelection::passes(const Event & event) {

  const vector<TopJet> wtaggedjets = event.get(h_wtaggedjets);
  const vector<TopJet> toptaggedjets = event.get(h_toptaggedjets);

  if((year == Year::is2018 || year == Year::isUL18) && ((event.isRealData && event.run >= m_runnumber) || !event.isRealData)) {
    for(const Electron & e : *event.electrons) {
      if(e.v4().Phi() > m_phi.first && e.v4().Phi() < m_phi.second && e.v4().Eta() > m_eta.first && e.v4().Eta() < m_eta.second) return true;
    }
    for(const TopJet & w : wtaggedjets) {
      if(w.v4().Phi() > m_phi.first && w.v4().Phi() < m_phi.second && w.v4().Eta() > m_eta.first && w.v4().Eta() < m_eta.second) return true;
    }
    for(const TopJet & t : toptaggedjets) {
      if(t.v4().Phi() > m_phi.first && t.v4().Phi() < m_phi.second && t.v4().Eta() > m_eta.first && t.v4().Eta() < m_eta.second) return true;
    }



    for(const Jet & t : *event.jets) {
      if(t.v4().Phi() > m_phi.first && t.v4().Phi() < m_phi.second && t.v4().Eta() > m_eta.first && t.v4().Eta() < m_eta.second) return true;
    }
  }

  return false;
}

}}
