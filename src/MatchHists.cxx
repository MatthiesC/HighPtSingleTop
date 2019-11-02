#include "UHH2/HighPtSingleTop/include/MatchHists.h"

#include "UHH2/common/include/Utils.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;


MatchHists::MatchHists(Context & ctx, const string & dirname):
  Hists(ctx, dirname) {
  
  h_GENtW = ctx.get_handle<SingleTopGen_tWch>("h_GENtW");
  h_toptaggedjet = ctx.get_handle<TopJet>("TopTaggedJet");
  h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");

  hist_deltaR_topQuark_topRecoJet = book<TH1F>("deltaR_topQuark_topRecoJet", "#DeltaR(gen. t quark, HOTVR t-tag)", 100, 0, 5);
  hist_deltaPhi_topQuark_topRecoJet = book<TH1F>("deltaPhi_topQuark_topRecoJet", "#Delta#phi(gen. t quark, HOTVR t-tag)", 100, 0, M_PI);
  hist_deltaR_topQuark_recoLepton = book<TH1F>("deltaR_topQuark_recoLepton", "#DeltaR(gen. t quark, rec. lepton)", 100, 0, 5);
  hist_deltaPhi_topQuark_recoLepton = book<TH1F>("deltaPhi_topQuark_recoLepton", "#Delta#phi(gen. t quark, rec. lepton)", 100, 0, M_PI);

}


void MatchHists::fill(const uhh2::Event & event) {

  if(!event.is_valid(h_GENtW)) return;

  const auto & GENtW = event.get(h_GENtW);
  const auto & topjet = event.get(h_toptaggedjet);
  const auto & primlep = event.get(h_primlep);

  const double w = event.weight;

  // calc histogram fills here
  hist_deltaR_topQuark_topRecoJet->Fill(uhh2::deltaR(GENtW.Top().v4(), topjet.v4()), w);
  hist_deltaPhi_topQuark_topRecoJet->Fill(uhh2::deltaPhi(GENtW.Top().v4(), topjet.v4()), w);
  hist_deltaR_topQuark_recoLepton->Fill(uhh2::deltaR(GENtW.Top().v4(), primlep.v4()), w);
  hist_deltaPhi_topQuark_recoLepton->Fill(uhh2::deltaPhi(GENtW.Top().v4(), primlep.v4()), w);
}
