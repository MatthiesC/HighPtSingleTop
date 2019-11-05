#include "UHH2/HighPtSingleTop/include/TopTagHists.h"

#include "UHH2/common/include/Utils.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;


TopTagHists::TopTagHists(Context & ctx, const string & dirname):
  Hists(ctx, dirname) {
  
  h_toptaggedjet = ctx.get_handle<TopJet>("TopTaggedJet");
  h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");

  const int nBins_dR = 100;
  const int nBins_dPhi = 128;
  const int nBins_dR_lowRes = 50;
  const int nBins_dPhi_lowRes = 64;

  hist_top_pt = book<TH1F>("top_pt", "t jet p_{T} [GeV]", 100, 0, 1600);
  hist_top_eta = book<TH1F>("top_eta", "t jet #eta", 100, -2.5, 2.5);
  hist_top_mass = book<TH1F>("top_mass", "t jet m_{jet} [GeV]", 100, 130, 230); // 140 - 220
  hist_top_phi = book<TH1F>("top_phi", "t jet #phi", 100, -M_PI, M_PI);
  hist_top_px = book<TH1F>("top_px", "t jet p_{x} [GeV]", 100, -2000, 2000);
  hist_top_py = book<TH1F>("top_py", "t jet p_{y} [GeV]", 100, -2000, 2000);
  hist_top_pz = book<TH1F>("top_pz", "t jet p_{z} [GeV]", 100, -5000, 5000);
  hist_top_energy = book<TH1F>("top_energy", "t jet energy [GeV]", 100, 0, 5000);
  hist_top_area = book<TH1F>("top_area", "t jet area", 100, 0, 20);

  hist_top_nsub = book<TH1F>("top_nsub", "t jet N_{subjets}", 6, 1.5, 7.5); // 2 - 7
  hist_top_fpt = book<TH1F>("top_fpt", "t jet f_{pT}(lead. subjet)", 100, 0, 1);
  hist_top_mpair = book<TH1F>("top_mpair", "t jet min. m_{ij} [GeV]", 100, 0, 200);
  hist_top_tau32 = book<TH1F>("top_tau32", "t jet #tau_{3}/#tau_{2}", 100, 0, 1);
  hist_top_tau21 = book<TH1F>("top_tau21", "t jet #tau_{2}/#tau_{1}", 100, 0, 1);
  hist_top_tau1 = book<TH1F>("top_tau1", "t jet #tau_{1}", 100, 0, 1);
  hist_top_tau2 = book<TH1F>("top_tau2", "t jet #tau_{2}", 100, 0, 1);
  hist_top_tau3 = book<TH1F>("top_tau3", "t jet #tau_{3}", 100, 0, 1);

  hist_top_dr_lepton = book<TH1F>("top_dr_lepton", "#DeltaR(t jet, lepton)", nBins_dR, 0, 5);
  hist_top_dphi_lepton = book<TH1F>("top_dphi_lepton", "#Delta#phi(t jet, lepton)", nBins_dPhi, 0, M_PI);
  hist_top_dphi_met = book<TH1F>("top_dphi_met", "#Delta#phi(t jet, p_{T}^{miss})", nBins_dPhi, 0, M_PI);
  hist_met_dphi_lepton = book<TH1F>("met_dphi_lepton", "#Delta#phi(p_{T}^{miss}, lepton)", nBins_dPhi, 0, M_PI);
}


void TopTagHists::fill(const uhh2::Event & event) {

  const auto & topjet = event.get(h_toptaggedjet);
  const auto & primlep = event.get(h_primlep);

  const double w = event.weight;


  // Lorentz vector
  hist_top_pt->Fill(topjet.v4().Pt(), w);
  hist_top_eta->Fill(topjet.v4().Eta(), w);
  hist_top_mass->Fill(topjet.v4().M(), w);
  hist_top_phi->Fill(topjet.v4().Phi(), w);
  hist_top_px->Fill(topjet.v4().px(), w);
  hist_top_py->Fill(topjet.v4().py(), w);
  hist_top_pz->Fill(topjet.v4().pz(), w);
  hist_top_energy->Fill(topjet.v4().e(), w);
  hist_top_area->Fill(topjet.jetArea(), w);

  // Substructure variables
  vector<Jet> subjets = topjet.subjets();
  sort_by_pt(subjets);
  double m12 = (subjets.at(0).v4() + subjets.at(1).v4()).M();
  double m13 = (subjets.at(0).v4() + subjets.at(2).v4()).M();
  double m23 = (subjets.at(1).v4() + subjets.at(2).v4()).M();
  double min_mass_ij = min(m12, min(m13, m23));
  hist_top_nsub->Fill(subjets.size(), w);
  hist_top_fpt->Fill(subjets.at(0).pt() / topjet.v4().Pt(), w);
  hist_top_mpair->Fill(min_mass_ij, w);
  hist_top_tau32->Fill(topjet.tau3_groomed() / topjet.tau2_groomed(), w);
  hist_top_tau21->Fill(topjet.tau2_groomed() / topjet.tau1_groomed(), w);
  hist_top_tau1->Fill(topjet.tau1_groomed(), w);
  hist_top_tau2->Fill(topjet.tau2_groomed(), w);
  hist_top_tau3->Fill(topjet.tau3_groomed(), w);

  // Relative position
  hist_top_dr_lepton->Fill(uhh2::deltaR(topjet.v4(), primlep.v4()), w);
  hist_top_dphi_lepton->Fill(uhh2::deltaPhi(topjet.v4(), primlep.v4()), w);
  hist_top_dphi_met->Fill(uhh2::deltaPhi(topjet.v4(), event.met->v4()), w);
  hist_met_dphi_lepton->Fill(uhh2::deltaPhi(event.met->v4(), primlep.v4()), w);
}
