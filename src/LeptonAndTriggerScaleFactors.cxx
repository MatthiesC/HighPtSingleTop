#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/Utils.h"

#include "UHH2/HighPtSingleTop/include/LeptonAndTriggerScaleFactors.h"

#include <TFile.h>

using namespace std;
using namespace uhh2;



/*
  Id, iso, and reco scale factors for muons and electrons
*/


MuonIdIsoScaleFactors2016::MuonIdIsoScaleFactors2016(Context & ctx) {

  string filepath_sf_id = ctx.get("uhh2Dir")+"HighPtSingleTop/data/ScaleFactors/2016/MuonID_EfficienciesAndSF_average_RunBtoH.root";
  m_sf_id.reset(new MCMuonScaleFactor(ctx, filepath_sf_id, "MC_NUM_TightID_DEN_genTracks_PAR_pt_eta/abseta_pt_ratio", 0.0, "tight_id", false, ctx.get("SystDirection_MuonId", "nominal")));

  string filepath_sf_iso = ctx.get("uhh2Dir")+"HighPtSingleTop/data/ScaleFactors/2016/MuonIso_EfficienciesAndSF_average_RunBtoH.root";
  m_sf_iso.reset(new MCMuonScaleFactor(ctx, filepath_sf_iso, "TightISO_TightID_pt_eta/abseta_pt_ratio", 0.0, "isolation", false, ctx.get("SystDirection_MuonIso", "nominal")));
}

bool MuonIdIsoScaleFactors2016::process(Event & event) {

  m_sf_id->process(event);
  m_sf_iso->process(event);

  return true;
}


MuonIdIsoScaleFactors2017::MuonIdIsoScaleFactors2017(Context & ctx) {

  string filepath_sf_id = ctx.get("uhh2Dir")+"HighPtSingleTop/data/ScaleFactors/2017/MuonID_94X_RunBCDEF_SF_ID.root";
  m_sf_id.reset(new MCMuonScaleFactor(ctx, filepath_sf_id, "NUM_TightID_DEN_genTracks_pt_abseta", 0.0, "tight_id", true, ctx.get("SystDirection_MuonId", "nominal")));

  string filepath_sf_iso = ctx.get("uhh2Dir")+"HighPtSingleTop/data/ScaleFactors/2017/MuonIso_94X_RunBCDEF_SF_ISO.root";
  m_sf_iso.reset(new MCMuonScaleFactor(ctx, filepath_sf_iso, "NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta", 0.0, "isolation", true, ctx.get("SystDirection_MuonIso", "nominal")));
}

bool MuonIdIsoScaleFactors2017::process(Event & event) {

  m_sf_id->process(event);
  m_sf_iso->process(event);

  return true;
}


MuonIdIsoScaleFactors2018::MuonIdIsoScaleFactors2018(Context & ctx) {

  string filepath_sf_id = ctx.get("uhh2Dir")+"HighPtSingleTop/data/ScaleFactors/2018/Muon_ID_SF_RunABCD.root";
  m_sf_id.reset(new MCMuonScaleFactor(ctx, filepath_sf_id, "NUM_TightID_DEN_TrackerMuons_pt_abseta", 0.0, "tight_id", true, ctx.get("SystDirection_MuonId", "nominal")));

  string filepath_sf_iso = ctx.get("uhh2Dir")+"HighPtSingleTop/data/ScaleFactors/2018/Muon_Iso_SF_RunABCD.root";
  m_sf_iso.reset(new MCMuonScaleFactor(ctx, filepath_sf_iso, "NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta", 0.0, "isolation", true, ctx.get("SystDirection_MuonIso", "nominal")));
}

bool MuonIdIsoScaleFactors2018::process(Event & event) {

  m_sf_id->process(event);
  m_sf_iso->process(event);

  return true;
}


