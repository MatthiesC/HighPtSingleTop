#include "UHH2/HighPtSingleTop/include/SingleTopGenHists_tChannel.h"
#include "TH1F.h"
#include "TH2F.h"


using namespace uhh2;


SingleTopGenHists_tChannel::SingleTopGenHists_tChannel(uhh2::Context & ctx, const std::string & dirname): Hists(ctx, dirname){
  
  h_singletopgen_tChannel = ctx.get_handle<SingleTopGen_tChannel>("singletopgen_tChannel");

  hist_sHat = book<TH1F>("sHat", "#hat{s} [GeV]", 500, 0, 5000);
  const int nx = 5;
  hist_decayChannel = book<TH1F>("decayChannel", "decay channel", nx, -0.5, 4.5);
  const char *decayChannels[nx] = {"hadronical","electronical","muonical","tauonical","not found"};
  for(int i=1;i<=nx;i++) hist_decayChannel->GetXaxis()->SetBinLabel(i,decayChannels[i-1]);
}


void SingleTopGenHists_tChannel::fill(const uhh2::Event & e){
  
  //do not fill histograms if singletopgen information has not been filled
  if(!e.is_valid(h_singletopgen_tChannel)) return;

  const auto & singletopgen = e.get(h_singletopgen_tChannel);

  const double w = e.weight;

  const LorentzVector Top = singletopgen.Top().v4();
  const LorentzVector bTop = singletopgen.bTop().v4();
  const LorentzVector WTop = singletopgen.WTop().v4();
  const LorentzVector WTopDecay1 = singletopgen.WTopDecay1().v4();
  const LorentzVector WTopDecay2 = singletopgen.WTopDecay2().v4();
  const LorentzVector Initial1 = singletopgen.Initial1().v4();
  const LorentzVector Initial2 = singletopgen.Initial2().v4();
  const LorentzVector bSpectator = singletopgen.bSpectator().v4();
  const LorentzVector scatteredQuark = singletopgen.scatteredQuark().v4();
  const LorentzVector radiatedGluonOrQuark = singletopgen.radiatedGluonOrQuark().v4();


  /////////////////////////////
  // Now fill the histograms //
  /////////////////////////////

  const double sHat = (Initial1 + Initial2).M();
  hist_sHat->Fill(sHat, w);
  hist_decayChannel->Fill(singletopgen.DecayChannel(), w);
}
