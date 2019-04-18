#pragma once

#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Hists.h"

namespace uhh2 {

  class HighPtSingleTopHists: public uhh2::Hists {
  public:
    HighPtSingleTopHists(uhh2::Context & ctx, const std::string & dirname);

    virtual void fill(const uhh2::Event & ev) override;
    virtual ~HighPtSingleTopHists();

  protected:

    uhh2::Event::Handle<FlavorParticle> h_primlep;
    TH1F *jets_number, *jet1_eta, *jet2_eta, *jet3_eta, *jet4_eta, *jet1_pt, *jet2_pt, *jet3_pt, *jet4_pt;
    TH1F *mtw, *met;
    TH2F *met_vs_mtw;
  };

}
