#include "UHH2/HighPtSingleTop/include/MatchHists.h"

#include "UHH2/common/include/Utils.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;


MatchHists::MatchHists(Context & ctx, const string & dirname, const string & object_name, double arg_MIN_PT, double arg_MAX_PT):
  Hists(ctx, dirname) {

  h_GENtW = ctx.get_handle<SingleTopGen_tWch>("h_GENtW");
  h_taggedjet = ctx.get_handle<TopJet>(object_name+"TaggedJet");
  h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");

  m_MIN_PT = arg_MIN_PT;
  m_MAX_PT = arg_MAX_PT;

  is_TopTagRegion = (object_name == "Top");
  is_WTagRegion = (object_name == "W");

  const int nx = 16;
  const char *decayChannels[nx] = {"t_{e}W_{e}", "t_{#mu}W_{e}", "t_{#tau}W_{e}", "t_{h}W_{e}", "t_{e}W_{#mu}", "t_{#mu}W_{#mu}", "t_{#tau}W_{#mu}", "t_{h}W_{#mu}", "t_{e}W_{#tau}", "t_{#mu}W_{#tau}", "t_{#tau}W_{#tau}", "t_{h}W_{#tau}", "t_{e}W_{h}", "t_{#mu}W_{h}", "t_{#tau}W_{h}", "t_{h}W_{h}"};

  const int nBins = 100;

  if(!(is_TopTagRegion || is_WTagRegion)) throw invalid_argument("MatchHists: Invalid object name.");
  const string label_taggedjet = is_TopTagRegion ? "t jet" : (is_WTagRegion ? "W jet" : "");


   // all matching scenarios

  hist_all_decayChannel = book<TH1F>("all_DecayChannel", "tW decay channel", nx, -0.5, 15.5);
  for(int i = 1; i <= nx; ++i) hist_all_decayChannel->GetXaxis()->SetBinLabel(i,decayChannels[i-1]);

  hist_all_deltaR_taggedjet_gentop = book<TH1F>("all_deltaR_taggedjet_gentop", ("#DeltaR("+label_taggedjet+", gen. t quark)").c_str(), nBins, 0, 5);
  hist_all_deltaR_taggedjet_genwass = book<TH1F>("all_deltaR_taggedjet_genwass", ("#DeltaR("+label_taggedjet+", gen. asso. W boson)").c_str(), nBins, 0, 5);
  hist_all_deltaR_taggedjet_genwtop = book<TH1F>("all_deltaR_taggedjet_genwtop", ("#DeltaR("+label_taggedjet+", gen. W boson from t decay)").c_str(), nBins, 0, 5);
  hist_all_deltaR_lepton_gentop = book<TH1F>("all_deltaR_lepton_gentop", ("#DeltaR("+label_taggedjet+", gen. t quark)").c_str(), nBins, 0, 5);
  hist_all_deltaR_lepton_genwass = book<TH1F>("all_deltaR_lepton_genwass", ("#DeltaR("+label_taggedjet+", gen. asso. W boson)").c_str(), nBins, 0, 5);
  hist_all_deltaR_lepton_genwtop = book<TH1F>("all_deltaR_lepton_genwtop", ("#DeltaR("+label_taggedjet+", gen. W boson from t decay)").c_str(), nBins, 0, 5);

  // 3 quarks matching scenario

  hist_3matched_decayChannel = book<TH1F>("3matched_DecayChannel", "tW decay channel", nx, -0.5, 15.5);
  for(int i = 1; i <= nx; ++i) hist_3matched_decayChannel->GetXaxis()->SetBinLabel(i,decayChannels[i-1]);

  hist_3matched_deltaR_taggedjet_gentop = book<TH1F>("3matched_deltaR_taggedjet_gentop", ("#DeltaR("+label_taggedjet+", gen. t quark)").c_str(), nBins, 0, 5);
  hist_3matched_deltaR_taggedjet_genwass = book<TH1F>("3matched_deltaR_taggedjet_genwass", ("#DeltaR("+label_taggedjet+", gen. asso. W boson)").c_str(), nBins, 0, 5);
  hist_3matched_deltaR_taggedjet_genwtop = book<TH1F>("3matched_deltaR_taggedjet_genwtop", ("#DeltaR("+label_taggedjet+", gen. W boson from t decay)").c_str(), nBins, 0, 5);
  hist_3matched_deltaR_lepton_gentop = book<TH1F>("3matched_deltaR_lepton_gentop", ("#DeltaR("+label_taggedjet+", gen. t quark)").c_str(), nBins, 0, 5);
  hist_3matched_deltaR_lepton_genwass = book<TH1F>("3matched_deltaR_lepton_genwass", ("#DeltaR("+label_taggedjet+", gen. asso. W boson)").c_str(), nBins, 0, 5);
  hist_3matched_deltaR_lepton_genwtop = book<TH1F>("3matched_deltaR_lepton_genwtop", ("#DeltaR("+label_taggedjet+", gen. W boson from t decay)").c_str(), nBins, 0, 5);

  // 2 quarks matching scenario

  hist_2matched_decayChannel = book<TH1F>("2matched_DecayChannel", "tW decay channel", nx, -0.5, 15.5);
  for(int i = 1; i <= nx; ++i) hist_2matched_decayChannel->GetXaxis()->SetBinLabel(i,decayChannels[i-1]);

  hist_2matched_deltaR_taggedjet_gentop = book<TH1F>("2matched_deltaR_taggedjet_gentop", ("#DeltaR("+label_taggedjet+", gen. t quark)").c_str(), nBins, 0, 5);
  hist_2matched_deltaR_taggedjet_genwass = book<TH1F>("2matched_deltaR_taggedjet_genwass", ("#DeltaR("+label_taggedjet+", gen. asso. W boson)").c_str(), nBins, 0, 5);
  hist_2matched_deltaR_taggedjet_genwtop = book<TH1F>("2matched_deltaR_taggedjet_genwtop", ("#DeltaR("+label_taggedjet+", gen. W boson from t decay)").c_str(), nBins, 0, 5);
  hist_2matched_deltaR_lepton_gentop = book<TH1F>("2matched_deltaR_lepton_gentop", ("#DeltaR("+label_taggedjet+", gen. t quark)").c_str(), nBins, 0, 5);
  hist_2matched_deltaR_lepton_genwass = book<TH1F>("2matched_deltaR_lepton_genwass", ("#DeltaR("+label_taggedjet+", gen. asso. W boson)").c_str(), nBins, 0, 5);
  hist_2matched_deltaR_lepton_genwtop = book<TH1F>("2matched_deltaR_lepton_genwtop", ("#DeltaR("+label_taggedjet+", gen. W boson from t decay)").c_str(), nBins, 0, 5);

  // 1 quark matching scenario

  hist_1matched_decayChannel = book<TH1F>("1matched_DecayChannel", "tW decay channel", nx, -0.5, 15.5);
  for(int i = 1; i <= nx; ++i) hist_1matched_decayChannel->GetXaxis()->SetBinLabel(i,decayChannels[i-1]);

  hist_1matched_deltaR_taggedjet_gentop = book<TH1F>("1matched_deltaR_taggedjet_gentop", ("#DeltaR("+label_taggedjet+", gen. t quark)").c_str(), nBins, 0, 5);
  hist_1matched_deltaR_taggedjet_genwass = book<TH1F>("1matched_deltaR_taggedjet_genwass", ("#DeltaR("+label_taggedjet+", gen. asso. W boson)").c_str(), nBins, 0, 5);
  hist_1matched_deltaR_taggedjet_genwtop = book<TH1F>("1matched_deltaR_taggedjet_genwtop", ("#DeltaR("+label_taggedjet+", gen. W boson from t decay)").c_str(), nBins, 0, 5);
  hist_1matched_deltaR_lepton_gentop = book<TH1F>("1matched_deltaR_lepton_gentop", ("#DeltaR("+label_taggedjet+", gen. t quark)").c_str(), nBins, 0, 5);
  hist_1matched_deltaR_lepton_genwass = book<TH1F>("1matched_deltaR_lepton_genwass", ("#DeltaR("+label_taggedjet+", gen. asso. W boson)").c_str(), nBins, 0, 5);
  hist_1matched_deltaR_lepton_genwtop = book<TH1F>("1matched_deltaR_lepton_genwtop", ("#DeltaR("+label_taggedjet+", gen. W boson from t decay)").c_str(), nBins, 0, 5);

  // 0 quarks matching scenario

  hist_0matched_decayChannel = book<TH1F>("0matched_DecayChannel", "tW decay channel", nx, -0.5, 15.5);
  for(int i = 1; i <= nx; ++i) hist_0matched_decayChannel->GetXaxis()->SetBinLabel(i,decayChannels[i-1]);

  hist_0matched_deltaR_taggedjet_gentop = book<TH1F>("0matched_deltaR_taggedjet_gentop", ("#DeltaR("+label_taggedjet+", gen. t quark)").c_str(), nBins, 0, 5);
  hist_0matched_deltaR_taggedjet_genwass = book<TH1F>("0matched_deltaR_taggedjet_genwass", ("#DeltaR("+label_taggedjet+", gen. asso. W boson)").c_str(), nBins, 0, 5);
  hist_0matched_deltaR_taggedjet_genwtop = book<TH1F>("0matched_deltaR_taggedjet_genwtop", ("#DeltaR("+label_taggedjet+", gen. W boson from t decay)").c_str(), nBins, 0, 5);
  hist_0matched_deltaR_lepton_gentop = book<TH1F>("0matched_deltaR_lepton_gentop", ("#DeltaR("+label_taggedjet+", gen. t quark)").c_str(), nBins, 0, 5);
  hist_0matched_deltaR_lepton_genwass = book<TH1F>("0matched_deltaR_lepton_genwass", ("#DeltaR("+label_taggedjet+", gen. asso. W boson)").c_str(), nBins, 0, 5);
  hist_0matched_deltaR_lepton_genwtop = book<TH1F>("0matched_deltaR_lepton_genwtop", ("#DeltaR("+label_taggedjet+", gen. W boson from t decay)").c_str(), nBins, 0, 5);
}


