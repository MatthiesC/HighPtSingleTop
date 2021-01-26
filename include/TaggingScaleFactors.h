#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/Utils.h"

#include "UHH2/HOTVR/include/HadronicTop.h"

#include "UHH2/HighPtSingleTop/include/TaggedJets.h"

#include <TGraphAsymmErrors.h>


class DeepAK8ScaleFactor: public uhh2::AnalysisModule {
 public:
  explicit DeepAK8ScaleFactor(uhh2::Context & ctx, const std::string & object_name, const bool & mass_decorrelated, const WTaggedJets::wp & working_point);
  virtual bool process(uhh2::Event & event) override;
  bool process_dummy(uhh2::Event & event);

 private:
  int syst_direction;
  Year year;
  double pt_minimum, pt_maximum;
  std::unique_ptr<TGraphAsymmErrors> sf_graph;
  uhh2::Event::Handle<float> h_deepak8_weight, h_deepak8_weight_up, h_deepak8_weight_down;
  uhh2::Event::Handle<TopJet> h_taggedjet;
};

class MyHOTVRScaleFactor: public uhh2::AnalysisModule {
 public:
  explicit MyHOTVRScaleFactor(uhh2::Context & ctx, const TopJetId & id_topjet, const std::string & gen_handle_name = "HadronicTop", const std::string & xmlpathname = "HOTVRTopTagSFs");
  virtual bool process(uhh2::Event & event) override;
  bool process_dummy(uhh2::Event & event);

 private:
  int syst_direction;
  void get_sf(double pt, int category);

  const TopJetId m_id_topjet;
  // uhh2::Event::Handle<std::vector<TopJet>> h_topjets;
  uhh2::Event::Handle<std::vector<GenTop>> h_tophad;

  double m_weight;
  double m_weight_up;
  double m_weight_down;
  double m_weight_merged_up;
  double m_weight_merged_down;
  double m_weight_semimerged_up;
  double m_weight_semimerged_down;
  double m_weight_notmerged_up;
  double m_weight_notmerged_down;

  TH1F *sf_merged, *sf_merged_up, *sf_merged_down;
  TH1F *sf_semi, *sf_semi_up, *sf_semi_down;
  TH1F *sf_not, *sf_not_up, *sf_not_down;

  uhh2::Event::Handle<float> h_weight_sfhotvr;
  uhh2::Event::Handle<float> h_weight_sfhotvr_up;
  uhh2::Event::Handle<float> h_weight_sfhotvr_down;
  uhh2::Event::Handle<float> h_weight_sfhotvr_merged_up;
  uhh2::Event::Handle<float> h_weight_sfhotvr_merged_down;
  uhh2::Event::Handle<float> h_weight_sfhotvr_semimerged_up;
  uhh2::Event::Handle<float> h_weight_sfhotvr_semimerged_down;
  uhh2::Event::Handle<float> h_weight_sfhotvr_notmerged_up;
  uhh2::Event::Handle<float> h_weight_sfhotvr_notmerged_down;
};
