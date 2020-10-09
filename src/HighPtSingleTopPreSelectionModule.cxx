#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/LumiSelection.h"
#include "UHH2/common/include/CommonModules.h"
#include "UHH2/common/include/AdditionalSelections.h"
#include "UHH2/common/include/ObjectIdUtils.h"
#include "UHH2/common/include/ElectronIds.h"
#include "UHH2/common/include/MuonIds.h"
#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/PrimaryLepton.h"
#include "UHH2/common/include/Utils.h"

#include "UHH2/HOTVR/include/HOTVRJetCorrectionModule.h"
#include "UHH2/HOTVR/include/HOTVRIds.h"

#include "UHH2/HighPtSingleTop/include/AndHists.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopSelections.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopHists.h"
#include "UHH2/HighPtSingleTop/include/SingleTopGen_tWch.h"
#include "UHH2/HighPtSingleTop/include/METXYCorrections.h"
#include "UHH2/HighPtSingleTop/include/MyEventHists.h"
#include "UHH2/HighPtSingleTop/include/LeptonAndTriggerScaleFactors.h"
#include "UHH2/HighPtSingleTop/include/HcalAndEcalModules.h"


using namespace std;
using namespace uhh2;

namespace uhh2 {

  class HighPtSingleTopPreSelectionModule: public AnalysisModule {
  public:

    explicit HighPtSingleTopPreSelectionModule(Context & ctx);
    virtual bool process(Event & event) override;

  private:

    bool debug;

    Year year;

    unique_ptr<CommonModules> common_modules;

    unique_ptr<AnalysisModule> clnr_muon, clnr_elec, clnr_hotvr, hotvr_jec_module, clnr_jetLeptonOverlap;
    unique_ptr<AnalysisModule> primarylep, sf_lepton, sf_prefiring;
    unique_ptr<AnalysisModule> met_xy_correction;

    unique_ptr<Selection> slct_lumi, slct_mttbarGenCut;
    unique_ptr<Selection> slct_1muon, slct_0muon, slct_1elec, slct_0elec;
    unique_ptr<Selection> slct_met, slct_1hotvr;

    unique_ptr<MttHist> hist_mtt_before, hist_mtt_after;
    unique_ptr<AndHists> hist_common, hist_cleaning, hist_prefiring, hist_1lepton, hist_leptonSF, hist_jetleptonoverlapremoval, hist_metXYcorrection, hist_met, hist_hotvrJEC, hist_hotvrCleaner, hist_1hotvr;

    bool is_muon, is_elec;
    string dataset_version, met_name, jet_collection;
  };


