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







namespace {

  // to share some code between JetCorrector and JetLeptonCleaner, provide some methods
  // dealing with jet energy corrections here:
  std::unique_ptr<FactorizedJetCorrector> build_corrector(const std::vector<std::string> & filenames){
    std::vector<JetCorrectorParameters> pars;
    for(const auto & filename : filenames){
      pars.emplace_back(locate_file(filename));
    }
    return uhh2::make_unique<FactorizedJetCorrector>(pars);
  }

  JetCorrectionUncertainty* corrector_uncertainty(uhh2::Context & ctx, const std::vector<std::string> & filenames, int &direction){

      auto dir = ctx.get("jecsmear_direction", "nominal");
      if (ctx.get("dataset_type") != "MC") {
        direction = 0;
      }
      else if(dir == "up"){
        direction = 1;
      }
      else if(dir == "down"){
        direction = -1;
      }
      else if(dir != "nominal"){
        // direction = 0 is default
        throw runtime_error("JetCorrector: invalid value jecsmear_direction='" + dir + "' (valid: 'nominal', 'up', 'down')");
      }

      // Get optional source of JEC, defaults the total uncertainty if the user doesn't specify one
      std::string source = ctx.get("jecsmear_source", "Total");

      //initialize JetCorrectionUncertainty if shift direction is not "nominal", else return NULL pointer
      if(direction!=0){
        //take name from the L1FastJet correction (0th element of filenames) and replace "L1FastJet" by "UncertaintySources" to get the proper name of the uncertainty file
        TString unc_file = locate_file(filenames[0]);
        if (unc_file.Contains("L1FastJet")) {
          unc_file.ReplaceAll("L1FastJet","UncertaintySources");
        }
        else if (unc_file.Contains("L2Relative")) {
          unc_file.ReplaceAll("L2Relative","UncertaintySources");
        }
        else {
          throw runtime_error("WARNING No JEC Uncertainty File found!");
        }
        JetCorrectionUncertainty* jec_uncertainty = new JetCorrectionUncertainty(*(new JetCorrectorParameters(unc_file.Data(), source)));
        return jec_uncertainty;
      }
      return NULL;

    }

}








Ak8Corrector::Ak8Corrector(uhh2::Context & ctx, const std::vector<std::string> & filenames){
  corrector = build_corrector(filenames);
  direction = 0;
  jec_uncertainty = corrector_uncertainty(ctx, filenames, direction) ;

  // chris
  h_ak8jets_rec = ctx.get_handle<vector<TopJet>>(ctx.get("Ak8recCollection"));
}

bool Ak8Corrector::process(uhh2::Event & event){
  //assert(event.topjets);

  vector<TopJet> ak8jets_rec = event.get(h_ak8jets_rec);

  for(auto & jet : ak8jets_rec){
    correct_jet(*corrector, jet, event, jec_uncertainty, direction);
  }
  return true;
}

// note: implement here because only here (and not in the header file), the destructor of FactorizedJetCorrector is known
Ak8Corrector::~Ak8Corrector(){}











void Ak8Corrections::fail_if_init() const{
  if(init_done){
    throw invalid_argument("Ak8Corrections::init already called!");
  }
}


Ak8Corrections::Ak8Corrections(){

  // chris
  //h_ak8jets_rec = ctx.get_handle<vector<TopJet>>(ctx.get("Ak8recCollection"));
  //h_ak8jets_gen = ctx.get_handle<vector<TopJet>>(ctx.get("Ak8genCollection"));

  tjec_tag_2016 = "Summer16_07Aug2017";
  tjec_ver_2016 = "11";

  tjec_tag_2017 = "Fall17_17Nov2017";
  tjec_ver_2017 = "32";

  tjec_tag_2018 = "Autumn18";
  tjec_ver_2018 = "7";

  tjec_tjet_coll = "dummy";
}


