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
    
    std::unique_ptr<AnalysisModule> sf_lumi, sf_pileup, sf_toppt;
    std::unique_ptr<AnalysisModule> scale_variation;
    
    std::unique_ptr<AndHists> hist_sf;

    bool is_data, is_mc, is_muon, is_elec, bTopPtReweighting;
    string dataset_version;
    string syst_PU;
  };


  HighPtSingleTopMainSelectionModule::HighPtSingleTopMainSelectionModule(Context & ctx){

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

    syst_PU = ctx.get("SystDirection_PU", "nominal");


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
    sf_pileup.reset(new MCPileupReweight(ctx, syst_PU));
    sf_toppt.reset(new TopPtReweight(ctx, 0.0615, -0.0005, "", "", bTopPtReweighting, 1.0)); // https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopPtReweighting
    scale_variation.reset(new MCScaleVariation(ctx));


    //------------//
    // SELECTIONS //
    //------------//


    //------------//
    // HISTOGRAMS //
    //------------//

    hist_sf.reset(new AndHists(ctx, "0_ScaleFactors"));


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
	// muon trigger, id, iso, tk
      }
      else if(is_elec) {
	// electron trigger, id, reco
      }
      if(bTopPtReweighting && dataset_version.find("TTbar") == 0) sf_toppt->process(event);
      scale_variation->process(event);
    }
    hist_sf->fill(event);

    // Place analysis routines into a new Module!!!
    // End of main selection
    return true;
  }


  UHH2_REGISTER_ANALYSIS_MODULE(HighPtSingleTopMainSelectionModule)
}
