#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/LumiSelection.h"
#include "UHH2/common/include/CommonModules.h"
#include "UHH2/common/include/AdditionalSelections.h"
#include "UHH2/common/include/TriggerSelection.h"
#include "UHH2/common/include/ObjectIdUtils.h"
#include "UHH2/common/include/ElectronIds.h"
#include "UHH2/common/include/MuonIds.h"
#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/PrimaryLepton.h"
#include "UHH2/common/include/Utils.h"

#include "UHH2/HighPtSingleTop/include/AndHists.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopSelections.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopHists.h"
#include "UHH2/HighPtSingleTop/include/SingleTopGen_tWch.h"
#include "UHH2/HighPtSingleTop/include/METXYCorrections.h"
#include "UHH2/HighPtSingleTop/include/MyEventHists.h"

#include "UHH2/HOTVR/include/HOTVRJetCorrectionModule.h"
#include "UHH2/HOTVR/include/HOTVRIds.h"


using namespace std;
using namespace uhh2;

namespace uhh2 {

  class HighPtSingleTopPreSelectionModule: public AnalysisModule {
  public:
    
    explicit HighPtSingleTopPreSelectionModule(Context & ctx);
    virtual bool process(Event & event) override;
    
  private:

    Year year;
    
    std::unique_ptr<CommonModules> common_modules;

    std::unique_ptr<AnalysisModule> clnr_muon, clnr_elec, clnr_hotvr, hotvr_jec_module, clnr_jetLeptonOverlap;
    std::unique_ptr<AnalysisModule> primarylep;
    std::unique_ptr<AnalysisModule> SingleTopGen_tWchProd;
    std::unique_ptr<AnalysisModule> met_xy_correction;
    
    std::unique_ptr<Selection> slct_mttbarGenCut, slct_tWgenSignal;
    std::unique_ptr<Selection> slct_lumi, slct_trigger;
    std::unique_ptr<Selection> slct_1muon, slct_0muon, slct_1elec, slct_0elec;
    std::unique_ptr<Selection> slct_met, slct_1jet, slct_1hotvr;
    
    std::unique_ptr<AndHists> hist_common, hist_trigger, hist_cleaning, hist_1lepton, hist_met, hist_1jet, hist_1hotvr;
    //std::unique_ptr<Hists> hist_met_xy_uncorr, hist_met_xy_corr;

    bool is_data, is_mc, is_muon, is_elec;
    string dataset_version;

    double muonPt_min, muonEta_max, muonPt_min_veto, muonEta_max_veto, muonIso_max;
    double elecPt_min, elecEta_max, elecPt_min_veto, elecEta_max_veto;
    double met_min;
    double jetPt_min, jetEta_max, hotvrPt_min, hotvrEta_max, hotvrDeltaRToLepton_min, jetDeltaRToLepton_min;
  };


