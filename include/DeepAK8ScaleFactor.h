#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/Utils.h"

#include "UHH2/HighPtSingleTop/include/TaggedJets.h"

#include <TGraphAsymmErrors.h>


class DeepAK8ScaleFactor: public uhh2::AnalysisModule {
 public:
  explicit DeepAK8ScaleFactor(uhh2::Context & ctx, const std::string & object_name, const bool & mass_decorrelated, const WTaggedJets::wp & working_point);
  virtual bool process(uhh2::Event & event) override;

 private:
  int syst_direction;
  Year year;
  double pt_minimum, pt_maximum;
  std::unique_ptr<TGraphAsymmErrors> sf_graph;
  uhh2::Event::Handle<float> h_deepak8_weight, h_deepak8_weight_up, h_deepak8_weight_down;
  uhh2::Event::Handle<TopJet> h_taggedjet;
};
