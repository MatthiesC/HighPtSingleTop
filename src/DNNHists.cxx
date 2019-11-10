#include "UHH2/HighPtSingleTop/include/DNNHists.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"

#include "UHH2/common/include/Utils.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;


DNNHists::DNNHists(Context & ctx, const string & dirname, double arg_MIN_PT, double arg_MAX_PT):
  Hists(ctx, dirname) {
  
  h_toptaggedjet = ctx.get_handle<TopJet>("TopTaggedJet");
  h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
  h_pseudotop = ctx.get_handle<LorentzVector>("PseudoTop");
  h_wboson = ctx.get_handle<LorentzVector>("WBosonLeptonic");
  h_bjets = ctx.get_handle<vector<Jet>>("BJets");

  m_MIN_PT = arg_MIN_PT;
  m_MAX_PT = arg_MAX_PT;

  int nBins = 100;
  int nBins_dPhi = 128;
  int nBins_lowRes = 50;
  int nBins_dPhi_lowRes = 64;
  if(m_MIN_PT >= 350) { // threshold for lower resolution plots
    nBins = nBins_lowRes;
    nBins_dPhi = nBins_dPhi_lowRes;
  }

  hist_tlep_mass = book<TH1F>("tlep_mass", "leptonic pseudo t quark mass [GeV]", nBins, 0, 500);
  hist_tlep_mt = book<TH1F>("tlep_mt", "leptonic pseudo t quark m_{T} [GeV]", nBins, 0, 500);
  hist_tlep_pt = book<TH1F>("tlep_pt", "leptonic pseudo t quark p_{T} [GeV]", nBins, 0, 1000);
  hist_tlep_eta = book<TH1F>("tlep_eta", "leptonic pseudo t quark #eta", nBins, -3, 3);
  hist_tlep_phi = book<TH1F>("tlep_phi", "leptonic pseudo t quark #phi", nBins_dPhi, -M_PI, M_PI);
  hist_dr_tlep_tjet = book<TH1F>("dr_tlep_tjet", "#DeltaR(leptonic pseudo t quark, t jet)", nBins, 0, 5);

  hist_tlep_mass_guaranteed_b = book<TH1F>("tlep_mass_guaranteed_b", "leptonic pseudo t quark mass [GeV]", nBins, 0, 500);
  hist_tlep_mt_guaranteed_b = book<TH1F>("tlep_mt_guaranteed_b", "leptonic pseudo t quark m_{T} [GeV]", nBins, 0, 500);
  hist_tlep_pt_guaranteed_b = book<TH1F>("tlep_pt_guaranteed_b", "leptonic pseudo t quark p_{T} [GeV]", nBins, 0, 1000);
  hist_tlep_eta_guaranteed_b = book<TH1F>("tlep_eta_guaranteed_b", "leptonic pseudo t quark #eta", nBins, -3, 3);
  hist_tlep_phi_guaranteed_b = book<TH1F>("tlep_phi_guaranteed_b", "leptonic pseudo t quark #phi", nBins_dPhi, -M_PI, M_PI);
  hist_dr_tlep_tjet_guaranteed_b = book<TH1F>("dr_tlep_tjet_guaranteed_b", "#DeltaR(leptonic pseudo t quark, t jet)", nBins, 0, 5);
  
  hist_wlep_mass = book<TH1F>("wlep_mass", "leptonic W boson mass [GeV]", nBins, 0, 200);
  hist_wlep_mt = book<TH1F>("wlep_mt", "leptonic W boson m_{T} [GeV]", nBins, 0, 500);
  hist_wlep_mt_calc = book<TH1F>("wlep_mt_calc", "leptonic W boson m_{T} [GeV]", nBins, 0, 500);
  hist_wlep_pt = book<TH1F>("wlep_pt", "leptonic W boson p_{T} [GeV]", nBins, 0, 1000);
  hist_wlep_eta = book<TH1F>("wlep_eta", "leptonic W boson #eta", nBins, -3, 3);
  hist_wlep_phi = book<TH1F>("wlep_phi", "leptonic W boson #phi", nBins_dPhi, -M_PI, M_PI);

  hist_ptbalance_wlep_tjet = book<TH1F>("ptbalance_wlep_tjet", "[p_{T}(lep. W) - p_{T}(t jet)] / [p_{T}(lep. W) + p_{T}(t jet)]", nBins, 0, 0);
  hist_ptbalance_tlep_tjet = book<TH1F>("ptbalance_tlep_tjet", "[p_{T}(lep. t) - p_{T}(t jet)] / [p_{T}(lep. t) + p_{T}(t jet)]", nBins, 0, 0);

  hist_dr_wlep_tjet = book<TH1F>("dr_wlep_tjet", "#DeltaR(lep. W, t jet)", nBins, 0, 5);
  hist_dphi_wlep_tjet = book<TH1F>("dphi_wlep_tjet", "#Delta#phi(lep. W, t jet)", nBins_dPhi, -M_PI, M_PI);
  hist_dphi_tlep_tjet = book<TH1F>("dphi_tlep_tjet", "#Delta#phi(lep. t, t jet)", nBins_dPhi, -M_PI, M_PI);

  hist_jets_number = book<TH1F>("jets_number", "number of AK4 jets", 11, -0.5, 10.5);
  hist_bjets_number = book<TH1F>("bjets_number", "number of b jets", 11, -0.5, 10.5);

  hist_dr_lepton_nextjet = book<TH1F>("dr_lepton_nextjet", "#DeltaR(lepton, closest AK4 jet)", nBins, 0, 5);
  hist_dr_lepton_nextbjet = book<TH1F>("dr_lepton_nextbjet", "#DeltaR(lepton, closest b jet) [if no b jet, closest jet]", nBins, 0, 5);
  hist_dr_lepton_nextbjet_guaranteed_b = book<TH1F>("dr_lepton_nextbjet_guaranteed_b", "#DeltaR(lepton, closest b jet)", nBins, 0, 5);

  hist_dr_tjet_lepton = book<TH1F>("dr_tjet_lepton", "#DeltaR(t jet, lepton)", nBins, 0, 5);
  hist_dphi_tjet_lepton = book<TH1F>("dphi_tjet_lepton", "#Delta#phi(t jet, lepton)", nBins_dPhi, 0, M_PI);
  hist_dphi_tjet_met = book<TH1F>("dphi_tjet_met", "#Delta#phi(t jet, p_{T}^{miss})", nBins_dPhi, 0, M_PI);
  hist_dphi_met_lepton = book<TH1F>("dphi_met_lepton", "#Delta#phi(p_{T}^{miss}, lepton)", nBins_dPhi, 0, M_PI);
}


