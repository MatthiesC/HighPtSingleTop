#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/HighPtSingleTop/include/Utils.h"
#include "UHH2/HighPtSingleTop/include/Constants.h"
// #include "UHH2/HighPtSingleTop/include/DNNHists.h"

#include "lwtnn/LightweightNeuralNetwork.hh"

namespace uhh2 { namespace btw {

//____________________________________________________________________________________________________
class DNNApplication: public uhh2::AnalysisModule {
  // friend class btw::DNNHists;

public:
  explicit DNNApplication(uhh2::Context & ctx, const ERegion_heavyTags & region_heavyTags);
  virtual bool process(uhh2::Event & event) override;
  const bool & apply() const { return bApplyDNN; };
  const std::set<std::string> & get_input_names() const { return fInputNames; };
  const std::set<std::string> & get_output_names() const { return fOutputHandleNames; };

private:
  void set_outputs(uhh2::Event & event);
  void set_dummy_outputs(uhh2::Event & event);

  const uhh2::Event::Handle<uhh2::btw::ERegion_heavyTags> fHandle_Region_heavyTags;
  const ERegion_heavyTags fRegion_heavyTags;

  bool bApplyDNN;

  std::unique_ptr<lwt::LightweightNeuralNetwork> fNeuralNetwork;
  std::set<std::string> fInputNames;
  std::map<std::string, uhh2::Event::Handle<float>> fInputHandles;
  std::set<std::string> fOutputNames; // as coming out of lwtnn json config
  std::set<std::string> fOutputHandleNames; // with dnnOutput ... prefix
  std::map<std::string, uhh2::Event::Handle<float>> fOutputHandles;

  const float fDummyOutputValue = -1.f;
};

}}
