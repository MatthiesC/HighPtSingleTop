#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/HighPtSingleTop/include/DNNSetup.h"


class DNNHists: public uhh2::Hists {

 public:

  DNNHists(uhh2::Context & ctx, const std::string & dirname, const std::vector<std::string> & arg_used_inputs, const std::vector<DNNInput> & arg_inputs_info, const std::vector<std::string> arg_output_names, const std::string & arg_binning_var_name="NO_BINNING_VARIABLE_GIVEN", const double & arg_MIN_VAL = 0., const double & arg_MAX_VAL = 9999.);
  virtual void fill(const uhh2::Event & event) override;

 protected:

  uhh2::Event::Handle<double> m_h_binning_var;
  uhh2::Event::Handle<double> m_h_event_weight;

  double m_MIN_VAL, m_MAX_VAL;

  std::vector<int> m_output_binnings;

  TH1F *hist_binning_var;

  std::vector<uhh2::Event::Handle<double>> m_h_output_values;
  std::vector<TH1F*> m_output_hists_all, m_output_hists_max;

  std::vector<DNNInput> m_inputs_info;
  std::vector<uhh2::Event::Handle<double>> m_h_input_values;
  std::vector<TH1F*> m_input_hists;
};
