#include "UHH2/HighPtSingleTop/include/GenSingleTopQuarkHists.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;


GenSingleTopQuarkHists::GenSingleTopQuarkHists(Context & ctx, const string & dirname, const string & channel):
  Hists(ctx, dirname) {

  _channel = channel;

  hist_topQuarkPt = book<TH1F>("topQuarkPt", "top quark p_{T} [GeV]", 1500, 0, 1500);
  hist_topQuarkPt_ggFusion = book<TH1F>("topQuarkPt_ggFusion", "top quark p_{T} [GeV]", 1500, 0, 1500);
  hist_topQuarkPt_qqAnnihilation = book<TH1F>("topQuarkPt_qqAnnihilation", "top quark p_{T} [GeV]", 1500, 0, 1500);
  hist_topQuarkPt_gbFusion = book<TH1F>("topQuarkPt_gbFusion", "top quark p_{T} [GeV]", 1500, 0, 1500);
  hist_topQuarkPt_else = book<TH1F>("topQuarkPt_else", "top quark p_{T} [GeV]", 1500, 0, 1500);

  if(_channel == "tW") {
    h_GENtW = ctx.get_handle<SingleTopGen_tWch>("h_GENtW");
  }

  hist_tW_incl_top_pt = book<TH1F>("tW_incl_top_pt", "top quark p_{T} [GeV]", 1500, 0, 1500);
  hist_tW_incl_top_eta = book<TH1F>("tW_incl_top_eta", "top quark #eta", 1000, -5, 5);
  hist_tW_incl_Wass_pt = book<TH1F>("tW_incl_Wass_pt", "W boson p_{T} [GeV]", 1500, 0, 1500);
  hist_tW_incl_Wass_eta = book<TH1F>("tW_incl_Wass_eta", "W boson #eta", 1000, -5, 5);
  hist_tW_incl_dPhi_tW = book<TH1F>("tW_incl_dPhi_tW", "#Delta#phi(t#minusW) [rad]", 1000, 0, M_PI);
  hist_tW_incl_bAss_pt = book<TH1F>("tW_incl_bAss_pt", "b quark p_{T} [GeV]", 1500, 0, 1500);
  hist_tW_incl_bAss_eta = book<TH1F>("tW_incl_bAss_eta", "b quark #eta", 1000, -5, 5);

  hist_tW_pt200_top_pt = book<TH1F>("tW_pt200_top_pt", "top quark p_{T} [GeV]", 1500, 0, 1500);
  hist_tW_pt200_top_eta = book<TH1F>("tW_pt200_top_eta", "top quark #eta", 1000, -5, 5);
  hist_tW_pt200_Wass_pt = book<TH1F>("tW_pt200_Wass_pt", "W boson p_{T} [GeV]", 1500, 0, 1500);
  hist_tW_pt200_Wass_eta = book<TH1F>("tW_pt200_Wass_eta", "W boson #eta", 1000, -5, 5);
  hist_tW_pt200_dPhi_tW = book<TH1F>("tW_pt200_dPhi_tW", "#Delta#phi(t#minusW) [rad]", 1000, 0, M_PI);
  hist_tW_pt200_bAss_pt = book<TH1F>("tW_pt200_bAss_pt", "b quark p_{T} [GeV]", 1500, 0, 1500);
  hist_tW_pt200_bAss_eta = book<TH1F>("tW_pt200_bAss_eta", "b quark #eta", 1000, -5, 5);

  hist_tW_pt400_top_pt = book<TH1F>("tW_pt400_top_pt", "top quark p_{T} [GeV]", 1500, 0, 1500);
  hist_tW_pt400_top_eta = book<TH1F>("tW_pt400_top_eta", "top quark #eta", 1000, -5, 5);
  hist_tW_pt400_Wass_pt = book<TH1F>("tW_pt400_Wass_pt", "W boson p_{T} [GeV]", 1500, 0, 1500);
  hist_tW_pt400_Wass_eta = book<TH1F>("tW_pt400_Wass_eta", "W boson #eta", 1000, -5, 5);
  hist_tW_pt400_dPhi_tW = book<TH1F>("tW_pt400_dPhi_tW", "#Delta#phi(t#minusW) [rad]", 1000, 0, M_PI);
  hist_tW_pt400_bAss_pt = book<TH1F>("tW_pt400_bAss_pt", "b quark p_{T} [GeV]", 1500, 0, 1500);
  hist_tW_pt400_bAss_eta = book<TH1F>("tW_pt400_bAss_eta", "b quark #eta", 1000, -5, 5);
}


