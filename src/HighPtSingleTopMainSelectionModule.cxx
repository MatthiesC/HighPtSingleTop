#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/TopPtReweight.h"

#include "UHH2/HighPtSingleTop/include/AndHists.h"


using namespace std;
using namespace uhh2;

namespace uhh2 {

  class HighPtSingleTopMainSelectionModule: public AnalysisModule {
  public:
    
    explicit HighPtSingleTopMainSelectionModule(Context & ctx);
    virtual bool process(Event & event) override;
    
  private:
    
    std::unique_ptr<AnalysisModule> sf_lumi, sf_pileup, sf_muon_trig, sf_muon_id, sf_muon_iso, sf_muon_trk, sf_toppt;
    std::unique_ptr<AnalysisModule> scale_variation;
    
    std::unique_ptr<AndHists> hist_sf, hist_topptreweighting;

    bool is_data, is_mc, is_muon, is_elec, bTopPtReweighting;
    string dataset_version;
    string syst_pileup, syst_muon_trigger, syst_muon_id, syst_muon_iso, syst_muon_trk;
  };


  HighPtSingleTopMainSelectionModule::HighPtSingleTopMainSelectionModule(Context & ctx) {

    //------//
    // KEYS //
    //------//
    
    is_data = ctx.get("dataset_type") == "DATA";
    is_mc   = ctx.get("dataset_type") == "MC";
    is_muon = ctx.get("analysis_channel") == "MUON";
    is_elec = ctx.get("analysis_channel") == "ELECTRON";

    if(!(is_data || is_mc)) throw runtime_error("HighPtSingleTopPreSelectionModule: Dataset is labeled as neither DATA nor MC. Please check the XML config file!");
    if(!(is_muon || is_elec)) throw runtime_error("HighPtSingleTopPreSelectionModule: Analysis channel ( ELECTRON / MUON ) not correctly given. Please check the XML config file!");

    dataset_version = ctx.get("dataset_version");

    bTopPtReweighting = ctx.get("TopPtReweighting") == true;

    syst_pileup = ctx.get("SystDirection_Pileup", "nominal");
    syst_muon_trigger = ctx.get("SystDirection_MuonTrig", "nominal");
    syst_muon_id = ctx.get("SystDirection_MuonId", "nominal");
    syst_muon_iso = ctx.get("SystDirection_MuonIso", "nominal");
    syst_muon_trk = ctx.get("SystDirection_MuonTrk", "nominal");


    //---------------------//
    // KINEMATIC VARIABLES //
    //---------------------//


    //-----------------//
    // IDENTIFICATIONS //
    //-----------------//


    //----------------//
    // EVENT CLEANING //
    //----------------//


    //---------------//
    // SCALE FACTORS //
    //---------------//

    sf_lumi.reset(new MCLumiWeight(ctx));
    sf_pileup.reset(new MCPileupReweight(ctx, syst_pileup));
    sf_muon_trig.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/matthies/Analysis_80x_v5/CMSSW_8_0_24_patch1/src/UHH2/common/data/MuonTrigger_EfficienciesAndSF_average_RunBtoH.root", "IsoMu24_OR_IsoTkMu24_PtEtaBins", 0.5, "trigger", true, syst_muon_trigger));
    sf_muon_id.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/matthies/Analysis_80x_v5/CMSSW_8_0_24_patch1/src/UHH2/common/data/MuonID_EfficienciesAndSF_average_RunBtoH.root", "MC_NUM_TightID_DEN_genTracks_PAR_pt_eta", 1, "tightID", true, syst_muon_id));
    sf_muon_iso.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/matthies/Analysis_80x_v5/CMSSW_8_0_24_patch1/src/UHH2/common/data/MuonIso_EfficienciesAndSF_average_RunBtoH.root", "TightISO_TightID_pt_eta", 1, "iso", true, syst_muon_iso));
    sf_muon_trk.reset(new MCMuonTrkScaleFactor(ctx, "/nfs/dust/cms/user/matthies/Analysis_80x_v5/CMSSW_8_0_24_patch1/src/UHH2/common/data/Tracking_EfficienciesAndSF_BCDEFGH.root", 1, "track", syst_muon_trk));
    sf_toppt.reset(new TopPtReweight(ctx, 0.0615, -0.0005, "", "", true, 1.0)); // https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopPtReweighting
    scale_variation.reset(new MCScaleVariation(ctx));


    //------------//
    // SELECTIONS //
    //------------//


    //------------//
    // HISTOGRAMS //
    //------------//

    hist_sf.reset(new AndHists(ctx, "0_ScaleFactors"));
    hist_topptreweighting.reset(new AndHists(ctx, "1_TopPtReweighting"));


    //---------------//
    // MISCELLANEOUS //
    //---------------//

  }


  //------------//
  // EVENT LOOP //
  //------------//

  bool HighPtSingleTopMainSelectionModule::process(Event & event) {

    // Apply scale factors
    if(is_mc) {
      sf_lumi->process(event);
      sf_pileup->process(event);
      if(is_muon) {
	sf_muon_trig->process(event);
	sf_muon_id->process(event);
	sf_muon_iso->process(event);
	sf_muon_trk->process(event);
      }
      else if(is_elec) {
	// electron trigger, id, reco
      }
      //if(bTopPtReweighting && dataset_version.find("TTbar") == 0) sf_toppt->process(event);
      scale_variation->process(event);
    }
    hist_sf->fill(event);

    // Apply top pt reweighting
    if(dataset_version.find("TTbar") == 0) sf_toppt->process(event);
    hist_topptreweighting->fill(event);

    // Place analysis routines into a new Module!!!
    // End of main selection
    return false;
  }


  UHH2_REGISTER_ANALYSIS_MODULE(HighPtSingleTopMainSelectionModule)
}