ElectronIdRecoScaleFactors2016::ElectronIdRecoScaleFactors2016(Context & ctx) {

  string filepath_sf_id = ctx.get("uhh2Dir")+"HighPtSingleTop/data/ScaleFactors/2016/2016LegacyReReco_ElectronTight_Fall17V2.root";
  m_sf_id.reset(new MCElecScaleFactor(ctx, filepath_sf_id, 0.0, "tight_id", ctx.get("SystDirection_ElectronId", "nominal")));

  string filepath_sf_reco = ctx.get("uhh2Dir")+"HighPtSingleTop/data/ScaleFactors/2016/EGM2D_BtoH_GT20GeV_RecoSF_Legacy2016.root";
  m_sf_reco.reset(new MCElecScaleFactor(ctx, filepath_sf_reco, 0.0, "reco", ctx.get("SystDirection_ElectronReco", "nominal")));
}

bool ElectronIdRecoScaleFactors2016::process(Event & event) {

  m_sf_id->process(event);
  m_sf_reco->process(event);

  return true;
}


ElectronIdRecoScaleFactors2017::ElectronIdRecoScaleFactors2017(Context & ctx) {

  string filepath_sf_id = ctx.get("uhh2Dir")+"HighPtSingleTop/data/ScaleFactors/2017/2017_ElectronTight.root";
  m_sf_id.reset(new MCElecScaleFactor(ctx, filepath_sf_id, 0.0, "tight_id", ctx.get("SystDirection_ElectronId", "nominal")));

  string filepath_sf_reco = ctx.get("uhh2Dir")+"HighPtSingleTop/data/ScaleFactors/2017/egammaEffi.txt_EGM2D_runBCDEF_passingRECO.root";
  m_sf_reco.reset(new MCElecScaleFactor(ctx, filepath_sf_reco, 0.0, "reco", ctx.get("SystDirection_ElectronReco", "nominal")));
}

bool ElectronIdRecoScaleFactors2017::process(Event & event) {

  m_sf_id->process(event);
  m_sf_reco->process(event);

  return true;
}


ElectronIdRecoScaleFactors2018::ElectronIdRecoScaleFactors2018(Context & ctx) {

  string filepath_sf_id = ctx.get("uhh2Dir")+"HighPtSingleTop/data/ScaleFactors/2018/2018_ElectronTight.root";
  m_sf_id.reset(new MCElecScaleFactor(ctx, filepath_sf_id, 0.0, "tight_id", ctx.get("SystDirection_ElectronId", "nominal")));

  string filepath_sf_reco = ctx.get("uhh2Dir")+"HighPtSingleTop/data/ScaleFactors/2018/egammaEffi.txt_EGM2D_updatedAll.root";
  m_sf_reco.reset(new MCElecScaleFactor(ctx, filepath_sf_reco, 0.0, "reco", ctx.get("SystDirection_ElectronReco", "nominal")));
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
    throw invalid_argument("LeptonScaleFactors: invalid analysis channel");
  }
}

bool LeptonScaleFactors::process(Event & event) {

  return m_sf_lepton->process(event);
}



/*
  Trigger weights classes; credits to B2G-20-006 (Arne & Anna: Search for vector-like T to tH to lepton+jets).
  Approval of electron SFs: https://indico.cern.ch/event/879922/
  Approval of muon SFs: https://indico.cern.ch/event/912058/
*/


