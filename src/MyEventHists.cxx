#include <vector>
#include <iostream>

#include "UHH2/HighPtSingleTop/include/MyEventHists.h"

#include "UHH2/core/include/Jet.h"
#include "UHH2/core/include/Muon.h"
#include "UHH2/core/include/Electron.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;

MyEventHists::MyEventHists(uhh2::Context & ctx, const std::string & dirname): Hists(ctx, dirname){

    N_PrimVertices = book<TH1F>("N_PrimaryVertices", "number of primary vertices", 51, -0.5, 50.5);
    N_TrueInteractions = book<TH1F>("N_TrueInteractions", "number of true interactions", 51, -0.5, 50.5);
    Weights = book<TH1F>("Weights", "weights", 60, -1, 2);
    MET = book<TH1F>("MET", "p_{T}^{miss}", 200, 0, 2000);
    MET_phi = book<TH1F>("MET_phi", "#phi(MET)", 100, -M_PI, M_PI);
    HThad = book<TH1F>("HT", "H_{T}^{jets}", 200, 0, 4000);
    HTlep = book<TH1F>("HTLep", "H_{T}^{lept}", 200, 0, 2000);
    ST = book<TH1F>("ST", "S_{T}", 200, 0, 5000);
}


void MyEventHists::fill(const uhh2::Event & e){

    assert(e.met && e.pvs);

    N_PrimVertices->Fill(e.pvs->size(), e.weight);
    if(e.genInfo) N_TrueInteractions->Fill(e.genInfo->pileup_TrueNumInteractions(), e.weight);
    Weights->Fill(e.weight);
    MET->Fill(e.met->pt(), e.weight);
    MET_phi->Fill(e.met->phi(), e.weight);

    vector<Jet> jets = *e.jets;
    vector<Muon> muons = *e.muons;
    vector<Electron> electrons = *e.electrons;

    double ht_had = 0;
    double ht_lep = 0;

    for(auto j : jets) {
      ht_had += j.pt();
    }
    for(auto l : electrons) {
      ht_lep += l.pt();
    }
    for(auto l : muons) {
      ht_lep += l.pt();
    }
    ht_lep += e.met->pt();

    HThad->Fill(ht_had, e.weight);
    HTlep->Fill(ht_lep, e.weight);
    ST->Fill(ht_had + ht_lep, e.weight);
}
