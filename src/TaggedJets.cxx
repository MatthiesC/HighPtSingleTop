#include "UHH2/HighPtSingleTop/include/TaggedJets.h"

#include "UHH2/common/include/Utils.h"

using namespace uhh2;
using namespace std;


TopTaggedJet::TopTaggedJet(Context & ctx,
			   TopJetId topjetid,
			   const std::string & h_name):
  h_toptaggedjet(ctx.get_handle<TopJet>(h_name)),
  m_topjetid(topjetid)
{}


bool TopTaggedJet::process(uhh2::Event & event) {
  assert(event.topjets);

  vector<TopJet> topjets = *event.topjets;
  sort_by_pt<TopJet>(topjets);

  TopJet toptaggedjet;
  bool toptag_found = false;

  for(auto t : topjets) {
    if(m_topjetid(t, event)) {
      toptaggedjet = t;
      toptag_found = true;
    }
  }

  if(!toptag_found) { // Needed for the 0toptag validation region
    toptaggedjet = topjets.at(0);
  }

  event.set(h_toptaggedjet, std::move(toptaggedjet));
  return true;
}


TopTaggedJet::~TopTaggedJet() {}


Ak8Jets::Ak8Jets(Context & ctx,
       const std::string & h_name_ak8jets):
  h_ak8jets_rec(ctx.get_handle<vector<TopJet>>(ctx.get("Ak8recCollection"))),
  h_ak8jets(ctx.get_handle<vector<TopJet>>(h_name_ak8jets))
{}


bool Ak8Jets::process(uhh2::Event & event) {
  event.set(h_ak8jets, event.get(h_ak8jets_rec));

  return true;
}


Ak8Jets::~Ak8Jets() {}


WTaggedJets::WTaggedJets(Context & ctx,
       const std::string & h_name_analysis,
       const std::string & h_name_veryloose,
       const std::string & h_name_loose,
       const std::string & h_name_medium,
       const std::string & h_name_tight,
       const std::string & h_name_verytight,
       const std::string & h_name_ak8jets,
       const bool & b_massDecorrelated_):
  h_ak8jets(ctx.get_handle<vector<TopJet>>(h_name_ak8jets)),
  h_wtaggedjets_veryloose(ctx.get_handle<vector<TopJet>>(h_name_veryloose)),
  h_wtaggedjets_loose(ctx.get_handle<vector<TopJet>>(h_name_loose)),
  h_wtaggedjets_medium(ctx.get_handle<vector<TopJet>>(h_name_medium)),
  h_wtaggedjets_tight(ctx.get_handle<vector<TopJet>>(h_name_tight)),
  h_wtaggedjets_verytight(ctx.get_handle<vector<TopJet>>(h_name_verytight)),
  h_wtaggedjets_analysis(ctx.get_handle<vector<TopJet>>(h_name_analysis)),
  year(extract_year(ctx)),
  b_massDecorrelated(b_massDecorrelated_)
{
  if(year == Year::is2016v3) { b_massDecorrelated ? wps = wps_W_2016_MD : wps = wps_W_2016; }
  else if(year == Year::is2017v2) { b_massDecorrelated ? wps = wps_W_2017_MD : wps = wps_W_2017; }
  else if(year == Year::is2018) { b_massDecorrelated ? wps = wps_W_2018_MD : wps = wps_W_2018; }
  else { runtime_error("WTaggedJets: Provided year information not valid."); }
}


bool WTaggedJets::process(Event & event) {

  vector<TopJet> ak8jets = event.get(h_ak8jets);
  vector<TopJet> wjets_veryloose, wjets_loose, wjets_medium, wjets_tight, wjets_verytight;

  // TODO: the following cuts are just valid for 2016!!! This really needs to be more sophisticated!!! Maybe also switch to DeepAK8!!!
  // WPs are year-dependent
  // https://twiki.cern.ch/twiki/bin/view/CMS/JetWtagging
  // https://twiki.cern.ch/twiki/bin/viewauth/CMS/DeepAK8Tagging2018WPsSFs
  for(auto & j : ak8jets) {

		bool b_massWindow = j.softdropmass() >= 65. && j.softdropmass() <= 105.;
    if(!b_massWindow) continue;

    // https://gitlab.cern.ch/DeepAK8/NNKit/tree/for94X
    double discr = b_massDecorrelated ? j.btag_MassDecorrelatedDeepBoosted_WvsQCD() : j.btag_DeepBoosted_WvsQCD(); // binarized NN output
    if(discr > wps.at(0)) continue;
    wjets_veryloose.push_back(j);
    if(discr > wps.at(1)) continue;
    wjets_loose.push_back(j);
    if(discr > wps.at(2)) continue;
    wjets_medium.push_back(j);
    if(discr > wps.at(3)) continue;
    wjets_tight.push_back(j);
    if(discr > wps.at(4)) continue;
    wjets_verytight.push_back(j);
  }

  event.set(h_wtaggedjets_analysis, wjets_tight); // TODO: Use an argument of the constructor to decide which WP to use on analysis level
  event.set(h_wtaggedjets_veryloose, std::move(wjets_veryloose));
  event.set(h_wtaggedjets_loose, std::move(wjets_loose));
  event.set(h_wtaggedjets_medium, std::move(wjets_medium));
  event.set(h_wtaggedjets_tight, std::move(wjets_tight));
  event.set(h_wtaggedjets_verytight, std::move(wjets_verytight));

  return true;
}


