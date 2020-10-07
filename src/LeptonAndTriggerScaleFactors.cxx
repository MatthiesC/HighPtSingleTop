#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/Utils.h"

#include "UHH2/HighPtSingleTop/include/LeptonAndTriggerScaleFactors.h"

using namespace std;
using namespace uhh2;


MuonIdIsoScaleFactors2016::MuonIdIsoScaleFactors2016(Context & ctx) {

  string filepath_sf_id = ctx.get("uhh2Dir")+"common/data/2016/MuonID_EfficienciesAndSF_average_RunBtoH.root";
  m_sf_id.reset(new MCMuonScaleFactor(ctx, filepath_sf_id, "MC_NUM_TightID_DEN_genTracks_PAR_pt_eta", 1.0, "tight_id", false, ctx.get("SystDirection_MuonId")));

  string filepath_sf_iso = ctx.get("uhh2Dir")+"common/data/2016/MuonIso_EfficienciesAndSF_average_RunBtoH.root";
  m_sf_iso.reset(new MCMuonScaleFactor(ctx, filepath_sf_iso, "TightISO_TightID_pt_eta", 1.0, "isolation", false, ctx.get("SystDirection_MuonIso")));
}

bool MuonIdIsoScaleFactors2016::process(Event & event) {

  m_sf_id->process(event);
  m_sf_iso->process(event);

  return true;
}


MuonIdIsoScaleFactors2017::MuonIdIsoScaleFactors2017(Context & ctx) {

  // string filepath_sf_id = ctx.get("uhh2Dir")+"common/data/2017/MuonID_94X_RunBCDEF_SF_ID.root";
  // m_sf_id.reset(new MCMuonScaleFactor(ctx, filepath_sf_id, "NUM_TightID_DEN_genTracks_pt_abseta", 1.0, "tight_id", true, ctx.get("SystDirection_MuonId")));
  //
  // string filepath_sf_iso = ctx.get("uhh2Dir")+"common/data/2017/MuonIso_94X_RunBCDEF_SF_ISO.root";
  // m_sf_iso.reset(new MCMuonScaleFactor(ctx, filepath_sf_iso, "NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta", 1.0, "isolation", true, ctx.get("SystDirection_MuonIso")));
}

bool MuonIdIsoScaleFactors2017::process(Event & event) {

  // m_sf_id->process(event);
  // m_sf_iso->process(event);

  return true;
}


MuonIdIsoScaleFactors2018::MuonIdIsoScaleFactors2018(Context & ctx) {

//   string filepath_sf_id = ctx.get("uhh2Dir")+"common/data/2018/Muon_ID_SF_RunABCD.root";
//   m_sf_id.reset(new MCMuonScaleFactor(ctx, filepath_sf_id, "NUM_TightID_DEN_TrackerMuons_pt_abseta", 1.0, "tight_id", true, ctx.get("SystDirection_MuonId")));
//
//   string filepath_sf_iso = ctx.get("uhh2Dir")+"common/data/2018/Muon_Iso_SF_RunABCD.root";
//   m_sf_iso.reset(new MCMuonScaleFactor(ctx, filepath_sf_iso, "NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta", 1.0, "isolation", true, ctx.get("SystDirection_MuonIso")));
}

bool MuonIdIsoScaleFactors2018::process(Event & event) {

  // m_sf_id->process(event);
  // m_sf_iso->process(event);

  return true;
}


ElectronIdRecoScaleFactors2016::ElectronIdRecoScaleFactors2016(Context & ctx) {

  string filepath_sf_id = ctx.get("uhh2Dir")+"HighPtSingleTop/data/ScaleFactors/2016/2016LegacyReReco_ElectronTight_Fall17V2.root";
  m_sf_id.reset(new MCElecScaleFactor(ctx, filepath_sf_id, 1.0, "tight_id", ctx.get("SystDirection_ElectronId")));

  string filepath_sf_reco = ctx.get("uhh2Dir")+"HighPtSingleTop/data/ScaleFactors/2016/EGM2D_BtoH_GT20GeV_RecoSF_Legacy2016.root";
  m_sf_reco.reset(new MCElecScaleFactor(ctx, filepath_sf_reco, 1.0, "reco", ctx.get("SystDirection_ElectronReco")));
}

