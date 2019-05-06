#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/JetHists.h"
#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/PrimaryLepton.h"
#include "UHH2/common/include/TopJetIds.h"

#include "UHH2/HighPtSingleTop/include/AndHists.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopHists.h"
#include "UHH2/HighPtSingleTop/include/HighPtSingleTopSelections.h"

#include "UHH2/HighPtSingleTop/include/TMVASetup.h"

#include "UHH2/HOTVR/include/HOTVRHists.h"
#include "UHH2/HOTVR/include/HOTVRIds.h"
#include "UHH2/HOTVR/include/HOTVRScaleFactor.h"
#include "UHH2/HOTVR/include/HadronicTop.h"


using namespace std;
using namespace uhh2;

namespace uhh2 {

  class HighPtSingleTopMainSelectionModule: public AnalysisModule {
  public:
    
    explicit HighPtSingleTopMainSelectionModule(Context & ctx);
    virtual bool process(Event & event) override;
    
  private:
    
    std::unique_ptr<AnalysisModule> sf_lumi, sf_pileup, sf_muon_trig, sf_muon_id, sf_muon_iso, sf_muon_trk, sf_toptag;//, sf_btag;
    std::unique_ptr<AnalysisModule> scale_variation, primarylep, hadronictop, tmva_setup;

    std::unique_ptr<Selection> slct_deltaRcut, slct_1toptag;//, slct_1btag;
    
    std::unique_ptr<AndHists> hist_noweights, hist_lumipuweights, hist_leptonsf, hist_deltaRcut, hist_1toptag;//, hist_1btag;
    std::unique_ptr<Hists> hist_BTagMCEfficiency;

    bool is_data, is_mc, is_muon, is_elec, bTopPtReweighting;
    string dataset_version;
    string syst_pileup, syst_muon_trigger, syst_muon_id, syst_muon_iso, syst_muon_trk, syst_hotvr_toptag, syst_btag;

    double hotvr_fpt_max, hotvr_jetmass_min, hotvr_jetmass_max, hotvr_mpair_min, hotvr_tau32_max;
    double deltaR_lepton_nextjet_min;

    TopJetId StandardHOTVRTopTagID;
    //JetId bjetID;

    std::vector<Event::Handle<float>> h_tmva_variables;
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

    syst_pileup = ctx.get("SystDirection_Pileup", "nominal");
    syst_muon_trigger = ctx.get("SystDirection_MuonTrig", "nominal");
    syst_muon_id = ctx.get("SystDirection_MuonId", "nominal");
    syst_muon_iso = ctx.get("SystDirection_MuonIso", "nominal");
    syst_muon_trk = ctx.get("SystDirection_MuonTrk", "nominal");
    syst_hotvr_toptag = ctx.get("SystDirection_HOTVRTopTagSF", "nominal");
    syst_btag = ctx.get("SystDirection_CSVv2BTagSF", "nominal");


    //---------------------//
    // KINEMATIC VARIABLES //
    //---------------------//

    // t-tagging criteria
    hotvr_fpt_max     = 0.8;
    hotvr_jetmass_min = 140;
    hotvr_jetmass_max = 220;
    hotvr_mpair_min   = 50;
    hotvr_tau32_max   = 0.56;

    deltaR_lepton_nextjet_min = 0.4; // minimum R-distance between primary lepton and next AK4 jet


    //-----------------//
    // IDENTIFICATIONS //
    //-----------------//

    StandardHOTVRTopTagID = AndId<TopJet>(HOTVRTopTag(hotvr_fpt_max, hotvr_jetmass_min, hotvr_jetmass_max, hotvr_mpair_min), Tau32Groomed(hotvr_tau32_max));
    //CSVBTag::wp btagWP = CSVBTag::WP_MEDIUM;
    //bjetID = CSVBTag(btagWP);


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
    scale_variation.reset(new MCScaleVariation(ctx));
    sf_toptag.reset(new HOTVRScaleFactor(ctx, StandardHOTVRTopTagID, syst_hotvr_toptag));
    //sf_btag.reset(new MCBTagScaleFactor(ctx, btagWP, "jets", syst_btag));


    //---------------//
    // MISCELLANEOUS //
    //---------------//

    primarylep.reset(new PrimaryLepton(ctx));
    hadronictop.reset(new HadronicTop(ctx));


    //------------//
    // SELECTIONS //
    //------------//

    slct_deltaRcut.reset(new DeltaRCut(ctx, deltaR_lepton_nextjet_min));
    slct_1toptag.reset(new NTopJetSelection(1, 1, StandardHOTVRTopTagID));
    //slct_1btag.reset(new NJetSelection(1, -1, bjetID));


    //------------//
    // HISTOGRAMS //
    //------------//

