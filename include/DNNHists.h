#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/HighPtSingleTop/include/DNNSetup.h"


class DNNHists: public uhh2::Hists {

 public:

  DNNHists(uhh2::Context & ctx, const std::string & dirname, const std::vector<std::string> inputs, const std::vector<DNNInput> inputs_info, const std::string binning_var_name, const double arg_MIN_VAL = 0., const double arg_MAX_VAL = 9999.);
  virtual void fill(const uhh2::Event & event) override;

 protected:

  uhh2::Event::Handle<double> _h_binning_var;
  uhh2::Event::Handle<double> _h_event_weight;
  uhh2::Event::Handle<double> _h_dnn_output;
  uhh2::Event::Handle<std::vector<Jet>> _h_xjets_bloose;
  uhh2::Event::Handle<std::vector<Jet>> _h_xjets_bmedium;
  uhh2::Event::Handle<std::vector<Jet>> _h_xjets_btight;

  double _MIN_VAL, _MAX_VAL;

  TH1F *hist_binning_var;
  TH1F *hist_dnn_output;
  TH1F *hist_dnn_output_20bins;
  TH1F *hist_dnn_output_10bins;

  std::vector<DNNInput> _inputs_info;
  std::vector<uhh2::Event::Handle<double>> _h_inputs;
  std::vector<TH1F*> _input_hists;

  TH1F *hist_xjets_bloose;
  TH1F *hist_xjets_bmedium;
  TH1F *hist_xjets_btight;
};
