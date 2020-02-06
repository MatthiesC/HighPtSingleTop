#include "UHH2/HighPtSingleTop/include/DNNHists.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"

#include "UHH2/common/include/Utils.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;


DNNHists::DNNHists(Context & ctx, const string & dirname, double arg_MIN_PT, double arg_MAX_PT):
  Hists(ctx, dirname) {

  h_dnn_output = ctx.get_handle<double>("DNN_Output");
  
  h_toptaggedjet = ctx.get_handle<TopJet>("TopTaggedJet");
  h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
  h_pseudotop = ctx.get_handle<LorentzVector>("PseudoTop");
  h_wboson = ctx.get_handle<LorentzVector>("WBosonLeptonic");
  h_xjets = ctx.get_handle<vector<Jet>>("TopExJets"); // xjets = eXtra jets (i.e. jets not from single top quark hypothesis)
  h_bxjets = ctx.get_handle<vector<Jet>>("TopExBJets");
  h_bxjets_loose = ctx.get_handle<vector<Jet>>("TopExBJetsLoose");
  h_bxjets_medium = ctx.get_handle<vector<Jet>>("TopExBJetsMedium");
  h_bxjets_tight = ctx.get_handle<vector<Jet>>("TopExBJetsTight");
  h_ijets = ctx.get_handle<vector<Jet>>("TopInJets"); // ijets = jets Inside top tag (i.e. jets likely from single top quark hypothesis)
  h_bijets = ctx.get_handle<vector<Jet>>("TopInBJets");
  h_bijets_loose = ctx.get_handle<vector<Jet>>("TopInBJetsLoose");
  h_bijets_medium = ctx.get_handle<vector<Jet>>("TopInBJetsMedium");
  h_bijets_tight = ctx.get_handle<vector<Jet>>("TopInBJetsTight");

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

  hist_dnn_output = book<TH1F>("dnn_output", "NN output", nBins, 0, 1);
  hist_dnn_output_extraLowRes = book<TH1F>("dnn_output_extraLowRes", "NN output", nBins/2, 0, 1);
  hist_dnn_output_10bins = book<TH1F>("dnn_output_10bins", "NN output", 10, 0, 1);

  hist_tlep_mass = book<TH1F>("tlep_mass", "leptonic pseudo t quark mass [GeV]", nBins, 0, 800);
  hist_wlep_mt = book<TH1F>("wlep_mt", "leptonic W boson m_{T} [GeV]", nBins, 0, 500);
  hist_ptbalance_wlep_tjet = book<TH1F>("ptbalance_wlep_tjet", "[p_{T}(lep. W) - p_{T}(t jet)] / [p_{T}(lep. W) + p_{T}(t jet)]", nBins, -1.2, 0.8);
  hist_ptbalance_tlep_tjet = book<TH1F>("ptbalance_tlep_tjet", "[p_{T}(lep. t) - p_{T}(t jet)] / [p_{T}(lep. t) + p_{T}(t jet)]", nBins, -1.2, 0.8);

  hist_dr_tjet_lepton = book<TH1F>("dr_tjet_lepton", "#DeltaR(t jet, lepton)", nBins, 0, 5);
  hist_dphi_tjet_met = book<TH1F>("dphi_tjet_met", "#Delta#phi(t jet, p_{T}^{miss})", nBins_dPhi, 0, M_PI);
  hist_dphi_met_lepton = book<TH1F>("dphi_met_lepton", "#Delta#phi(p_{T}^{miss}, lepton)", nBins_dPhi, 0, M_PI);

  hist_tjet_tau32 = book<TH1F>("tjet_tau32", "t-tagged jet #tau_{3}/#tau_{2}", nBins, 0, 1);
  hist_tjet_tau21 = book<TH1F>("tjet_tau21", "t-tagged jet #tau_{2}/#tau_{1}", nBins, 0, 1);
  hist_tjet_tau1 = book<TH1F>("tjet_tau1", "t-tagged jet #tau_{1}", nBins, 0, 1);
  hist_tjet_tau2 = book<TH1F>("tjet_tau2", "t-tagged jet #tau_{2}", nBins, 0, 1);
  hist_tjet_tau3 = book<TH1F>("tjet_tau3", "t-tagged jet #tau_{3}", nBins, 0, 1);
  hist_tjet_fpt = book<TH1F>("tjet_fpt", "t-tagged jet f_{pT}^{1}", nBins, 0, 1);
  hist_tjet_mass = book<TH1F>("tjet_mass", "t-tagged jet m_{jet}", nBins, 130, 230);
  hist_tjet_mij_min = book<TH1F>("tjet_mij_min", "t-tagged jet m_{ij}^{min}", nBins, 0, 200);

  hist_xjets_number = book<TH1F>("xjets_number", "number of xjets", 11, -0.5, 10.5);
  hist_bxjets_number = book<TH1F>("bxjets_number", "number of b xjets", 11, -0.5, 10.5);
  hist_bxjets_loose_number = book<TH1F>("bxjets_loose_number", "number of loose b xjets", 11, -0.5, 10.5);
  hist_bxjets_medium_number = book<TH1F>("bxjets_medium_number", "number of medium b xjets", 11, -0.5, 10.5);
  hist_bxjets_tight_number = book<TH1F>("bxjets_tight_number", "number of tight b xjets", 11, -0.5, 10.5);

  hist_ijets_number = book<TH1F>("ijets_number", "number of ijets", 11, -0.5, 10.5);
  hist_bijets_number = book<TH1F>("bijets_number", "number of b ijets", 11, -0.5, 10.5);
  hist_bijets_loose_number = book<TH1F>("bijets_loose_number", "number of loose b ijets", 11, -0.5, 10.5);
  hist_bijets_medium_number = book<TH1F>("bijets_medium_number", "number of medium b ijets", 11, -0.5, 10.5);
  hist_bijets_tight_number = book<TH1F>("bijets_tight_number", "number of tight b ijets", 11, -0.5, 10.5);

  hist_ht_ijets = book<TH1F>("ht_ijets", "H_{T} [GeV] of ijets", nBins, 0, 1000);
  hist_pt_ijets = book<TH1F>("pt_ijets", "p_{T} [GeV] of ijets", nBins, 0, 1000);
  hist_pt_ijets_over_pt_topjet = book<TH1F>("pt_ijets_over_pt_topjet", "p_{T}(ijets) / p_{T}(t jet)", nBins, 0, 2);
  hist_dr_ijets_topjet = book<TH1F>("dr_ijets_topjet", "#DeltaR(ijets axis, t jet axis)", nBins, 0, 2);

  hist_dr_lepton_nextxjet = book<TH1F>("dr_lepton_nextxjet", "#DeltaR(lepton, closest xjet)", nBins, 0, 5);
  hist_dr_lepton_nextbxjet = book<TH1F>("dr_lepton_nextbxjet", "#DeltaR(lepton, closest b xjet) [if no b xjet, closest xjet]", nBins, 0, 5);
  hist_dr_lepton_nextbxjet_guaranteed_b = book<TH1F>("dr_lepton_nextbxjet_guaranteed_b", "#DeltaR(lepton, closest b xjet)", nBins, 0, 5);

  // sphericity tensor of ak4 jets outside t-tagged jet
  hist_s11_xjets = book<TH1F>("s11_xjets", "S_{11} of xjets", nBins, -1, 1);
  hist_s12_xjets = book<TH1F>("s12_xjets", "S_{12} of xjets", nBins, -1, 1);
  hist_s13_xjets = book<TH1F>("s13_xjets", "S_{13} of xjets", nBins, -1, 1);
  hist_s22_xjets = book<TH1F>("s22_xjets", "S_{22} of xjets", nBins, -1, 1);
  hist_s23_xjets = book<TH1F>("s23_xjets", "S_{23} of xjets", nBins, -1, 1);
  hist_s33_xjets = book<TH1F>("s33_xjets", "S_{33} of xjets", nBins, -1, 1);

  hist_ht_xjets = book<TH1F>("ht_xjets", "H_{T} [GeV] of xjets", nBins, 0, 1000);

  hist_pt_xjet1 = book<TH1F>("pt_xjet1", "p_{T} [GeV] of leading xjet", nBins, 0, 1000);
  hist_eta_xjet1 = book<TH1F>("eta_xjet1", "#eta of leading xjet", nBins, -3, 3);
  hist_deepjet_xjet1 = book<TH1F>("deepjet_xjet1", "DeepJet discriminator of leading xjet", nBins, 0, 1);
  hist_mjet_xjet1 = book<TH1F>("mjet_xjet1", "m_{jet} [GeV] of leading xjet", nBins, 0, 100);

  hist_mass_lep_xjet1 = book<TH1F>("mass_lep_xjet1", "m(lepton, lead. xjet) [GeV]", nBins, 0, 500);

  hist_pt_xjet2 = book<TH1F>("pt_xjet2", "p_{T} [GeV] of subleading xjet", nBins, 0, 1000);
  hist_eta_xjet2 = book<TH1F>("eta_xjet2", "#eta of subleading xjet", nBins, -3, 3);
  hist_deepjet_xjet2 = book<TH1F>("deepjet_xjet2", "DeepJet discriminator of subleading xjet", nBins, 0, 1);
  hist_mjet_xjet2 = book<TH1F>("mjet_xjet2", "m_{jet} [GeV] of subleading xjet", nBins, 0, 100);

  hist_dphi_xjet12 = book<TH1F>("dPhi_xjet12", "#Delta#phi betw. lead./sublead. xjet", nBins_dPhi, 0, M_PI);
  hist_dr_xjet12 = book<TH1F>("dR_xjet12", "#DeltaR betw. lead./sublead. xjet", nBins, 0, 5);
  hist_pt_xjet12 = book<TH1F>("pt_xjet12", "p_{T} [GeV] of system of lead.+sublead. xjet", nBins, 0, 1000);
  hist_dr_xjet1l = book<TH1F>("dR_xjet1l", "#DeltaR betw. lepton/lead. xjet", nBins, 0, 5);
  hist_dr_xjet2l = book<TH1F>("dR_xjet2l", "#DeltaR betw. lepton/sublead. xjet", nBins, 0, 5);
}


