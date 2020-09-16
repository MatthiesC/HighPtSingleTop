#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/MCWeight.h"

#include "UHH2/HighPtSingleTop/include/GenSingleTopQuarkHists.h"
#include "UHH2/HighPtSingleTop/include/SingleTopGen_tWch.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopSelections.h"


using namespace std;
using namespace uhh2;


class GenSingleTopQuarkModule: public AnalysisModule {
public:

  explicit GenSingleTopQuarkModule(Context & ctx);
  virtual bool process(Event & event) override;

private:

  string dataset_version;
  unique_ptr<AnalysisModule> sf_lumi, GENtW;
  unique_ptr<Selection> slct_GenBJet;
  unique_ptr<Hists> histograms;
  unique_ptr<Hists> histograms_tW;
  unique_ptr<Hists> histograms_tW_bjet_yes;
  unique_ptr<Hists> histograms_tW_bjet_no;
};


GenSingleTopQuarkModule::GenSingleTopQuarkModule(Context & ctx) {

  dataset_version = ctx.get("dataset_version");

  sf_lumi.reset(new MCLumiWeight(ctx));
  histograms.reset(new GenSingleTopQuarkHists(ctx, "allChannels"));
  histograms_tW.reset(new GenSingleTopQuarkHists(ctx, "tW", "tW"));
  histograms_tW_bjet_yes.reset(new GenSingleTopQuarkHists(ctx, "tW_bjet_yes", "tW"));
  histograms_tW_bjet_no.reset(new GenSingleTopQuarkHists(ctx, "tW_bjet_no", "tW"));

  GENtW.reset(new SingleTopGen_tWchProducer(ctx, "h_GENtW"));
  slct_GenBJet.reset(new tWgenSelection(ctx, "GenBJet"));
}


bool GenSingleTopQuarkModule::process(Event & event) {

  sf_lumi->process(event);
  histograms->fill(event);

  bool is_tW = dataset_version.find("ST_tW") == 0;
  if(is_tW) {
    GENtW->process(event);
    histograms_tW->fill(event);
    if(slct_GenBJet->passes(event)) histograms_tW_bjet_yes->fill(event);
    else histograms_tW_bjet_no->fill(event);
  }

  return false;
}

UHH2_REGISTER_ANALYSIS_MODULE(GenSingleTopQuarkModule)