bool ElectronIdRecoScaleFactors2016::process(Event & event) {

  m_sf_id->process(event);
  m_sf_reco->process(event);

  return true;
}


ElectronIdRecoScaleFactors2017::ElectronIdRecoScaleFactors2017(Context & ctx) {

  string filepath_sf_id = ctx.get("uhh2Dir")+"HighPtSingleTop/data/ScaleFactors/2017/2017_ElectronTight.root";
  m_sf_id.reset(new MCElecScaleFactor(ctx, filepath_sf_id, 1.0, "tight_id", ctx.get("SystDirection_ElectronId")));

  string filepath_sf_reco = ctx.get("uhh2Dir")+"HighPtSingleTop/data/ScaleFactors/2017/egammaEffi.txt_EGM2D_runBCDEF_passingRECO.root";
  m_sf_reco.reset(new MCElecScaleFactor(ctx, filepath_sf_reco, 1.0, "reco", ctx.get("SystDirection_ElectronReco")));
}

bool ElectronIdRecoScaleFactors2017::process(Event & event) {

  m_sf_id->process(event);
  m_sf_reco->process(event);

  return true;
}


ElectronIdRecoScaleFactors2018::ElectronIdRecoScaleFactors2018(Context & ctx) {

  string filepath_sf_id = ctx.get("uhh2Dir")+"HighPtSingleTop/data/ScaleFactors/2018/2018_ElectronTight.root";
  m_sf_id.reset(new MCElecScaleFactor(ctx, filepath_sf_id, 1.0, "tight_id", ctx.get("SystDirection_ElectronId")));

  string filepath_sf_reco = ctx.get("uhh2Dir")+"HighPtSingleTop/data/ScaleFactors/2018/egammaEffi.txt_EGM2D_updatedAll.root";
  m_sf_reco.reset(new MCElecScaleFactor(ctx, filepath_sf_reco, 1.0, "reco", ctx.get("SystDirection_ElectronReco")));
}

bool ElectronIdRecoScaleFactors2018::process(Event & event) {

  m_sf_id->process(event);
  m_sf_reco->process(event);

  return true;
}


LeptonScaleFactors::LeptonScaleFactors(Context & ctx) {

  m_sf_lepton.reset(new YearSwitcher(ctx));

  if(ctx.get("analysis_channel") == "ele") {
    m_sf_lepton->setup2016(std::make_shared<ElectronIdRecoScaleFactors2016>(ctx));
    m_sf_lepton->setup2017(std::make_shared<ElectronIdRecoScaleFactors2017>(ctx));
    m_sf_lepton->setup2018(std::make_shared<ElectronIdRecoScaleFactors2018>(ctx));
  }
  else if(ctx.get("analysis_channel") == "muo") {
    m_sf_lepton->setup2016(std::make_shared<MuonIdIsoScaleFactors2016>(ctx));
    m_sf_lepton->setup2017(std::make_shared<MuonIdIsoScaleFactors2017>(ctx));
    m_sf_lepton->setup2018(std::make_shared<MuonIdIsoScaleFactors2018>(ctx));
  }
  else {
    throw invalid_argument("LeptonScaleFactors: no analysis channel specified in xml file (ele or muo)");
  }
}

bool LeptonScaleFactors::process(Event & event) {

  return m_sf_lepton->process(event);
}


MuonTriggerScaleFactors2016::MuonTriggerScaleFactors2016(Context & ctx) {

  string filepath_sf_trigger = ctx.get("uhh2Dir")+"common/data/2016/MuonTrigger_EfficienciesAndSF_average_RunBtoH.root";
  m_sf_trigger.reset(new MCMuonScaleFactor(ctx, filepath_sf_trigger, "IsoMu24_OR_IsoTkMu24_PtEtaBins", 0.5, "trigger", false, ctx.get("SystDirection_MuonTrigger")));
}