void Ak8Corrections::init(Context & ctx){
  if(init_done){
    throw invalid_argument("Ak8Corrections::init called twice!");
  }
  init_done = true;

  is_mc = ctx.get("dataset_type") == "MC";
  year = extract_year(ctx);

  // setup correction tjet type for JECs
  std::string userTopJetColl = string2lowercase(ctx.get("Ak8recCollection"));

  std::string algo = "";
  // algo size
  if (userTopJetColl.find("ak4") != std::string::npos) {
    algo = "AK4";
  }
  else if(userTopJetColl.find("ak8") != std::string::npos){
    algo = "AK8";
  }
  else if (userTopJetColl.find("ak8") == std::string::npos) {
    std::cout << "Ak8Corrections.cxx: Cannot determine tjet cone + radius (neither AK4 nor AK8) - going to assume it is AK8 for JECs" << '\n';
    algo = "AK8";
  }

  std::string pus = "PFchs";
  // Pileup subtraction
  if (userTopJetColl.find("puppi") != std::string::npos) {
    pus = "PFPuppi";
  } else if (userTopJetColl.find("chs") == std::string::npos) {
    std::cout << "Cannot determine pileup subtraction (neither CHS nor PUPPI) - going to assume it is CHS for JECs" << std::endl;
  }
  tjec_tjet_coll = algo + pus;

  if(is_mc){
    tjet_corrector_MC.reset(new YearSwitcher(ctx));
    tjet_corrector_MC->setup2016(std::make_shared<Ak8Corrector>(ctx, JERFiles::JECFilesMC(tjec_tag_2016, tjec_ver_2016, tjec_tjet_coll)));
    tjet_corrector_MC->setup2017(std::make_shared<Ak8Corrector>(ctx, JERFiles::JECFilesMC(tjec_tag_2017, tjec_ver_2017, tjec_tjet_coll)));
    tjet_corrector_MC->setup2018(std::make_shared<Ak8Corrector>(ctx, JERFiles::JECFilesMC(tjec_tag_2018, tjec_ver_2018, tjec_tjet_coll)));

    JERSmearing::SFtype1 JER_sf;
    std::string resFilename = "";
    if (year == Year::is2016v2 || year == Year::is2016v3) {
      JER_sf = JERSmearing::SF_13TeV_Summer16_25nsV1;
      resFilename = "common/data/2016/Summer16_25nsV1_MC_PtResolution_"+algo+pus+".txt";
    } else if (year == Year::is2017v1 || year == Year::is2017v2) {
      JER_sf = JERSmearing::SF_13TeV_Fall17_V3;
      resFilename = "common/data/2017/Fall17_V3_MC_PtResolution_"+algo+pus+".txt";
    } else if (year == Year::is2018) {
      JER_sf = JERSmearing::SF_13TeV_Autumn18_RunABCD_V4;
      resFilename = "common/data/2018/Autumn18_V4_MC_PtResolution_"+algo+pus+".txt";
    } else {
      throw runtime_error("Cannot find suitable jet resolution file & scale factors for this year for JetResolutionSmearer");
    }

    tjet_resolution_smearer.reset(new GenericJetResolutionSmearer(ctx, ctx.get("Ak8recCollection"), ctx.get("Ak8genCollection"), JER_sf, resFilename)); // TODO: Check event handle strings!!!!!
  }
  else{
    tjec_switcher_16.reset(new RunSwitcher(ctx, "2016"));
    for (const auto & runItr : runPeriods2016) { // runPeriods defined in common/include/Utils.h
      tjec_switcher_16->setupRun(runItr, std::make_shared<Ak8Corrector>(ctx, JERFiles::JECFilesDATA(tjec_tag_2016, tjec_ver_2016, tjec_tjet_coll, runItr)));
    }

    tjec_switcher_17.reset(new RunSwitcher(ctx, "2017"));
    for (const auto & runItr : runPeriods2017) {
      tjec_switcher_17->setupRun(runItr, std::make_shared<Ak8Corrector>(ctx, JERFiles::JECFilesDATA(tjec_tag_2017, tjec_ver_2017, tjec_tjet_coll, runItr)));
    }

    tjec_switcher_18.reset(new RunSwitcher(ctx, "2018"));
    for (const auto & runItr : runPeriods2018) {
      tjec_switcher_18->setupRun(runItr, std::make_shared<Ak8Corrector>(ctx, JERFiles::JECFilesDATA(tjec_tag_2018, tjec_ver_2018, tjec_tjet_coll, runItr)));
    }

    tjet_corrector_data.reset(new YearSwitcher(ctx));
    tjet_corrector_data->setup2016(tjec_switcher_16);
    tjet_corrector_data->setup2017(tjec_switcher_17);
    tjet_corrector_data->setup2018(tjec_switcher_18);
  }
}

bool Ak8Corrections::process(uhh2::Event & event){
  if(!init_done){
    throw runtime_error("Ak8Corrections::init not called (has to be called in AnalysisModule constructor)");
  }
  if (is_mc) {
    tjet_corrector_MC->process(event);
  } else {
    tjet_corrector_data->process(event);
  }

  if(is_mc) tjet_resolution_smearer->process(event);
  return true;
}





Ak8LeptonDeltaRCleaner::Ak8LeptonDeltaRCleaner(Context & ctx, const double & minDR, const string & h_primlep_name) {

  _minDR = minDR;
  h_ak8jets_rec = ctx.get_handle<vector<TopJet>>(ctx.get("Ak8recCollection"));
  h_primlep = ctx.get_handle<FlavorParticle>(h_primlep_name);
}


bool Ak8LeptonDeltaRCleaner::process(uhh2::Event & event) {

  //assert(event.topjets);
  vector<TopJet> initial_topjets = event.get(h_ak8jets_rec);
  //cout << "initial " << initial_topjets.size() << endl;
  vector<TopJet> cleaned_topjets;
  const auto & primlep = event.get(h_primlep);

  for(const auto & tjet : initial_topjets) {
    if(uhh2::deltaR(tjet, primlep) > _minDR) {
      cleaned_topjets.push_back(tjet);
    }
  }
  //cout << "cleaned " << cleaned_topjets.size() << endl;

  event.set(h_ak8jets_rec, cleaned_topjets);

  //event.topjets->clear();
  //event.topjets->reserve(cleaned_topjets.size());
  //for(auto & j : cleaned_topjets) event.topjets->push_back(j);

  return true;
}