int MatchHists::get_number_of_matched_quarks(const TopJet & taggedjet, const vector<GenParticle> & daughters) {

  int nMatched(0);
  double deltaRmatch(0.8); // AK8 radius
  if(is_TopTagRegion) deltaRmatch = min(1.5, max(0.1, 600.0 / (taggedjet.pt() * taggedjet.JEC_factor_raw()))); // HOTVR radius

  for(GenParticle gp : daughters) {
    if(deltaRmatch > deltaR(gp.v4(), taggedjet.v4())) ++nMatched;
  }

  return nMatched;
}


void MatchHists::fill(const Event & event) {

  if(!event.is_valid(h_GENtW)) return;

  const auto & GENtW = event.get(h_GENtW);
  const auto & taggedjet = event.get(h_taggedjet);
  const auto & lepton = event.get(h_primlep);

  const double w = event.weight;

  if(taggedjet.v4().pt() >= m_MIN_PT && taggedjet.v4().pt() < m_MAX_PT) {

    hist_all_decayChannel->Fill(GENtW.DecayChannel(), w);

    hist_all_deltaR_taggedjet_gentop->Fill(deltaR(GENtW.Top().v4(), taggedjet.v4()), w);
    hist_all_deltaR_taggedjet_genwass->Fill(deltaR(GENtW.WAss().v4(), taggedjet.v4()), w);
    hist_all_deltaR_taggedjet_genwtop->Fill(deltaR(GENtW.WTop().v4(), taggedjet.v4()), w);
    hist_all_deltaR_lepton_gentop->Fill(deltaR(GENtW.Top().v4(), lepton.v4()), w);
    hist_all_deltaR_lepton_genwass->Fill(deltaR(GENtW.WAss().v4(), lepton.v4()), w);
    hist_all_deltaR_lepton_genwtop->Fill(deltaR(GENtW.WTop().v4(), lepton.v4()), w);

    vector<GenParticle> daughters;
    if(is_TopTagRegion) {
      daughters.push_back(GENtW.bTop());
      daughters.push_back(GENtW.WTopDecay1());
      daughters.push_back(GENtW.WTopDecay2());
    }
    else {
      daughters.push_back(GENtW.WAssDecay1());
      daughters.push_back(GENtW.WAssDecay2());
    }

    const int nMatched = get_number_of_matched_quarks(taggedjet, daughters);

    if(nMatched == 3) {

      hist_3matched_decayChannel->Fill(GENtW.DecayChannel(), w);

      hist_3matched_deltaR_taggedjet_gentop->Fill(deltaR(GENtW.Top().v4(), taggedjet.v4()), w);
      hist_3matched_deltaR_taggedjet_genwass->Fill(deltaR(GENtW.WAss().v4(), taggedjet.v4()), w);
      hist_3matched_deltaR_taggedjet_genwtop->Fill(deltaR(GENtW.WTop().v4(), taggedjet.v4()), w);
      hist_3matched_deltaR_lepton_gentop->Fill(deltaR(GENtW.Top().v4(), lepton.v4()), w);
      hist_3matched_deltaR_lepton_genwass->Fill(deltaR(GENtW.WAss().v4(), lepton.v4()), w);
      hist_3matched_deltaR_lepton_genwtop->Fill(deltaR(GENtW.WTop().v4(), lepton.v4()), w);
    }
    else if(nMatched == 2) {

      hist_2matched_decayChannel->Fill(GENtW.DecayChannel(), w);

      hist_2matched_deltaR_taggedjet_gentop->Fill(deltaR(GENtW.Top().v4(), taggedjet.v4()), w);
      hist_2matched_deltaR_taggedjet_genwass->Fill(deltaR(GENtW.WAss().v4(), taggedjet.v4()), w);
      hist_2matched_deltaR_taggedjet_genwtop->Fill(deltaR(GENtW.WTop().v4(), taggedjet.v4()), w);
      hist_2matched_deltaR_lepton_gentop->Fill(deltaR(GENtW.Top().v4(), lepton.v4()), w);
      hist_2matched_deltaR_lepton_genwass->Fill(deltaR(GENtW.WAss().v4(), lepton.v4()), w);
      hist_2matched_deltaR_lepton_genwtop->Fill(deltaR(GENtW.WTop().v4(), lepton.v4()), w);
    }
    else if(nMatched == 1) {

      hist_1matched_decayChannel->Fill(GENtW.DecayChannel(), w);

      hist_1matched_deltaR_taggedjet_gentop->Fill(deltaR(GENtW.Top().v4(), taggedjet.v4()), w);
      hist_1matched_deltaR_taggedjet_genwass->Fill(deltaR(GENtW.WAss().v4(), taggedjet.v4()), w);
      hist_1matched_deltaR_taggedjet_genwtop->Fill(deltaR(GENtW.WTop().v4(), taggedjet.v4()), w);
      hist_1matched_deltaR_lepton_gentop->Fill(deltaR(GENtW.Top().v4(), lepton.v4()), w);
      hist_1matched_deltaR_lepton_genwass->Fill(deltaR(GENtW.WAss().v4(), lepton.v4()), w);
      hist_1matched_deltaR_lepton_genwtop->Fill(deltaR(GENtW.WTop().v4(), lepton.v4()), w);
    }
    else { // nMatched == 0

      hist_0matched_decayChannel->Fill(GENtW.DecayChannel(), w);

      hist_0matched_deltaR_taggedjet_gentop->Fill(deltaR(GENtW.Top().v4(), taggedjet.v4()), w);
      hist_0matched_deltaR_taggedjet_genwass->Fill(deltaR(GENtW.WAss().v4(), taggedjet.v4()), w);
      hist_0matched_deltaR_taggedjet_genwtop->Fill(deltaR(GENtW.WTop().v4(), taggedjet.v4()), w);
      hist_0matched_deltaR_lepton_gentop->Fill(deltaR(GENtW.Top().v4(), lepton.v4()), w);
      hist_0matched_deltaR_lepton_genwass->Fill(deltaR(GENtW.WAss().v4(), lepton.v4()), w);
      hist_0matched_deltaR_lepton_genwtop->Fill(deltaR(GENtW.WTop().v4(), lepton.v4()), w);
    }
  }
}
