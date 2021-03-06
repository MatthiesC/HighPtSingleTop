#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"


class DNNCustomHists: public uhh2::Hists {

 public:

  DNNCustomHists(uhh2::Context & ctx, const std::string & dirname, double arg_MIN_PT = 0.0, double arg_MAX_PT = 99999.0);
  virtual void fill(const uhh2::Event & event) override;

 protected:

  TH1F *hist_dnn_output;
  TH1F *hist_dnn_output_extraLowRes;
  TH1F *hist_dnn_output_10bins;

  TH1F *hist_tlep_mass;
  TH1F *hist_wlep_mt;
  TH1F *hist_ptbalance_wlep_tjet;
  TH1F *hist_ptbalance_tlep_tjet;

  TH1F *hist_dr_tjet_lepton;
  TH1F *hist_dphi_tjet_met;
  TH1F *hist_dphi_met_lepton;

  TH1F *hist_tjet_tau32;
  TH1F *hist_tjet_tau21;
  TH1F *hist_tjet_tau1;
  TH1F *hist_tjet_tau2;
  TH1F *hist_tjet_tau3;
  TH1F *hist_tjet_fpt;
  TH1F *hist_tjet_mass;
  TH1F *hist_tjet_mij_min;

  TH1F *hist_xjets_number;
  TH1F *hist_bxjets_number;
  TH1F *hist_bxjets_loose_number;
  TH1F *hist_bxjets_medium_number;
  TH1F *hist_bxjets_tight_number;

  TH1F *hist_ijets_number;
  TH1F *hist_bijets_number;
  TH1F *hist_bijets_loose_number;
  TH1F *hist_bijets_medium_number;
  TH1F *hist_bijets_tight_number;

  TH1F *hist_ht_ijets;
  TH1F *hist_pt_ijets;
  TH1F *hist_pt_ijets_over_pt_topjet;
  TH1F *hist_dr_ijets_topjet;

  TH1F *hist_dr_lepton_nextxjet;
  TH1F *hist_dr_lepton_nextbxjet;
  //TH1F *hist_dr_lepton_nextbxjet_guaranteed_b;

  // sphericity tensor of ak4 jets outside t-tagged jet
  //TH1F *hist_s11_xjets;
  //TH1F *hist_s12_xjets;
  //TH1F *hist_s13_xjets;
  //TH1F *hist_s22_xjets;
  //TH1F *hist_s23_xjets;
  //TH1F *hist_s33_xjets;

  TH1F *hist_ht_xjets;

  TH1F *hist_pt_xjet1;
  TH1F *hist_eta_xjet1;
  TH1F *hist_deepjet_xjet1;
  TH1F *hist_mjet_xjet1;

  TH1F *hist_mass_lep_xjet1;

  TH1F *hist_pt_xjet2;
  TH1F *hist_eta_xjet2;
  TH1F *hist_deepjet_xjet2;
  TH1F *hist_mjet_xjet2;

  TH1F *hist_dphi_xjet12;
  TH1F *hist_dr_xjet12;
  TH1F *hist_pt_xjet12;
  TH1F *hist_dr_xjet1l;
  TH1F *hist_dr_xjet1t;
  TH1F *hist_dr_xjet2l;

  TH1F *hist_deepjet_ijet1;
  TH1F *hist_deepjet_ijet2;

  TH1F *hist_ijets_deepjet1_deepjet;
  TH1F *hist_ijets_deepjet2_deepjet;
  TH1F *hist_ijets_deepjetNot1_deepjet;

  TH1F *hist_xjets_deepjet1_deepjet;
  TH1F *hist_xjets_deepjet1_pt;
  TH1F *hist_xjets_deepjet1_eta;
  TH1F *hist_xjets_deepjet1_mjet;
  TH1F *hist_xjets_deepjet1_mass_lepton;
  TH1F *hist_xjets_deepjet1_dr_lepton;
  TH1F *hist_xjets_deepjet1_dr_topjet;

  TH1F *hist_xjets_deepjet2_deepjet;
  TH1F *hist_xjets_deepjetNot1_deepjet;

  uhh2::Event::Handle<double> h_dnn_output;
  uhh2::Event::Handle<TopJet> h_toptaggedjet;
  uhh2::Event::Handle<FlavorParticle> h_primlep;
  uhh2::Event::Handle<LorentzVector> h_pseudotop;
  uhh2::Event::Handle<LorentzVector> h_wboson;
  uhh2::Event::Handle<std::vector<Jet>> h_xjets;
  uhh2::Event::Handle<std::vector<Jet>> h_bxjets;
  uhh2::Event::Handle<std::vector<Jet>> h_bxjets_loose;
  uhh2::Event::Handle<std::vector<Jet>> h_bxjets_medium;
  uhh2::Event::Handle<std::vector<Jet>> h_bxjets_tight;
  uhh2::Event::Handle<std::vector<Jet>> h_ijets;
  uhh2::Event::Handle<std::vector<Jet>> h_bijets;
  uhh2::Event::Handle<std::vector<Jet>> h_bijets_loose;
  uhh2::Event::Handle<std::vector<Jet>> h_bijets_medium;
  uhh2::Event::Handle<std::vector<Jet>> h_bijets_tight;

  double m_MIN_PT, m_MAX_PT;
};
