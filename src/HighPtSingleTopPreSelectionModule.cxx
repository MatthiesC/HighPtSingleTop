#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/LumiSelection.h"
#include "UHH2/common/include/CommonModules.h"
#include "UHH2/common/include/AdditionalSelections.h"
#include "UHH2/common/include/ObjectIdUtils.h"
#include "UHH2/common/include/ElectronIds.h"
#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/MuonIds.h"
#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/PrimaryLepton.h"
#include "UHH2/common/include/Utils.h"

// #include "UHH2/HOTVR/include/HOTVRJetCorrectionModule.h"
// #include "UHH2/HOTVR/include/HOTVRIds.h"

#include "UHH2/HighPtSingleTop/include/Constants.h"
// #include "UHH2/HighPtSingleTop/include/Ak8Corrections.h"
// #include "UHH2/HighPtSingleTop/include/MyAk8Hists.h"
#include "UHH2/HighPtSingleTop/include/AndHists.h"
// #include "UHH2/HighPtSingleTop/include/HighPtSingleTopSelections.h"
// #include "UHH2/HighPtSingleTop/include/HighPtSingleTopHists.h"
#include "UHH2/HighPtSingleTop/include/SingleTopGen_tWch.h"
// #include "UHH2/HighPtSingleTop/include/METXYCorrections.h"
// #include "UHH2/HighPtSingleTop/include/MyEventHists.h"
#include "UHH2/HighPtSingleTop/include/LeptonAndTriggerScaleFactors.h"
// #include "UHH2/HighPtSingleTop/include/HcalAndEcalModules.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"
// #include "UHH2/HighPtSingleTop/include/TaggedJets.h"

#include "UHH2/LegacyTopTagging/include/Utils.h"
#include "UHH2/LegacyTopTagging/include/TopJetCorrections.h"
#include "UHH2/LegacyTopTagging/include/METXYCorrection.h"
#include "UHH2/LegacyTopTagging/include/AndHists.h"
#include "UHH2/LegacyTopTagging/include/LeptonScaleFactors.h"


using namespace std;
using namespace uhh2;

namespace uhh2 { namespace btw {

  class HighPtSingleTopPreSelectionModule: public AnalysisModule {
  public:

    explicit HighPtSingleTopPreSelectionModule(Context & ctx);
    virtual bool process(Event & event) override;

  private:

    bool debug;

    unique_ptr<AnalysisModule> sf_lumi;
    unique_ptr<AnalysisModule> sf_prefire;
    unique_ptr<CommonModules> common_modules;

    unique_ptr<AnalysisModule> clnr_muon;
    unique_ptr<AnalysisModule> clnr_elec;
    unique_ptr<AnalysisModule> primarylep;
    // unique_ptr<AnalysisModule> sf_lepton;
    unique_ptr<AnalysisModule> sf_muon_id;
    unique_ptr<AnalysisModule> sf_muon_id_dummy;
    unique_ptr<AnalysisModule> sf_elec_id;
    unique_ptr<AnalysisModule> sf_elec_id_dummy;
    unique_ptr<AnalysisModule> sf_elec_reco;
    unique_ptr<AnalysisModule> sf_elec_reco_dummy;

    unique_ptr<AnalysisModule> met_xy_correction;

    unique_ptr<Selection> slct_lumi;
    unique_ptr<Selection> slct_mttbarGenCut;
    unique_ptr<Selection> slct_1muon;
    unique_ptr<Selection> slct_0muon;
    unique_ptr<Selection> slct_1elec;
    unique_ptr<Selection> slct_0elec;
    unique_ptr<Selection> slct_met;

    unique_ptr<ltt::TopJetCorrections> corrections_hotvr;
    unique_ptr<ltt::TopJetCorrections> corrections_ak8;
    unique_ptr<ltt::TopJetCleaning> cleaner_hotvr;
    unique_ptr<ltt::TopJetCleaning> cleaner_ak8;
    unique_ptr<Selection> slct_1hotvr;
    unique_ptr<Selection> slct_1ak8;

