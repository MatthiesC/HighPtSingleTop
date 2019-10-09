#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/MCWeight.h"

#include "UHH2/HighPtSingleTop/include/SingleTopGen_tChannel.h"
#include "UHH2/HighPtSingleTop/include/SingleTopGenHists_tChannel.h"


using namespace std;
using namespace uhh2;

namespace uhh2 {

  class GeneratorStudiesModule_tChannel: public AnalysisModule {
  public:
    
    explicit GeneratorStudiesModule_tChannel(Context & ctx);
    virtual bool process(Event & event) override;
    
  private:

    string dataset_version;
    std::unique_ptr<AnalysisModule> sf_lumi;//, sf_pileup;
    std::unique_ptr<AnalysisModule> SingleTopGen_tChannelProd;
    std::unique_ptr<Hists> histograms;
  };


  GeneratorStudiesModule_tChannel::GeneratorStudiesModule_tChannel(Context & ctx){

    dataset_version = ctx.get("dataset_version");

    sf_lumi.reset(new MCLumiWeight(ctx));
    //sf_pileup.reset(new MCPileupReweight(ctx,"nominal"));
    histograms.reset(new SingleTopGenHists_tChannel(ctx, "NoCuts"));

    SingleTopGen_tChannelProd.reset(new SingleTopGen_tChannelProducer(ctx, "singletopgen_tChannel"));
  }


  bool GeneratorStudiesModule_tChannel::process(Event & event) {

    sf_lumi->process(event);
    //sf_pileup->process(event);
    SingleTopGen_tChannelProd->process(event);
    histograms->fill(event);
    
    return true;
  }


  UHH2_REGISTER_ANALYSIS_MODULE(GeneratorStudiesModule_tChannel)
}