ElectronTriggerWeights::ElectronTriggerWeights(Context & ctx) {

  h_ele_weight      = ctx.declare_event_output<float>("weight_sfelec_trigger");
  h_ele_weight_up   = ctx.declare_event_output<float>("weight_sfelec_trigger_up");
  h_ele_weight_down = ctx.declare_event_output<float>("weight_sfelec_trigger_down");

  auto dataset_type = ctx.get("dataset_type");
  bool is_mc = dataset_type == "MC";
  if(!is_mc){
    cout << "Warning: ElectronTriggerWeights will not have an effect on this non-MC sample (dataset_type = '" + dataset_type + "')" << endl;
    return;
  }
  year = extract_year(ctx);
  TString yeartag = "";
  if(year == Year::is2016v3 || year == Year::is2016v2) yeartag = "2016";
  else if(year == Year::is2017v1 || year == Year::is2017v2) yeartag = "2017";
  else if(year == Year::is2018) yeartag = "2018";
  else throw runtime_error("ElectronTriggerWeights: invalid year");

  unique_ptr<TFile> file_pt1, file_pt2;
  TString path = ctx.get("uhh2Dir");
  if(yeartag == "2016") {
    file_pt1.reset(new TFile(path+"/"+yeartag+"/ElectronTriggerScaleFactors_eta_ele_binned_official_pt30to175_withsyst.root","READ"));
    file_pt2.reset(new TFile(path+"/"+yeartag+"/ElectronTriggerScaleFactors_eta_ele_binned_official_pt175toInf.root","READ"));
  }
  else if(yeartag == "2017" || yeartag == "2018") {
    file_pt1.reset(new TFile(path+"/"+yeartag+"/ElectronTriggerScaleFactors_eta_ele_binned_official_pt30to200_withsyst.root","READ"));
    file_pt2.reset(new TFile(path+"/"+yeartag+"/ElectronTriggerScaleFactors_eta_ele_binned_official_pt200toInf.root","READ"));
  }
  if(yeartag == "2016") pt_bins = {30, 175};
  else if(yeartag == "2017" || yeartag == "2018") pt_bins = {30, 200};

  g_sf_pt1.reset((TGraphAsymmErrors*)file_pt1->Get("ScaleFactors"));
  g_sf_pt2.reset((TGraphAsymmErrors*)file_pt2->Get("ScaleFactors"));

  string syst_direction_ = ctx.get("SystDirection_ElectronTrigger", "nominal");
  if(syst_direction_ == "up") {
    syst_direction = 1;
  }
  else if(syst_direction_ == "down") {
    syst_direction = -1;
  }
  else {
    syst_direction = 0;
  }
}

bool ElectronTriggerWeights::process(Event & event) {

  if(event.isRealData) {
    event.set(h_ele_weight, 1.);
    event.set(h_ele_weight_up, 1.);
    event.set(h_ele_weight_down, 1.);
    return true;
  }

  const Electron ele = event.electrons->at(0);
  double eta = ele.eta();
  double pt = ele.pt();
  if(fabs(eta) > 2.4) throw runtime_error("ElectronTriggerWeights::process(): electron |eta| > 2.4 is not supported.");
  if(pt < pt_bins[0]) throw runtime_error("ElectronTriggerWeights::process(): electron pt too small, not supported.");

  // find number of correct eta bin
  int idx = 0;
  if(pt < pt_bins[1]) {
    bool keep_going = true;
    while(keep_going) {
      double x,y;
      g_sf_pt1->GetPoint(idx,x,y);
      keep_going = eta > x + g_sf_pt1->GetErrorXhigh(idx);
      if(keep_going) idx++;
    }
  }
  else {
    bool keep_going = true;
    while(keep_going) {
      double x,y;
      g_sf_pt2->GetPoint(idx,x,y);
      keep_going = eta > x + g_sf_pt2->GetErrorXhigh(idx);
      if(keep_going) idx++;
    }
  }

  // access scale factors, possibility to add 2% t&p (tag & probe) systematic uncertainty. TODO: Check this!
  double sf, sf_up, sf_down, dummy_x;
  double stat_up = -1., stat_down = -1., tp = 0.0, total_up = -1., total_down = -1.;
  if(pt < pt_bins[1]) {
    g_sf_pt1->GetPoint(idx,dummy_x,sf);

    stat_up = g_sf_pt1->GetErrorYhigh(idx);
    stat_down = g_sf_pt1->GetErrorYlow(idx);
    total_up = sqrt(pow(stat_up,2) + pow(tp,2));
    total_down = sqrt(pow(stat_down,2) + pow(tp,2));

    sf_up = sf + total_up;
    sf_down = sf - total_down;
  }
  else {
    g_sf_pt2->GetPoint(idx,dummy_x,sf);

    stat_up = g_sf_pt2->GetErrorYhigh(idx);
    stat_down = g_sf_pt2->GetErrorYlow(idx);
    total_up = sqrt(pow(stat_up,2) + pow(tp,2));
    total_down = sqrt(pow(stat_down,2) + pow(tp,2));

    sf_up = sf + total_up;
    sf_down = sf - total_down;
  }

  event.set(h_ele_weight, sf);
  event.set(h_ele_weight_up, sf_up);
  event.set(h_ele_weight_down, sf_down);

  if (syst_direction == 1) {
    event.weight *= sf_up;
  } else if (syst_direction == -1) {
    event.weight *= sf_down;
  } else {
    event.weight *= sf;
  }

  return true;
}


