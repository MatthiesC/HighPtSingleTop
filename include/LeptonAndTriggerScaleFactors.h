#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/YearRunSwitchers.h"

#include <TGraphAsymmErrors.h>


class MuonIdIsoScaleFactors2016: public uhh2::AnalysisModule {
 public:
  explicit MuonIdIsoScaleFactors2016(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

 private:
  std::unique_ptr<AnalysisModule> m_sf_id, m_sf_iso;
};

class MuonIdIsoScaleFactors2017: public uhh2::AnalysisModule {
 public:
  explicit MuonIdIsoScaleFactors2017(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

 private:
  std::unique_ptr<AnalysisModule> m_sf_id, m_sf_iso;
};

class MuonIdIsoScaleFactors2018: public uhh2::AnalysisModule {
 public:
  explicit MuonIdIsoScaleFactors2018(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

 private:
  std::unique_ptr<AnalysisModule> m_sf_id, m_sf_iso;
};

class ElectronIdRecoScaleFactors2016: public uhh2::AnalysisModule {
 public:
  explicit ElectronIdRecoScaleFactors2016(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

 private:
  std::unique_ptr<AnalysisModule> m_sf_id, m_sf_reco;
};

class ElectronIdRecoScaleFactors2017: public uhh2::AnalysisModule {
 public:
  explicit ElectronIdRecoScaleFactors2017(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

 private:
  std::unique_ptr<AnalysisModule> m_sf_id, m_sf_reco;
};

class ElectronIdRecoScaleFactors2018: public uhh2::AnalysisModule {
 public:
  explicit ElectronIdRecoScaleFactors2018(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

 private:
  std::unique_ptr<AnalysisModule> m_sf_id, m_sf_reco;
};

class LeptonScaleFactors: public uhh2::AnalysisModule {
 public:
  explicit LeptonScaleFactors(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event);

 private:
  std::unique_ptr<YearSwitcher> m_sf_lepton;
};

class ElectronTriggerWeights: public uhh2::AnalysisModule {
 public:
  explicit ElectronTriggerWeights(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

 private:
  int syst_direction;
  Year year;
  std::vector<float> pt_bins;
  std::unique_ptr<TGraphAsymmErrors> g_sf_pt1, g_sf_pt2;
  uhh2::Event::Handle<float> h_ele_weight, h_ele_weight_up, h_ele_weight_down;
};

class MuonTriggerWeights: public uhh2::AnalysisModule {
 public:
  explicit MuonTriggerWeights(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

 private:
  int syst_direction;
  Year year;
  std::unique_ptr<TGraphAsymmErrors> g_sf_0to0p9, g_sf_0p9to1p2, g_sf_1p2to2p1, g_sf_2p1to2p4;
  uhh2::Event::Handle<float> h_muo_weight, h_muo_weight_up, h_muo_weight_down;
};

class TriggerScaleFactors: public uhh2::AnalysisModule {
 public:
  explicit TriggerScaleFactors(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event);

 private:
  std::unique_ptr<uhh2::AnalysisModule> m_sf_trigger;
};
