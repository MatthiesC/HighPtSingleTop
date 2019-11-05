#include "UHH2/HighPtSingleTop/include/MatchHists.h"

#include "UHH2/common/include/Utils.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;


MatchHists::MatchHists(Context & ctx, const string & dirname, double arg_MIN_PT, double arg_MAX_PT):
  Hists(ctx, dirname) {
  
  h_GENtW = ctx.get_handle<SingleTopGen_tWch>("h_GENtW");
  h_toptaggedjet = ctx.get_handle<TopJet>("TopTaggedJet");
  h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");

  m_MIN_PT = arg_MIN_PT;
  m_MAX_PT = arg_MAX_PT;

  int nBins = 100;
  int nBins_dR = 100;
  int nBins_dPhi = 128;
  int nBins_lowRes = 50;
  int nBins_dR_lowRes = 50;
  int nBins_dPhi_lowRes = 64;
  if(m_MIN_PT >= 350) { // threshold for lower resolution plots
    nBins = nBins_lowRes;
    nBins_dR = nBins_dR_lowRes;
    nBins_dPhi = nBins_dPhi_lowRes;
  }

  hist_deltaR_topQuark_topRecoJet = book<TH1F>("deltaR_topQuark_topRecoJet", "#DeltaR(gen. t quark, t jet)", nBins_dR, 0, 5);
  hist_deltaPhi_topQuark_topRecoJet = book<TH1F>("deltaPhi_topQuark_topRecoJet", "#Delta#phi(gen. t quark, t jet)", nBins_dPhi, 0, M_PI);
  hist_deltaR_topQuark_recoLepton = book<TH1F>("deltaR_topQuark_recoLepton", "#DeltaR(gen. t quark, rec. lepton)", nBins_dR, 0, 5);
  hist_deltaPhi_topQuark_recoLepton = book<TH1F>("deltaPhi_topQuark_recoLepton", "#Delta#phi(gen. t quark, rec. lepton)", nBins_dPhi, 0, M_PI);

  hist_deltaR_WBoson_topRecoJet = book<TH1F>("deltaR_WBoson_topRecoJet", "#DeltaR(gen. assoc. W, t jet)", nBins_dR, 0, 5);
  hist_deltaPhi_WBoson_topRecoJet = book<TH1F>("deltaPhi_WBoson_topRecoJet", "#Delta#phi(gen. assoc. W, t jet)", nBins_dPhi, 0, M_PI);
  hist_deltaR_WBoson_recoLepton = book<TH1F>("deltaR_WBoson_recoLepton", "#DeltaR(gen. assoc. W, rec. lepton)", nBins_dR, 0, 5);
  hist_deltaPhi_WBoson_recoLepton = book<TH1F>("deltaPhi_WBoson_recoLepton", "#Delta#phi(gen. assoc. W, rec. lepton)", nBins_dPhi, 0, M_PI);

  /* // This does not work... maybe implement pi Xaxis in SFramePlotter...
  for(auto hist : {hist_deltaPhi_topQuark_topRecoJet, hist_deltaPhi_topQuark_topRecoJet, hist_deltaPhi_WBoson_topRecoJet, hist_deltaPhi_WBoson_recoLepton,
        hist_deltaPhi_topQuark_topRecoJet_Pt0to400, hist_deltaPhi_topQuark_topRecoJet_Pt0to400, hist_deltaPhi_WBoson_topRecoJet_Pt0to400, hist_deltaPhi_WBoson_recoLepton_Pt0to400,
        hist_deltaPhi_topQuark_topRecoJet_Pt400toInf, hist_deltaPhi_topQuark_topRecoJet_Pt400toInf, hist_deltaPhi_WBoson_topRecoJet_Pt400toInf, hist_deltaPhi_WBoson_recoLepton_Pt400toInf}) {
    hist->GetXaxis()->SetNdivisions(-802);
    hist->GetXaxis()->ChangeLabel(1,-1,-1,-1,-1,-1,"0");
    hist->GetXaxis()->ChangeLabel(2,-1,-1,-1,-1,-1,"#pi/2");
    hist->GetXaxis()->ChangeLabel(-1,-1,-1,-1,-1,-1,"#pi");
  }
  */

  const int nx = 16;
  hist_decayChannel = book<TH1F>("DecayChannel", "tW decay channel", nx, -0.5, 15.5);
  const char *decayChannels[nx] = {"t_{e}W_{e}", "t_{#mu}W_{e}", "t_{#tau}W_{e}", "t_{h}W_{e}",
                                   "t_{e}W_{#mu}", "t_{#mu}W_{#mu}", "t_{#tau}W_{#mu}", "t_{h}W_{#mu}",
                                   "t_{e}W_{#tau}", "t_{#mu}W_{#tau}", "t_{#tau}W_{#tau}", "t_{h}W_{#tau}",
				   "t_{e}W_{h}", "t_{#mu}W_{h}", "t_{#tau}W_{h}", "t_{h}W_{h}"};
  for(int i=1; i<=nx; ++i) hist_decayChannel->GetXaxis()->SetBinLabel(i,decayChannels[i-1]);

  hist_topRecoJet_pt = book<TH1F>("topRecoJet_pt", "t jet p_{T} [GeV]", nBins, 0, 1000);
  hist_topRecoJet_eta = book<TH1F>("topRecoJet_eta", "t jet #eta", nBins, -2.5, 2.5);
  hist_topRecoJet_mass = book<TH1F>("topRecoJet_mass", "t jet m_{jet} [GeV]", nBins, 120, 240); // 140 - 220
  hist_topRecoJet_phi = book<TH1F>("topRecoJet_phi", "t jet #phi", nBins_dPhi, -M_PI, M_PI);
  hist_topRecoJet_px = book<TH1F>("topRecoJet_px", "t jet p_{x} [GeV]", nBins, -2000, 2000);
  hist_topRecoJet_py = book<TH1F>("topRecoJet_py", "t jet p_{y} [GeV]", nBins, -2000, 2000);
  hist_topRecoJet_pz = book<TH1F>("topRecoJet_pz", "t jet p_{z} [GeV]", nBins, -5000, 5000);
  hist_topRecoJet_energy = book<TH1F>("topRecoJet_energy", "t jet energy [GeV]", nBins, 0, 5000);
  hist_topRecoJet_area = book<TH1F>("topRecoJet_area", "t jet area", nBins, 0, 20);

  hist_topRecoJet_nsub = book<TH1F>("topRecoJet_nsub", "t jet N_{subjets}", 6, 1.5, 7.5); // 2 - 7
  hist_topRecoJet_fpt = book<TH1F>("topRecoJet_fpt", "t jet f_{pT}(lead. subjet)", nBins, 0, 1);
  hist_topRecoJet_mpair = book<TH1F>("topRecoJet_mpair", "t jet min. m_{ij} [GeV]", nBins, 0, 200);
  hist_topRecoJet_tau32 = book<TH1F>("topRecoJet_tau32", "t jet #tau_{3}/#tau_{2}", nBins, 0, 1);
  hist_topRecoJet_tau21 = book<TH1F>("topRecoJet_tau21", "t jet #tau_{2}/#tau_{1}", nBins, 0, 1);
  hist_topRecoJet_tau1 = book<TH1F>("topRecoJet_tau1", "t jet #tau_{1}", nBins, 0, 1);
  hist_topRecoJet_tau2 = book<TH1F>("topRecoJet_tau2", "t jet #tau_{2}", nBins, 0, 1);
  hist_topRecoJet_tau3 = book<TH1F>("topRecoJet_tau3", "t jet #tau_{3}", nBins, 0, 1);

  hist_topRecoJet_dr_lepton = book<TH1F>("topRecoJet_dr_lepton", "#DeltaR(t jet, lepton)", nBins_dR, 0, 5);
  hist_topRecoJet_dphi_lepton = book<TH1F>("topRecoJet_dphi_lepton", "#Delta#phi(t jet, lepton)", nBins_dPhi, 0, M_PI);
  hist_topRecoJet_dphi_met = book<TH1F>("topRecoJet_dphi_met", "#Delta#phi(t jet, p_{T}^{miss})", nBins_dPhi, 0, M_PI);
  hist_met_dphi_lepton = book<TH1F>("met_dphi_lepton", "#Delta#phi(p_{T}^{miss}, lepton)", nBins_dPhi, 0, M_PI);
}


