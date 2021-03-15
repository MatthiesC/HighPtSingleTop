#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"


class VariablesOfInterest: public uhh2::AnalysisModule {

public:
  explicit VariablesOfInterest(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

private:
  uhh2::Event::Handle<int> m_h_which_region;

  uhh2::Event::Handle<TopJet> m_h_tjet;
  uhh2::Event::Handle<TopJet> m_h_wjet;
  uhh2::Event::Handle<FlavorParticle> m_h_primlep;
  uhh2::Event::Handle<LorentzVector> m_h_wboson;
  uhh2::Event::Handle<LorentzVector> m_h_pseudotop;

  void set_ttag_variables(uhh2::Event & event);
  void set_ttag_dummies(uhh2::Event & event);
  void set_wtag_variables(uhh2::Event & event);
  void set_wtag_dummies(uhh2::Event & event);

  uhh2::Event::Handle<double> m_h_voi_ttag__tjet_pt;
  uhh2::Event::Handle<double> m_h_voi_ttag__tjet_eta;
  uhh2::Event::Handle<double> m_h_voi_ttag__lepton_pt;
  uhh2::Event::Handle<double> m_h_voi_ttag__tW_dphi;
  uhh2::Event::Handle<double> m_h_voi_ttag__tW_mass;

  uhh2::Event::Handle<double> m_h_voi_wtag__pseudotop_pt;
  uhh2::Event::Handle<double> m_h_voi_wtag__pseudotop_eta;
  uhh2::Event::Handle<double> m_h_voi_wtag__lepton_pt;
  uhh2::Event::Handle<double> m_h_voi_wtag__tW_dphi;
  uhh2::Event::Handle<double> m_h_voi_wtag__tW_mass;
};
