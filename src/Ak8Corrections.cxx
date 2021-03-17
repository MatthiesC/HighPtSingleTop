#include "UHH2/HighPtSingleTop/include/Ak8Corrections.h"

#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/JetCorrectionSets.h"
#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/EventVariables.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/EventVariables.h"
#include "UHH2/common/include/LumiSelection.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/common/include/TriggerSelection.h"

#include "UHH2/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "UHH2/JetMETObjects/interface/JetCorrectorParameters.h"

using namespace uhh2;
using namespace std;


/*
* AK8 CORRECTIONS SETUP
*/

Ak8Corrections::Ak8Corrections() {

  tjec_tag_2016 = "Summer16_07Aug2017";
  tjec_ver_2016 = "11";

  tjec_tag_2017 = "Fall17_17Nov2017";
  tjec_ver_2017 = "32";

  tjec_tag_2018 = "Autumn18";
  tjec_ver_2018 = "19";
}

void Ak8Corrections::init(Context & ctx) {

  if(init_done) {
    throw invalid_argument("Ak8Corrections::init called twice!");
  }
  init_done = true;

  is_mc = ctx.get("dataset_type") == "MC";
  year = extract_year(ctx);

  std::string userTopJetColl = string2lowercase(ctx.get("Ak8recCollection"));

  std::string algo = "";
  if(userTopJetColl.find("ak4") != std::string::npos) {
    algo = "AK4";
  }
  else if(userTopJetColl.find("ak8") != std::string::npos) {
    algo = "AK8";
  }
  else if(userTopJetColl.find("ak8") == std::string::npos) {
    std::cout << "Ak8Corrections.cxx: Cannot determine tjet cone + radius (neither AK4 nor AK8) - going to assume it is AK8 for JECs" << '\n';
    algo = "AK8";
  }

  std::string pus = "PFchs"; // Pileup subtraction
  if(userTopJetColl.find("puppi") != std::string::npos) {
    pus = "PFPuppi";
  }
  else if(userTopJetColl.find("chs") == std::string::npos) {
    std::cout << "Cannot determine pileup subtraction (neither CHS nor PUPPI) - going to assume it is CHS for JECs" << std::endl;
  }
  tjec_tjet_coll = algo + pus;

  if(is_mc) {
    tjet_corrector_MC.reset(new YearSwitcher(ctx));
    tjet_corrector_MC->setup2016(std::make_shared<GenericTopJetCorrector>(ctx, JERFiles::JECFilesMC(tjec_tag_2016, tjec_ver_2016, tjec_tjet_coll), ctx.get("Ak8recCollection")));
    tjet_corrector_MC->setup2017(std::make_shared<GenericTopJetCorrector>(ctx, JERFiles::JECFilesMC(tjec_tag_2017, tjec_ver_2017, tjec_tjet_coll), ctx.get("Ak8recCollection")));
    tjet_corrector_MC->setup2018(std::make_shared<GenericTopJetCorrector>(ctx, JERFiles::JECFilesMC(tjec_tag_2018, tjec_ver_2018, tjec_tjet_coll), ctx.get("Ak8recCollection")));

    std::string jer_tag = "";
    if(year == Year::is2016v2 || year == Year::is2016v3) {
      jer_tag = "Summer16_25nsV1";
    }
    else if (year == Year::is2017v1 || year == Year::is2017v2) {
      jer_tag = "Fall17_V3";
    }
    else if (year == Year::is2018) {
      jer_tag = "Autumn18_V7";
    }
    else {
      throw runtime_error("Cannot find suitable jet resolution file & scale factors for this year for JetResolutionSmearer");
    }

    tjet_resolution_smearer.reset(new GenericJetResolutionSmearer(ctx, ctx.get("Ak8recCollection"), ctx.get("Ak8genCollection"), JERFiles::JERPathStringMC(jer_tag, tjec_tjet_coll, "SF"), JERFiles::JERPathStringMC(jer_tag, tjec_tjet_coll, "PtResolution")));
  }
  else {
    tjec_switcher_16.reset(new RunSwitcher(ctx, "2016"));
    for(const auto & runItr : runPeriods2016) { // runPeriods defined in common/include/Utils.h
      tjec_switcher_16->setupRun(runItr, std::make_shared<GenericTopJetCorrector>(ctx, JERFiles::JECFilesDATA(tjec_tag_2016, tjec_ver_2016, tjec_tjet_coll, runItr), ctx.get("Ak8recCollection")));
    }

    tjec_switcher_17.reset(new RunSwitcher(ctx, "2017"));
    for(const auto & runItr : runPeriods2017) {
      tjec_switcher_17->setupRun(runItr, std::make_shared<GenericTopJetCorrector>(ctx, JERFiles::JECFilesDATA(tjec_tag_2017, tjec_ver_2017, tjec_tjet_coll, runItr), ctx.get("Ak8recCollection")));
    }

    tjec_switcher_18.reset(new RunSwitcher(ctx, "2018"));
    for(const auto & runItr : runPeriods2018) {
      tjec_switcher_18->setupRun(runItr, std::make_shared<GenericTopJetCorrector>(ctx, JERFiles::JECFilesDATA(tjec_tag_2018, tjec_ver_2018, tjec_tjet_coll, runItr), ctx.get("Ak8recCollection")));
    }

    tjet_corrector_data.reset(new YearSwitcher(ctx));
    tjet_corrector_data->setup2016(tjec_switcher_16);
    tjet_corrector_data->setup2017(tjec_switcher_17);
    tjet_corrector_data->setup2018(tjec_switcher_18);
  }
}

bool Ak8Corrections::process(uhh2::Event & event) {

  if(!init_done) {
    throw runtime_error("Ak8Corrections::init not called (has to be called in AnalysisModule constructor)");
  }
  if(is_mc) {
    tjet_corrector_MC->process(event);
  }
  else {
    tjet_corrector_data->process(event);
  }
  if(is_mc) tjet_resolution_smearer->process(event);
  return true;
}


/*
* AK8 CLEANING
*/

Ak8Cleaning::Ak8Cleaning(Context & ctx, const double minPt, const double maxEta, const double minDR, const string & h_primlep_name) {

  _minPt = minPt;
  _maxEta = maxEta;
  _minDR = minDR;
  h_ak8jets_rec = ctx.get_handle<vector<TopJet>>(ctx.get("Ak8recCollection"));
  h_primlep = ctx.get_handle<FlavorParticle>(h_primlep_name);
}

bool Ak8Cleaning::process(uhh2::Event & event) {

  vector<TopJet> initial_topjets = event.get(h_ak8jets_rec);
  vector<TopJet> cleaned_topjets;
  const auto & primlep = event.get(h_primlep);

  for(const auto & tjet : initial_topjets) {
    if(uhh2::deltaR(tjet, primlep) < _minDR) continue;
    if(abs(tjet.v4().eta()) > _maxEta) continue;
    if(tjet.v4().pt() < _minPt) continue;
    cleaned_topjets.push_back(tjet);
  }
  if(jet_pt_sorter) sort_by_pt<TopJet>(cleaned_topjets);

  event.set(h_ak8jets_rec, cleaned_topjets);

  return true;
}