WTaggedJets::~WTaggedJets() {}


BTaggedJets::BTaggedJets(Context & ctx,
			 BTag::algo btagalgo,
			 BTag::wp workingpoint,
			 const std::string & h_name_analysis,
			 const std::string & h_name_loose,
			 const std::string & h_name_medium,
			 const std::string & h_name_tight):
  h_btaggedjets_loose(ctx.get_handle<vector<Jet>>(h_name_loose)),
  h_btaggedjets_medium(ctx.get_handle<vector<Jet>>(h_name_medium)),
  h_btaggedjets_tight(ctx.get_handle<vector<Jet>>(h_name_tight)),
  h_btaggedjets_analysis(ctx.get_handle<vector<Jet>>(h_name_analysis)),
  m_btagalgo(btagalgo),
  m_workingpoint(workingpoint)
{}


bool BTaggedJets::process(uhh2::Event & event) {
  assert(event.jets);

  vector<Jet> bjets_loose, bjets_medium, bjets_tight;

  JetId loose_id = BTag(m_btagalgo, BTag::WP_LOOSE);
  JetId medium_id = BTag(m_btagalgo, BTag::WP_MEDIUM);
  JetId tight_id = BTag(m_btagalgo, BTag::WP_TIGHT);

  for(auto j : *event.jets) {
    if(loose_id(j, event)) bjets_loose.push_back(j);
    if(medium_id(j, event)) bjets_medium.push_back(j);
    if(tight_id(j, event)) bjets_tight.push_back(j);
  }

  if (m_workingpoint == BTag::WP_LOOSE) { event.set(h_btaggedjets_analysis, bjets_loose); }
  else if (m_workingpoint == BTag::WP_MEDIUM) { event.set(h_btaggedjets_analysis, bjets_medium); }
  else if (m_workingpoint == BTag::WP_TIGHT) { event.set(h_btaggedjets_analysis, bjets_tight); }

  event.set(h_btaggedjets_loose, std::move(bjets_loose));
  event.set(h_btaggedjets_medium, std::move(bjets_medium));
  event.set(h_btaggedjets_tight, std::move(bjets_tight));

  return true;
}


BTaggedJets::~BTaggedJets() {}


NonTopAK4Jets::NonTopAK4Jets(Context & ctx,
			     BTag::algo btagalgo,
			     BTag::wp workingpoint,
			     double toptag_radius,
			     const std::string & h_name_topEx_jets, // jets outside top tag
			     const std::string & h_name_topEx_bAnalysis,
			     const std::string & h_name_topEx_bLoose,
			     const std::string & h_name_topEx_bMedium,
			     const std::string & h_name_topEx_bTight,
			     const std::string & h_name_topIn_jets, // jets inside top tag (or rather jets that have an overlap with top tag)
			     const std::string & h_name_topIn_bAnalysis,
			     const std::string & h_name_topIn_bLoose,
			     const std::string & h_name_topIn_bMedium,
			     const std::string & h_name_topIn_bTight,
			     const std::string & h_name_TopTag,
			     const double rho):
  h_topEx_ak4(ctx.get_handle<vector<Jet>>(h_name_topEx_jets)),
  h_topEx_ak4_bLoose(ctx.get_handle<vector<Jet>>(h_name_topEx_bLoose)),
  h_topEx_ak4_bMedium(ctx.get_handle<vector<Jet>>(h_name_topEx_bMedium)),
  h_topEx_ak4_bTight(ctx.get_handle<vector<Jet>>(h_name_topEx_bTight)),
  h_topEx_ak4_bAnalysis(ctx.get_handle<vector<Jet>>(h_name_topEx_bAnalysis)),
  h_topIn_ak4(ctx.get_handle<vector<Jet>>(h_name_topIn_jets)),
  h_topIn_ak4_bLoose(ctx.get_handle<vector<Jet>>(h_name_topIn_bLoose)),
  h_topIn_ak4_bMedium(ctx.get_handle<vector<Jet>>(h_name_topIn_bMedium)),
  h_topIn_ak4_bTight(ctx.get_handle<vector<Jet>>(h_name_topIn_bTight)),
  h_topIn_ak4_bAnalysis(ctx.get_handle<vector<Jet>>(h_name_topIn_bAnalysis)),
  h_toptaggedjet(ctx.get_handle<TopJet>(h_name_TopTag)),
  m_rho(rho),
  m_btagalgo(btagalgo),
  m_workingpoint(workingpoint),
  m_toptag_radius(toptag_radius)
{}