    unique_ptr<MttHist> hist_mtt_before;
    unique_ptr<MttHist> hist_mtt_after;
    unique_ptr<ltt::AndHists> hist_nocuts;
    unique_ptr<ltt::AndHists> hist_common;
    unique_ptr<ltt::AndHists> hist_1lepton;
    unique_ptr<ltt::AndHists> hist_met_xy_correction;
    unique_ptr<ltt::AndHists> hist_met;
    unique_ptr<ltt::AndHists> hist_largejet_correction;
    unique_ptr<ltt::AndHists> hist_largejet_cleaning;
    unique_ptr<ltt::AndHists> hist_1largejet;

    // bool is_muo, is_ele;
    string dataset_version;
    bool is_TTbar, is_TTbar_Mtt, is_TTbar_syst;

    Event::Handle<LepChannel> handle_LepChannel;
  };


  HighPtSingleTopPreSelectionModule::HighPtSingleTopPreSelectionModule(Context & ctx){

    //------//
    // KEYS //
    //------//

    debug = string2bool(ctx.get("debug"));

    // is_muo = extract_channel(ctx) == Channel::isMuo;
    // is_ele = extract_channel(ctx) == Channel::isEle;

    dataset_version = ctx.get("dataset_version");
    is_TTbar = dataset_version.find("TTbar") == 0;
    is_TTbar_Mtt = is_TTbar && dataset_version.find("Mtt") != string::npos;
    is_TTbar_syst = is_TTbar && dataset_version.find("syst") != string::npos;


    //---------------------//
    // KINEMATIC VARIABLES //
    //---------------------//

    const double jetPt_min = 30.0;
    const double jetEta_max = 2.5;

    const double muonPt_min_veto = 30.0;
    const double muonEta_max_veto = 2.4;
    const double muonPt_min = 40.0;
    const double muonEta_max = muonEta_max_veto;
    // double muonIso_max = 0.15;

    const double elecPt_min_veto = 30.0;
    const double elecEta_max_veto = 2.4;
    const double elecPt_min = 40.0;
    const double elecEta_max = elecEta_max_veto;

    const double met_min = 50.0;

    const double hotvr_pt_min = 200.;
    const double hotvr_eta_max = 2.5;
    const double hotvr_dr_lep_min = 1.5;
    const double ak8_pt_min = 200.;
    const double ak8_eta_max = 2.5;
    const double ak8_dr_lep_min = 0.8;

    // double jetDeltaRToLepton_min = 0.4;

    //-----------------//
    // IDENTIFICATIONS //
    //-----------------//

    const MuonId muonID_veto = AndId<Muon>(MuonID(Muon::Selector::CutBasedIdLoose), PtEtaCut(muonPt_min_veto, muonEta_max_veto));
    const Muon::Selector muonSelector = Muon::CutBasedIdTight;
    const MuonId muonID = AndId<Muon>(MuonID(muonSelector), PtEtaCut(muonPt_min, muonEta_max));

    const ElectronId elecID_veto = AndId<Electron>(ElectronTagID(Electron::mvaEleID_Fall17_noIso_V2_wpLoose), PtEtaCut(elecPt_min_veto, elecEta_max_veto));
    const Electron::tag electronTag = Electron::mvaEleID_Fall17_noIso_V2_wp90;
    const ElectronId elecID = AndId<Electron>(ElectronTagID(electronTag), PtEtaCut(elecPt_min, elecEta_max));

    // MuonId muonID_veto = AndId<Muon>(MuonID(Muon::Selector::CutBasedIdLoose), PtEtaCut(muonPt_min_veto, muonEta_max_veto));
    // MuonId muonID;
    // if(is_QCDsideband && is_muo) {
    //   muonID = AndId<Muon>(MuonID(Muon::Selector::CutBasedIdTight), PtEtaCut(muonPt_min, muonEta_max), InvMuonIso(muonIso_max));
    // } else {
    //   // muonID = AndId<Muon>(MuonID(Muon::Selector::CutBasedIdTight), PtEtaCut(muonPt_min, muonEta_max), MuonIso(muonIso_max));
    //   muonID = AndId<Muon>(MuonID(Muon::Selector::CutBasedIdTight), PtEtaCut(muonPt_min, muonEta_max));
    // }
    //
    // // Recommendation for complete Run 2: https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2
    // ElectronId elecID_veto = AndId<Electron>(ElectronID_Fall17_veto, PtEtaCut(elecPt_min_veto, elecEta_max_veto));
    // ElectronId elecID;
    // if(is_QCDsideband && is_ele) {
    //   elecID = AndId<Electron>(MyElectronID_Fall17_tight_invertedIso, PtEtaCut(elecPt_min, elecEta_max));
    // } else {
    //   // elecID = AndId<Electron>(ElectronID_Fall17_tight, PtEtaCut(elecPt_min, elecEta_max));
    //   elecID = AndId<Electron>(ElectronID_Fall17_tight_noIso, PtEtaCut(elecPt_min, elecEta_max));
    // }

    JetPFID::wp jetPFID;
    if(ctx.get("JetCollection") == "jetsAk4CHS") jetPFID = JetPFID::WP_TIGHT_CHS;
    else if(ctx.get("JetCollection") == "jetsAk4Puppi") jetPFID = JetPFID::WP_TIGHT_PUPPI;
    else throw runtime_error("HighPtSingleTopPreSelectionModule: Please check jet PF ID");

    JetId jetID = PtEtaCut(jetPt_min, jetEta_max);

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
    common_modules->disable_mclumiweight(); // done manually
    common_modules->init(ctx);

    met_xy_correction.reset(new ltt::METXYCorrector(ctx));

    clnr_muon.reset(new MuonCleaner(muonID));
    clnr_elec.reset(new ElectronCleaner(elecID));

    corrections_hotvr.reset(new ltt::TopJetCorrections());
    corrections_hotvr->switch_topjet_corrections(false);
    corrections_hotvr->switch_subjet_corrections(true);
    corrections_hotvr->switch_rebuilding_topjets_from_subjets(true);
    corrections_hotvr->init(ctx);
    corrections_ak8.reset(new ltt::TopJetCorrections(ctx.get("AK8Collection_rec"), ctx.get("AK8Collection_gen")));
    corrections_ak8->init(ctx);

    cleaner_hotvr.reset(new ltt::TopJetCleaning(ctx, hotvr_pt_min, hotvr_eta_max, hotvr_dr_lep_min));
    cleaner_ak8.reset(new ltt::TopJetCleaning(ctx, ak8_pt_min, ak8_eta_max, ak8_dr_lep_min, ctx.get("AK8Collection_rec")));

    slct_1hotvr.reset(new NTopJetSelection(1, -1));
    slct_1ak8.reset(new NTopJetSelection(1, -1, boost::none, ctx.get_handle<vector<TopJet>>(ctx.get("AK8Collection_rec"))));

    //------------//
    // SELECTIONS //
    //------------//

    slct_lumi.reset(new LumiSelection(ctx));
    slct_mttbarGenCut.reset(new MttbarGenSelection(0, 700));
    slct_1muon.reset(new NMuonSelection(1, 1));
    slct_0muon.reset(new NMuonSelection(0, 0));
    slct_1elec.reset(new NElectronSelection(1, 1));
    slct_0elec.reset(new NElectronSelection(0, 0));
    slct_met.reset(new ltt::METSelection(met_min));

    //------------//
    // HISTOGRAMS //
    //------------//

    hist_mtt_before.reset(new MttHist(ctx, "0_MttBefore"));
    hist_mtt_after.reset(new MttHist(ctx, "0_MttAfter"));
    hist_nocuts.reset(new ltt::AndHists(ctx, "0_NoCuts"));
    hist_common.reset(new ltt::AndHists(ctx, "1_Common"));
    hist_1lepton.reset(new ltt::AndHists(ctx, "2_OneLepton"));
    hist_met_xy_correction.reset(new ltt::AndHists(ctx, "3_METXYCorr"));
    hist_met.reset(new ltt::AndHists(ctx, "4_MET", true));
    hist_largejet_correction.reset(new ltt::AndHists(ctx, "5_LargeJetCorr", true));
    hist_largejet_cleaning.reset(new ltt::AndHists(ctx, "6_LargeJetCleaning", true));
    hist_1largejet.reset(new ltt::AndHists(ctx, "7_OneLargeJet", true));


    //---------------//
    // MISCELLANEOUS //
    //---------------//

    primarylep.reset(new PrimaryLepton(ctx));
    sf_prefire.reset(new ltt::PrefiringWeights(ctx));
    sf_lumi.reset(new MCLumiWeight(ctx));

    // sf_lepton.reset(new LeptonScaleFactors(ctx));
    sf_muon_id.reset(new ltt::MuonIdScaleFactors(ctx, muonSelector));
    sf_muon_id_dummy.reset(new ltt::MuonIdScaleFactors(ctx, muonSelector, true));
    sf_elec_id.reset(new ltt::ElectronIdScaleFactors(ctx, electronTag));
    sf_elec_id_dummy.reset(new ltt::ElectronIdScaleFactors(ctx, electronTag, true));
    sf_elec_reco.reset(new ltt::ElectronRecoScaleFactors(ctx));
    sf_elec_reco_dummy.reset(new ltt::ElectronRecoScaleFactors(ctx, true));

    handle_LepChannel = ctx.declare_event_output<LepChannel>("LepChannel");
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
    sf_lumi->process(event);
    sf_prefire->process(event);

    if(debug) cout << "Mttbar gencut for the inclusive TTbar sample" << endl;
    if(is_TTbar && !is_TTbar_syst) {
      hist_mtt_before->fill(event);
      if(!is_TTbar_Mtt && !slct_mttbarGenCut->passes(event)) return false;
      hist_mtt_after->fill(event);
    }
    hist_nocuts->fill(event);

    if(debug) cout << "CommonModules: jet, electron, muon id cleaning; MET+PV filter; AK4+MET corrections; PU weights" << endl;
    if(!common_modules->process(event)) return false;
    hist_common->fill(event);

    if(debug) cout << "Single-lepton selection and veto on additional leptons" << endl;
    // if(is_muo) {
    //   if(!(slct_1muon->passes(event) && slct_0elec->passes(event))) return false;
    //   clnr_muon->process(event);
    //   if(!slct_1muon->passes(event)) return false;
    //   event.set(handle_LepChannel, LepChannel::muo)
    // }
    // else if(is_ele) {
    //   if(!(slct_1elec->passes(event) && slct_0muon->passes(event))) return false;
    //   clnr_elec->process(event);
    //   if(!slct_1elec->passes(event)) return false;
    //   event.set(handle_LepChannel, LepChannel::ele)
    // }

    const bool passes_muoChannel_vetoes = slct_1muon->passes(event) && slct_0elec->passes(event);
    const bool passes_eleChannel_vetoes = slct_1elec->passes(event) && slct_0muon->passes(event);
    if(passes_muoChannel_vetoes) {
      clnr_muon->process(event);
      if(!slct_1muon->passes(event)) return false;
      event.set(handle_LepChannel, LepChannel::muo);
      sf_muon_id->process(event);
      sf_elec_id_dummy->process(event);
      sf_elec_reco_dummy->process(event);
    }
    else if(passes_eleChannel_vetoes) {
      clnr_elec->process(event);
      if(!slct_1elec->passes(event)) return false;
      event.set(handle_LepChannel, LepChannel::ele);
      sf_elec_id->process(event);
      sf_elec_reco->process(event);
      sf_muon_id_dummy->process(event);
    }
    else {
      return false;
    }

    primarylep->process(event); // needed for top jet cleaning
    // sf_lepton->process(event);
    hist_1lepton->fill(event);

    if(debug) cout << "MET XY correction" << endl;
    met_xy_correction->process(event);
    hist_met_xy_correction->fill(event);

    if(debug) cout << "MET selection" << endl;
    if(!slct_met->passes(event)) return false;
    hist_met->fill(event);

    if(debug) cout << "Apply corrections to top jet collections and clean them" << endl;
    corrections_hotvr->process(event);
    corrections_ak8->process(event);
    hist_largejet_correction->fill(event);
    cleaner_hotvr->process(event);
    cleaner_ak8->process(event);
    hist_largejet_cleaning->fill(event);

    if(debug) cout << "At least one large-R jet (HOTVR or AK8)" << endl;
    const bool has_hotvr_jet = slct_1hotvr->passes(event);
    const bool has_ak8_jet = slct_1ak8->passes(event);
    if(!(has_hotvr_jet || has_ak8_jet)) return false;
    hist_1largejet->fill(event);

    if(debug) cout << "End of PreSelectionModule" << endl;
    return true;
  }


UHH2_REGISTER_ANALYSIS_MODULE(HighPtSingleTopPreSelectionModule)

}}