MuonTriggerWeights::MuonTriggerWeights(Context & ctx) {

  h_muo_weight      = ctx.declare_event_output<float>("weight_sfmuon_trigger");
  h_muo_weight_up   = ctx.declare_event_output<float>("weight_sfmuon_trigger_up");
  h_muo_weight_down = ctx.declare_event_output<float>("weight_sfmuon_trigger_down");

  auto dataset_type = ctx.get("dataset_type");
  bool is_mc = dataset_type == "MC";
  if(!is_mc) {
    cout << "Warning: MuonTriggerWeights will not have an effect on this non-MC sample (dataset_type = '" + dataset_type + "')" << endl;
    return;
  }
  year = extract_year(ctx);
  TString yeartag = "";
  if(year == Year::is2016v3 || year == Year::is2016v2) yeartag = "2016";
  else if(year == Year::is2017v1 || year == Year::is2017v2) yeartag = "2017";
  else if(year == Year::is2018) yeartag = "2018";
  else throw runtime_error("MuonTriggerWeights: invalid year");

  unique_ptr<TFile> file_0to0p9, file_0p9to1p2, file_1p2to2p1, file_2p1to2p4;
  TString path = ctx.get("uhh2Dir");
  file_0to0p9.reset(new TFile(path+"/"+yeartag+"/MuonTriggerScaleFactors_pt_mu_binned_official_eta0to0p9_withsyst.root","READ"));
  file_0p9to1p2.reset(new TFile(path+"/"+yeartag+"/MuonTriggerScaleFactors_pt_mu_binned_official_eta0p9to1p2_withsyst.root","READ"));
  file_1p2to2p1.reset(new TFile(path+"/"+yeartag+"/MuonTriggerScaleFactors_pt_mu_binned_official_eta1p2to2p1_withsyst.root","READ"));
  file_2p1to2p4.reset(new TFile(path+"/"+yeartag+"/MuonTriggerScaleFactors_pt_mu_binned_official_rebin_eta2p1to2p4_withsyst.root","READ"));

  g_sf_0to0p9.reset((TGraphAsymmErrors*)file_0to0p9->Get("ScaleFactors"));
  g_sf_0p9to1p2.reset((TGraphAsymmErrors*)file_0p9to1p2->Get("ScaleFactors"));
  g_sf_1p2to2p1.reset((TGraphAsymmErrors*)file_1p2to2p1->Get("ScaleFactors"));
  g_sf_2p1to2p4.reset((TGraphAsymmErrors*)file_2p1to2p4->Get("ScaleFactors"));

  string syst_direction_ = ctx.get("SystDirection_MuonTrigger", "nominal");
  if(syst_direction_ == "up") {
    syst_direction = 1;
  }
  else if(syst_direction_ == "down") {
    syst_direction = -1;
  }
  else {
    syst_direction = 0;
  }
}

