#include "UHH2/HighPtSingleTop/include/WTagHists.h"

#include "UHH2/common/include/Utils.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;


WTagHists::WTagHists(Context & ctx, const string & dirname, double arg_MIN_PT, double arg_MAX_PT):
  Hists(ctx, dirname) {

  h_wtaggedjets = ctx.get_handle<vector<TopJet>>("WJets");
  h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");

  m_MIN_PT = arg_MIN_PT;
  m_MAX_PT = arg_MAX_PT;

  const int nBins_dR = 100;
  const int nBins_dPhi = 128;

  hist_w_pt = book<TH1F>("w_pt", "W jet p_{T} [GeV]", 100, 0, 1600);
  hist_w_pt_1GeV = book<TH1F>("w_pt_1GeV", "W jet p_{T} [GeV]", 1600, 0, 1600);
  hist_w_eta = book<TH1F>("w_eta", "W jet #eta", 100, -2.5, 2.5);
  hist_w_mass = book<TH1F>("w_mass", "W jet m_{jet} [GeV]", 100, 0, 200);
  hist_w_SDmass = book<TH1F>("w_SDmass", "W jet m_{jet} [GeV]", 100, 0, 200);
  hist_w_phi = book<TH1F>("w_phi", "W jet #phi", 100, -M_PI, M_PI);
  hist_w_px = book<TH1F>("w_px", "W jet p_{x} [GeV]", 100, -2000, 2000);
  hist_w_py = book<TH1F>("w_py", "W jet p_{y} [GeV]", 100, -2000, 2000);
  hist_w_pz = book<TH1F>("w_pz", "W jet p_{z} [GeV]", 100, -5000, 5000);
  hist_w_energy = book<TH1F>("w_energy", "W jet energy [GeV]", 100, 0, 5000);
  hist_w_area = book<TH1F>("w_area", "W jet area", 100, 0, 20);

  hist_w_tau32 = book<TH1F>("w_tau32", "W jet #tau_{3}/#tau_{2}", 100, 0, 1);
  hist_w_tau21 = book<TH1F>("w_tau21", "W jet #tau_{2}/#tau_{1}", 100, 0, 1);
  hist_w_tau1 = book<TH1F>("w_tau1", "W jet #tau_{1}", 100, 0, 1);
  hist_w_tau2 = book<TH1F>("w_tau2", "W jet #tau_{2}", 100, 0, 1);
  hist_w_tau3 = book<TH1F>("w_tau3", "W jet #tau_{3}", 100, 0, 1);
  hist_w_DeepAK8 = book<TH1F>("w_DeepAK8", "W jet: DeepAK8 score (W vs. QCD)", 100, 0, 1);
  hist_w_DeepAK8MD = book<TH1F>("w_DeepAK8MD", "W jet: MD-DeepAK8 score (W vs. QCD)", 100, 0, 1);
  
  hist_w_dr_lepton = book<TH1F>("w_dr_lepton", "#DeltaR(W jet, lepton)", nBins_dR, 0, 5);
  hist_w_dphi_lepton = book<TH1F>("w_dphi_lepton", "#Delta#phi(W jet, lepton)", nBins_dPhi, 0, M_PI);
  hist_w_dphi_met = book<TH1F>("w_dphi_met", "#Delta#phi(W jet, p_{T}^{miss})", nBins_dPhi, 0, M_PI);
  hist_met_dphi_lepton = book<TH1F>("met_dphi_lepton", "#Delta#phi(p_{T}^{miss}, lepton)", nBins_dPhi, 0, M_PI);
}


void WTagHists::fill(const uhh2::Event & event) {

  const auto & wjet = event.get(h_wtaggedjets).at(0); // TODO: not so elegant, define specific handle for the one W tag!!!
  const auto & primlep = event.get(h_primlep);

  const double w = event.weight;


  if(wjet.v4().pt() >= m_MIN_PT && wjet.v4().pt() < m_MAX_PT) {

    // Lorentz vector
    hist_w_pt->Fill(wjet.v4().Pt(), w);
    hist_w_pt_1GeV->Fill(wjet.v4().Pt(), w);
    hist_w_eta->Fill(wjet.v4().Eta(), w);
    hist_w_mass->Fill(wjet.v4().M(), w);
    hist_w_SDmass->Fill(wjet.softdropmass(), w);
    hist_w_phi->Fill(wjet.v4().Phi(), w);
    hist_w_px->Fill(wjet.v4().px(), w);
    hist_w_py->Fill(wjet.v4().py(), w);
    hist_w_pz->Fill(wjet.v4().pz(), w);
    hist_w_energy->Fill(wjet.v4().e(), w);
    hist_w_area->Fill(wjet.jetArea(), w);

    // Substructure variables
    hist_w_tau32->Fill(wjet.tau3() / wjet.tau2(), w);
    hist_w_tau21->Fill(wjet.tau2() / wjet.tau1(), w);
    hist_w_tau1->Fill(wjet.tau1(), w);
    hist_w_tau2->Fill(wjet.tau2(), w);
    hist_w_tau3->Fill(wjet.tau3(), w);
    hist_w_DeepAK8->Fill(wjet.btag_DeepBoosted_WvsQCD(), w);
    hist_w_DeepAK8MD->Fill(wjet.btag_MassDecorrelatedDeepBoosted_WvsQCD(), w);

    // Relative position
    hist_w_dr_lepton->Fill(uhh2::deltaR(wjet.v4(), primlep.v4()), w);
    hist_w_dphi_lepton->Fill(uhh2::deltaPhi(wjet.v4(), primlep.v4()), w);
    hist_w_dphi_met->Fill(uhh2::deltaPhi(wjet.v4(), event.met->v4()), w);
    hist_met_dphi_lepton->Fill(uhh2::deltaPhi(event.met->v4(), primlep.v4()), w);
  }

}
