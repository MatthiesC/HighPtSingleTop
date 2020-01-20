#include "UHH2/HighPtSingleTop/include/DiscriminatorHists.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"

#include "UHH2/common/include/Utils.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;


DiscriminatorHists::DiscriminatorHists(Context & ctx, const string & dirname, double arg_MIN_PT, double arg_MAX_PT):
  Hists(ctx, dirname) {
  
  h_toptaggedjet = ctx.get_handle<TopJet>("TopTaggedJet");
  h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
  h_pseudotop = ctx.get_handle<LorentzVector>("PseudoTop");
  h_wboson = ctx.get_handle<LorentzVector>("WBosonLeptonic");
  h_jets = ctx.get_handle<vector<Jet>>("NonTopJets");
  h_bjets = ctx.get_handle<vector<Jet>>("NonTopBJets");
  h_bjets_loose = ctx.get_handle<vector<Jet>>("NonTopBJetsLoose");
  h_bjets_medium = ctx.get_handle<vector<Jet>>("NonTopBJetsMedium");
  h_bjets_tight = ctx.get_handle<vector<Jet>>("NonTopBJetsTight");

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

  hist_tlep_mass = book<TH1F>("tlep_mass", "leptonic pseudo t quark mass [GeV]", nBins, 0, 800);
  hist_wlep_mt = book<TH1F>("wlep_mt", "leptonic W boson m_{T} [GeV]", nBins, 0, 500);
  hist_ptbalance_wlep_tjet = book<TH1F>("ptbalance_wlep_tjet", "[p_{T}(lep. W) - p_{T}(t jet)] / [p_{T}(lep. W) + p_{T}(t jet)]", nBins, -1.2, 0.8);
  hist_ptbalance_tlep_tjet = book<TH1F>("ptbalance_tlep_tjet", "[p_{T}(lep. t) - p_{T}(t jet)] / [p_{T}(lep. t) + p_{T}(t jet)]", nBins, -1.2, 0.8);

  hist_dr_tjet_lepton = book<TH1F>("dr_tjet_lepton", "#DeltaR(t jet, lepton)", nBins, 0, 5);
  hist_dphi_tjet_met = book<TH1F>("dphi_tjet_met", "#Delta#phi(t jet, p_{T}^{miss})", nBins_dPhi, 0, M_PI);
  hist_dphi_met_lepton = book<TH1F>("dphi_met_lepton", "#Delta#phi(p_{T}^{miss}, lepton)", nBins_dPhi, 0, M_PI);

  hist_tjet_tau32 = book<TH1F>("tjet_tau32", "t-tagged jet #tau_{3}/#tau_{2}", nBins, 0, 1);
  hist_tjet_tau21 = book<TH1F>("tjet_tau21", "t-tagged jet #tau_{2}/#tau_{1}", nBins, 0, 1);
  hist_tjet_fpt = book<TH1F>("tjet_fpt", "t-tagged jet f_{pT}^{1}", nBins, 0, 1);
  hist_tjet_mass = book<TH1F>("tjet_mass", "t-tagged jet m_{jet}", nBins, 130, 230);
  hist_tjet_mij_min = book<TH1F>("tjet_mij_min", "t-tagged jet m_{ij}^{min}", nBins, 0, 200);

  hist_jets_number = book<TH1F>("jets_number", "number of AK4 jets w/o t-jet overlap", 11, -0.5, 10.5);
  hist_bjets_number = book<TH1F>("bjets_number", "number of b jets w/o t-jet overlap", 11, -0.5, 10.5);
  hist_bjets_loose_number = book<TH1F>("bjets_loose_number", "number of loose b jets w/o t-jet overlap", 11, -0.5, 10.5);
  hist_bjets_medium_number = book<TH1F>("bjets_medium_number", "number of medium b jets w/o t-jet overlap", 11, -0.5, 10.5);
  hist_bjets_tight_number = book<TH1F>("bjets_tight_number", "number of tight b jets w/o t-jet overlap", 11, -0.5, 10.5);

  hist_dr_lepton_nextjet = book<TH1F>("dr_lepton_nextjet", "#DeltaR(lepton, closest AK4 jet w/o t-jet overlap)", nBins, 0, 5);
  hist_dr_lepton_nextbjet = book<TH1F>("dr_lepton_nextbjet", "#DeltaR(lepton, closest b jet w/o t-jet overlap) [if no b jet, closest jet]", nBins, 0, 5);
  hist_dr_lepton_nextbjet_guaranteed_b = book<TH1F>("dr_lepton_nextbjet_guaranteed_b", "#DeltaR(lepton, closest b jet w/o t-jet overlap)", nBins, 0, 5);

  // sphericity tensor of ak4 jets outside t-tagged jet
  hist_s11_lep = book<TH1F>("s11_lep", "S_{11} of AK4 jets w/o t-jet overlap", nBins, 0, 1);
  hist_s12_lep = book<TH1F>("s12_lep", "S_{12} of AK4 jets w/o t-jet overlap", nBins, 0, 1);
  hist_s13_lep = book<TH1F>("s13_lep", "S_{13} of AK4 jets w/o t-jet overlap", nBins, 0, 1);
  hist_s22_lep = book<TH1F>("s22_lep", "S_{22} of AK4 jets w/o t-jet overlap", nBins, 0, 1);
  hist_s23_lep = book<TH1F>("s23_lep", "S_{23} of AK4 jets w/o t-jet overlap", nBins, 0, 1);
  hist_s33_lep = book<TH1F>("s33_lep", "S_{33} of AK4 jets w/o t-jet overlap", nBins, 0, 1);

}


