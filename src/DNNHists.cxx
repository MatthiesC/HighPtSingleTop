#include "UHH2/HighPtSingleTop/include/DNNHists.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"

#include "UHH2/common/include/Utils.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;


DNNHists::DNNHists(Context & ctx, const string & dirname, const vector<string> & arg_used_inputs, const vector<DNNInput> & arg_inputs_info, const string & arg_binning_var_name, const double & arg_MIN_VAL, const double & arg_MAX_VAL):
  Hists(ctx, dirname) {

  // _h_binning_var = ctx.get_handle<double>(binning_var_name);
  m_h_event_weight = ctx.get_handle<double>("DNNinfo_event_weight");
  // _h_dnn_output = ctx.get_handle<double>("DNN_Output");
  // _h_xjets_bloose = ctx.get_handle<vector<Jet>>("ExBJetsLoose_Top");
  // _h_xjets_bmedium = ctx.get_handle<vector<Jet>>("ExBJetsMedium_Top");
  // _h_xjets_btight = ctx.get_handle<vector<Jet>>("ExBJetsTight_Top");

  m_MIN_VAL = arg_MIN_VAL;
  m_MAX_VAL = arg_MAX_VAL;

  int nBins = 100;

  // hist_binning_var = book<TH1F>("binning_var", binning_var_name.c_str(), 10, _MIN_VAL, _MAX_VAL);
  //
  // hist_dnn_output = book<TH1F>("dnn_output", "NN output", nBins, 0, 1);
  // hist_dnn_output_50bins = book<TH1F>("dnn_output_50bins", "NN output", 50, 0, 1);
  // hist_dnn_output_20bins = book<TH1F>("dnn_output_20bins", "NN output", 20, 0, 1);
  // hist_dnn_output_10bins = book<TH1F>("dnn_output_10bins", "NN output", 10, 0, 1);

  for(auto input_name : arg_used_inputs) {
    for(uint j = 0; j < arg_inputs_info.size(); j++) {
      if(arg_inputs_info.at(j).name() == input_name) m_inputs_info.push_back(arg_inputs_info.at(j));
    }
  }

  for(uint i = 0; i < m_inputs_info.size(); i++) {
    m_h_input_values.push_back(ctx.get_handle<double>(m_inputs_info.at(i).name()));
    m_input_hists.push_back(book<TH1F>(m_inputs_info.at(i).name().c_str(), m_inputs_info.at(i).label().c_str(), m_inputs_info.at(i).nbins(), m_inputs_info.at(i).xlow(), m_inputs_info.at(i).xhigh()));
  }

  // hist_xjets_bloose = book<TH1F>("xjets_bloose", "Number of b-tagged xjets (loose WP)", 11, -0.5, 10.5);
  // hist_xjets_bmedium = book<TH1F>("xjets_bmedium", "Number of b-tagged xjets (medium WP)", 11, -0.5, 10.5);
  // hist_xjets_btight = book<TH1F>("xjets_btight", "Number of b-tagged xjets (tight WP)", 11, -0.5, 10.5);
}


void DNNHists::fill(const Event & event) {

  // const double binning_var = event.get(_h_binning_var);

  // if(binning_var <= m_MIN_VAL || binning_var > m_MAX_VAL) return;

  const double w = event.get(m_h_event_weight);
  // const double dnn_val = event.get(_h_dnn_output);

  vector<double> input_values;
  for(auto h : m_h_input_values) {
    input_values.push_back(event.get(h));
  }

  // hist_binning_var->Fill(binning_var, w);
  // hist_dnn_output->Fill(dnn_val, w);
  // hist_dnn_output_50bins->Fill(dnn_val, w);
  // hist_dnn_output_20bins->Fill(dnn_val, w);
  // hist_dnn_output_10bins->Fill(dnn_val, w);

  for(uint i = 0; i < m_inputs_info.size(); i++) {
    m_input_hists.at(i)->Fill(input_values.at(i), w);
  }

  // vector<Jet> xjets_bloose = event.get(_h_xjets_bloose);
  // vector<Jet> xjets_bmedium = event.get(_h_xjets_bmedium);
  // vector<Jet> xjets_btight = event.get(_h_xjets_btight);

  // hist_xjets_bloose->Fill(xjets_bloose.size(), w);
  // hist_xjets_bmedium->Fill(xjets_bmedium.size(), w);
  // hist_xjets_btight->Fill(xjets_btight.size(), w);

  return;
}
