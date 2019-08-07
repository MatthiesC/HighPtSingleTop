#include "UHH2/HighPtSingleTop/include/TMVASetup.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"

#include "UHH2/HOTVR/include/HOTVRIds.h"

#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/Utils.h"

using namespace std;
using namespace uhh2;


TMVASetup::TMVASetup(Context & ctx, vector<string> & tmva_names_, TopJetId & topjetID_):
  tmva_names(tmva_names_), topjetID(topjetID_) {

  for(std::string tmva_name : tmva_names) {
    h_tmva_variables.push_back(ctx.get_handle<float>(tmva_name));
  }

}


bool TMVASetup::process(Event & event) {

  vector<float> input_var;

  float w = event.weight;
  vector<Jet> jets = *event.jets;
  vector<TopJet> hotvrjets = *event.topjets;
  MET met = *event.met;
  const auto lepton = returnPrimaryLepton(event);
  TopJet toptag; // let's find the t-tagged hotvr jet:
  for (TopJet hj : hotvrjets) {
    if(!(topjetID(hj, event))) continue;
    toptag = hj;
  }

  // fill input_var, must be same order as in tmva_names
  input_var.push_back(jets.at(0).pt());
  input_var.push_back(jets.size() > 1 ? jets.at(1).pt() : -1);
  input_var.push_back(jets.size() > 2 ? jets.at(2).pt() : -1);
  input_var.push_back(jets.size() > 3 ? jets.at(3).pt() : -1);
  input_var.push_back(jets.size() > 4 ? jets.at(4).pt() : -1);
  input_var.push_back(jets.size() > 5 ? jets.at(5).pt() : -1);
  input_var.push_back(abs(jets.at(0).eta()));
  input_var.push_back(jets.size() > 1 ? abs(jets.at(1).eta()) : -1);
  input_var.push_back(jets.size() > 2 ? abs(jets.at(2).eta()) : -1);
  input_var.push_back(jets.size() > 3 ? abs(jets.at(3).eta()) : -1);
  input_var.push_back(jets.size() > 4 ? abs(jets.at(4).eta()) : -1);
  input_var.push_back(jets.size() > 5 ? abs(jets.at(5).eta()) : -1);
  input_var.push_back(jets.size());
  input_var.push_back(hotvrjets.at(0).pt());
  input_var.push_back(hotvrjets.size() > 1 ? hotvrjets.at(1).pt() : -1);
  input_var.push_back(hotvrjets.size() > 2 ? hotvrjets.at(2).pt() : -1);
  input_var.push_back(hotvrjets.size() > 3 ? hotvrjets.at(3).pt() : -1);
  input_var.push_back(abs(hotvrjets.at(0).eta()));
  input_var.push_back(hotvrjets.size() > 1 ? abs(hotvrjets.at(1).eta()) : -1);
  input_var.push_back(hotvrjets.size() > 2 ? abs(hotvrjets.at(2).eta()) : -1);
  input_var.push_back(hotvrjets.size() > 3 ? abs(hotvrjets.at(3).eta()) : -1);
  input_var.push_back(hotvrjets.size());
  input_var.push_back(TMath::ATan(TMath::Sqrt(jets.at(0).jetArea()/M_PI))); // actually: Use arctan(r) instead of A (A=r**2 pi)
  input_var.push_back(jets.size() > 1 ? TMath::ATan(TMath::Sqrt(jets.at(1).jetArea()/M_PI)) : -1);
  input_var.push_back(jets.size() > 2 ? TMath::ATan(TMath::Sqrt(jets.at(2).jetArea()/M_PI)) : -1);
  input_var.push_back(jets.size() > 3 ? TMath::ATan(TMath::Sqrt(jets.at(3).jetArea()/M_PI)) : -1);
  input_var.push_back(jets.size() > 4 ? TMath::ATan(TMath::Sqrt(jets.at(4).jetArea()/M_PI)) : -1);
  input_var.push_back(jets.size() > 5 ? TMath::ATan(TMath::Sqrt(jets.at(5).jetArea()/M_PI)) : -1);
  input_var.push_back(TMath::ATan(TMath::Sqrt(hotvrjets.at(0).jetArea()/M_PI)));
  input_var.push_back(hotvrjets.size() > 1 ? TMath::ATan(TMath::Sqrt(hotvrjets.at(1).jetArea()/M_PI)) : -1);
  input_var.push_back(hotvrjets.size() > 2 ? TMath::ATan(TMath::Sqrt(hotvrjets.at(2).jetArea()/M_PI)) : -1);
  input_var.push_back(hotvrjets.size() > 3 ? TMath::ATan(TMath::Sqrt(hotvrjets.at(3).jetArea()/M_PI)) : -1);
  input_var.push_back(met.pt());
  input_var.push_back(TMath::Cos(uhh2::deltaPhi(met.v4(), lepton.v4())));
  input_var.push_back(TMath::Cos(uhh2::deltaPhi(met.v4(), (nextJetToMET(event, jets))->v4())));
  input_var.push_back(TMath::Cos(uhh2::deltaPhi(met.v4(), hotvrjets.at(0).v4())));
  input_var.push_back(TMath::Cos(uhh2::deltaPhi(lepton.v4(), (nextJet(lepton, jets)->v4()))));
  input_var.push_back(TMath::Cos(uhh2::deltaPhi(lepton.v4(), hotvrjets.at(0).v4())));
  input_var.push_back(calcMTW(lepton, event));
  input_var.push_back(TMath::ATan(uhh2::deltaR(lepton.v4(), (nextJet(lepton, jets)->v4()))));
  input_var.push_back(TMath::ATan(uhh2::deltaR(lepton.v4(), hotvrjets.at(0).v4())));
  input_var.push_back(abs(lepton.eta()));
  input_var.push_back(lepton.pt());
  input_var.push_back(jets.size() > 1 ? TMath::ATan(uhh2::deltaR(jets.at(0).v4(), (closestParticle(jets.at(0), jets))->v4())) : -1);
  input_var.push_back(jets.size() > 1 ? TMath::Cos(uhh2::deltaPhi(jets.at(0).v4(), (closestParticle(jets.at(0), jets))->v4())) : -1); // careful: -1 also means that deltaPhi == pi
  input_var.push_back(jets.size() > 1 ? TMath::ATan(uhh2::deltaR(jets.at(1).v4(), (closestParticle(jets.at(1), jets))->v4())) : -1);
  input_var.push_back(jets.size() > 1 ? TMath::Cos(uhh2::deltaPhi(jets.at(1).v4(), (closestParticle(jets.at(1), jets))->v4())) : -1); // careful: -1 also means that deltaPhi == pi
  input_var.push_back(w); // event weight
  input_var.push_back(toptag.pt()); // target variable

  for(unsigned int i = 0; i < tmva_names.size(); i++) {
    //event.set(ctx.get_handle<float>(tmva_names.at(i)), input_var.at(i));
    event.set(h_tmva_variables.at(i), input_var.at(i));
  }

  return true;
}


