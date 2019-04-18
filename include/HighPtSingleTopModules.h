#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/ObjectIdUtils.h"
#include "UHH2/common/include/MuonIds.h"
#include "UHH2/common/include/TopJetIds.h"
#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/ElectronIds.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/TriggerSelection.h"
#include "UHH2/common/include/Utils.h" 

#include "UHH2/HOTVR/include/HOTVRJetCorrector.h"


class ObjectSetup: public uhh2::AnalysisModule {
 public:
  explicit ObjectSetup(uhh2::Context & context);
  virtual bool process(uhh2::Event & event) override;

 private:
  bool is_mc;

  // 2016 run numbers
  const int runnr_BCD = 276811;
  const int runnr_EFearly = 278802;
  const int runnr_FlateG = 280385;

  // Kinematic variables
  double muonPt_min_veto;
  double muonEta_max_veto;
  double elecPt_min_veto;
  double elecEta_max_veto;
  double jetPt_min;
  double jetEta_max;
  double hotvrPt_min;
  double hotvrEta_max;

  // Identifications
  PrimaryVertexId pvID;
  MuonId muonID_preJLC, muonID_veto;
  ElectronId elecID_preJLC, elecID_veto;
  JetId jetPFID, jetID;
  TopJetId hotvrID;

  std::unique_ptr<JetCorrector> jec_ak4_mc, jec_ak4_BCD, jec_ak4_EFearly, jec_ak4_FlateG, jec_ak4_H;
  std::unique_ptr<JetLeptonCleaner> jlc_mc, jlc_BCD, jlc_EFearly, jlc_FlateG, jlc_H;
  std::unique_ptr<HOTVRJetCorrector> jec_hotvr_mc, jec_hotvr_BCD, jec_hotvr_EFearly, jec_hotvr_FlateG, jec_hotvr_H;
  std::unique_ptr<AnalysisModule> clnr_pv, clnr_muon_preJLC, clnr_muon_veto, clnr_elec_preJLC, clnr_elec_veto, clnr_PFjet, clnr_jet, clnr_hotvr, jlc_hotvr;
  std::unique_ptr<JetResolutionSmearer> jet_resolution_smearer;
  std::unique_ptr<AndSelection> metfilters;
};
