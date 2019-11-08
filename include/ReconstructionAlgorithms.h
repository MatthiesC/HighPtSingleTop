#pragma once

#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/FlavorParticle.h"
#include "UHH2/core/include/AnalysisModule.h"


class WBosonLeptonic: public uhh2::AnalysisModule {
 public:
  explicit WBosonLeptonic(uhh2::Context & ctx,
			  const std::string & h_name_wboson="WBosonLeptonic",
			  const std::string & h_name_primlep="PrimaryLepton");

  virtual bool process(uhh2::Event & event) override;

  virtual ~WBosonLeptonic() {};

 private:
  uhh2::Event::Handle<LorentzVector> h_wboson;
  uhh2::Event::Handle<FlavorParticle> h_primlep;
};