void MatchHists::fill(const uhh2::Event & event) {

  if(!event.is_valid(h_GENtW)) return;

  const auto & GENtW = event.get(h_GENtW);
  const auto & topjet = event.get(h_toptaggedjet);
  const auto & primlep = event.get(h_primlep);

  const double w = event.weight;

  if(topjet.v4().pt() >= m_MIN_PT && topjet.v4().pt() < m_MAX_PT) {

    // calc histogram fills here
    hist_deltaR_topQuark_topRecoJet->Fill(uhh2::deltaR(GENtW.Top().v4(), topjet.v4()), w);
    hist_deltaPhi_topQuark_topRecoJet->Fill(uhh2::deltaPhi(GENtW.Top().v4(), topjet.v4()), w);
    hist_deltaR_topQuark_recoLepton->Fill(uhh2::deltaR(GENtW.Top().v4(), primlep.v4()), w);
    hist_deltaPhi_topQuark_recoLepton->Fill(uhh2::deltaPhi(GENtW.Top().v4(), primlep.v4()), w);

    hist_deltaR_WBoson_topRecoJet->Fill(uhh2::deltaR(GENtW.WAss().v4(), topjet.v4()), w);
    hist_deltaPhi_WBoson_topRecoJet->Fill(uhh2::deltaPhi(GENtW.WAss().v4(), topjet.v4()), w);
    hist_deltaR_WBoson_recoLepton->Fill(uhh2::deltaR(GENtW.WAss().v4(), primlep.v4()), w);
    hist_deltaPhi_WBoson_recoLepton->Fill(uhh2::deltaPhi(GENtW.WAss().v4(), primlep.v4()), w);

    hist_decayChannel->Fill(GENtW.DecayChannel(), w);

    // Lorentz vector
    hist_topRecoJet_pt->Fill(topjet.v4().Pt(), w);
    hist_topRecoJet_eta->Fill(topjet.v4().Eta(), w);
    hist_topRecoJet_mass->Fill(topjet.v4().M(), w);
    hist_topRecoJet_phi->Fill(topjet.v4().Phi(), w);
    hist_topRecoJet_px->Fill(topjet.v4().px(), w);
    hist_topRecoJet_py->Fill(topjet.v4().py(), w);
    hist_topRecoJet_pz->Fill(topjet.v4().pz(), w);
    hist_topRecoJet_energy->Fill(topjet.v4().e(), w);
    hist_topRecoJet_area->Fill(topjet.jetArea(), w);
  
    // Substructure variables
    vector<Jet> subjets = topjet.subjets();
    sort_by_pt(subjets);
    double m12 = (subjets.at(0).v4() + subjets.at(1).v4()).M();
    double m13 = (subjets.at(0).v4() + subjets.at(2).v4()).M();
    double m23 = (subjets.at(1).v4() + subjets.at(2).v4()).M();
    double min_mass_ij = min(m12, min(m13, m23));
    hist_topRecoJet_nsub->Fill(subjets.size(), w);
    hist_topRecoJet_fpt->Fill(subjets.at(0).pt() / topjet.v4().Pt(), w);
    hist_topRecoJet_mpair->Fill(min_mass_ij, w);
    hist_topRecoJet_tau32->Fill(topjet.tau3_groomed() / topjet.tau2_groomed(), w);
    hist_topRecoJet_tau21->Fill(topjet.tau2_groomed() / topjet.tau1_groomed(), w);
    hist_topRecoJet_tau1->Fill(topjet.tau1_groomed(), w);
    hist_topRecoJet_tau2->Fill(topjet.tau2_groomed(), w);
    hist_topRecoJet_tau3->Fill(topjet.tau3_groomed(), w);
  
    // Relative position
    hist_topRecoJet_dr_lepton->Fill(uhh2::deltaR(topjet.v4(), primlep.v4()), w);
    hist_topRecoJet_dphi_lepton->Fill(uhh2::deltaPhi(topjet.v4(), primlep.v4()), w);
    hist_topRecoJet_dphi_met->Fill(uhh2::deltaPhi(topjet.v4(), event.met->v4()), w);
    hist_met_dphi_lepton->Fill(uhh2::deltaPhi(event.met->v4(), primlep.v4()), w);
  }
  
}
