#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"


class DNNSetup: public uhh2::AnalysisModule {

 public:

  explicit DNNSetup(uhh2::Context & ctx, std::vector<uhh2::Event::Handle<float>> & h_dnn_inputs, const unsigned int & n_hotvr, const unsigned int & n_jets, const float & zero_padding_value = -3);
  virtual bool process(uhh2::Event & event) override;

 private:

  std::vector<uhh2::Event::Handle<float>> m_h_dnn_inputs;

  const unsigned int m_n_hotvr, m_n_jets; // How many HOTVR/AK4 jets shall be considered?
  const float m_zeropadding;

  std::vector<std::string> template_event, template_hotvr, template_jet, template_lepton;
  std::vector<std::string> m_inputs;
};