void DiscriminatorHists::fill(const uhh2::Event & event) {

  const auto & topjet = event.get(h_toptaggedjet);
  const auto & primlep = event.get(h_primlep);
  const auto & pseudotop = event.get(h_pseudotop);
  const auto & wboson = event.get(h_wboson);
  const auto & jets = event.get(h_jets);
  const auto & bjets = event.get(h_bjets);
  const auto & bjets_loose = event.get(h_bjets_loose);
  const auto & bjets_medium = event.get(h_bjets_medium);
  const auto & bjets_tight = event.get(h_bjets_tight);

  const double w = event.weight;


  if(topjet.v4().pt() >= m_MIN_PT && topjet.v4().pt() < m_MAX_PT) {
    
    hist_tlep_mass->Fill(pseudotop.M(), w);
    hist_wlep_mt->Fill(calcMTW(primlep, event), w);
    hist_ptbalance_wlep_tjet->Fill((wboson.Pt() - topjet.v4().Pt())/(wboson.Pt() + topjet.v4().Pt()), w);
    hist_ptbalance_tlep_tjet->Fill((pseudotop.Pt() - topjet.v4().Pt())/(pseudotop.Pt() + topjet.v4().Pt()), w);

    hist_dr_tjet_lepton->Fill(uhh2::deltaR(topjet.v4(), primlep.v4()), w);
    hist_dphi_tjet_met->Fill(uhh2::deltaPhi(topjet.v4(), event.met->v4()), w);
    hist_dphi_met_lepton->Fill(uhh2::deltaPhi(event.met->v4(), primlep.v4()), w);

    hist_tjet_tau32->Fill(topjet.tau3_groomed() / topjet.tau2_groomed(), w);
    hist_tjet_tau21->Fill(topjet.tau2_groomed() / topjet.tau1_groomed(), w);
    hist_tjet_fpt->Fill(topjet.subjets().at(0).v4().pt() / topjet.v4().pt(), w);
    hist_tjet_mass->Fill(topjet.v4().M(), w);
    hist_tjet_mij_min->Fill(min((topjet.subjets().at(0).v4() + topjet.subjets().at(1).v4()).M(), min((topjet.subjets().at(0).v4() + topjet.subjets().at(2).v4()).M(), (topjet.subjets().at(1).v4() + topjet.subjets().at(2).v4()).M())), w);

    hist_jets_number->Fill(jets.size(), w);
    hist_bjets_number->Fill(bjets.size(), w);
    hist_bjets_loose_number->Fill(bjets_loose.size(), w);
    hist_bjets_medium_number->Fill(bjets_medium.size(), w);
    hist_bjets_tight_number->Fill(bjets_tight.size(), w);

    if(jets.size() > 0) {
      hist_dr_lepton_nextjet->Fill(uhh2::deltaR(primlep.v4(), nextJet(primlep, jets)->v4()), w);
      if(bjets.size() > 0) {
	hist_dr_lepton_nextbjet->Fill(uhh2::deltaR(primlep.v4(), nextJet(primlep, bjets)->v4()), w);
	hist_dr_lepton_nextbjet_guaranteed_b->Fill(uhh2::deltaR(primlep.v4(), nextJet(primlep, bjets)->v4()), w);
      } else {
	hist_dr_lepton_nextbjet->Fill(uhh2::deltaR(primlep.v4(), nextJet(primlep, jets)->v4()), w);
      }
    }

    // jets sphericity tensor in leptonic hemisphere
    double s11_lep(0), s12_lep(0), s13_lep(0), s22_lep(0), s23_lep(0), s33_lep(0), mag_lep(0);
    for(const auto jet : jets) {
      mag_lep += (jet.v4().Px()*jet.v4().Px()+jet.v4().Py()*jet.v4().Py()+jet.v4().Pz()*jet.v4().Pz());
      s11_lep += jet.v4().Px()*jet.v4().Px();
      s12_lep += jet.v4().Px()*jet.v4().Py();
      s13_lep += jet.v4().Px()*jet.v4().Pz();
      s22_lep += jet.v4().Py()*jet.v4().Py();
      s23_lep += jet.v4().Py()*jet.v4().Pz();
      s33_lep += jet.v4().Pz()*jet.v4().Pz();
    }
    s11_lep = s11_lep / mag_lep;
    s12_lep = s12_lep / mag_lep;
    s13_lep = s13_lep / mag_lep;
    s22_lep = s22_lep / mag_lep;
    s23_lep = s23_lep / mag_lep;
    s33_lep = s33_lep / mag_lep;

    hist_s11_lep->Fill(s11_lep, w);
    hist_s12_lep->Fill(s12_lep, w);
    hist_s13_lep->Fill(s13_lep, w);
    hist_s22_lep->Fill(s22_lep, w);
    hist_s23_lep->Fill(s23_lep, w);
    hist_s33_lep->Fill(s33_lep, w);
  }

}
