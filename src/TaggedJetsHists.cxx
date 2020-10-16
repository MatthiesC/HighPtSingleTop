#include "UHH2/HighPtSingleTop/include/TaggedJetsHists.h"

#include "UHH2/common/include/Utils.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"

using namespace std;
using namespace uhh2;


TaggedJetsHists::TaggedJetsHists(Context & ctx, const string & dirname, const string & h_name_taggedjet, const string & h_name_postfix, double arg_MIN_PT, double arg_MAX_PT):
  Hists(ctx, dirname) {

  m_h_taggedjet = ctx.get_handle<TopJet>(h_name_taggedjet);

  m_h_all_bloose = ctx.get_handle<vector<Jet>>("BJetsLoose");
  m_h_all_bmedium = ctx.get_handle<vector<Jet>>("BJetsMedium");
  m_h_all_btight = ctx.get_handle<vector<Jet>>("BJetsTight");

  m_h_ex = ctx.get_handle<vector<Jet>>("ExJets"+h_name_postfix);
  m_h_ex_bloose = ctx.get_handle<vector<Jet>>("ExBJetsLoose"+h_name_postfix);
  m_h_ex_bmedium = ctx.get_handle<vector<Jet>>("ExBJetsMedium"+h_name_postfix);
  m_h_ex_btight = ctx.get_handle<vector<Jet>>("ExBJetsTight"+h_name_postfix);

  m_h_in = ctx.get_handle<vector<Jet>>("InJets"+h_name_postfix);
  m_h_in_bloose = ctx.get_handle<vector<Jet>>("InBJetsLoose"+h_name_postfix);
  m_h_in_bmedium = ctx.get_handle<vector<Jet>>("InBJetsMedium"+h_name_postfix);
  m_h_in_btight = ctx.get_handle<vector<Jet>>("InBJetsTight"+h_name_postfix);

  m_MIN_PT = arg_MIN_PT;
  m_MAX_PT = arg_MAX_PT;

  hist_n_all = book<TH1F>("n_all", "N(jets)", 10, -0.5, 10.5);
  hist_n_all_bloose = book<TH1F>("n_all_bloose", "N(loose b jets)", 10, -0.5, 10.5);
  hist_n_all_bmedium = book<TH1F>("n_all_bmedium", "N(medium b jets)", 10, -0.5, 10.5);
  hist_n_all_btight = book<TH1F>("n_all_btight", "N(tight b jets)", 10, -0.5, 10.5);

  hist_n_all_b_loose_medium_2d = book<TH2F>("n_all_b_loose_medium_2d", "N(b jets) (x: loose, y: medium)", 10, -0.5, 10.5, 10, -0.5, 10.5);
  hist_n_all_b_loose_tight_2d = book<TH2F>("n_all_b_loose_tight_2d", "N(b jets) (x: loose, y: tight)", 10, -0.5, 10.5, 10, -0.5, 10.5);
  hist_n_all_b_medium_tight_2d = book<TH2F>("n_all_b_medium_tight_2d", "N(b jets) (x: medium, y: tight)", 10, -0.5, 10.5, 10, -0.5, 10.5);
  hist_n_all_b_all_3d = book<TH3F>("n_all_b_all_3d", "N(b jets) (x: loose, y: medium, z: tight)", 10, -0.5, 10.5, 10, -0.5, 10.5, 10, -0.5, 10.5);

  hist_n_ex = book<TH1F>("n_ex", "N(xjets)", 10, -0.5, 10.5);
  hist_n_ex_bloose = book<TH1F>("n_ex_bloose", "N(loose b xjets)", 10, -0.5, 10.5);
  hist_n_ex_bmedium = book<TH1F>("n_ex_bmedium", "N(medium b xjets)", 10, -0.5, 10.5);
  hist_n_ex_btight = book<TH1F>("n_ex_btight", "N(tight b xjets)", 10, -0.5, 10.5);

  hist_n_in = book<TH1F>("n_in", "N(ijets)", 10, -0.5, 10.5);
  hist_n_in_bloose = book<TH1F>("n_in_bloose", "N(loose b ijets)", 10, -0.5, 10.5);
  hist_n_in_bmedium = book<TH1F>("n_in_bmedium", "N(medium b ijets)", 10, -0.5, 10.5);
  hist_n_in_btight = book<TH1F>("n_in_btight", "N(tight b ijets)", 10, -0.5, 10.5);

  hist_n_inex_b_loose_2d = book<TH2F>("n_inex_b_loose_2d", "N(loose b jets) (x: xjets, y: ijets)", 10, -0.5, 10.5, 10, -0.5, 10.5);
  hist_n_inex_b_medium_2d = book<TH2F>("n_inex_b_medium_2d", "N(medium b jets) (x: xjets, y: ijets)", 10, -0.5, 10.5, 10, -0.5, 10.5);
  hist_n_inex_b_tight_2d = book<TH2F>("n_inex_b_tight_2d", "N(tight b jets) (x: xjets, y: ijets)", 10, -0.5, 10.5, 10, -0.5, 10.5);
}


void TaggedJetsHists::fill(const uhh2::Event & event) {

  const auto & taggedjet = event.get(m_h_taggedjet);

  const auto & all = *event.jets;
  const auto & all_bloose = event.get(m_h_all_bloose);
  const auto & all_bmedium = event.get(m_h_all_bmedium);
  const auto & all_btight = event.get(m_h_all_btight);

  const auto & ex = event.get(m_h_ex);
  const auto & ex_bloose = event.get(m_h_ex_bloose);
  const auto & ex_bmedium = event.get(m_h_ex_bmedium);
  const auto & ex_btight = event.get(m_h_ex_btight);

  const auto & in = event.get(m_h_in);
  const auto & in_bloose = event.get(m_h_in_bloose);
  const auto & in_bmedium = event.get(m_h_in_bmedium);
  const auto & in_btight = event.get(m_h_in_btight);

  const double w = event.weight;


  if(taggedjet.v4().pt() >= m_MIN_PT && taggedjet.v4().pt() < m_MAX_PT) {

    hist_n_all->Fill(all.size(), w);
    hist_n_all_bloose->Fill(all_bloose.size(), w);
    hist_n_all_bmedium->Fill(all_bmedium.size(), w);
    hist_n_all_btight->Fill(all_btight.size(), w);

    hist_n_all_b_loose_medium_2d->Fill(all_bloose.size(), all_bmedium.size(), w);
    hist_n_all_b_loose_tight_2d->Fill(all_bloose.size(), all_btight.size(), w);
    hist_n_all_b_medium_tight_2d->Fill(all_bmedium.size(), all_btight.size(), w);
    hist_n_all_b_all_3d->Fill(all_bloose.size(), all_bmedium.size(), all_btight.size(), w);

    hist_n_ex->Fill(ex.size(), w);
    hist_n_ex_bloose->Fill(ex_bloose.size(), w);
    hist_n_ex_bmedium->Fill(ex_bmedium.size(), w);
    hist_n_ex_btight->Fill(ex_btight.size(), w);

    hist_n_in->Fill(in.size(), w);
    hist_n_in_bloose->Fill(in_bloose.size(), w);
    hist_n_in_bmedium->Fill(in_bmedium.size(), w);
    hist_n_in_btight->Fill(in_btight.size(), w);

    hist_n_inex_b_loose_2d->Fill(ex_bloose.size(), in_bloose.size(), w);
    hist_n_inex_b_medium_2d->Fill(ex_bmedium.size(), in_bmedium.size(), w);
    hist_n_inex_b_tight_2d->Fill(ex_btight.size(), in_btight.size(), w);
  }

}
