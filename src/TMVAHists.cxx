#include <vector>
#include <iostream>

#include "UHH2/HighPtSingleTop/include/TMVAHists.h"

#include "UHH2/core/include/Jet.h"
#include "UHH2/core/include/Muon.h"
#include "UHH2/core/include/Electron.h"

#include "UHH2/common/include/JetIds.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;

TMVAHists::TMVAHists(uhh2::Context & ctx, const std::string & dirname): Hists(ctx, dirname){

  h_BDTResponse = ctx.get_handle<float>("BDTResponse");

  hist_BDT10 = book<TH1F>("BDTresponse10", "BDT output", 10, -1, 1);
  hist_BDT20 = book<TH1F>("BDTresponse20", "BDT output", 20, -1, 1);
  hist_BDT50 = book<TH1F>("BDTresponse50", "BDT output", 50, -1, 1);
  hist_BDT100 = book<TH1F>("BDTresponse100", "BDT output", 100, -1, 1);

  hist_nbjets = book<TH1F>("Nbjets", "number of b-tagged AK4 jets", 6, -0.5, 5.5);

  hist_hotvr1_pt_bdt03 = book<TH1F>("hotvr1_pt_bdt03", "p_{T}(leading HOTVR jet, BDT output > 0.3)", 50, 0, 1000);
  hist_hotvr1_pt_bdt03_100GeVbins = book<TH1F>("hotvr1_pt_bdt03_100GeVbins", "p_{T}(leading HOTVR jet, BDT output > 0.3)", 10, 0, 1000);
  hist_hotvr1_mass_bdt03 = book<TH1F>("hotvr1_mass_bdt03", "m_{jet}(leading HOTVR jet, BDT output > 0.3)", 40, 140, 220);
  hist_hotvr1_pt_bdt09 = book<TH1F>("hotvr1_pt_bdt09", "p_{T}(leading HOTVR jet, BDT output > 0.9)", 50, 0, 1000);
  hist_hotvr1_pt_bdt09_100GeVbins = book<TH1F>("hotvr1_pt_bdt09_100GeVbins", "p_{T}(leading HOTVR jet, BDT output > 0.9)", 10, 0, 1000);
  hist_hotvr1_mass_bdt09 = book<TH1F>("hotvr1_mass_bdt09", "m_{jet}(leading HOTVR jet, BDT output > 0.9)", 40, 140, 220);

  hist_1b_hotvr1_pt_bdt03 = book<TH1F>("1b_hotvr1_pt_bdt03", "p_{T}(leading HOTVR jet, BDT output > 0.3, 1/2 b-tags)", 50, 0, 1000);
  hist_1b_hotvr1_pt_bdt03_100GeVbins = book<TH1F>("1b_hotvr1_pt_bdt03_100GeVbins", "p_{T}(leading HOTVR jet, BDT output > 0.3, 1/2 b-tags)", 10, 0, 1000);
  hist_1b_hotvr1_mass_bdt03 = book<TH1F>("1b_hotvr1_mass_bdt03", "m_{jet}(leading HOTVR jet, BDT output > 0.3, 1/2 b-tags)", 40, 140, 220);
  hist_1b_hotvr1_pt_bdt09 = book<TH1F>("1b_hotvr1_pt_bdt09", "p_{T}(leading HOTVR jet, BDT output > 0.9, 1/2 b-tags)", 50, 0, 1000);
  hist_1b_hotvr1_pt_bdt09_100GeVbins = book<TH1F>("1b_hotvr1_pt_bdt09_100GeVbins", "p_{T}(leading HOTVR jet, BDT output > 0.9, 1/2 b-tags)", 10, 0, 1000);
  hist_1b_hotvr1_mass_bdt09 = book<TH1F>("1b_hotvr1_mass_bdt09", "m_{jet}(leading HOTVR jet, BDT output > 0.9, 1/2 b-tags)", 40, 140, 220);

  hist_3b_hotvr1_pt_bdt03 = book<TH1F>("3b_hotvr1_pt_bdt03", "p_{T}(leading HOTVR jet, BDT output > 0.3, 3 b-tags)", 50, 0, 1000);
  hist_3b_hotvr1_pt_bdt03_100GeVbins = book<TH1F>("3b_hotvr1_pt_bdt03_100GeVbins", "p_{T}(leading HOTVR jet, BDT output > 0.3, 3 b-tags)", 10, 0, 1000);
  hist_3b_hotvr1_mass_bdt03 = book<TH1F>("3b_hotvr1_mass_bdt03", "m_{jet}(leading HOTVR jet, BDT output > 0.3, 3 b-tags)", 40, 140, 220);
  hist_3b_hotvr1_pt_bdt09 = book<TH1F>("3b_hotvr1_pt_bdt09", "p_{T}(leading HOTVR jet, BDT output > 0.9, 3 b-tags)", 50, 0, 1000);
  hist_3b_hotvr1_pt_bdt09_100GeVbins = book<TH1F>("3b_hotvr1_pt_bdt09_100GeVbins", "p_{T}(leading HOTVR jet, BDT output > 0.9, 3 b-tags)", 10, 0, 1000);
  hist_3b_hotvr1_mass_bdt09 = book<TH1F>("3b_hotvr1_mass_bdt09", "m_{jet}(leading HOTVR jet, BDT output > 0.9, 3 b-tags)", 40, 140, 220);
}


