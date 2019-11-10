#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"


class DNNHists: public uhh2::Hists {

 public:

  DNNHists(uhh2::Context & ctx, const std::string & dirname, double arg_MIN_PT = 0.0, double arg_MAX_PT = 99999.0);
  virtual void fill(const uhh2::Event & event) override;

 protected:
  
  TH1F *hist_tlep_mass, *hist_tlep_pt, *hist_tlep_eta, *hist_tlep_phi, *hist_dr_tlep_tjet;
  TH1F *hist_tlep_mass_guaranteed_b, *hist_tlep_pt_guaranteed_b, *hist_tlep_eta_guaranteed_b, *hist_tlep_phi_guaranteed_b, *hist_dr_tlep_tjet_guaranteed_b;

  TH1F *hist_wlep_mass, *hist_wlep_mt, *hist_wlep_pt, *hist_wlep_eta, *hist_wlep_phi;

  TH1F *hist_ptbalance_wlep_tjet, *hist_ptbalance_tlep_tjet;

  TH1F *hist_dr_wlep_tjet, *hist_dphi_wlep_tjet, *hist_dphi_tlep_tjet;

  TH1F *hist_jets_number, *hist_bjets_number;

  TH1F *hist_dr_lepton_nextjet, *hist_dr_lepton_nextbjet, *hist_dr_lepton_nextbjet_guaranteed_b;

  TH1F *hist_dr_tjet_lepton, *hist_dphi_tjet_lepton, *hist_dphi_tjet_met, *hist_dphi_met_lepton;
  
  uhh2::Event::Handle<TopJet> h_toptaggedjet;
  uhh2::Event::Handle<FlavorParticle> h_primlep;
  uhh2::Event::Handle<LorentzVector> h_pseudotop;
  uhh2::Event::Handle<LorentzVector> h_wboson;
  uhh2::Event::Handle<std::vector<Jet>> h_bjets;

  double m_MIN_PT, m_MAX_PT;
};