  HighPtSingleTopPreSelectionModule::HighPtSingleTopPreSelectionModule(Context & ctx){

    //------//
    // KEYS //
    //------//

    debug = string2lowercase(ctx.get("Debug", "false")) == "true";

    year = extract_year(ctx);

    is_muon = ctx.get("analysis_channel") == "muo";
    is_elec = ctx.get("analysis_channel") == "ele";

    if(!(is_muon || is_elec)) throw runtime_error("HighPtSingleTopPreSelectionModule: Analysis channel ( ele / muo ) not correctly given. Please check the XML config file!");

    dataset_version = ctx.get("dataset_version");
    met_name = ctx.get("METName");
    jet_collection = ctx.get("JetCollection");


    //---------------------//
    // KINEMATIC VARIABLES //
    //---------------------//

    double muonPt_min_veto = 30.0;
    double muonEta_max_veto = 2.4;

    double elecPt_min_veto = 30.0;
    double elecEta_max_veto = 2.4;

    double jetPt_min = 30.0;
    double jetEta_max = 2.4;

    double hotvrPt_min = 200.0;
    double hotvrEta_max = 2.5;

    double hotvrDeltaRToLepton_min = 1.5;
    double jetDeltaRToLepton_min = 0.4;

    double muonPt_min = 50.0;
    double muonEta_max = 2.4;
    double muonIso_max = 0.15;

    double elecPt_min = 50.0;
    double elecEta_max = 2.4;

    double met_min = 50.0;


    //-----------------//
    // IDENTIFICATIONS //
    //-----------------//

    MuonId muonID_veto = AndId<Muon>(MuonID(Muon::Selector::CutBasedIdLoose), PtEtaCut(muonPt_min_veto, muonEta_max_veto));
    MuonId muonID = AndId<Muon>(MuonID(Muon::Selector::CutBasedIdTight), PtEtaCut(muonPt_min, muonEta_max), MuonIso(muonIso_max));

    // Recommendation for complete Run 2: https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2
    ElectronId elecID_veto = AndId<Electron>(ElectronID_Fall17_veto, PtEtaCut(elecPt_min_veto, elecEta_max_veto));
    ElectronId elecID = AndId<Electron>(ElectronID_Fall17_tight, PtEtaCut(elecPt_min, elecEta_max));

    JetPFID::wp jetPFID;
    if(jet_collection == "jetsAk4CHS") jetPFID = JetPFID::WP_TIGHT_CHS;
    else if(jet_collection == "jetsAk4Puppi") jetPFID = JetPFID::WP_TIGHT_PUPPI;
    else throw runtime_error("HighPtSingleTopPreSelectionModule: Please check jet PF ID");

    JetId jetID = PtEtaCut(jetPt_min, jetEta_max);

    TopJetId hotvrID = AndId<TopJet>(PtEtaCut(hotvrPt_min, hotvrEta_max), DeltaRCut(ctx, hotvrDeltaRToLepton_min)); // through away all HOTVR jets with lepton close by <-- Roman's recommendation from October 16, 2019


    //--------------------------//
    // CLEANING AND CORRECTIONS //
    //--------------------------//

    common_modules.reset(new CommonModules());
    common_modules->change_pf_id(jetPFID);
    common_modules->set_jet_id(jetID);
    common_modules->set_muon_id(muonID_veto);
    common_modules->set_electron_id(elecID_veto);
    common_modules->switch_metcorrection(true);
    common_modules->switch_jetlepcleaner(false); // switch off jet lepton cleaning <-- Roman's recommendation from October 16, 2019
    common_modules->switch_jetPtSorter(true);
    common_modules->disable_lumisel(); // done manually
    common_modules->init(ctx);

    met_xy_correction.reset(new METXYCorrections(ctx));
    hotvr_jec_module.reset(new HOTVRJetCorrectionModule(ctx));

    clnr_muon.reset(new MuonCleaner(muonID));
    clnr_elec.reset(new ElectronCleaner(elecID));
    clnr_hotvr.reset(new TopJetCleaner(ctx, hotvrID));
    clnr_jetLeptonOverlap.reset(new JetLeptonOverlapRemoval(ctx, jetDeltaRToLepton_min));


    //------------//
    // SELECTIONS //
    //------------//

    slct_lumi.reset(new LumiSelection(ctx));
    slct_mttbarGenCut.reset(new MttbarGenSelection(0, 700));
    slct_1muon.reset(new NMuonSelection(1, 1));
    slct_0muon.reset(new NMuonSelection(0, 0));
    slct_1elec.reset(new NElectronSelection(1, 1));
    slct_0elec.reset(new NElectronSelection(0, 0));
    slct_met.reset(new METSelection(met_min));
    slct_1hotvr.reset(new NTopJetSelection(1, -1));


    //------------//
    // HISTOGRAMS //
    //------------//

    hist_mtt_before.reset(new MttHist(ctx, "0_MttBefore"));
    hist_mtt_after.reset(new MttHist(ctx, "0_MttAfter"));
    hist_common.reset(new AndHists(ctx, "0_Common"));
    hist_prefiring.reset(new AndHists(ctx, "0_PrefiringSFs"));
    hist_1lepton.reset(new AndHists(ctx, "1_OneLepton"));
    hist_leptonSF.reset(new AndHists(ctx, "1_LeptonSFs"));
    hist_jetleptonoverlapremoval.reset(new AndHists(ctx, "1_JetLeptonCleaner"));
    hist_metXYcorrection.reset(new AndHists(ctx, "1_METXYCorrection"));
    hist_met.reset(new AndHists(ctx, "2_MET"));
    hist_hotvrJEC.reset(new AndHists(ctx, "2_HotvrJEC"));
    hist_hotvrCleaner.reset(new AndHists(ctx, "2_HotvrCleaner"));
    hist_1hotvr.reset(new AndHists(ctx, "3_OneHotvr"));


    //---------------//
    // MISCELLANEOUS //
    //---------------//

    primarylep.reset(new PrimaryLepton(ctx));
    sf_lepton.reset(new LeptonScaleFactors(ctx));
    sf_prefiring.reset(new PrefiringWeights(ctx));
  }


