#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"


class DiscriminatorHists: public uhh2::Hists {

 public:

  DiscriminatorHists(uhh2::Context & ctx, const std::string & dirname, double arg_MIN_PT = 0.0, double arg_MAX_PT = 99999.0);
  virtual void fill(const uhh2::Event & event) override;

 protected:
  
  TH1F *hist_tlep_mass;
  TH1F *hist_wlep_mt;
  TH1F *hist_ptbalance_wlep_tjet;
  TH1F *hist_ptbalance_tlep_tjet;

  TH1F *hist_dr_tjet_lepton;
  TH1F *hist_dphi_tjet_met;
  TH1F *hist_dphi_met_lepton;

  TH1F *hist_tjet_tau32;
  TH1F *hist_tjet_tau21;
  TH1F *hist_tjet_fpt;
  TH1F *hist_tjet_mass;
  TH1F *hist_tjet_mij_min;

  TH1F *hist_jets_number;
  TH1F *hist_bjets_number;
  TH1F *hist_bjets_loose_number;
  TH1F *hist_bjets_medium_number;
  TH1F *hist_bjets_tight_number;

  TH1F *hist_dr_lepton_nextjet;
  TH1F *hist_dr_lepton_nextbjet;
  TH1F *hist_dr_lepton_nextbjet_guaranteed_b;

  // sphericity tensor of ak4 jets outside t-tagged jet
  TH1F *hist_s11_lep;
  TH1F *hist_s12_lep;
  TH1F *hist_s13_lep;
  TH1F *hist_s22_lep;
  TH1F *hist_s23_lep;
  TH1F *hist_s33_lep;

  uhh2::Event::Handle<TopJet> h_toptaggedjet;
  uhh2::Event::Handle<FlavorParticle> h_primlep;
  uhh2::Event::Handle<LorentzVector> h_pseudotop;
  uhh2::Event::Handle<LorentzVector> h_wboson;
  uhh2::Event::Handle<std::vector<Jet>> h_jets;
  uhh2::Event::Handle<std::vector<Jet>> h_bjets;
  uhh2::Event::Handle<std::vector<Jet>> h_bjets_loose;
  uhh2::Event::Handle<std::vector<Jet>> h_bjets_medium;
  uhh2::Event::Handle<std::vector<Jet>> h_bjets_tight;

  double m_MIN_PT, m_MAX_PT;
};
