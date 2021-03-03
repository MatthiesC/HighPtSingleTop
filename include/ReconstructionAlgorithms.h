#pragma once

#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/FlavorParticle.h"
#include "UHH2/core/include/AnalysisModule.h"


class WBosonLeptonic: public uhh2::AnalysisModule {
 public:
  explicit WBosonLeptonic(uhh2::Context & ctx,
			  const std::string & h_name_wboson = "WBosonLeptonic",
			  const std::string & h_name_primlep = "PrimaryLepton");

  virtual bool process(uhh2::Event & event) override;

  virtual ~WBosonLeptonic() {};

 private:
  uhh2::Event::Handle<LorentzVector> h_wboson;
  uhh2::Event::Handle<FlavorParticle> h_primlep;
};


class PseudoTopLeptonic: public uhh2::AnalysisModule {
 public:
  explicit PseudoTopLeptonic(uhh2::Context & ctx,
			     const bool use_btagging_info,
			     const std::string & h_name_wboson = "WBosonLeptonic",
			     const std::string & h_name_bjets = "BJets",
           const std::string & h_name_primlep = "PrimaryLepton",
			     const std::string & h_name_pseudotop = "PseudoTop",
			     const double topmassMC = 172.5);

  virtual bool process(uhh2::Event & event) override;

  virtual ~PseudoTopLeptonic() {};

 private:
  bool m_use_btagging_info;
  uhh2::Event::Handle<LorentzVector> h_wboson;
  uhh2::Event::Handle<std::vector<Jet>> h_bjets;
  uhh2::Event::Handle<FlavorParticle> h_primlep;
  uhh2::Event::Handle<LorentzVector> h_pseudotop;
  double m_topmassMC;
};
