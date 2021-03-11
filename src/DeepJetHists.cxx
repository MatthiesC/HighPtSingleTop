#include "UHH2/HighPtSingleTop/include/DeepJetHists.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"

#include "UHH2/common/include/Utils.h"

#include "TH1F.h"
#include "TH2F.h"
// #include "TH3F.h"

using namespace std;
using namespace uhh2;


DeepJetHists::DeepJetHists(Context & ctx, const string & dirname):
  Hists(ctx, dirname) {

  m_h_all_bloose = ctx.get_handle<vector<Jet>>("BJetsLoose");
  m_h_all_bmedium = ctx.get_handle<vector<Jet>>("BJetsMedium");
  m_h_all_btight = ctx.get_handle<vector<Jet>>("BJetsTight");

  const int nBins = 11;

  hist_n_all = book<TH1F>("n_all", "N(jets)", nBins, -0.5, 10.5);
  hist_n_all_bloose = book<TH1F>("n_all_bloose", "N(loose b jets)", nBins, -0.5, 10.5);
  hist_n_all_bmedium = book<TH1F>("n_all_bmedium", "N(medium b jets)", nBins, -0.5, 10.5);
  hist_n_all_btight = book<TH1F>("n_all_btight", "N(tight b jets)", nBins, -0.5, 10.5);

  hist_n_all_b_loose_medium_2d = book<TH2F>("n_all_b_loose_medium_2d", "N(b jets) (x: loose, y: medium)", nBins, -0.5, 10.5, nBins, -0.5, 10.5);
  hist_n_all_b_loose_tight_2d = book<TH2F>("n_all_b_loose_tight_2d", "N(b jets) (x: loose, y: tight)", nBins, -0.5, 10.5, nBins, -0.5, 10.5);
  hist_n_all_b_medium_tight_2d = book<TH2F>("n_all_b_medium_tight_2d", "N(b jets) (x: medium, y: tight)", nBins, -0.5, 10.5, nBins, -0.5, 10.5);

  hist_jet1dj_dj = book<TH1F>("jet1dj_dj", "#it{O}_{DeepJet}(#it{O}_{DeepJet}-leading jet)", 100, 0, 1);
  hist_jet1dj_pt = book<TH1F>("jet1dj_pt", "p_{T}(#it{O}_{DeepJet}-leading jet) [GeV]", 100, 0, 1000);
  hist_jet2dj_dj = book<TH1F>("jet2dj_dj", "#it{O}_{DeepJet}(#it{O}_{DeepJet}-subleading jet)", 100, 0, 1);
  hist_jet2dj_pt = book<TH1F>("jet2dj_pt", "p_{T}(#it{O}_{DeepJet}-subleading jet) [GeV]", 100, 0, 1000);
  hist_jet3dj_dj = book<TH1F>("jet3dj_dj", "#it{O}_{DeepJet}(#it{O}_{DeepJet}-subsubleading jet)", 100, 0, 1);
  hist_jet3dj_pt = book<TH1F>("jet3dj_pt", "p_{T}(#it{O}_{DeepJet}-subsubleading jet) [GeV]", 100, 0, 1000);

  hist_jet1pt_dj = book<TH1F>("jet1pt_dj", "#it{O}_{DeepJet}(p_{T}-leading jet)", 100, 0, 1);
  hist_jet1pt_pt = book<TH1F>("jet1pt_pt", "p_{T}(p_{T}-leading jet) [GeV]", 100, 0, 1000);
  hist_jet2pt_dj = book<TH1F>("jet2pt_dj", "#it{O}_{DeepJet}(p_{T}-subleading jet)", 100, 0, 1);
  hist_jet2pt_pt = book<TH1F>("jet2pt_pt", "p_{T}(p_{T}-subleading jet) [GeV]", 100, 0, 1000);
  hist_jet3pt_dj = book<TH1F>("jet3pt_dj", "#it{O}_{DeepJet}(p_{T}-subsubleading jet)", 100, 0, 1);
  hist_jet3pt_pt = book<TH1F>("jet3pt_pt", "p_{T}(p_{T}-subsubleading jet) [GeV]", 100, 0, 1000);
}


void DeepJetHists::fill(const uhh2::Event & event) {

  const auto & all = *event.jets;
  vector<Jet> all_sortedByDj = all;
  sort_by_deepjet(all_sortedByDj);
  const auto & all_bloose = event.get(m_h_all_bloose);
  const auto & all_bmedium = event.get(m_h_all_bmedium);
  const auto & all_btight = event.get(m_h_all_btight);

  const double w = event.weight;

  hist_n_all->Fill(all.size(), w);
  hist_n_all_bloose->Fill(all_bloose.size(), w);
  hist_n_all_bmedium->Fill(all_bmedium.size(), w);
  hist_n_all_btight->Fill(all_btight.size(), w);

  hist_n_all_b_loose_medium_2d->Fill(all_bloose.size(), all_bmedium.size(), w);
  hist_n_all_b_loose_tight_2d->Fill(all_bloose.size(), all_btight.size(), w);
  hist_n_all_b_medium_tight_2d->Fill(all_bmedium.size(), all_btight.size(), w);

  if(all.size() > 0) hist_jet1dj_dj->Fill(all_sortedByDj.at(0).btag_DeepJet(), w);
  if(all.size() > 0) hist_jet1dj_pt->Fill(all_sortedByDj.at(0).v4().Pt(), w);
  if(all.size() > 1) hist_jet2dj_dj->Fill(all_sortedByDj.at(1).btag_DeepJet(), w);
  if(all.size() > 1) hist_jet2dj_pt->Fill(all_sortedByDj.at(1).v4().Pt(), w);
  if(all.size() > 2) hist_jet3dj_dj->Fill(all_sortedByDj.at(2).btag_DeepJet(), w);
  if(all.size() > 2) hist_jet3dj_pt->Fill(all_sortedByDj.at(2).v4().Pt(), w);

  if(all.size() > 0) hist_jet1pt_dj->Fill(all.at(0).btag_DeepJet(), w);
  if(all.size() > 0) hist_jet1pt_pt->Fill(all.at(0).v4().Pt(), w);
  if(all.size() > 1) hist_jet2pt_dj->Fill(all.at(1).btag_DeepJet(), w);
  if(all.size() > 1) hist_jet2pt_pt->Fill(all.at(1).v4().Pt(), w);
  if(all.size() > 2) hist_jet3pt_dj->Fill(all.at(2).btag_DeepJet(), w);
  if(all.size() > 2) hist_jet3pt_pt->Fill(all.at(2).v4().Pt(), w);
}
