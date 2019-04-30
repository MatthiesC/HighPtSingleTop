#include "UHH2/HighPtSingleTop/include/HighPtSingleTopModules.h"

using namespace std;
using namespace uhh2;


ObjectSetup::ObjectSetup(Context & ctx) {

  //------//
  // KEYS //
  //------//

  is_mc = ctx.get("dataset_type") == "MC";


  //---------------------//
  // KINEMATIC VARIABLES //
  //---------------------//

  muonPt_min_veto = 30.0;
  muonEta_max_veto = 2.4;

  elecPt_min_veto = 30.0;
  elecEta_max_veto = 2.4;

  jetPt_min = 30.0;
  jetEta_max = 2.4;

  hotvrPt_min = 0.0; // needs to be zero so that migrations from < 200 GeV bins into the boosted sector can still be calculated once the unfolding happens (although these migrations are probably tiny)
  hotvrEta_max = 2.5;


  //-----------------//
  // IDENTIFICATIONS //
  //-----------------//

  pvID = StandardPrimaryVertexId();
  muonID_preJLC = AndId<Muon>(MuonIDLoose(), PtEtaCut(muonPt_min_veto, muonEta_max_veto));
  muonID_veto = AndId<Muon>(MuonIDLoose(), PtEtaCut(muonPt_min_veto, muonEta_max_veto));
  elecID_preJLC = AndId<Electron>(ElectronID_Spring16_veto, PtEtaCut(elecPt_min_veto, elecEta_max_veto));
  elecID_veto = AndId<Electron>(ElectronID_Spring16_veto, PtEtaCut(elecPt_min_veto, elecEta_max_veto));
  jetPFID = JetPFID(JetPFID::WP_TIGHT); // See: https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
  jetID = PtEtaCut(jetPt_min, jetEta_max);
  hotvrID = PtEtaCut(hotvrPt_min, hotvrEta_max);


  //-----------------//
  // OBJECT CLEANING //
  //-----------------//

  clnr_pv.reset(new PrimaryVertexCleaner(pvID));
  clnr_muon_preJLC.reset(new MuonCleaner(muonID_preJLC));
  clnr_muon_veto.reset(new MuonCleaner(muonID_veto));
  clnr_elec_preJLC.reset(new ElectronCleaner(elecID_preJLC));
  clnr_elec_veto.reset(new ElectronCleaner(elecID_veto));
  clnr_PFjet.reset(new JetCleaner(ctx, jetPFID));
  clnr_jet.reset(new JetCleaner(ctx, jetID));
  clnr_hotvr.reset(new TopJetCleaner(ctx, hotvrID));


  //-----------------//
  // JET CORRECTIONS //
  //-----------------//

  jlc_hotvr.reset(new HOTVRJetLeptonCleaner());
  if(is_mc) {  
    jec_ak4_mc.reset(new JetCorrector(ctx, JERFiles::Summer16_23Sep2016_V4_L123_AK4PFchs_MC));
    jet_resolution_smearer.reset(new JetResolutionSmearer(ctx));
    jlc_mc.reset(new JetLeptonCleaner(ctx, JERFiles::Summer16_23Sep2016_V4_L123_AK4PFchs_MC));
    jec_hotvr_mc.reset(new HOTVRJetCorrector(ctx, JERFiles::Summer16_23Sep2016_V4_L123_AK4PFPuppi_MC));
  } else {
    // AK4
    jec_ak4_BCD.reset(new JetCorrector(ctx, JERFiles::Summer16_23Sep2016_V4_BCD_L123_AK4PFchs_DATA));
    jec_ak4_EFearly.reset(new JetCorrector(ctx, JERFiles::Summer16_23Sep2016_V4_EF_L123_AK4PFchs_DATA));
    jec_ak4_FlateG.reset(new JetCorrector(ctx, JERFiles::Summer16_23Sep2016_V4_G_L123_AK4PFchs_DATA));
    jec_ak4_H.reset(new JetCorrector(ctx, JERFiles::Summer16_23Sep2016_V4_H_L123_AK4PFchs_DATA));
    jlc_BCD.reset(new JetLeptonCleaner(ctx, JERFiles::Summer16_23Sep2016_V4_BCD_L123_AK4PFchs_DATA));
    jlc_EFearly.reset(new JetLeptonCleaner(ctx, JERFiles::Summer16_23Sep2016_V4_EF_L123_AK4PFchs_DATA));
    jlc_FlateG.reset(new JetLeptonCleaner(ctx, JERFiles::Summer16_23Sep2016_V4_G_L123_AK4PFchs_DATA));
    jlc_H.reset(new JetLeptonCleaner(ctx, JERFiles::Summer16_23Sep2016_V4_H_L123_AK4PFchs_DATA));
    // HOTVR
    jec_hotvr_BCD.reset(new HOTVRJetCorrector(ctx, JERFiles::Summer16_23Sep2016_V4_BCD_L123_AK4PFPuppi_DATA));
    jec_hotvr_EFearly.reset(new HOTVRJetCorrector(ctx, JERFiles::Summer16_23Sep2016_V4_EF_L123_AK4PFPuppi_DATA));
    jec_hotvr_FlateG.reset(new HOTVRJetCorrector(ctx, JERFiles::Summer16_23Sep2016_V4_G_L123_AK4PFPuppi_DATA));
    jec_hotvr_H.reset(new HOTVRJetCorrector(ctx, JERFiles::Summer16_23Sep2016_V4_H_L123_AK4PFPuppi_DATA));
  }


  //-------------//
  // MET FILTERS //
  //-------------//

  // See: https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2

  metfilters.reset(new AndSelection(ctx, "metfilters"));
  metfilters->add<TriggerSelection>("HBHENoiseFilter", "Flag_HBHENoiseFilter");
  metfilters->add<TriggerSelection>("HBHENoiseIsoFilter", "Flag_HBHENoiseIsoFilter");
  metfilters->add<TriggerSelection>("globalTightHalo2016Filter", "Flag_globalTightHalo2016Filter");
  metfilters->add<TriggerSelection>("EcalDeadCellTriggerPrimitiveFilter", "Flag_EcalDeadCellTriggerPrimitiveFilter");
  if(!is_mc) metfilters->add<TriggerSelection>("eeBadScFilter", "Flag_eeBadScFilter"); // only suggested for data
  metfilters->add<TriggerSelection>("chargedHadronTrackResolutionFilter", "Flag_chargedHadronTrackResolutionFilter"); 
  metfilters->add<TriggerSelection>("muonBadTrackFilter", "Flag_muonBadTrackFilter");
  metfilters->add<NPVSelection>("1 good PV", 1, -1, pvID);
}


