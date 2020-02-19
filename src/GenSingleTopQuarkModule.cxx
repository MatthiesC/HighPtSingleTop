#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/MCWeight.h"

#include "UHH2/HighPtSingleTop/include/GenSingleTopQuarkHists.h"

using namespace std;
using namespace uhh2;


class GenSingleTopQuarkModule: public AnalysisModule {
public:

  explicit GenSingleTopQuarkModule(Context & ctx);
  virtual bool process(Event & event) override;

private:

  string dataset_version;
  unique_ptr<AnalysisModule> sf_lumi;
  unique_ptr<Hists> histograms;
};


GenSingleTopQuarkModule::GenSingleTopQuarkModule(Context & ctx) {

  dataset_version = ctx.get("dataset_version");

  sf_lumi.reset(new MCLumiWeight(ctx));
  histograms.reset(new GenSingleTopQuarkHists(ctx, "GenSingleTopQuarkHists"));
}


bool GenSingleTopQuarkModule::process(Event & event) {

  sf_lumi->process(event);
  histograms->fill(event);
  return false;
}

UHH2_REGISTER_ANALYSIS_MODULE(GenSingleTopQuarkModule)