void DNNHists::fill(const uhh2::Event & event) {

  const auto & topjet = event.get(h_toptaggedjet);
  const auto & primlep = event.get(h_primlep);
  const auto & pseudotop = event.get(h_pseudotop);
  const auto & wboson = event.get(h_wboson);
  const auto & bjets = event.get(h_bjets);

  const vector<Jet> jets = *event.jets;
  const double w = event.weight;


  if(topjet.v4().pt() >= m_MIN_PT && topjet.v4().pt() < m_MAX_PT) {
    
    hist_tlep_mass->Fill(pseudotop.M(), w);
    hist_tlep_mt->Fill(sqrt(pseudotop.Px() * pseudotop.Px() + pseudotop.Py() * pseudotop.Py()), w);
    hist_tlep_pt->Fill(pseudotop.Pt(), w);
    hist_tlep_eta->Fill(pseudotop.Eta(), w);
    hist_tlep_phi->Fill(pseudotop.Phi(), w);
    hist_dr_tlep_tjet->Fill(uhh2::deltaR(pseudotop, topjet.v4()), w);

    if(bjets.size() > 0) {
      hist_tlep_mass_guaranteed_b->Fill(pseudotop.M(), w);
      hist_tlep_mt_guaranteed_b->Fill(sqrt(pseudotop.Px() * pseudotop.Px() + pseudotop.Py() * pseudotop.Py()), w);
      hist_tlep_pt_guaranteed_b->Fill(pseudotop.Pt(), w);
      hist_tlep_eta_guaranteed_b->Fill(pseudotop.Eta(), w);
      hist_tlep_phi_guaranteed_b->Fill(pseudotop.Phi(), w);
      hist_dr_tlep_tjet_guaranteed_b->Fill(uhh2::deltaR(pseudotop, topjet.v4()), w);
    }

    hist_wlep_mass->Fill(wboson.M(), w);
    hist_wlep_mt->Fill(sqrt(wboson.Px() * wboson.Px() + wboson.Py() * wboson.Py()), w);
    hist_wlep_mt_calc->Fill(calcMTW(primlep, event), w);
    hist_wlep_pt->Fill(wboson.Pt(), w);
    hist_wlep_eta->Fill(wboson.Eta(), w);
    hist_wlep_phi->Fill(wboson.Phi(), w);

    hist_ptbalance_wlep_tjet->Fill((wboson.Pt() - topjet.v4().Pt())/(wboson.Pt() + topjet.v4().Pt()), w);
    hist_ptbalance_tlep_tjet->Fill((pseudotop.Pt() - topjet.v4().Pt())/(pseudotop.Pt() + topjet.v4().Pt()), w);

    hist_dr_wlep_tjet->Fill(uhh2::deltaR(wboson, topjet.v4()), w);
    hist_dphi_wlep_tjet->Fill(uhh2::deltaPhi(wboson, topjet.v4()), w);
    hist_dphi_tlep_tjet->Fill(uhh2::deltaPhi(pseudotop, topjet.v4()), w);

    hist_jets_number->Fill(jets.size(), w);
    hist_bjets_number->Fill(bjets.size(), w);

    hist_dr_lepton_nextjet->Fill(uhh2::deltaR(primlep.v4(), nextJet(primlep, jets)->v4()), w);
    if(bjets.size() > 0) {
      hist_dr_lepton_nextbjet->Fill(uhh2::deltaR(primlep.v4(), nextJet(primlep, bjets)->v4()), w);
      hist_dr_lepton_nextbjet_guaranteed_b->Fill(uhh2::deltaR(primlep.v4(), nextJet(primlep, bjets)->v4()), w);
    } else {
      hist_dr_lepton_nextbjet->Fill(uhh2::deltaR(primlep.v4(), nextJet(primlep, jets)->v4()), w);
    }

    hist_dr_tjet_lepton->Fill(uhh2::deltaR(topjet.v4(), primlep.v4()), w);
    hist_dphi_tjet_lepton->Fill(uhh2::deltaPhi(topjet.v4(), primlep.v4()), w);
    hist_dphi_tjet_met->Fill(uhh2::deltaPhi(topjet.v4(), event.met->v4()), w);
    hist_dphi_met_lepton->Fill(uhh2::deltaPhi(event.met->v4(), primlep.v4()), w);
  }

}
