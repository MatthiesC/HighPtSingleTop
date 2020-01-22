#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/TopJetIds.h"


class DNNSetup: public uhh2::AnalysisModule {

 public:

  explicit DNNSetup(uhh2::Context & ctx, std::vector<uhh2::Event::Handle<float>> & h_dnn_inputs, const unsigned int & n_hotvr, const unsigned int & n_jets, const TopJetId & topJetId, const JetId & bJetId, const float & zero_padding_value = -10);
  virtual bool process(uhh2::Event & event) override;

 private:

  std::vector<uhh2::Event::Handle<float>> m_h_dnn_inputs;

  uhh2::Event::Handle<TopJet> h_toptaggedjet;
  uhh2::Event::Handle<FlavorParticle> h_primlep;
  uhh2::Event::Handle<LorentzVector> h_pseudotop;
  uhh2::Event::Handle<LorentzVector> h_wboson;
  uhh2::Event::Handle<std::vector<Jet>> h_xjets;

  const unsigned int m_n_hotvr, m_n_jets; // How many HOTVR/AK4 jets shall be considered?
  const TopJetId m_topjetid;
  const JetId m_bjetid;
  const float m_zeropadding;

  std::vector<std::string> template_event, template_hotvr, template_jet, template_lepton, template_custom;
  std::vector<std::string> m_inputs;
};
