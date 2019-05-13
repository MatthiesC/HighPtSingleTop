#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/TopJetIds.h"

#include "TMVA/Reader.h"


class TMVASetup: public uhh2::AnalysisModule {
 public:
  explicit TMVASetup(uhh2::Context & ctx_, std::vector<std::string> & tmva_names_, TopJetId & topjetid_);
  virtual bool process(uhh2::Event & event) override;

 private:
  std::vector<uhh2::Event::Handle<float>> h_tmva_variables;
  std::vector<std::string> tmva_names;
  TopJetId topjetID;
};


class TMVAApplication: public uhh2::AnalysisModule {
 public:
  explicit TMVAApplication(uhh2::Context & ctx, const std::string & tmva_weightfile, const std::string & handle_name = "BDTResponse");
  virtual bool process(uhh2::Event & event) override;
  virtual ~TMVAApplication();

 private:
  float floatvars[25];
  TMVA::Reader *reader;
  uhh2::Event::Handle<float> h_BDTResponse;
};