  //------------//
  // EVENT LOOP //
  //------------//

  bool HighPtSingleTopPreSelectionModule::process(Event & event) {

    /* Event weights applied within this Module:
       - MC lumi weight (via CommonModules)
       - MC pileup reweight (via CommonModules)
       - MC muon id/isolation and electron id/reco scale factors
       Scale factors are not saved after the processing is done and need to be re-applied during the main selection!
    */

    if(debug) {
      cout << endl;
      cout << "+-----------+" << endl;
      cout << "| NEW EVENT |" << endl;
      cout << "+-----------+" << endl;
    }

    if(debug) cout << "Lumi selection (need to do this manually before CommonModules)" << endl; // else getting error for some data samples, e.g. "RunSwitcher cannot handle run number 275656 for year 2016"
    if(event.isRealData && !slct_lumi->passes(event)) return false;

    if(debug) cout << "Mttbar gencut for the inclusive TTbar sample" << endl;
    if(dataset_version.find("TTbar") == 0) {
      hist_mtt_before->fill(event);
      if(dataset_version.find("Mtt0to700") != string::npos && !slct_mttbarGenCut->passes(event)) return false;
      if(dataset_version.find("Mtt700toInf") != string::npos && slct_mttbarGenCut->passes(event)) return false;
      hist_mtt_after->fill(event);
    }

    if(debug) cout << "CommonModules: Lumi selection, initial cleaning, MET+PV filter, and lumi+PU weights" << endl;
    if(!common_modules->process(event)) return false;
    hist_common->fill(event);

    if(debug) cout << "Apply prefiring weights" << endl;
    sf_prefiring->process(event);
    hist_prefiring->fill(event);

    if(debug) cout << "Single-lepton selection and veto on additional leptons" << endl;
    if(is_muon)
      {
        if(!(slct_1muon->passes(event) && slct_0elec->passes(event))) return false;
        clnr_muon->process(event);
        if(!slct_1muon->passes(event)) return false;
      }
    else if(is_elec)
      {
        if(!(slct_1elec->passes(event) && slct_0muon->passes(event))) return false;
        clnr_elec->process(event);
        if(!slct_1elec->passes(event)) return false;
      }
    hist_1lepton->fill(event);

    if(debug) cout << "Apply lepton id/iso/reco scale factors" << endl;
    sf_lepton->process(event);
    hist_leptonSF->fill(event);

    if(debug) cout << "Remove AK4 jets overlapping with the lepton" << endl;
    primarylep->process(event);
    clnr_jetLeptonOverlap->process(event);
    hist_jetleptonoverlapremoval->fill(event);

    if(debug) cout << "Correct MET XY" << endl;
    if(met_name == "slimmedMETs") { // only apply XY corrections if using PF MET; Puppi MET does not need XY corrections
      met_xy_correction->process(event);
      hist_metXYcorrection->fill(event);
    }

    if(debug) cout << "MET selection" << endl;
    if(!slct_met->passes(event)) return false;
    hist_met->fill(event);

    if(debug) cout << "Correct HOTVR jets" << endl;
    hotvr_jec_module->process(event);
    hist_hotvrJEC->fill(event);

    if(debug) cout << "Clean HOTVR jets" << endl;
    clnr_hotvr->process(event);
    hist_hotvrCleaner->fill(event);

    if(debug) cout << "At least one HOTVR jet" << endl;
    if(!slct_1hotvr->passes(event)) return false;
    hist_1hotvr->fill(event);

    // End of Module
    return true;
  }


  UHH2_REGISTER_ANALYSIS_MODULE(HighPtSingleTopPreSelectionModule)
}
