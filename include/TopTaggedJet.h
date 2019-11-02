#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/TopJetIds.h"


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
