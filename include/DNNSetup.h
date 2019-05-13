#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"


class DNNSetup: public uhh2::AnalysisModule {
 public:
  explicit DNNSetup(uhh2::Context & ctx, const unsigned int & n_hotvr, const unsigned int & n_jets, const float & zero_padding_value);
  virtual bool process(uhh2::Event & event) override;

 private:
  const unsigned int m_n_hotvr, m_n_jets; // How many HOTVR/AK4 jets shall be considered?
  const float m_zeropadding; // Float value used to fill variables which are not available for certain events (e.g. too few AK4 jets)
  std::vector<uhh2::Event::Handle<float>> h_inputs;
  std::vector<std::string> m_inputs;
  std::vector<float> m_values;
};
