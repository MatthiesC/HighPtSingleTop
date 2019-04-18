#include "UHH2/HighPtSingleTop/include/HighPtSingleTopHists.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"

#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/Utils.h"

#include "TH1F.h"
#include "TH2F.h"
#include <iostream>


using namespace std;
using namespace uhh2;


HighPtSingleTopHists::HighPtSingleTopHists(Context & ctx, const string & dirname): Hists(ctx, dirname) {

  h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");

  jets_number = book<TH1F>("N_jets", "number of AK4 jets", 21, -0.5, 20.5);  
  jet1_eta = book<TH1F>("eta_jet1", "#eta^{jet 1}", 50, -2.5, 2.5);
  jet2_eta = book<TH1F>("eta_jet2", "#eta^{jet 2}", 50, -2.5, 2.5);
  jet3_eta = book<TH1F>("eta_jet3", "#eta^{jet 3}", 50, -2.5, 2.5);
  jet4_eta = book<TH1F>("eta_jet4", "#eta^{jet 4}", 50, -2.5, 2.5);
  jet1_pt = book<TH1F>("pt_jet1", "p_{T}^{jet 1} [GeV]", 100, 0, 1000);
  jet2_pt = book<TH1F>("pt_jet2", "p_{T}^{jet 2} [GeV]", 100, 0, 1000);
  jet3_pt = book<TH1F>("pt_jet3", "p_{T}^{jet 3} [GeV]", 100, 0, 1000);
  jet4_pt = book<TH1F>("pt_jet4", "p_{T}^{jet 4} [GeV]", 100, 0, 1000);

  mtw = book<TH1F>("mtw", "M_{T}(W) [GeV]", 50, 0, 500);
  met = book<TH1F>("met", "p_{T}^{miss} [GeV]", 50, 0, 1000);

  met_vs_mtw = book<TH2F>("met_vs_mtw", "p_{T}^{miss} [GeV] vs. M_{T}(W) [GeV]", 50, 0, 200, 50, 0, 1000);
}


void HighPtSingleTopHists::fill(const Event & event) {

  double weight = event.weight;
  
  const FlavorParticle &lepton = event.get(h_primlep);

  vector<Jet> jets = *event.jets;
  unsigned int Njets = jets.size();

  jets_number->Fill(Njets, weight);
  sort_by_pt<Jet>(jets);
  if(Njets >= 1) {
    jet1_eta->Fill(jets.at(0).eta(), weight);
    jet1_pt->Fill(jets.at(0).pt(), weight);
  }
  if(Njets >= 2) {
    jet2_eta->Fill(jets.at(1).eta(), weight);
    jet2_pt->Fill(jets.at(1).pt(), weight);
  }
  if(Njets >= 3) {
    jet3_eta->Fill(jets.at(2).eta(), weight);
    jet3_pt->Fill(jets.at(2).pt(), weight);
  }
  if(Njets >= 4) {
    jet4_eta->Fill(jets.at(3).eta(), weight);
    jet4_pt->Fill(jets.at(3).pt(), weight);
  }

  auto MTW = calcMTW(lepton, event);
  auto MET = event.met->pt();

  mtw->Fill(MTW, weight);
  met->Fill(MET, weight);
  met_vs_mtw->Fill(MTW, MET, weight);

}

HighPtSingleTopHists::~HighPtSingleTopHists() {}