TMVAApplication::TMVAApplication(Context & ctx, const string & tmva_weightfile, const string & handle_name) {

  reader = new TMVA::Reader("Silent");

  int i = 0;

  reader->AddVariable("tmva_jet1_pt", &floatvars[i++]);
  reader->AddVariable("tmva_jet2_pt", &floatvars[i++]);
  reader->AddVariable("tmva_jet1_eta", &floatvars[i++]);
  reader->AddVariable("tmva_jet2_eta", &floatvars[i++]);
  reader->AddVariable("tmva_number_ak4", &floatvars[i++]);
  reader->AddSpectator("tmva_hotvr1_pt", &floatvars[i++]);
  reader->AddVariable("tmva_number_hotvr", &floatvars[i++]);
  reader->AddVariable("tmva_jet1_area", &floatvars[i++]);
  reader->AddVariable("tmva_jet2_area", &floatvars[i++]);
  reader->AddVariable("tmva_met_pt", &floatvars[i++]);
  reader->AddVariable("tmva_deltaPhi_met_lepton", &floatvars[i++]);
  reader->AddVariable("tmva_deltaPhi_met_ak4", &floatvars[i++]);
  reader->AddVariable("tmva_deltaPhi_met_hotvr1", &floatvars[i++]);
  reader->AddVariable("tmva_deltaPhi_lepton_ak4", &floatvars[i++]);
  reader->AddVariable("tmva_deltaPhi_lepton_hotvr1", &floatvars[i++]);
  reader->AddVariable("tmva_mtw", &floatvars[i++]);
  reader->AddVariable("tmva_deltaR_lepton_ak4", &floatvars[i++]);
  reader->AddVariable("tmva_deltaR_lepton_hotvr1", &floatvars[i++]);
  reader->AddVariable("tmva_lepton_eta", &floatvars[i++]);
  reader->AddVariable("tmva_lepton_pt", &floatvars[i++]);
  reader->AddVariable("tmva_deltaR_jet1_ak4", &floatvars[i++]);
  reader->AddVariable("tmva_deltaPhi_jet1_ak4", &floatvars[i++]);
  reader->AddVariable("tmva_deltaR_jet2_ak4", &floatvars[i++]);
  reader->AddVariable("tmva_deltaPhi_jet2_ak4", &floatvars[i++]);
  reader->AddSpectator("tmva_toptag_pt", &floatvars[i++]);

  reader->BookMVA("BDT", tmva_weightfile);

  h_BDTResponse = ctx.get_handle<float>(handle_name);
}


