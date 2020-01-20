#include "UHH2/HighPtSingleTop/include/TaggedJets.h"

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

  TopJet toptaggedjet;

  for(auto t : *event.topjets) {
    if(m_topjetid(t, event)) toptaggedjet = t;
  }

  event.set(h_toptaggedjet, std::move(toptaggedjet));
  return true;
}


TopTaggedJet::~TopTaggedJet() {}



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
			     const std::string & h_name_nontopjets,
			     const std::string & h_name_bAnalysis,
			     const std::string & h_name_bLoose,
			     const std::string & h_name_bMedium,
			     const std::string & h_name_bTight,
			     const std::string & h_name_TopTag,
			     const double rho):
  h_nontopak4(ctx.get_handle<vector<Jet>>(h_name_nontopjets)),
  h_nontopak4_bLoose(ctx.get_handle<vector<Jet>>(h_name_bLoose)),
  h_nontopak4_bMedium(ctx.get_handle<vector<Jet>>(h_name_bMedium)),
  h_nontopak4_bTight(ctx.get_handle<vector<Jet>>(h_name_bTight)),
  h_nontopak4_bAnalysis(ctx.get_handle<vector<Jet>>(h_name_bAnalysis)),
  h_toptaggedjet(ctx.get_handle<TopJet>(h_name_TopTag)),
  m_rho(rho),
  m_btagalgo(btagalgo),
  m_workingpoint(workingpoint)
{}


bool NonTopAK4Jets::process(uhh2::Event & event) {
  assert(event.jets);

  vector<Jet> nontop_ak4, nontop_ak4_bLoose, nontop_ak4_bMedium, nontop_ak4_bTight;

  JetId loose_id = BTag(m_btagalgo, BTag::WP_LOOSE);
  JetId medium_id = BTag(m_btagalgo, BTag::WP_MEDIUM);
  JetId tight_id = BTag(m_btagalgo, BTag::WP_TIGHT);
  
  TopJet toptaggedjet = event.get(h_toptaggedjet);

  for(auto j : *event.jets) {
    double dR = uhh2::deltaR(j.v4(), toptaggedjet.v4());
    double Reff = min(1.5, max(0.1, m_rho/toptaggedjet.v4().pt()));
    if(dR > Reff) {
      nontop_ak4.push_back(j);
      if(loose_id(j, event)) nontop_ak4_bLoose.push_back(j);
      if(medium_id(j, event)) nontop_ak4_bMedium.push_back(j);
      if(tight_id(j, event)) nontop_ak4_bTight.push_back(j);
    }
  }

  if (m_workingpoint == BTag::WP_LOOSE) { event.set(h_nontopak4_bAnalysis, nontop_ak4_bLoose); }
  else if (m_workingpoint == BTag::WP_MEDIUM) { event.set(h_nontopak4_bAnalysis, nontop_ak4_bMedium); }
  else if (m_workingpoint == BTag::WP_TIGHT) { event.set(h_nontopak4_bAnalysis, nontop_ak4_bTight); }

  event.set(h_nontopak4, std::move(nontop_ak4));
  event.set(h_nontopak4_bLoose, std::move(nontop_ak4_bLoose));
  event.set(h_nontopak4_bMedium, std::move(nontop_ak4_bMedium));
  event.set(h_nontopak4_bTight, std::move(nontop_ak4_bTight));

  return true;
}


NonTopAK4Jets::~NonTopAK4Jets() {}
