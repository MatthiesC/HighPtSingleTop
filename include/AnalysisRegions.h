#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Utils.h"

#include "UHH2/HighPtSingleTop/include/Constants.h"
#include "UHH2/HighPtSingleTop/include/Utils.h"


namespace uhh2 { namespace btw {

//____________________________________________________________________________________________________
// bool

//____________________________________________________________________________________________________
class AnalysisRegionSetter: public uhh2::AnalysisModule {
public:
  AnalysisRegionSetter(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

private:
  const uhh2::Event::Handle<std::vector<Jet>> fHandle_bJets;
  // const uhh2::Event::Handle<std::vector<Jet>> fHandle_bJets_loose;
  // const uhh2::Event::Handle<std::vector<Jet>> fHandle_bJets_medium;
  const uhh2::Event::Handle<std::vector<Jet>> fHandle_bJets_tight;
  const uhh2::Event::Handle<std::vector<TopJet>> fHandle_tJets;
  const uhh2::Event::Handle<std::vector<TopJet>> fHandle_WJets;

  const uhh2::Event::Handle<ERegion_bTags> fHandle_Region_bTags;
  const uhh2::Event::Handle<ERegion_heavyTags> fHandle_Region_heavyTags;
  const uhh2::Event::Handle<ERegion> fHandle_Region;

  const uhh2::Event::Handle<int> fHandle_Region_bTags_int;
  const uhh2::Event::Handle<int> fHandle_Region_heavyTags_int;
  const uhh2::Event::Handle<int> fHandle_Region_int;
};

//____________________________________________________________________________________________________
class AnalysisRegionHists: public uhh2::Hists {
public:
  AnalysisRegionHists(uhh2::Context & ctx, const std::string & dirname);
  virtual void fill(const uhh2::Event & event) override;

private:
  uhh2::Event::Handle<ERegion> fHandle_Region;
  Year fYear;
  int fYearInt;
  Channel fChannel;
  int fChannelInt;

protected:
  TH1F *hist_region;
  TH1F *hist_region_rel;
  TH1F *hist_region_year_channel;
  TH1F *hist_region_year_channel_rel;
  TH1F *hist_region_year;
  TH1F *hist_region_year_rel;
  TH1F *hist_region_channel;
  TH1F *hist_region_channel_rel;
};

}}
