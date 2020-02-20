#include "UHH2/HighPtSingleTop/include/DNNHists.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"

#include "UHH2/common/include/Utils.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;


DNNHists::DNNHists(Context & ctx, const string & dirname, const vector<string> inputs, const vector<DNNInput> inputs_info, const string binning_var_name, const double arg_MIN_VAL, const double arg_MAX_VAL):
  Hists(ctx, dirname) {

  _h_binning_var = ctx.get_handle<double>(binning_var_name);
  _h_event_weight = ctx.get_handle<double>("DNN_EventWeight");
  _h_dnn_output = ctx.get_handle<double>("DNN_Output");
  _h_dnn_output__HighBoost = ctx.get_handle<double>("DNN_Output__HighBoost");

  _MIN_VAL = arg_MIN_VAL;
  _MAX_VAL = arg_MAX_VAL;

  int nBins = 100;

  hist_binning_var = book<TH1F>("binning_var", binning_var_name.c_str(), 10, _MIN_VAL, _MAX_VAL);

  hist_dnn_output = book<TH1F>("dnn_output", "NN output (trained on low boost)", nBins, 0, 1);
  hist_dnn_output_20bins = book<TH1F>("dnn_output_20bins", "NN output (trained on low boost)", 20, 0, 1);
  hist_dnn_output_10bins = book<TH1F>("dnn_output_10bins", "NN output (trained on low boost)", 10, 0, 1);
  hist_dnn_output__HighBoost = book<TH1F>("dnn_output__HighBoost", "NN output (trained on high boost)", nBins, 0, 1);
  hist_dnn_output_20bins__HighBoost = book<TH1F>("dnn_output_20bins__HighBoost", "NN output (trained on high boost)", 20, 0, 1);
  hist_dnn_output_10bins__HighBoost = book<TH1F>("dnn_output_10bins__HighBoost", "NN output (trained on high boost)", 10, 0, 1);

  for(auto i : inputs) {
    for(uint j = 0; j < inputs_info.size(); j++) {
      if(inputs_info.at(j).name() == i) _inputs_info.push_back(inputs_info.at(j));
    }
  }

  for(uint i = 0; i < _inputs_info.size(); i++) {
    _h_inputs.push_back(ctx.get_handle<double>(_inputs_info.at(i).name()));
    _input_hists.push_back(book<TH1F>(_inputs_info.at(i).name().c_str(), _inputs_info.at(i).label().c_str(), _inputs_info.at(i).nbins(), _inputs_info.at(i).xlow(), _inputs_info.at(i).xhigh()));
  }
}


void DNNHists::fill(const Event & event) {

  const double binning_var = event.get(_h_binning_var);

  if(binning_var <= _MIN_VAL || binning_var > _MAX_VAL) return;

  const double w = event.get(_h_event_weight);
  const double dnn_val = event.get(_h_dnn_output);
  const double dnn_val__HighBoost = event.get(_h_dnn_output__HighBoost);

  vector<double> input_vals;
  for(auto h : _h_inputs) {
    input_vals.push_back(event.get(h));
  }

  hist_binning_var->Fill(binning_var, w);
  hist_dnn_output->Fill(dnn_val, w);
  hist_dnn_output_20bins->Fill(dnn_val, w);
  hist_dnn_output_10bins->Fill(dnn_val, w);
  hist_dnn_output__HighBoost->Fill(dnn_val__HighBoost, w);
  hist_dnn_output_20bins__HighBoost->Fill(dnn_val__HighBoost, w);
  hist_dnn_output_10bins__HighBoost->Fill(dnn_val__HighBoost, w);

  for(uint i = 0; i < _inputs_info.size(); i++) {
    _input_hists.at(i)->Fill(input_vals.at(i), w);
  }

  return;
}
