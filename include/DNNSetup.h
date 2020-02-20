#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/TopJetIds.h"


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

  explicit DNNSetup(uhh2::Context & ctx, std::vector<uhh2::Event::Handle<double>> & h_dnn_inputs, const unsigned int & n_hotvr, const unsigned int & n_jets, const TopJetId & topJetId, const JetId & bJetId, const double & zero_padding_value = -10);
  virtual bool process(uhh2::Event & event) override;

  std::vector<std::string> inputs() { return m_input_names; };
  std::vector<DNNInput> inputs_info() { return m_inputs_info; };

 private:

  std::vector<uhh2::Event::Handle<double>> m_h_dnn_inputs;

  uhh2::Event::Handle<TopJet> h_toptaggedjet;
  uhh2::Event::Handle<FlavorParticle> h_primlep;
  uhh2::Event::Handle<LorentzVector> h_pseudotop;
  uhh2::Event::Handle<LorentzVector> h_wboson;
  uhh2::Event::Handle<std::vector<Jet>> h_xjets;
  uhh2::Event::Handle<std::vector<Jet>> h_ijets;

  uhh2::Event::Handle<double> h_event_weight;
  uhh2::Event::Handle<double> h_toptag_pt;

  const unsigned int m_n_hotvr, m_n_jets; // How many HOTVR/AK4 jets shall be considered?
  const TopJetId m_topjetid;
  const JetId m_bjetid;
  const double m_zeropadding;

  std::vector<DNNInput> template_event, template_hotvr, template_jet, template_lepton, template_custom, m_inputs_info;
  std::vector<std::string> m_input_names;
};