bool ObjectSetup::process(Event & event) {

  // Primary vertex cleaning  
  clnr_pv->process(event);

  // MET filters
  if(!metfilters->passes(event)) return false;

  // Pre-JLC lepton cleaning
  clnr_muon_preJLC->process(event);
  clnr_elec_preJLC->process(event);

  // Jet cleaning and energy corrections
  clnr_PFjet->process(event);
  jlc_hotvr->process(event);
  if(is_mc) {
    jlc_mc->process(event);
    jec_ak4_mc->process(event);
    jec_ak4_mc->correct_met(event);
    jet_resolution_smearer->process(event);
    jec_hotvr_mc->process(event);
  } else {
    if(event.run <= runnr_BCD) {
      jlc_BCD->process(event);
      jec_ak4_BCD->process(event);
      jec_ak4_BCD->correct_met(event);
      jec_hotvr_BCD->process(event);
    } else if(event.run < runnr_EFearly) { // < is correct, not <=
      jlc_EFearly->process(event); 
      jec_ak4_EFearly->process(event);
      jec_ak4_EFearly->correct_met(event);
      jec_hotvr_EFearly->process(event);
    } else if(event.run <= runnr_FlateG) {
      jlc_FlateG->process(event);
      jec_ak4_FlateG->process(event);
      jec_ak4_FlateG->correct_met(event);
      jec_hotvr_FlateG->process(event);
    } else if(event.run > runnr_FlateG) {
      jlc_H->process(event);
      jec_ak4_H->process(event);
      jec_ak4_H->correct_met(event);
      jec_hotvr_H->process(event);
    } else throw runtime_error("ObjectSetup: Run number not covered by if-statements in process routine.");
  }
  clnr_jet->process(event);
  sort_by_pt(*event.jets);
  clnr_hotvr->process(event);
  sort_by_pt(*event.topjets);

  // Veto lepton cleaning
  clnr_muon_veto->process(event);
  clnr_elec_veto->process(event);

  return true;
}