void GenSingleTopQuarkHists::fill(const uhh2::Event & event) {

  GenParticle top, initial1, initial2;
  for(auto gp : *event.genparticles) {
    if(gp.index() == 0) initial1 = gp;
    else if(gp.index() == 1) initial2 = gp;
    if(abs(gp.pdgId()) == 6) { top = gp; break; };
  }

  hist_topQuarkPt->Fill(top.v4().pt(), event.weight);

  bool is_ggFusion = initial1.pdgId() == 21 && initial2.pdgId() == 21;
  bool is_qqAnnihilation = initial1.pdgId() == -initial2.pdgId() && abs(initial1.pdgId()) <= 6;
  bool is_gbFusion = (initial1.pdgId() == 21 && abs(initial2.pdgId()) == 5) || (abs(initial1.pdgId()) == 5 && initial2.pdgId() == 21);

  if(is_ggFusion) {
    hist_topQuarkPt_ggFusion->Fill(top.v4().pt(), event.weight);
  }
  else if(is_qqAnnihilation) {
    hist_topQuarkPt_qqAnnihilation->Fill(top.v4().pt(), event.weight);
  }
  else if(is_gbFusion) {
    hist_topQuarkPt_gbFusion->Fill(top.v4().pt(), event.weight);
  }
  else {
    hist_topQuarkPt_else->Fill(top.v4().pt(), event.weight);
  }

  if(_channel == "tW") {
    // todox
    const auto & GENtW = event.get(h_GENtW);

    bool has_bAss = GENtW.HasAssociatedBottom();

    hist_tW_incl_top_pt->Fill(top.v4().pt(), event.weight);
    hist_tW_incl_top_eta->Fill(top.v4().eta(), event.weight);
    hist_tW_incl_Wass_pt->Fill(GENtW.WAss().v4().pt(), event.weight);
    hist_tW_incl_Wass_eta->Fill(GENtW.WAss().v4().eta(), event.weight);
    hist_tW_incl_dPhi_tW->Fill(deltaPhi(top.v4(), GENtW.WAss().v4()), event.weight);
    if(has_bAss) hist_tW_incl_bAss_pt->Fill(GENtW.bAss().v4().pt(), event.weight);
    if(has_bAss) hist_tW_incl_bAss_eta->Fill(GENtW.bAss().v4().eta(), event.weight);

    if(top.v4().pt() > 200) {
      hist_tW_pt200_top_pt->Fill(top.v4().pt(), event.weight);
      hist_tW_pt200_top_eta->Fill(top.v4().eta(), event.weight);
      hist_tW_pt200_Wass_pt->Fill(GENtW.WAss().v4().pt(), event.weight);
      hist_tW_pt200_Wass_eta->Fill(GENtW.WAss().v4().eta(), event.weight);
      hist_tW_pt200_dPhi_tW->Fill(deltaPhi(top.v4(), GENtW.WAss().v4()), event.weight);
      if(has_bAss) hist_tW_pt200_bAss_pt->Fill(GENtW.bAss().v4().pt(), event.weight);
      if(has_bAss) hist_tW_pt200_bAss_eta->Fill(GENtW.bAss().v4().eta(), event.weight);
    }

    if(top.v4().pt() > 400) {
      hist_tW_pt400_top_pt->Fill(top.v4().pt(), event.weight);
      hist_tW_pt400_top_eta->Fill(top.v4().eta(), event.weight);
      hist_tW_pt400_Wass_pt->Fill(GENtW.WAss().v4().pt(), event.weight);
      hist_tW_pt400_Wass_eta->Fill(GENtW.WAss().v4().eta(), event.weight);
      hist_tW_pt400_dPhi_tW->Fill(deltaPhi(top.v4(), GENtW.WAss().v4()), event.weight);
      if(has_bAss) hist_tW_pt400_bAss_pt->Fill(GENtW.bAss().v4().pt(), event.weight);
      if(has_bAss) hist_tW_pt400_bAss_eta->Fill(GENtW.bAss().v4().eta(), event.weight);
    }

  }
}
