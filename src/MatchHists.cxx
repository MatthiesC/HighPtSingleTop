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
  hist_deltaPhi_topQuark_topRecoJet = book<TH1F>("deltaPhi_topQuark_topRecoJet", "#Delta#phi(gen. t quark, HOTVR t-tag)", 128, 0, M_PI);
  hist_deltaR_topQuark_recoLepton = book<TH1F>("deltaR_topQuark_recoLepton", "#DeltaR(gen. t quark, rec. lepton)", 100, 0, 5);
  hist_deltaPhi_topQuark_recoLepton = book<TH1F>("deltaPhi_topQuark_recoLepton", "#Delta#phi(gen. t quark, rec. lepton)", 128, 0, M_PI);

  hist_deltaR_WBoson_topRecoJet = book<TH1F>("deltaR_WBoson_topRecoJet", "#DeltaR(gen. assoc. W, HOTVR t-tag)", 100, 0, 5);
  hist_deltaPhi_WBoson_topRecoJet = book<TH1F>("deltaPhi_WBoson_topRecoJet", "#Delta#phi(gen. assoc. W, HOTVR t-tag)", 128, 0, M_PI);
  hist_deltaR_WBoson_recoLepton = book<TH1F>("deltaR_WBoson_recoLepton", "#DeltaR(gen. assoc. W, rec. lepton)", 100, 0, 5);
  hist_deltaPhi_WBoson_recoLepton = book<TH1F>("deltaPhi_WBoson_recoLepton", "#Delta#phi(gen. assoc. W, rec. lepton)", 128, 0, M_PI);

  hist_deltaR_topQuark_topRecoJet_Pt0to400 = book<TH1F>("deltaR_topQuark_topRecoJet_Pt0to400", "#DeltaR(gen. t quark, HOTVR t-tag)", 100, 0, 5);
  hist_deltaPhi_topQuark_topRecoJet_Pt0to400 = book<TH1F>("deltaPhi_topQuark_topRecoJet_Pt0to400", "#Delta#phi(gen. t quark, HOTVR t-tag)", 128, 0, M_PI);
  hist_deltaR_topQuark_recoLepton_Pt0to400 = book<TH1F>("deltaR_topQuark_recoLepton_Pt0to400", "#DeltaR(gen. t quark, rec. lepton)", 100, 0, 5);
  hist_deltaPhi_topQuark_recoLepton_Pt0to400 = book<TH1F>("deltaPhi_topQuark_recoLepton_Pt0to400", "#Delta#phi(gen. t quark, rec. lepton)", 128, 0, M_PI);

  hist_deltaR_WBoson_topRecoJet_Pt0to400 = book<TH1F>("deltaR_WBoson_topRecoJet_Pt0to400", "#DeltaR(gen. assoc. W, HOTVR t-tag)", 100, 0, 5);
  hist_deltaPhi_WBoson_topRecoJet_Pt0to400 = book<TH1F>("deltaPhi_WBoson_topRecoJet_Pt0to400", "#Delta#phi(gen. assoc. W, HOTVR t-tag)", 128, 0, M_PI);
  hist_deltaR_WBoson_recoLepton_Pt0to400 = book<TH1F>("deltaR_WBoson_recoLepton_Pt0to400", "#DeltaR(gen. assoc. W, rec. lepton)", 100, 0, 5);
  hist_deltaPhi_WBoson_recoLepton_Pt0to400 = book<TH1F>("deltaPhi_WBoson_recoLepton_Pt0to400", "#Delta#phi(gen. assoc. W, rec. lepton)", 128, 0, M_PI);

  hist_deltaR_topQuark_topRecoJet_Pt400toInf = book<TH1F>("deltaR_topQuark_topRecoJet_Pt400toInf", "#DeltaR(gen. t quark, HOTVR t-tag)", 20, 0, 5);
  hist_deltaPhi_topQuark_topRecoJet_Pt400toInf = book<TH1F>("deltaPhi_topQuark_topRecoJet_Pt400toInf", "#Delta#phi(gen. t quark, HOTVR t-tag)", 16, 0, M_PI);
  hist_deltaR_topQuark_recoLepton_Pt400toInf = book<TH1F>("deltaR_topQuark_recoLepton_Pt400toInf", "#DeltaR(gen. t quark, rec. lepton)", 20, 0, 5);
  hist_deltaPhi_topQuark_recoLepton_Pt400toInf = book<TH1F>("deltaPhi_topQuark_recoLepton_Pt400toInf", "#Delta#phi(gen. t quark, rec. lepton)", 16, 0, M_PI);

  hist_deltaR_WBoson_topRecoJet_Pt400toInf = book<TH1F>("deltaR_WBoson_topRecoJet_Pt400toInf", "#DeltaR(gen. assoc. W, HOTVR t-tag)", 20, 0, 5);
  hist_deltaPhi_WBoson_topRecoJet_Pt400toInf = book<TH1F>("deltaPhi_WBoson_topRecoJet_Pt400toInf", "#Delta#phi(gen. assoc. W, HOTVR t-tag)", 16, 0, M_PI);
  hist_deltaR_WBoson_recoLepton_Pt400toInf = book<TH1F>("deltaR_WBoson_recoLepton_Pt400toInf", "#DeltaR(gen. assoc. W, rec. lepton)", 20, 0, 5);
  hist_deltaPhi_WBoson_recoLepton_Pt400toInf = book<TH1F>("deltaPhi_WBoson_recoLepton_Pt400toInf", "#Delta#phi(gen. assoc. W, rec. lepton)", 16, 0, M_PI);

  for(auto hist : {hist_deltaPhi_topQuark_topRecoJet, hist_deltaPhi_topQuark_topRecoJet, hist_deltaPhi_WBoson_topRecoJet, hist_deltaPhi_WBoson_recoLepton,
        hist_deltaPhi_topQuark_topRecoJet_Pt0to400, hist_deltaPhi_topQuark_topRecoJet_Pt0to400, hist_deltaPhi_WBoson_topRecoJet_Pt0to400, hist_deltaPhi_WBoson_recoLepton_Pt0to400,
        hist_deltaPhi_topQuark_topRecoJet_Pt400toInf, hist_deltaPhi_topQuark_topRecoJet_Pt400toInf, hist_deltaPhi_WBoson_topRecoJet_Pt400toInf, hist_deltaPhi_WBoson_recoLepton_Pt400toInf}) {
    hist->GetXaxis()->SetNdivisions(-802);
    hist->GetXaxis()->ChangeLabel(1,-1,-1,-1,-1,-1,"0");
    hist->GetXaxis()->ChangeLabel(2,-1,-1,-1,-1,-1,"#pi/2");
    hist->GetXaxis()->ChangeLabel(-1,-1,-1,-1,-1,-1,"#pi");
  }

  const int nx = 17;
  hist_decayChannel = book<TH1F>("DecayChannel", "tW decay channel", nx, -0.5, 16.5);
  const char *decayChannels[nx] = {"t_{e}W_{e}", "t_{#mu}W_{e}", "t_{#tau}W_{e}", "t_{h}W_{e}",
                                   "t_{e}W_{#mu}", "t_{#mu}W_{#mu}", "t_{#tau}W_{#mu}", "t_{h}W_{#mu}",
                                   "t_{e}W_{#tau}", "t_{#mu}W_{#tau}", "t_{#tau}W_{#tau}", "t_{h}W_{#tau}",
				   "t_{e}W_{h}", "t_{#mu}W_{h}", "t_{#tau}W_{h}", "t_{h}W_{h}",
                                   "N/A"};
  for(int i=1; i<=nx; ++i) hist_decayChannel->GetXaxis()->SetBinLabel(i,decayChannels[i-1]);
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

  hist_deltaR_WBoson_topRecoJet->Fill(uhh2::deltaR(GENtW.WAss().v4(), topjet.v4()), w);
  hist_deltaPhi_WBoson_topRecoJet->Fill(uhh2::deltaPhi(GENtW.WAss().v4(), topjet.v4()), w);
  hist_deltaR_WBoson_recoLepton->Fill(uhh2::deltaR(GENtW.WAss().v4(), primlep.v4()), w);
  hist_deltaPhi_WBoson_recoLepton->Fill(uhh2::deltaPhi(GENtW.WAss().v4(), primlep.v4()), w);

  if(topjet.v4().pt() < 400) {
    hist_deltaR_topQuark_topRecoJet_Pt0to400->Fill(uhh2::deltaR(GENtW.Top().v4(), topjet.v4()), w);
    hist_deltaPhi_topQuark_topRecoJet_Pt0to400->Fill(uhh2::deltaPhi(GENtW.Top().v4(), topjet.v4()), w);
    hist_deltaR_topQuark_recoLepton_Pt0to400->Fill(uhh2::deltaR(GENtW.Top().v4(), primlep.v4()), w);
    hist_deltaPhi_topQuark_recoLepton_Pt0to400->Fill(uhh2::deltaPhi(GENtW.Top().v4(), primlep.v4()), w);
    
    hist_deltaR_WBoson_topRecoJet_Pt0to400->Fill(uhh2::deltaR(GENtW.WAss().v4(), topjet.v4()), w);
    hist_deltaPhi_WBoson_topRecoJet_Pt0to400->Fill(uhh2::deltaPhi(GENtW.WAss().v4(), topjet.v4()), w);
    hist_deltaR_WBoson_recoLepton_Pt0to400->Fill(uhh2::deltaR(GENtW.WAss().v4(), primlep.v4()), w);
    hist_deltaPhi_WBoson_recoLepton_Pt0to400->Fill(uhh2::deltaPhi(GENtW.WAss().v4(), primlep.v4()), w);
  }

  if(topjet.v4().pt() >= 400) {
    hist_deltaR_topQuark_topRecoJet_Pt400toInf->Fill(uhh2::deltaR(GENtW.Top().v4(), topjet.v4()), w);
    hist_deltaPhi_topQuark_topRecoJet_Pt400toInf->Fill(uhh2::deltaPhi(GENtW.Top().v4(), topjet.v4()), w);
    hist_deltaR_topQuark_recoLepton_Pt400toInf->Fill(uhh2::deltaR(GENtW.Top().v4(), primlep.v4()), w);
    hist_deltaPhi_topQuark_recoLepton_Pt400toInf->Fill(uhh2::deltaPhi(GENtW.Top().v4(), primlep.v4()), w);
    
    hist_deltaR_WBoson_topRecoJet_Pt400toInf->Fill(uhh2::deltaR(GENtW.WAss().v4(), topjet.v4()), w);
    hist_deltaPhi_WBoson_topRecoJet_Pt400toInf->Fill(uhh2::deltaPhi(GENtW.WAss().v4(), topjet.v4()), w);
    hist_deltaR_WBoson_recoLepton_Pt400toInf->Fill(uhh2::deltaR(GENtW.WAss().v4(), primlep.v4()), w);
    hist_deltaPhi_WBoson_recoLepton_Pt400toInf->Fill(uhh2::deltaPhi(GENtW.WAss().v4(), primlep.v4()), w);
  }
  
  hist_decayChannel->Fill(GENtW.DecayChannel(), w);
}