bool NonTopAK4Jets::process(uhh2::Event & event) {
  assert(event.jets);

  vector<Jet> topEx_ak4, topEx_ak4_bLoose, topEx_ak4_bMedium, topEx_ak4_bTight;
  vector<Jet> topIn_ak4, topIn_ak4_bLoose, topIn_ak4_bMedium, topIn_ak4_bTight;

  JetId loose_id = BTag(m_btagalgo, BTag::WP_LOOSE);
  JetId medium_id = BTag(m_btagalgo, BTag::WP_MEDIUM);
  JetId tight_id = BTag(m_btagalgo, BTag::WP_TIGHT);

  TopJet toptaggedjet = event.get(h_toptaggedjet);

  for(auto j : *event.jets) {
    double dR = uhh2::deltaR(j.v4(), toptaggedjet.v4());
    double Reff = min(1.5, max(0.1, m_rho / ( toptaggedjet.v4().pt() * toptaggedjet.JEC_factor_raw() ) ));
    if(m_toptag_radius >= 0) Reff = m_toptag_radius;
    if(dR > Reff+0.4) { // need to add AK4 radius of 0.4 here in order to ensure that the AK4 jet is actually outside of t jet
      topEx_ak4.push_back(j);
      if(loose_id(j, event)) topEx_ak4_bLoose.push_back(j);
      if(medium_id(j, event)) topEx_ak4_bMedium.push_back(j);
      if(tight_id(j, event)) topEx_ak4_bTight.push_back(j);
    }
    else {
      topIn_ak4.push_back(j);
      if(loose_id(j, event)) topIn_ak4_bLoose.push_back(j);
      if(medium_id(j, event)) topIn_ak4_bMedium.push_back(j);
      if(tight_id(j, event)) topIn_ak4_bTight.push_back(j);
    }
  }

  if (m_workingpoint == BTag::WP_LOOSE) {
    event.set(h_topEx_ak4_bAnalysis, topEx_ak4_bLoose);
    event.set(h_topIn_ak4_bAnalysis, topIn_ak4_bLoose);
  }
  else if (m_workingpoint == BTag::WP_MEDIUM) {
    event.set(h_topEx_ak4_bAnalysis, topEx_ak4_bMedium);
    event.set(h_topIn_ak4_bAnalysis, topIn_ak4_bMedium);
  }
  else if (m_workingpoint == BTag::WP_TIGHT) {
    event.set(h_topEx_ak4_bAnalysis, topEx_ak4_bTight);
    event.set(h_topIn_ak4_bAnalysis, topIn_ak4_bTight);
  }

  event.set(h_topEx_ak4, std::move(topEx_ak4));
  event.set(h_topEx_ak4_bLoose, std::move(topEx_ak4_bLoose));
  event.set(h_topEx_ak4_bMedium, std::move(topEx_ak4_bMedium));
  event.set(h_topEx_ak4_bTight, std::move(topEx_ak4_bTight));

  event.set(h_topIn_ak4, std::move(topIn_ak4));
  event.set(h_topIn_ak4_bLoose, std::move(topIn_ak4_bLoose));
  event.set(h_topIn_ak4_bMedium, std::move(topIn_ak4_bMedium));
  event.set(h_topIn_ak4_bTight, std::move(topIn_ak4_bTight));

  return true;
}


NonTopAK4Jets::~NonTopAK4Jets() {}
