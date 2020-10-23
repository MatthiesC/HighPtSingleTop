#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "lwtnn/LightweightNeuralNetwork.hh"
#include "lwtnn/parse_json.hh"


class DNNApplication: public uhh2::AnalysisModule {

 public:

  explicit DNNApplication(uhh2::Context & ctx, const std::string & object_name);
  virtual bool process(uhh2::Event & event) override;
  bool process_dummy(uhh2::Event & event);

  std::vector<std::string> get_output_names() { return m_dnn_config_outputNames; };

 private:

  std::unique_ptr<lwt::LightweightNeuralNetwork> m_NeuralNetwork;
  std::vector<std::string> m_dnn_config_inputNames, m_dnn_config_outputNames;
  std::vector<uhh2::Event::Handle<double>> m_h_inputs, m_h_outputs;
};
