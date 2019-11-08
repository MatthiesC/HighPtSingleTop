#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/TopJetIds.h"
#include "UHH2/common/include/JetIds.h"


class TopTaggedJet: public uhh2::AnalysisModule {
 public:
  explicit TopTaggedJet(uhh2::Context & ctx,
			TopJetId topjetid,
			const std::string & h_name="TopTaggedJet");

  virtual bool process(uhh2::Event & event) override;

  virtual ~TopTaggedJet();

 private:
  uhh2::Event::Handle<TopJet> h_toptaggedjet;
  TopJetId m_topjetid;
};


class BTaggedJets: public uhh2::AnalysisModule {
 public:
  explicit BTaggedJets(uhh2::Context & ctx,
		       BTag::algo btagalgo,
		       BTag::wp workingpoint,
		       const std::string & h_name_analysis="BJets",
		       const std::string & h_name_loose="BJetsLoose",
		       const std::string & h_name_medium="BJetsMedium",
		       const std::string & h_name_tight="BJetsTight");
  
  virtual bool process(uhh2::Event & event) override;

  virtual ~BTaggedJets();

 private:
  uhh2::Event::Handle<std::vector<Jet>> h_btaggedjets_loose;
  uhh2::Event::Handle<std::vector<Jet>> h_btaggedjets_medium;
  uhh2::Event::Handle<std::vector<Jet>> h_btaggedjets_tight;
  uhh2::Event::Handle<std::vector<Jet>> h_btaggedjets_analysis;

  BTag::algo m_btagalgo;
  BTag::wp m_workingpoint;
};
