#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/Utils.h"


class DNNInput {

 public:

  DNNInput(std::string arg_input_name, std::string arg_plot_label = "NO PLOT LABEL SET", double arg_xlow = 0, double arg_xhigh = 1, uint arg_nbins = 100);
  std::string name() const { return _input_name; };
  void set_name(std::string new_name) { _input_name = new_name; };
  std::string label() const { return _plot_label; };
  double xlow() const { return _xlow; };
  double xhigh() const { return _xhigh; };
  uint nbins() const { return _nbins; };

 private:

  std::string _input_name;
  std::string _plot_label;
  double _xlow;
  double _xhigh;
  uint _nbins;
};


class DNNSetup: public uhh2::AnalysisModule {

 public:

  explicit DNNSetup(uhh2::Context & ctx, const BTag::algo & algo, const double & zero_padding_value = 0.);
  virtual bool process(uhh2::Event & event) override;

  std::vector<std::string> get_input_names_ttag() { return m_input_names_ttag; };
  std::vector<std::string> get_input_names_wtag() { return m_input_names_wtag; };
  std::vector<DNNInput> get_inputs_info_ttag() { return m_inputs_info_ttag; };
  std::vector<DNNInput> get_inputs_info_wtag() { return m_inputs_info_wtag; };

 private:

  void calculate_inputs_for_ttag_dnn(uhh2::Event & event);
  void set_dummy_inputs_for_ttag_dnn(uhh2::Event & event);
  void calculate_inputs_for_wtag_dnn(uhh2::Event & event);
  void set_dummy_inputs_for_wtag_dnn(uhh2::Event & event);

  BTag::algo m_btag_algo;
  double m_zeropadding;

  Year m_year;
  int m_year_int;
  int m_channel_int;

  uhh2::Event::Handle<int> m_h_which_region;

  uhh2::Event::Handle<TopJet> m_h_tjet;
  uhh2::Event::Handle<TopJet> m_h_wjet;
  uhh2::Event::Handle<FlavorParticle> m_h_primlep;
  uhh2::Event::Handle<LorentzVector> m_h_wboson;
  uhh2::Event::Handle<LorentzVector> m_h_pseudotop;
  uhh2::Event::Handle<std::vector<Jet>> m_h_xjets_tjet;
  uhh2::Event::Handle<std::vector<Jet>> m_h_ijets_tjet;
  uhh2::Event::Handle<std::vector<Jet>> m_h_xjets_wjet;
  uhh2::Event::Handle<std::vector<Jet>> m_h_ijets_wjet;

  uhh2::Event::Handle<double> m_h_event_weight;

  std::vector<DNNInput> m_inputs_template_ttag, m_inputs_template_wtag, m_inputs_info_ttag, m_inputs_info_wtag;
  std::vector<std::string> m_input_names_ttag, m_input_names_wtag;

  std::vector<uhh2::Event::Handle<double>> m_h_dnn_inputs_ttag, m_h_dnn_inputs_wtag;
};