void DNNHists::fill(const uhh2::Event & event) {

  const double dnn_output = event.get(h_dnn_output);

  const auto & topjet = event.get(h_toptaggedjet);
  const auto & primlep = event.get(h_primlep);
  const auto & pseudotop = event.get(h_pseudotop);
  const auto & wboson = event.get(h_wboson);
  const auto & xjets = event.get(h_xjets);
  const auto & bxjets = event.get(h_bxjets);
  const auto & bxjets_loose = event.get(h_bxjets_loose);
  const auto & bxjets_medium = event.get(h_bxjets_medium);
  const auto & bxjets_tight = event.get(h_bxjets_tight);
  const auto & ijets = event.get(h_ijets);
  const auto & bijets = event.get(h_bijets);
  const auto & bijets_loose = event.get(h_bijets_loose);
  const auto & bijets_medium = event.get(h_bijets_medium);
  const auto & bijets_tight = event.get(h_bijets_tight);
  const vector<Jet> jets = *event.jets;

  const double w = event.weight;


  if(topjet.v4().pt() >= m_MIN_PT && topjet.v4().pt() < m_MAX_PT) {

    hist_dnn_output->Fill(dnn_output, w);
    hist_dnn_output_extraLowRes->Fill(dnn_output, w);
    hist_dnn_output_10bins->Fill(dnn_output, w);

    hist_tlep_mass->Fill(pseudotop.M(), w);
    hist_wlep_mt->Fill(calcMTW(primlep, event), w);
    hist_ptbalance_wlep_tjet->Fill((wboson.Pt() - topjet.v4().Pt())/(wboson.Pt() + topjet.v4().Pt()), w);
    hist_ptbalance_tlep_tjet->Fill((pseudotop.Pt() - topjet.v4().Pt())/(pseudotop.Pt() + topjet.v4().Pt()), w);

    hist_dr_tjet_lepton->Fill(uhh2::deltaR(topjet.v4(), primlep.v4()), w);
    hist_dphi_tjet_met->Fill(uhh2::deltaPhi(topjet.v4(), event.met->v4()), w);
    hist_dphi_met_lepton->Fill(uhh2::deltaPhi(event.met->v4(), primlep.v4()), w);

    hist_tjet_tau32->Fill(topjet.tau3_groomed() / topjet.tau2_groomed(), w);
    hist_tjet_tau21->Fill(topjet.tau2_groomed() / topjet.tau1_groomed(), w);
    hist_tjet_tau1->Fill(topjet.tau1_groomed(), w);
    hist_tjet_tau2->Fill(topjet.tau2_groomed(), w);
    hist_tjet_tau3->Fill(topjet.tau3_groomed(), w);
    hist_tjet_fpt->Fill(topjet.subjets().at(0).v4().pt() / topjet.v4().pt(), w);
    hist_tjet_mass->Fill(topjet.v4().M(), w);
    hist_tjet_mij_min->Fill(min((topjet.subjets().at(0).v4() + topjet.subjets().at(1).v4()).M(), min((topjet.subjets().at(0).v4() + topjet.subjets().at(2).v4()).M(), (topjet.subjets().at(1).v4() + topjet.subjets().at(2).v4()).M())), w);

    hist_xjets_number->Fill(xjets.size(), w);
    hist_bxjets_number->Fill(bxjets.size(), w);
    hist_bxjets_loose_number->Fill(bxjets_loose.size(), w);
    hist_bxjets_medium_number->Fill(bxjets_medium.size(), w);
    hist_bxjets_tight_number->Fill(bxjets_tight.size(), w);

    hist_ijets_number->Fill(ijets.size(), w);
    hist_bijets_number->Fill(bijets.size(), w);
    hist_bijets_loose_number->Fill(bijets_loose.size(), w);
    hist_bijets_medium_number->Fill(bijets_medium.size(), w);
    hist_bijets_tight_number->Fill(bijets_tight.size(), w);

    double ht_ijets(0);
    LorentzVector ak4_topjet(0, 0, 0, 0);
    for(const auto jet : ijets) {
      ht_ijets += jet.v4().Pt();
      ak4_topjet += jet.v4();
    }

    if(ijets.size() > 0) {
      hist_ht_ijets->Fill(ht_ijets, w);
      hist_pt_ijets->Fill(ak4_topjet.pt(), w);
      hist_pt_ijets_over_pt_topjet->Fill(ak4_topjet.pt() / topjet.v4().pt(), w);
      hist_dr_ijets_topjet->Fill(uhh2::deltaR(ak4_topjet, topjet.v4()), w);
    }

    if(xjets.size() > 0) {
      hist_dr_lepton_nextxjet->Fill(uhh2::deltaR(primlep.v4(), nextJet(primlep, xjets)->v4()), w);
      if(bxjets.size() > 0) {
	hist_dr_lepton_nextbxjet->Fill(uhh2::deltaR(primlep.v4(), nextJet(primlep, bxjets)->v4()), w);
	hist_dr_lepton_nextbxjet_guaranteed_b->Fill(uhh2::deltaR(primlep.v4(), nextJet(primlep, bxjets)->v4()), w);
      } else {
	hist_dr_lepton_nextbxjet->Fill(uhh2::deltaR(primlep.v4(), nextJet(primlep, xjets)->v4()), w);
      }
    }

    // xjets sphericity tensor s_ij
    double s11_xjets(0), s12_xjets(0), s13_xjets(0), s22_xjets(0), s23_xjets(0), s33_xjets(0), mag_xjets(0);
    double ht_xjets(0);
    for(const auto jet : xjets) {
      mag_xjets += (jet.v4().Px()*jet.v4().Px()+jet.v4().Py()*jet.v4().Py()+jet.v4().Pz()*jet.v4().Pz());
      s11_xjets += jet.v4().Px()*jet.v4().Px();
      s12_xjets += jet.v4().Px()*jet.v4().Py();
      s13_xjets += jet.v4().Px()*jet.v4().Pz();
      s22_xjets += jet.v4().Py()*jet.v4().Py();
      s23_xjets += jet.v4().Py()*jet.v4().Pz();
      s33_xjets += jet.v4().Pz()*jet.v4().Pz();

      ht_xjets += jet.v4().Pt();
    }

    s11_xjets = s11_xjets / mag_xjets;
    s12_xjets = s12_xjets / mag_xjets;
    s13_xjets = s13_xjets / mag_xjets;
    s22_xjets = s22_xjets / mag_xjets;
    s23_xjets = s23_xjets / mag_xjets;
    s33_xjets = s33_xjets / mag_xjets;

    hist_s11_xjets->Fill(s11_xjets, w);
    hist_s12_xjets->Fill(s12_xjets, w);
    hist_s13_xjets->Fill(s13_xjets, w);
    hist_s22_xjets->Fill(s22_xjets, w);
    hist_s23_xjets->Fill(s23_xjets, w);
    hist_s33_xjets->Fill(s33_xjets, w);

    hist_ht_xjets->Fill(ht_xjets, w);

    if(xjets.size() >= 1) {
      hist_pt_xjet1->Fill(xjets.at(0).v4().Pt(), w);
      hist_eta_xjet1->Fill(xjets.at(0).v4().Eta(), w);
      hist_deepjet_xjet1->Fill(xjets.at(0).btag_DeepJet(), w);
      hist_mjet_xjet1->Fill(xjets.at(0).v4().M(), w);

      hist_mass_lep_xjet1->Fill((xjets.at(0).v4() + primlep.v4()).M(), w);
    }
    if(xjets.size() >= 2) {
      hist_pt_xjet2->Fill(xjets.at(1).v4().Pt(), w);
      hist_eta_xjet2->Fill(xjets.at(1).v4().Eta(), w);
      hist_deepjet_xjet2->Fill(xjets.at(1).btag_DeepJet(), w);
      hist_mjet_xjet2->Fill(xjets.at(1).v4().M(), w);

      hist_dphi_xjet12->Fill(uhh2::deltaPhi(xjets.at(0).v4(), xjets.at(1).v4()), w);
      hist_dr_xjet12->Fill(uhh2::deltaR(xjets.at(0).v4(), xjets.at(1).v4()), w);
      hist_pt_xjet12->Fill((xjets.at(0).v4() + xjets.at(1).v4()).Pt(), w);
      hist_dr_xjet1l->Fill(uhh2::deltaR(xjets.at(0).v4(), primlep.v4()), w);
      hist_dr_xjet2l->Fill(uhh2::deltaR(xjets.at(1).v4(), primlep.v4()), w);
    }

  }

}