bool MuonTriggerWeights::process(Event & event){

  if(event.isRealData) {
    event.set(h_muo_weight, 1.);
    event.set(h_muo_weight_up, 1.);
    event.set(h_muo_weight_down, 1.);
    return true;
  }

  const Muon muon = event.muons->at(0);
  double eta = fabs(muon.eta());
  double pt = (muon.pt() >= 1000.0 ? 999.0 : muon.pt());
  if(eta > 2.4) throw runtime_error("MuonTriggerWeights::process(): muon |eta| > 2.4 is not supported.");
  if(pt < 30.0) throw runtime_error("MuonTriggerWeights::process(): muon pt too small, not supported.");

  // find number of correct pt bin
  int idx = 0;
  if(eta < 0.9) {
    bool keep_going = true;
    while(keep_going) {
      double x,y;
      g_sf_0to0p9->GetPoint(idx,x,y);
      keep_going = (pt > x + g_sf_0to0p9->GetErrorXhigh(idx));
      if(keep_going) idx++;
    }
  }
  else if(eta < 1.2) {
    bool keep_going = true;
    while(keep_going) {
      double x,y;
      g_sf_0p9to1p2->GetPoint(idx,x,y);
      keep_going = (pt > x + g_sf_0p9to1p2->GetErrorXhigh(idx));
      if(keep_going) idx++;
    }
  }
  else if(eta < 2.1) {
    bool keep_going = true;
    while(keep_going) {
      double x,y;
      g_sf_1p2to2p1->GetPoint(idx,x,y);
      keep_going = (pt > x + g_sf_1p2to2p1->GetErrorXhigh(idx));
      if(keep_going) idx++;
    }
  }
  else {
    bool keep_going = true;
    while(keep_going) {
      double x,y;
      g_sf_2p1to2p4->GetPoint(idx,x,y);
      keep_going = (pt > x + g_sf_2p1to2p4->GetErrorXhigh(idx));
      if(keep_going) idx++;
    }
  }

  // access scale factors, possibility to add 2% t&p (tag & probe) systematic uncertainty. TODO: Check this!
  double sf, sf_up, sf_down, dummy_x;
  double stat_up = -1., stat_down = -1., tp = 0.0, total_up = -1., total_down = -1.;
  if(eta < 0.9) {
    g_sf_0to0p9->GetPoint(idx,dummy_x,sf);

    stat_up = g_sf_0to0p9->GetErrorYhigh(idx);
    stat_down = g_sf_0to0p9->GetErrorYlow(idx);
    total_up = sqrt(pow(stat_up,2) + pow(tp,2));
    total_down = sqrt(pow(stat_down,2) + pow(tp,2));

    sf_up = sf + total_up;
    sf_down = sf - total_down;
  }
  else if(eta < 1.2) {
    g_sf_0p9to1p2->GetPoint(idx,dummy_x,sf);

    stat_up = g_sf_0p9to1p2->GetErrorYhigh(idx);
    stat_down = g_sf_0p9to1p2->GetErrorYlow(idx);
    total_up = sqrt(pow(stat_up,2) + pow(tp,2));
    total_down = sqrt(pow(stat_down,2) + pow(tp,2));

    sf_up = sf + total_up;
    sf_down = sf - total_down;
  }
  else if(eta < 2.1) {
    g_sf_1p2to2p1->GetPoint(idx,dummy_x,sf);

    stat_up = g_sf_1p2to2p1->GetErrorYhigh(idx);
    stat_down = g_sf_1p2to2p1->GetErrorYlow(idx);
    total_up = sqrt(pow(stat_up,2) + pow(tp,2));
    total_down = sqrt(pow(stat_down,2) + pow(tp,2));

    sf_up = sf + total_up;
    sf_down = sf - total_down;
  }
  else {
    g_sf_2p1to2p4->GetPoint(idx,dummy_x,sf);

    stat_up = g_sf_2p1to2p4->GetErrorYhigh(idx);
    stat_down = g_sf_2p1to2p4->GetErrorYlow(idx);
    total_up = sqrt(pow(stat_up,2) + pow(tp,2));
    total_down = sqrt(pow(stat_down,2) + pow(tp,2));

    sf_up = sf + total_up;
    sf_down = sf - total_down;
  }

  event.set(h_muo_weight, sf);
  event.set(h_muo_weight_up, sf_up);
  event.set(h_muo_weight_down, sf_down);

  if (syst_direction == 1) {
    event.weight *= sf_up;
  } else if (syst_direction == -1) {
    event.weight *= sf_down;
  } else {
    event.weight *= sf;
  }

  return true;
}


TriggerScaleFactors::TriggerScaleFactors(Context & ctx) {

  if(ctx.get("analysis_channel") == "ele") {
    m_sf_trigger.reset(new ElectronTriggerWeights(ctx));
  }
  else if(ctx.get("analysis_channel") == "muo") {
    m_sf_trigger.reset(new MuonTriggerWeights(ctx));
  }
  else {
    throw invalid_argument("TriggerScaleFactors: invalid analysis channel");
  }
}

bool TriggerScaleFactors::process(Event & event) {

  return m_sf_trigger->process(event);
}
