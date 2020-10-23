#include "UHH2/HighPtSingleTop/include/DNNHists.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"

#include "UHH2/common/include/Utils.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;


DNNHists::DNNHists(Context & ctx, const string & dirname, const vector<string> & arg_used_inputs, const vector<DNNInput> & arg_inputs_info, const vector<string> arg_output_names, const string & arg_binning_var_name, const double & arg_MIN_VAL, const double & arg_MAX_VAL):
  Hists(ctx, dirname) {

  m_h_binning_var = ctx.get_handle<double>(arg_binning_var_name);
  m_h_event_weight = ctx.get_handle<double>("DNNinfo_event_weight");

  m_MIN_VAL = arg_MIN_VAL;
  m_MAX_VAL = arg_MAX_VAL;

  int nBins = 100;
  m_output_binnings = {nBins, 50, 20, 10};

  hist_binning_var = book<TH1F>("binning_var", arg_binning_var_name.c_str(), 10, m_MIN_VAL, m_MAX_VAL);

  for(auto output_name : arg_output_names) {
    m_h_output_values.push_back(ctx.get_handle<double>(output_name));
    for(int n_bins : m_output_binnings) {
      m_output_hists.push_back(book<TH1F>((output_name+"_"+to_string(n_bins)+"bins").c_str(), output_name.c_str(), n_bins, 0, 1)); // TODO: Make histogram name and label more beautiful
    }
  }

  for(auto input_name : arg_used_inputs) {
    for(uint j = 0; j < arg_inputs_info.size(); j++) {
      if(arg_inputs_info.at(j).name() == input_name) m_inputs_info.push_back(arg_inputs_info.at(j));
    }
  }

  for(uint i = 0; i < m_inputs_info.size(); i++) {
    m_h_input_values.push_back(ctx.get_handle<double>(m_inputs_info.at(i).name()));
    m_input_hists.push_back(book<TH1F>(m_inputs_info.at(i).name().c_str(), m_inputs_info.at(i).label().c_str(), m_inputs_info.at(i).nbins(), m_inputs_info.at(i).xlow(), m_inputs_info.at(i).xhigh()));
  }
}


void DNNHists::fill(const Event & event) {

  const double binning_var = event.get(m_h_binning_var);

  if(binning_var <= m_MIN_VAL || binning_var > m_MAX_VAL) return;

  const double w = event.get(m_h_event_weight);

  hist_binning_var->Fill(binning_var, w);

  vector<double> output_values;
  for(auto h : m_h_output_values) {
    output_values.push_back(event.get(h));
  }
  for(uint i = 0; i < m_h_output_values.size(); i++) {
    for(uint j = 0; j < m_output_binnings.size(); j++) {
      m_output_hists.at(i+j)->Fill(output_values.at(i), w);
    }
  }

  vector<double> input_values;
  for(auto h : m_h_input_values) {
    input_values.push_back(event.get(h));
  }
  for(uint i = 0; i < m_inputs_info.size(); i++) {
    m_input_hists.at(i)->Fill(input_values.at(i), w);
  }

  return;
}
