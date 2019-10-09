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

  // kinematic histograms, binned in top quark pt:

  hist_top_pt = book<TH1F>("top_pt", "top quark p_{T} [GeV]", 500, 0, 2000);
  hist_top_eta = book<TH1F>("top_eta", "top quark #eta", 500, -5, 5);
  hist_top_eta_pt0to100 = book<TH1F>("top_eta_pt0to100", "top quark #eta", 500, -5, 5);
  hist_top_eta_pt100to200 = book<TH1F>("top_eta_pt100to200", "top quark #eta", 500, -5, 5);
  hist_top_eta_pt200to500 = book<TH1F>("top_eta_pt200to500", "top quark #eta", 500, -5, 5);
  hist_top_eta_pt500to1000 = book<TH1F>("top_eta_pt500to1000", "top quark #eta", 500, -5, 5);
  hist_top_eta_pt1000toInf = book<TH1F>("top_eta_pt1000toInf", "top quark #eta", 500, -5, 5);

  hist_bSpec_pt = book<TH1F>("bSpec_pt", "b spectator p_{T} [GeV]", 500, 0, 2000);
  hist_bSpec_eta = book<TH1F>("bSpec_eta", "b spectator #eta", 500, -5, 5);
  hist_bSpec_eta_pt0to100 = book<TH1F>("bSpec_eta_pt0to100", "b spectator #eta", 500, -5, 5);
  hist_bSpec_eta_pt100to200 = book<TH1F>("bSpec_eta_pt100to200", "b spectator #eta", 500, -5, 5);
  hist_bSpec_eta_pt200to500 = book<TH1F>("bSpec_eta_pt200to500", "b spectator #eta", 500, -5, 5);
  hist_bSpec_eta_pt500to1000 = book<TH1F>("bSpec_eta_pt500to1000", "b spectator #eta", 500, -5, 5);
  hist_bSpec_eta_pt1000toInf = book<TH1F>("bSpec_eta_pt1000toInf", "b spectator #eta", 500, -5, 5);

  hist_scaQu_pt = book<TH1F>("scaQu_pt", "scattered quark p_{T} [GeV]", 500, 0, 2000);
  hist_scaQu_eta = book<TH1F>("scaQu_eta", "scattered quark #eta", 500, -5, 5);
  hist_scaQu_eta_pt0to100 = book<TH1F>("scaQu_eta_pt0to100", "scattered quark #eta", 500, -5, 5);
  hist_scaQu_eta_pt100to200 = book<TH1F>("scaQu_eta_pt100to200", "scattered quark #eta", 500, -5, 5);
  hist_scaQu_eta_pt200to500 = book<TH1F>("scaQu_eta_pt200to500", "scattered quark #eta", 500, -5, 5);
  hist_scaQu_eta_pt500to1000 = book<TH1F>("scaQu_eta_pt500to1000", "scattered quark #eta", 500, -5, 5);
  hist_scaQu_eta_pt1000toInf = book<TH1F>("scaQu_eta_pt1000toInf", "scattered quark #eta", 500, -5, 5);
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

  hist_top_pt->Fill(Top.pt(), w);
  hist_top_eta->Fill(Top.eta(), w);
  hist_bSpec_pt->Fill(bSpectator.pt(), w);
  hist_bSpec_eta->Fill(bSpectator.eta(), w);
  hist_scaQu_pt->Fill(scatteredQuark.pt(), w);
  hist_scaQu_eta->Fill(scatteredQuark.eta(), w);

  if(Top.pt() >= 0 && Top.pt() < 100){
    hist_top_eta_pt0to100->Fill(Top.eta(), w);
    hist_bSpec_eta_pt0to100->Fill(bSpectator.eta(), w);
    hist_scaQu_eta_pt0to100->Fill(scatteredQuark.eta(), w);
  }
  else if(Top.pt() < 200){
    hist_top_eta_pt100to200->Fill(Top.eta(), w);
    hist_bSpec_eta_pt100to200->Fill(bSpectator.eta(), w);
    hist_scaQu_eta_pt100to200->Fill(scatteredQuark.eta(), w);
  }
  else if(Top.pt() < 500){
    hist_top_eta_pt200to500->Fill(Top.eta(), w);
    hist_bSpec_eta_pt200to500->Fill(bSpectator.eta(), w);
    hist_scaQu_eta_pt200to500->Fill(scatteredQuark.eta(), w);
  }
  else if(Top.pt() < 1000){
    hist_top_eta_pt500to1000->Fill(Top.eta(), w);
    hist_bSpec_eta_pt500to1000->Fill(bSpectator.eta(), w);
    hist_scaQu_eta_pt500to1000->Fill(scatteredQuark.eta(), w);
  }
  else{
    hist_top_eta_pt1000toInf->Fill(Top.eta(), w);
    hist_bSpec_eta_pt1000toInf->Fill(bSpectator.eta(), w);
    hist_scaQu_eta_pt1000toInf->Fill(scatteredQuark.eta(), w);
  }
}
