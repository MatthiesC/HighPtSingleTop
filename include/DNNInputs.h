#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/LegacyTopTagging/include/Utils.h"
#include "UHH2/LegacyTopTagging/include/Constants.h"

#include "UHH2/HighPtSingleTop/include/Utils.h"
#include "UHH2/HighPtSingleTop/include/Constants.h"
// #include "UHH2/HighPtSingleTop/include/DNNHists.h"

namespace uhh2 { namespace btw {

//____________________________________________________________________________________________________
typedef struct {
  uint index;
  std::string name;
  std::string plot_label;
  float x_low;
  float x_high;
  uint n_bins = 100;
} DNNInput;

//____________________________________________________________________________________________________
// Prepares float handles which can be used as DNN inputs. The set of inputs that is actually used by
// the DNN is defined by the lwtnn application modules.
class DNNInputs: public uhh2::AnalysisModule {
  // friend class btw::DNNHists;

public:
  explicit DNNInputs(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;
  const std::map<std::string, DNNInput> & get_inputs_map() const { return fMap_Inputs; };

private:
  void set_inputs(uhh2::Event & event, const uhh2::btw::ERegion_heavyTags & region_heavyTags);
  void set_dummy_inputs(uhh2::Event & event);

  const Year fYear;
  const uhh2::ltt::Channel fChannel;
  const uhh2::Event::Handle<uhh2::btw::ERegion_heavyTags> fHandle_Region_heavyTags;
  const uhh2::Event::Handle<FlavorParticle> fHandle_PrimaryLepton;
  const uhh2::Event::Handle<uhh2::btw::LeptonicHemisphere> fHandle_LeptonicHemisphere;
  const uhh2::Event::Handle<std::vector<Jet>> fHandle_pairedPUPPIjets;
  const uhh2::Event::Handle<std::vector<Jet>> fHandle_CHSjets;
  const uhh2::Event::Handle<TopJet> fHandle_TheTopJet;
  const uhh2::Event::Handle<TopJet> fHandle_TheWJet;

  const float fZeroPaddingValue = 0.f;
  const uint fNJetsLimit = 6; // maximum number of AK4 jets that may be considered for inputs
  std::map<std::string, DNNInput> fMap_Inputs;
  std::vector<uhh2::Event::Handle<float>> fInputHandles;
};

}}
