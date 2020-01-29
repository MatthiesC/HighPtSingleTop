#pragma once

#include "UHH2/core/include/fwd.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Selection.h"
#include "UHH2/core/include/FlavorParticle.h"
#include "UHH2/core/include/AnalysisModule.h" // without that, can't use uhh2::Context

#include "UHH2/HighPtSingleTop/include/SingleTopGen_tWch.h"

#include "UHH2/common/include/Utils.h"
#include "UHH2/common/include/TopJetIds.h"


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

  class DeltaRLeptonNextJetCut: public uhh2::Selection {
  public:
    DeltaRLeptonNextJetCut(uhh2::Context & ctx, double deltaR_min_ = 0, std::string primlep_name_ = "PrimaryLepton");
    virtual bool passes(const uhh2::Event & event) override;
  private:
    uhh2::Event::Handle<FlavorParticle> h_primlepton;
    double deltaR_min;
  };

  class DeltaPhiTopLeptonCut: public uhh2::Selection {
  public:
    DeltaPhiTopLeptonCut(uhh2::Context & ctx, double & deltaPhi_min_, TopJetId & topJetId_, std::string primlep_name_ = "PrimaryLepton");
    virtual bool passes(const uhh2::Event & event) override;
  private:
    uhh2::Event::Handle<FlavorParticle> h_primlepton;
    double deltaPhi_min;
    TopJetId topJetId;
  };

  class tWgenSignalSelection: public uhh2::Selection {
  public:
    tWgenSignalSelection(uhh2::Context & ctx, bool is_muon_);
    virtual bool passes(const uhh2::Event & event) override;
  private:
    uhh2::Event::Handle<SingleTopGen_tWch> h_GENtW;
    bool is_muon;
  };

  class tWgenSelection: public uhh2::Selection {
  public:
    tWgenSelection(uhh2::Context & ctx, std::string decay_, bool is_muon_);
    virtual bool passes(const uhh2::Event & event) override;
  private:
    uhh2::Event::Handle<SingleTopGen_tWch> h_GENtW;
    std::string m_decay;
    bool m_is_muon;
  };

  class WJetsGenSelection: public uhh2::Selection {
  public:
    WJetsGenSelection(uhh2::Context & ctx, std::string jets_type);
    virtual bool passes(const uhh2::Event & event) override;
  private:
    std::string m_jets_type;
    bool m_is_wjets;
  };

  class JetLeptonOverlapRemoval: public uhh2::AnalysisModule {
  public:
    explicit JetLeptonOverlapRemoval(uhh2::Context & ctx, double & deltaR_min_, std::string primlep_name_ = "PrimaryLepton");
    virtual bool process(uhh2::Event & event) override;
  private:
    uhh2::Event::Handle<FlavorParticle> h_primlepton;
    double deltaR_min;
  };

  class MergeScenarioSelection: public uhh2::Selection {
  public:
    MergeScenarioSelection(uhh2::Context & ctx, unsigned int number_of_merged_quarks, std::string toptaggedjet_name_ = "TopTaggedJet");
    virtual bool passes(const uhh2::Event & event) override;
  private:
    uhh2::Event::Handle<TopJet> h_toptaggedjet;
    uhh2::Event::Handle<SingleTopGen_tWch> h_GENtW;
    unsigned int m_mergedQuarks;
  };

  
  // copied from Alex
  class HighPtSingleTopTriggerSelection : public uhh2::Selection {
  public:
    HighPtSingleTopTriggerSelection(uhh2::Context &ctx);
    virtual bool passes(const uhh2::Event &event) override;
  private:
    Year year;
    bool is_muo, is_ele;
    std::unique_ptr<uhh2::Selection> trig_isomu24, trig_isotkmu24, trig_isomu27;
    std::unique_ptr<uhh2::Selection> trig_ele27, trig_ele32, trig_ele35;
    std::unique_ptr<uhh2::Selection> trig_photon175, trig_photon200;
  };
}
