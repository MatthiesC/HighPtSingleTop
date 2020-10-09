#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"


class PrefiringWeights: public uhh2::AnalysisModule {
 public:
  explicit PrefiringWeights(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

 private:
  int syst_direction;
};