    hist_noweights.reset(new AndHists(ctx, "0_NoWeights"));
    hist_noweights->add_hist(new HighPtSingleTopHists(ctx, "0_NoWeights_CustomHists"));
    hist_lumipuweights.reset(new AndHists(ctx, "1_LumiAndPileupWeights"));
    hist_lumipuweights->add_hist(new HighPtSingleTopHists(ctx, "1_LumiAndPileupWeights_CustomHists"));
    hist_leptonsf.reset(new AndHists(ctx, "2_LeptonScaleFactors"));
    hist_leptonsf->add_hist(new HighPtSingleTopHists(ctx, "2_LeptonScaleFactors_CustomHists"));
    hist_deltaRcut.reset(new AndHists(ctx, "3_DeltaRCut"));
    hist_deltaRcut->add_hist(new HighPtSingleTopHists(ctx, "3_DeltaRCut_CustomHists"));
    hist_1toptag.reset(new AndHists(ctx, "4_OneTopTag"));
    hist_1toptag->add_hist(new HighPtSingleTopHists(ctx, "4_OneTopTag_CustomHists"));
    hist_1toptag->add_hist(new HOTVRHists(ctx, "4_OneTopTag_HOTVRTopTag", StandardHOTVRTopTagID));
    //hist_BTagMCEfficiency.reset(new BTagMCEfficiencyHists(ctx, "BTagMCEfficiency", btagWP));
    //hist_1btag.reset(new AndHists(ctx, "5_OneBTag"));
    //hist_1btag->add_hist(new HighPtSingleTopHists(ctx, "5_OneBTag_CustomHists"));
    //hist_1btag->add_hist(new HOTVRHists(ctx, "5_OneBTag_HOTVRTopTag", StandardHOTVRTopTagID));


    //---------------//
    // EVENT OUTPUTS //
    //---------------//

    std::vector<std::string> tmva_names = {"tmva_jet1_pt", "tmva_jet2_pt", "tmva_jet3_pt", "tmva_jet4_pt", "tmva_jet5_pt", "tmva_jet6_pt", 
					   "tmva_jet1_eta", "tmva_jet2_eta", "tmva_jet3_eta", "tmva_jet4_eta", "tmva_jet5_eta", "tmva_jet6_eta", "tmva_number_ak4",
					   "tmva_hotvr1_pt", "tmva_hotvr2_pt", "tmva_hotvr3_pt", "tmva_hotvr4_pt", "tmva_hotvr1_eta", "tmva_hotvr2_eta", "tmva_hotvr3_eta", "tmva_hotvr4_eta", "tmva_number_hotvr",
					   "tmva_jet1_area", "tmva_jet2_area", "tmva_jet3_area", "tmva_jet4_area", "tmva_jet5_area", "tmva_jet6_area", 
					   "tmva_hotvr1_area", "tmva_hotvr2_area", "tmva_hotvr3_area", "tmva_hotvr4_area",
					   "tmva_met_pt", "tmva_deltaPhi_met_lepton", "tmva_deltaPhi_met_ak4", "tmva_deltaPhi_met_hotvr1", "tmva_deltaPhi_lepton_ak4", "tmva_deltaPhi_lepton_hotvr1", "tmva_mtw",
					   "tmva_deltaR_lepton_ak4", "tmva_deltaR_lepton_hotvr1",
					   "tmva_lepton_eta", "tmva_lepton_pt",
					   "tmva_deltaR_jet1_ak4", "tmva_deltaPhi_jet1_ak4", "tmva_deltaR_jet2_ak4", "tmva_deltaPhi_jet2_ak4",
					   "tmva_eventweight", "tmva_toptag_pt"};
    for(unsigned int i = 0; i < tmva_names.size(); i++) {
      h_tmva_variables.push_back(ctx.declare_event_output<float>(tmva_names.at(i)));
    }
    tmva_setup.reset(new TMVASetup(ctx, tmva_names, StandardHOTVRTopTagID));
  }


  //------------//
  // EVENT LOOP //
  //------------//

  bool HighPtSingleTopMainSelectionModule::process(Event & event) {

    // Scale variations
    if(is_mc) scale_variation->process(event);

    // Apply scale factors
    hist_noweights->fill(event);
    if(is_mc) {
      sf_lumi->process(event);
      sf_pileup->process(event);
    }
    hist_lumipuweights->fill(event);
    if(is_mc) {
      if(is_muon) {
	sf_muon_trig->process(event);
	sf_muon_id->process(event);
	sf_muon_iso->process(event);
	sf_muon_trk->process(event);
      }
      else if(is_elec) {
	// electron trigger, id, reco
      }
    }
    hist_leptonsf->fill(event);

    // Apply cut on DeltaR(lepton, jet) to reject QCD
    primarylep->process(event);
    if(!slct_deltaRcut->passes(event)) return false;
    hist_deltaRcut->fill(event);

    // Require exactly one HOTVR t-tag
    if(!slct_1toptag->passes(event)) return false;
    hadronictop->process(event);
    sf_toptag->process(event);
    hist_1toptag->fill(event);

    // Do some TMVA shenanigans ...
    tmva_setup->process(event);

    // Require at least one AK4 b-tag (change to DeepCSV once using 10_2_X)
    //hist_BTagMCEfficiency->fill(event);
    //if(!slct_1btag->passes(event)) return false;
    //sf_btag->process(event);
    //hist_1btag->fill(event);

    

    // Place analysis routines into a new Module!!!
    // End of main selection
    return true;
  }


  UHH2_REGISTER_ANALYSIS_MODULE(HighPtSingleTopMainSelectionModule)
}
