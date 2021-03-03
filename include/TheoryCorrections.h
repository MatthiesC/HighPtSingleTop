#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"


class TopPtReweighting: public uhh2::AnalysisModule {
 public:
  explicit TopPtReweighting(uhh2::Context & ctx, double a, double b, const std::string& weight_name="weight_ttbar_toppt");
  virtual bool process(uhh2::Event & event) override;
  bool process_dummy(uhh2::Event & event);

 private:
  double a_, b_;
  uhh2::Event::Handle<double> h_weight;
};


class VJetsReweighting: public uhh2::AnalysisModule {
 public:
  explicit VJetsReweighting(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;
  bool process_dummy(uhh2::Event & event);

 private:
  uhh2::Event::Handle<double> h_weight;
};