void TMVAHists::fill(const uhh2::Event & event){

  float BDTResponse = event.get(h_BDTResponse);

  vector<Jet> jets = *event.jets;

  float hotvr1_pt = (*event.topjets).at(0).v4().pt();
  float hotvr1_mass = (*event.topjets).at(0).v4().M();

  DeepCSVBTag bjetID(DeepCSVBTag::WP_MEDIUM);
  int n_bjets = 0;
  for(Jet j : jets) {
    if(bjetID(j,event)) ++n_bjets;
  }

  hist_BDT10->Fill(BDTResponse, event.weight);
  hist_BDT20->Fill(BDTResponse, event.weight);
  hist_BDT50->Fill(BDTResponse, event.weight);
  hist_BDT100->Fill(BDTResponse, event.weight);
  hist_nbjets->Fill(n_bjets, event.weight);

  if(BDTResponse > 0.3) {
    hist_hotvr1_pt_bdt03->Fill(hotvr1_pt, event.weight);
    hist_hotvr1_pt_bdt03_100GeVbins->Fill(hotvr1_pt, event.weight);
    hist_hotvr1_mass_bdt03->Fill(hotvr1_mass, event.weight);
  }
  if(BDTResponse > 0.9) {
    hist_hotvr1_pt_bdt09->Fill(hotvr1_pt, event.weight);
    hist_hotvr1_pt_bdt09_100GeVbins->Fill(hotvr1_pt, event.weight);
    hist_hotvr1_mass_bdt09->Fill(hotvr1_mass, event.weight);
  }

  if(n_bjets == 1 || n_bjets == 2) {
    if(BDTResponse > 0.3) {
      hist_1b_hotvr1_pt_bdt03->Fill(hotvr1_pt, event.weight);
      hist_1b_hotvr1_pt_bdt03_100GeVbins->Fill(hotvr1_pt, event.weight);
      hist_1b_hotvr1_mass_bdt03->Fill(hotvr1_mass, event.weight);
    }
    if(BDTResponse > 0.9) {
      hist_1b_hotvr1_pt_bdt09->Fill(hotvr1_pt, event.weight);
      hist_1b_hotvr1_pt_bdt09_100GeVbins->Fill(hotvr1_pt, event.weight);
      hist_1b_hotvr1_mass_bdt09->Fill(hotvr1_mass, event.weight);
    }
  }
  else if(n_bjets >= 3) {
    if(BDTResponse > 0.3) {
      hist_3b_hotvr1_pt_bdt03->Fill(hotvr1_pt, event.weight);
      hist_3b_hotvr1_pt_bdt03_100GeVbins->Fill(hotvr1_pt, event.weight);
      hist_3b_hotvr1_mass_bdt03->Fill(hotvr1_mass, event.weight);
    }
    if(BDTResponse > 0.9) {
      hist_3b_hotvr1_pt_bdt09->Fill(hotvr1_pt, event.weight);
      hist_3b_hotvr1_pt_bdt09_100GeVbins->Fill(hotvr1_pt, event.weight);
      hist_3b_hotvr1_mass_bdt09->Fill(hotvr1_mass, event.weight);
    }
  }
}
