#include "UHH2/HighPtSingleTop/include/BTagHists.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"

#include "UHH2/common/include/Utils.h"

#include "TH1F.h"
#include "TH2F.h"
// #include "TH3F.h"

using namespace std;
using namespace uhh2;


BTagHists::BTagHists(Context & ctx, const string & dirname, const BTag::algo & algo):
  Hists(ctx, dirname) {

  m_btag_algo = algo;

  // m_h_all_bloose = ctx.get_handle<vector<Jet>>("BJetsLoose");
  // m_h_all_bmedium = ctx.get_handle<vector<Jet>>("BJetsMedium");
  // m_h_all_btight = ctx.get_handle<vector<Jet>>("BJetsTight");

  const int nBins = 11;

  hist_n_all = book<TH1F>("n_all", "N(jets)", nBins, -0.5, 10.5);
  hist_n_all_bloose = book<TH1F>("n_all_bloose", "N(loose b jets)", nBins, -0.5, 10.5);
  hist_n_all_bmedium = book<TH1F>("n_all_bmedium", "N(medium b jets)", nBins, -0.5, 10.5);
  hist_n_all_btight = book<TH1F>("n_all_btight", "N(tight b jets)", nBins, -0.5, 10.5);

  hist_n_all_b_loose_medium_2d = book<TH2F>("n_all_b_loose_medium_2d", "N(b jets) (x: loose, y: medium)", nBins, -0.5, 10.5, nBins, -0.5, 10.5);
  hist_n_all_b_loose_tight_2d = book<TH2F>("n_all_b_loose_tight_2d", "N(b jets) (x: loose, y: tight)", nBins, -0.5, 10.5, nBins, -0.5, 10.5);
  hist_n_all_b_medium_tight_2d = book<TH2F>("n_all_b_medium_tight_2d", "N(b jets) (x: medium, y: tight)", nBins, -0.5, 10.5, nBins, -0.5, 10.5);

  hist_jet1btag_btag = book<TH1F>("jet1btag_btag", "#it{O}_{b-tag}(#it{O}_{b-tag}-leading jet)", 100, 0, 1);
  hist_jet1btag_pt = book<TH1F>("jet1btag_pt", "p_{T}(#it{O}_{b-tag}-leading jet) [GeV]", 100, 0, 1000);
  hist_jet2btag_btag = book<TH1F>("jet2btag_btag", "#it{O}_{b-tag}(#it{O}_{b-tag}-subleading jet)", 100, 0, 1);
  hist_jet2btag_pt = book<TH1F>("jet2btag_pt", "p_{T}(#it{O}_{b-tag}-subleading jet) [GeV]", 100, 0, 1000);
  hist_jet3btag_btag = book<TH1F>("jet3btag_btag", "#it{O}_{b-tag}(#it{O}_{b-tag}-subsubleading jet)", 100, 0, 1);
  hist_jet3btag_pt = book<TH1F>("jet3btag_pt", "p_{T}(#it{O}_{b-tag}-subsubleading jet) [GeV]", 100, 0, 1000);

  hist_jet1pt_btag = book<TH1F>("jet1pt_btag", "#it{O}_{b-tag}(p_{T}-leading jet)", 100, 0, 1);
  hist_jet1pt_pt = book<TH1F>("jet1pt_pt", "p_{T}(p_{T}-leading jet) [GeV]", 100, 0, 1000);
  hist_jet2pt_btag = book<TH1F>("jet2pt_btag", "#it{O}_{b-tag}(p_{T}-subleading jet)", 100, 0, 1);
  hist_jet2pt_pt = book<TH1F>("jet2pt_pt", "p_{T}(p_{T}-subleading jet) [GeV]", 100, 0, 1000);
  hist_jet3pt_btag = book<TH1F>("jet3pt_btag", "#it{O}_{b-tag}(p_{T}-subsubleading jet)", 100, 0, 1);
  hist_jet3pt_pt = book<TH1F>("jet3pt_pt", "p_{T}(p_{T}-subsubleading jet) [GeV]", 100, 0, 1000);
}


void BTagHists::fill(const Event & event) {

  const auto & all = *event.jets;
  vector<Jet> all_sortedByBTag = all;
  sort_by_btagdisc(all_sortedByBTag, m_btag_algo);

  // const auto & all_bloose = event.get(m_h_all_bloose);
  // const auto & all_bmedium = event.get(m_h_all_bmedium);
  // const auto & all_btight = event.get(m_h_all_btight);

  vector<Jet> all_bloose, all_bmedium, all_btight;
  JetId loose_id = BTag(m_btag_algo, BTag::WP_LOOSE);
  JetId medium_id = BTag(m_btag_algo, BTag::WP_MEDIUM);
  JetId tight_id = BTag(m_btag_algo, BTag::WP_TIGHT);
  for(auto jet : all) {
    if(loose_id(jet, event)) all_bloose.push_back(jet);
    if(medium_id(jet, event)) all_bmedium.push_back(jet);
    if(tight_id(jet, event)) all_btight.push_back(jet);
  }

  const double w = event.weight;

  hist_n_all->Fill(all.size(), w);
  hist_n_all_bloose->Fill(all_bloose.size(), w);
  hist_n_all_bmedium->Fill(all_bmedium.size(), w);
  hist_n_all_btight->Fill(all_btight.size(), w);

  hist_n_all_b_loose_medium_2d->Fill(all_bloose.size(), all_bmedium.size(), w);
  hist_n_all_b_loose_tight_2d->Fill(all_bloose.size(), all_btight.size(), w);
  hist_n_all_b_medium_tight_2d->Fill(all_bmedium.size(), all_btight.size(), w);

  if(all.size() > 0) hist_jet1btag_btag->Fill(btagdisc(all_sortedByBTag.at(0), m_btag_algo), w);
  if(all.size() > 0) hist_jet1btag_pt->Fill(all_sortedByBTag.at(0).v4().Pt(), w);
  if(all.size() > 1) hist_jet2btag_btag->Fill(btagdisc(all_sortedByBTag.at(1), m_btag_algo), w);
  if(all.size() > 1) hist_jet2btag_pt->Fill(all_sortedByBTag.at(1).v4().Pt(), w);
  if(all.size() > 2) hist_jet3btag_btag->Fill(btagdisc(all_sortedByBTag.at(2), m_btag_algo), w);
  if(all.size() > 2) hist_jet3btag_pt->Fill(all_sortedByBTag.at(2).v4().Pt(), w);

  if(all.size() > 0) hist_jet1pt_btag->Fill(btagdisc(all.at(0), m_btag_algo), w);
  if(all.size() > 0) hist_jet1pt_pt->Fill(all.at(0).v4().Pt(), w);
  if(all.size() > 1) hist_jet2pt_btag->Fill(btagdisc(all.at(1), m_btag_algo), w);
  if(all.size() > 1) hist_jet2pt_pt->Fill(all.at(1).v4().Pt(), w);
  if(all.size() > 2) hist_jet3pt_btag->Fill(btagdisc(all.at(2), m_btag_algo), w);
  if(all.size() > 2) hist_jet3pt_pt->Fill(all.at(2).v4().Pt(), w);
}