bool TMVAApplication::process(Event & event) {

  int i = 0;

  vector<Jet> jets = *event.jets;
  vector<TopJet> hotvrjets = *event.topjets;
  MET met = *event.met;
  const auto lepton = returnPrimaryLepton(event);

  floatvars[i++] = float(jets.at(0).pt());
  floatvars[i++] = float(jets.at(1).pt());
  floatvars[i++] = float(abs(jets.at(0).eta()));
  floatvars[i++] = float(abs(jets.at(1).eta()));
  floatvars[i++] = float(jets.size());
  floatvars[i++] = float(0); // spectator
  floatvars[i++] = float(hotvrjets.size());
  floatvars[i++] = float(TMath::ATan(TMath::Sqrt(jets.at(0).jetArea()/M_PI)));
  floatvars[i++] = float(TMath::ATan(TMath::Sqrt(jets.at(1).jetArea()/M_PI)));
  floatvars[i++] = float(met.pt());
  floatvars[i++] = float(TMath::Cos(uhh2::deltaPhi(met.v4(), lepton.v4())));
  floatvars[i++] = float(TMath::Cos(uhh2::deltaPhi(met.v4(), (nextJetToMET(event, jets))->v4())));
  floatvars[i++] = float(TMath::Cos(uhh2::deltaPhi(met.v4(), hotvrjets.at(0).v4())));
  floatvars[i++] = float(TMath::Cos(uhh2::deltaPhi(lepton.v4(), (nextJet(lepton, jets)->v4()))));
  floatvars[i++] = float(TMath::Cos(uhh2::deltaPhi(lepton.v4(), hotvrjets.at(0).v4())));
  floatvars[i++] = float(calcMTW(lepton, event));
  floatvars[i++] = float(TMath::ATan(uhh2::deltaR(lepton.v4(), (nextJet(lepton, jets)->v4()))));
  floatvars[i++] = float(TMath::ATan(uhh2::deltaR(lepton.v4(), hotvrjets.at(0).v4())));
  floatvars[i++] = float(abs(lepton.eta()));
  floatvars[i++] = float(lepton.pt());
  floatvars[i++] = float(jets.size() > 1 ? TMath::ATan(uhh2::deltaR(jets.at(0).v4(), (closestParticle(jets.at(0), jets))->v4())) : -1);
  floatvars[i++] = float(jets.size() > 1 ? TMath::Cos(uhh2::deltaPhi(jets.at(0).v4(), (closestParticle(jets.at(0), jets))->v4())) : -1);
  floatvars[i++] = float(jets.size() > 1 ? TMath::ATan(uhh2::deltaR(jets.at(1).v4(), (closestParticle(jets.at(1), jets))->v4())) : -1);
  floatvars[i++] = float(jets.size() > 1 ? TMath::Cos(uhh2::deltaPhi(jets.at(1).v4(), (closestParticle(jets.at(1), jets))->v4())) : -1);
  floatvars[i++] = float(0); // spectator
  
  float response = -100;
  response = reader->EvaluateMVA("BDT");

  event.set(h_BDTResponse, response);

  return true;
}


TMVAApplication::~TMVAApplication() {

  delete reader;
}