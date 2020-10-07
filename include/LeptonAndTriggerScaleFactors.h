#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/YearRunSwitchers.h"

#include <TRandom.h>


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

class MuonTriggerScaleFactors2016: public uhh2::AnalysisModule {
 public:
  explicit MuonTriggerScaleFactors2016(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

 private:
  std::unique_ptr<AnalysisModule> m_sf_trigger;
};

class MuonTriggerScaleFactors2017: public uhh2::AnalysisModule {
 public:
  explicit MuonTriggerScaleFactors2017(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

 private:
  std::unique_ptr<AnalysisModule> m_sf_trigger;
};

class MuonTriggerScaleFactors2018: public uhh2::AnalysisModule {
 public:
  explicit MuonTriggerScaleFactors2018(uhh2::Context & ctx, long int seed = 123456789);
  virtual bool process(uhh2::Event & event) override;

 private:
  TRandom *m_rng;
  long int m_seed;
  const double m_lumi_fraction = 8.95 / 59.74;
  std::unique_ptr<AnalysisModule> m_sf_trigger_beforehand, m_sf_trigger_afterwards;
};

class ElectronTriggerScaleFactors2016: public uhh2::AnalysisModule {
 public:
  explicit ElectronTriggerScaleFactors2016(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

 private:
  std::unique_ptr<AnalysisModule> m_sf_trigger;
};

class ElectronTriggerScaleFactors2017: public uhh2::AnalysisModule {
 public:
  explicit ElectronTriggerScaleFactors2017(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

 private:
  std::unique_ptr<AnalysisModule> m_sf_trigger;
};

class ElectronTriggerScaleFactors2018: public uhh2::AnalysisModule {
 public:
  explicit ElectronTriggerScaleFactors2018(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

 private:
  std::unique_ptr<AnalysisModule> m_sf_trigger;
};

class TriggerScaleFactors: public uhh2::AnalysisModule {
 public:
  explicit TriggerScaleFactors(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event);

 private:
  std::unique_ptr<YearSwitcher> m_sf_trigger;
};
