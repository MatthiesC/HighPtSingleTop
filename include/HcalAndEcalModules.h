#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Selection.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/Utils.h"


class PrefiringWeights: public uhh2::AnalysisModule {
 public:
  explicit PrefiringWeights(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

 private:
  Year year;
  int syst_direction;
};

// class HEMIssueSelection: public uhh2::Selection {
//  public:
//   HEMIssueSelection(uhh2::Context & ctx, const std::string & h_name_ak8jets="Ak8Jets");
//   virtual bool passes(const uhh2::Event & event) override;
//   double MCWeight() const { return (1. - m_affected_lumi_fraction); }
//
//  private:
//   Year year;
//   const int m_runnumber = 319077;
//   const std::pair<float, float> m_eta = {-3.2,-1.3};
//   const std::pair<float, float> m_phi = {-1.57,-0.87};
//   const double m_affected_lumi_fraction = 0.64844705699; // (Run 319077(17.370008 pb-1) + Run C + Run D) / all 2018  // TODO: Check this value. Trusting Alex right now
//   uhh2::Event::Handle<std::vector<TopJet>> h_ak8jets;
// };

class HEMIssueSelection: public uhh2::Selection {
 public:
  HEMIssueSelection(uhh2::Context & ctx, const std::string & h_name_wtaggedjets="WJets", const std::string & h_name_toptaggedjets="TopTaggedJets");
  virtual bool passes(const uhh2::Event & event) override;
  double MCWeight() const { return (1. - m_affected_lumi_fraction); }

 private:
  Year year;
  const int m_runnumber = 319077;
  const std::pair<float, float> m_eta = {-3.2,-1.3};
  const std::pair<float, float> m_phi = {-1.57,-0.87};
  const double m_affected_lumi_fraction = 0.64844705699; // (Run 319077(17.370008 pb-1) + Run C + Run D) / all 2018  // TODO: Check this value. Trusting Alex right now
  uhh2::Event::Handle<std::vector<TopJet>> h_wtaggedjets, h_toptaggedjets;
};
