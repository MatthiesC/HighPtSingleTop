#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"


class DNNHists: public uhh2::Hists {

 public:

  DNNHists(uhh2::Context & ctx, const std::string & dirname, const std::vector<std::string> inputs, const std::string binning_var_name, const double arg_MIN_VAL = 0., const double arg_MAX_VAL = 99999.);
  virtual void fill(const uhh2::Event & event) override;

 protected:

  uhh2::Event::Handle<double> _h_binning_var;
  uhh2::Event::Handle<double> _h_event_weight;
  uhh2::Event::Handle<double> _h_dnn_output;

  double _MIN_VAL, _MAX_VAL;

  TH1F *hist_binning_var;
  TH1F *hist_dnn_output;
  TH1F *hist_dnn_output_20bins;
  TH1F *hist_dnn_output_10bins;

  std::vector<std::string> _inputs;
  std::vector<uhh2::Event::Handle<double>> _h_inputs;
  std::vector<TH1F*> _input_hists;
};
