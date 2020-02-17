#include "UHH2/HighPtSingleTop/include/DNNHists.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"

#include "UHH2/common/include/Utils.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;


DNNHists::DNNHists(Context & ctx, const string & dirname, const vector<string> inputs, const string binning_var_name, const double arg_MIN_VAL, const double arg_MAX_VAL):
  Hists(ctx, dirname) {

  _h_binning_var = ctx.get_handle<double>(binning_var_name);
  _h_event_weight = ctx.get_handle<double>("DNN_EventWeight");
  _h_dnn_output = ctx.get_handle<double>("DNN_Output");

  _MIN_VAL = arg_MIN_VAL;
  _MAX_VAL = arg_MAX_VAL;

  int nBins = 100;

  hist_binning_var = book<TH1F>("binning_var", binning_var_name.c_str(), 10, _MIN_VAL, _MAX_VAL);

  hist_dnn_output = book<TH1F>("dnn_output", "NN output", nBins, 0, 1);
  hist_dnn_output_20bins = book<TH1F>("dnn_output_20bins", "NN output", 20, 0, 1);
  hist_dnn_output_10bins = book<TH1F>("dnn_output_10bins", "NN output", 10, 0, 1);

  _inputs = inputs;
  for(auto i : _inputs) {
    _h_inputs.push_back(ctx.get_handle<double>(i));
    _input_hists.push_back(book<TH1F>(i.c_str(), i.c_str(), nBins, 0, 9999)); // TODO: adjust x-axis edges
  }
}


void DNNHists::fill(const Event & event) {

  const double binning_var = event.get(_h_binning_var);

  if(binning_var <= _MIN_VAL && binning_var > _MAX_VAL) return;

  const double w = event.get(_h_event_weight);
  const double dnn_val = event.get(_h_dnn_output);

  vector<double> input_vals;
  for(auto h : _h_inputs) {
    input_vals.push_back(event.get(h));
  }

  hist_binning_var->Fill(binning_var, w);
  hist_dnn_output->Fill(dnn_val, w);
  hist_dnn_output_20bins->Fill(dnn_val, w);
  hist_dnn_output_10bins->Fill(dnn_val, w);

  for(uint i = 0; i < _inputs.size(); i++) {
    _input_hists.at(i)->Fill(input_vals.at(i), w);
  }

  return;
}