bool MuonTriggerScaleFactors2016::process(Event & event) {

  // m_sf_trigger->process(event);

  return true;
}


MuonTriggerScaleFactors2017::MuonTriggerScaleFactors2017(Context & ctx) {

  // string filepath_sf_trigger = ctx.get("uhh2Dir")+"common/data/2017/";
  // m_sf_trigger.reset(new MCMuonScaleFactor(ctx, filepath_sf_trigger, "IsoMu27_PtEtaBins", 0.5, "trigger", false, ctx.get("SystDirection_MuonTrigger")));
}

bool MuonTriggerScaleFactors2017::process(Event & event) {

  // m_sf_trigger->process(event);

  return true;
}


MuonTriggerScaleFactors2018::MuonTriggerScaleFactors2018(Context & ctx, long int seed): m_seed(seed) {

  // TODO: Check if this procedure for the muon HLT update is okay.
  // string filepath_sf_trigger_beforehand = ctx.get("uhh2Dir")+"common/data/2018/Muon_Trigger_Eff_SF_BeforeMuonHLTUpdate.root";
  // string filepath_sf_trigger_afterwards = ctx.get("uhh2Dir")+"common/data/2018/Muon_Trigger_Eff_SF_AfterMuonHLTUpdate.root";
  // m_sf_trigger_beforehand.reset(new MCMuonScaleFactor(ctx, filepath_sf_trigger_beforehand, "IsoMu24_PtEtaBins", 0.5, "trigger", false, ctx.get("SystDirection_MuonTrigger")));
  // m_sf_trigger_afterwards.reset(new MCMuonScaleFactor(ctx, filepath_sf_trigger_afterwards, "IsoMu24_PtEtaBins", 0.5, "trigger", false, ctx.get("SystDirection_MuonTrigger")));
}

bool MuonTriggerScaleFactors2018::process(Event & event) {

  // if(m_rng->Uniform() < m_lumi_fraction) {
  //   m_sf_trigger_beforehand->process(event);
  // }
  // else {
  //   m_sf_trigger_afterwards->process(event);
  // }

  return true;
}


ElectronTriggerScaleFactors2016::ElectronTriggerScaleFactors2016(Context & ctx) {

  // TODO
}

bool ElectronTriggerScaleFactors2016::process(Event & event) {

  // TODO

  return true;
}


ElectronTriggerScaleFactors2017::ElectronTriggerScaleFactors2017(Context & ctx) {

  // TODO
}

bool ElectronTriggerScaleFactors2017::process(Event & event) {

  // TODO

  return true;
}


ElectronTriggerScaleFactors2018::ElectronTriggerScaleFactors2018(Context & ctx) {

  // TODO
}

bool ElectronTriggerScaleFactors2018::process(Event & event) {

  // TODO

  return true;
}


TriggerScaleFactors::TriggerScaleFactors(Context & ctx) {

  m_sf_trigger.reset(new YearSwitcher(ctx));

  if(ctx.get("analysis_channel") == "ele") {
    m_sf_trigger->setup2016(std::make_shared<ElectronTriggerScaleFactors2016>(ctx));
    m_sf_trigger->setup2017(std::make_shared<ElectronTriggerScaleFactors2017>(ctx));
    m_sf_trigger->setup2018(std::make_shared<ElectronTriggerScaleFactors2018>(ctx));
  }
  else if(ctx.get("analysis_channel") == "muo") {
    m_sf_trigger->setup2016(std::make_shared<MuonTriggerScaleFactors2016>(ctx));
    m_sf_trigger->setup2017(std::make_shared<MuonTriggerScaleFactors2017>(ctx));
    m_sf_trigger->setup2018(std::make_shared<MuonTriggerScaleFactors2018>(ctx));
  }
  else {
    throw invalid_argument("TriggerScaleFactors: no analysis channel specified in xml file (ele or muo)");
  }
}

bool TriggerScaleFactors::process(Event & event) {

  return m_sf_trigger->process(event);
}
