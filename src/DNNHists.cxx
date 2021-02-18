#include "UHH2/HighPtSingleTop/include/DNNHists.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"

#include "UHH2/common/include/Utils.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;


DNNHists::DNNHists(Context & ctx, const string & dirname, const vector<string> & arg_used_inputs, const vector<DNNInput> & arg_inputs_info, const vector<string> arg_output_names, const string & arg_binning_var_name, const double & arg_MIN_VAL, const double & arg_MAX_VAL):
  Hists(ctx, dirname) {

  binning_var_given = !arg_binning_var_name.empty();
  if(binning_var_given) m_h_binning_var = ctx.get_handle<double>(arg_binning_var_name);
  m_h_event_weight = ctx.get_handle<double>("DNNinfo_event_weight");

  m_MIN_VAL = arg_MIN_VAL;
  m_MAX_VAL = arg_MAX_VAL;

  int nBins = 100;
  m_output_binnings = {nBins, 50, 25, 10, 5, 1000};

  hist_counting = book<TH1F>("counting", "Counting", 1, 0, 1);
  hist_binning_var = book<TH1F>("binning_var", arg_binning_var_name.c_str(), 20, m_MIN_VAL, m_MAX_VAL);

  for(auto output_name : arg_output_names) {
    m_h_output_values.push_back(ctx.get_handle<double>(output_name));
    for(int n_bins : m_output_binnings) {
      m_output_hists_all.push_back(book<TH1F>((output_name+"_all_"+to_string(n_bins)+"bins").c_str(), (output_name+"_all").c_str(), n_bins, 0, 1)); // TODO: Make histogram name and label more beautiful
      m_output_hists_max.push_back(book<TH1F>((output_name+"_max_"+to_string(n_bins)+"bins").c_str(), (output_name+"_max").c_str(), n_bins, 0, 1)); // TODO: Make histogram name and label more beautiful
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

  double binning_var(0); // needs to be initialized to avoid compilation errors
  if(binning_var_given) binning_var = event.get(m_h_binning_var);

  if(binning_var_given && (binning_var <= m_MIN_VAL || binning_var > m_MAX_VAL)) return;

  const double w = event.get(m_h_event_weight);

  hist_counting->Fill(0.5, w);
  if(binning_var_given) hist_binning_var->Fill(binning_var, w);

  vector<double> output_values;
  for(auto h : m_h_output_values) {
    output_values.push_back(event.get(h));
  }
  uint i_max(0);
  double max_output_value(0);
  for(uint i = 0; i < output_values.size(); i++) {
    if(output_values.at(i) > max_output_value) {
      max_output_value = output_values.at(i);
      i_max = i;
    }
  }
  for(uint i = 0; i < m_h_output_values.size(); i++) {
    for(uint j = 0; j < m_output_binnings.size(); j++) {
      m_output_hists_all.at(i*m_output_binnings.size()+j)->Fill(output_values.at(i), w);
      if(i == i_max) m_output_hists_max.at(i*m_output_binnings.size()+j)->Fill(max_output_value, w);
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