  HighPtSingleTopPreSelectionModule::HighPtSingleTopPreSelectionModule(Context & ctx){

    //------//
    // KEYS //
    //------//

    year = extract_year(ctx);

    is_data = ctx.get("dataset_type") == "DATA";
    is_mc   = ctx.get("dataset_type") == "MC";
    is_muon = ctx.get("analysis_channel") == "MUON";
    is_elec = ctx.get("analysis_channel") == "ELECTRON";

    if(!(is_data || is_mc)) throw runtime_error("HighPtSingleTopPreSelectionModule: Dataset is labeled as neither DATA nor MC. Please check the XML config file!");
    if(!(is_muon || is_elec)) throw runtime_error("HighPtSingleTopPreSelectionModule: Analysis channel ( ELECTRON / MUON ) not correctly given. Please check the XML config file!");

    dataset_version = ctx.get("dataset_version");


    //---------------------//
    // KINEMATIC VARIABLES //
    //---------------------//

    muonPt_min_veto = 30.0;
    muonEta_max_veto = 2.4;

    elecPt_min_veto = 30.0;
    elecEta_max_veto = 2.4;

    jetPt_min = 30.0;
    jetEta_max = 2.4;

    hotvrPt_min = 200.0; // needs to be zero so that migrations from < 200 GeV bins into the boosted sector can still be calculated once the unfolding happens (although these migrations are probably tiny)
    hotvrEta_max = 2.5;

    hotvrDeltaRToLepton_min = 1.5;
    jetDeltaRToLepton_min = 0.4;

    muonPt_min  = 50.0;
    muonEta_max =  2.4;
    muonIso_max =  0.15;

    elecPt_min  = 50.0;
    elecEta_max =  2.4;

    met_min = 50.0;


    //-----------------//
    // IDENTIFICATIONS //
    //-----------------//

    MuonId muonID_veto = AndId<Muon>(MuonID(Muon::Selector::CutBasedIdLoose), PtEtaCut(muonPt_min_veto, muonEta_max_veto));
    MuonId muonID = AndId<Muon>(MuonID(Muon::Selector::CutBasedIdTight), PtEtaCut(muonPt_min, muonEta_max), MuonIso(muonIso_max));
    ElectronId elecID_veto;
    ElectronId elecID;
    if (year == Year::is2016v2 || year == Year::is2016v3)
      {
	elecID_veto = AndId<Electron>(ElectronID_Summer16_veto, PtEtaCut(elecPt_min_veto, elecEta_max_veto));
	elecID = AndId<Electron>(ElectronID_Summer16_tight, PtEtaCut(elecPt_min, elecEta_max));
      }
    else
      {
	elecID_veto = AndId<Electron>(ElectronID_Fall17_veto, PtEtaCut(elecPt_min_veto, elecEta_max_veto));
	elecID = AndId<Electron>(ElectronID_Fall17_tight, PtEtaCut(elecPt_min, elecEta_max));
      }
    JetId jetID = PtEtaCut(jetPt_min, jetEta_max);
    TopJetId hotvrID = AndId<TopJet>(PtEtaCut(hotvrPt_min, hotvrEta_max), DeltaRCut(ctx, hotvrDeltaRToLepton_min)); // // through away all HOTVR jets with lepton close by <-- Roman's recommendation from October 16, 2019


    //----------------//
    // EVENT CLEANING //
    //----------------//

    common_modules.reset(new CommonModules());
    common_modules->switch_jetlepcleaner(false); // switch off jet lepton cleaning <-- Roman's recommendation from October 16, 2019
    common_modules->switch_jetPtSorter(true);
    common_modules->switch_metcorrection(true);
    common_modules->set_jet_id(jetID);
    common_modules->set_muon_id(muonID_veto);
    common_modules->set_electron_id(elecID_veto);
    common_modules->init(ctx);

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
    slct_tWgenSignal.reset(new tWgenSignalSelection(ctx, is_muon));

    // For recommendations, see https://twiki.cern.ch/twiki/bin/view/CMS/SingleTopTWRun2
    // https://twiki.cern.ch/twiki/bin/view/CMS/MuonHLT2016
    // https://twiki.cern.ch/twiki/bin/view/CMS/EgHLTRunIISummary
    // https://indico.cern.ch/event/662745/   Approval of SFs for one of the previously used trigger combinations (see git commit history)
    slct_trigger.reset(new HighPtSingleTopTriggerSelection(ctx));

    slct_1muon.reset(new NMuonSelection(1, 1));
    slct_0muon.reset(new NMuonSelection(0, 0));
    slct_1elec.reset(new NElectronSelection(1, 1));
    slct_0elec.reset(new NElectronSelection(0, 0));
    slct_met.reset(new METSelection(met_min));
    slct_1jet.reset(new NJetSelection(1, -1));
    slct_1hotvr.reset(new NTopJetSelection(1, -1));


    //------------//
    // HISTOGRAMS //
    //------------//

    //hist_met_xy_uncorr.reset(new MyEventHists(ctx, "00_MET_uncorr"));
    //hist_met_xy_corr.reset(new MyEventHists(ctx, "01_MET_corr"));


    hist_common.reset(new AndHists(ctx, "0_Common"));
    hist_trigger.reset(new AndHists(ctx, "1_Trigger"));
    hist_1lepton.reset(new AndHists(ctx, "2_OneLepton"));
    hist_met.reset(new AndHists(ctx, "3_MET"));
    hist_1jet.reset(new AndHists(ctx, "4_OneJet"));
    hist_1hotvr.reset(new AndHists(ctx, "5_OneHotvr"));


    //---------------//
    // MISCELLANEOUS //
    //---------------//

    primarylep.reset(new PrimaryLepton(ctx));
    SingleTopGen_tWchProd.reset(new SingleTopGen_tWchProducer(ctx, "h_GENtW"));
    met_xy_correction.reset(new METXYCorrections(ctx));
  }


  //------------//
  // EVENT LOOP //
  //------------//

  bool HighPtSingleTopPreSelectionModule::process(Event & event) {
    
    /* No event weights will be applied in this Module!!! Except for (via CommonModules):
       - MC lumi weight
       - MC pileup reweight
       I.e. the histograms filled here have little to no physical meaning! Be aware of that!
     */

    // Lumi selection
    if(is_data && !slct_lumi->passes(event)) return false;

    // Split up tW samples into SIGNAL and OTHER depending on MC truth info
    if(dataset_version.find("ST_tW") == 0) {
      SingleTopGen_tWchProd->process(event);
      if(dataset_version.find("ST_tW_signal") == 0 && !slct_tWgenSignal->passes(event)) return false;
      else if(dataset_version.find("ST_tW_other") == 0 && slct_tWgenSignal->passes(event)) return false;
    }

    // Mttbar gencut
    if(dataset_version.find("TTbar_M0to700") == 0 && !slct_mttbarGenCut->passes(event)) return false;

    // Initial cleaning, MET+PV filter, and lumi+PU weights
    //hist_met_xy_uncorr->fill(event);
    //hist_met_xy_corr->fill(event);

    if(!common_modules->process(event)) return false;
    hist_common->fill(event);
    met_xy_correction->process(event);

    // Trigger paths
    if(!slct_trigger->passes(event)) return false;
    hist_trigger->fill(event);

    // Single-lepton selection and veto on additional leptons
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
    primarylep->process(event);
    hist_1lepton->fill(event);

    // MET selection
    if(!slct_met->passes(event)) return false;
    hist_met->fill(event);

    // At least one AK4 jet
    clnr_jetLeptonOverlap->process(event);
    if(!slct_1jet->passes(event)) return false;
    hist_1jet->fill(event);

    // At least one HOTVR jet
    hotvr_jec_module->process(event);
    clnr_hotvr->process(event);
    if(!slct_1hotvr->passes(event)) return false;
    hist_1hotvr->fill(event);

    // Place additional selections into a new Module!!!
    // End of preselection
    return true;
  }


  UHH2_REGISTER_ANALYSIS_MODULE(HighPtSingleTopPreSelectionModule)
}
