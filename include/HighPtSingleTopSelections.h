#pragma once

#include "UHH2/core/include/fwd.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Selection.h"
#include "UHH2/core/include/FlavorParticle.h"
#include "UHH2/core/include/AnalysisModule.h" // without that, can't use uhh2::Context

#include "UHH2/HighPtSingleTop/include/SingleTopGen_tWch.h"


namespace uhh2 {
    
  class METSelection: public uhh2::Selection {
  public:
    METSelection(double met_min_ = 0);
    virtual bool passes(const uhh2::Event & event) override;
  private:
    double met_min;
  };

  class MTWSelection: public uhh2::Selection {
  public:
    MTWSelection(uhh2::Context & ctx, double mtw_min_ = 0, std::string primlep_name_ = "PrimaryLepton");
    virtual bool passes(const uhh2::Event & event) override;
  private:
    uhh2::Event::Handle<FlavorParticle> h_primlepton;
    double mtw_min;
  };

  class DeltaRCut: public uhh2::Selection {
  public:
    DeltaRCut(uhh2::Context & ctx, double deltaR_min_ = 0, std::string primlep_name_ = "PrimaryLepton");
    virtual bool passes(const uhh2::Event & event) override;
  private:
    uhh2::Event::Handle<FlavorParticle> h_primlepton;
    double deltaR_min;
  };

  class tWgenSignalSelection: public uhh2::Selection {
  public:
    tWgenSignalSelection(uhh2::Context & ctx, bool is_muon_);
    virtual bool passes(const uhh2::Event & event) override;
  private:
    uhh2::Event::Handle<SingleTopGen_tWch> h_GENtW;
    bool is_muon;
  };
}
