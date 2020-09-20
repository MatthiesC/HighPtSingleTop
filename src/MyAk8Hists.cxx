#include <vector>
#include <iostream>

#include "UHH2/HighPtSingleTop/include/MyAk8Hists.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;


MyAk8Hists::MyAk8Hists(Context & ctx, const string & dirname, const string & ak8_handle_name): Hists(ctx, dirname) {

    h_ak8jets = ctx.get_handle<vector<TopJet>>(ak8_handle_name);
    h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");

    hist_number = book<TH1F>("number", "Number of AK8 jets", 11, -0.5, 10.5);
    hist_drLepton = book<TH1F>("drLepton", "#DeltaR(AK8 jet, lepton)", 100, 0, 5);

    hist_ak8jet1_pt = book<TH1F>("ak8jet1_pt", "Leading AK8 jet p_{T} [GeV]", 100, 0, 1000);
    hist_ak8jet1_pz = book<TH1F>("ak8jet1_pz", "Leading AK8 jet p_{z} [GeV]", 100, 0, 1000);
    hist_ak8jet1_eta = book<TH1F>("ak8jet1_eta", "Leading AK8 jet #eta", 100, -2.5, 2.5);
    hist_ak8jet1_phi = book<TH1F>("ak8jet1_phi", "Leading AK8 jet #phi [rad]", 100, -M_PI, M_PI);
    hist_ak8jet1_mass = book<TH1F>("ak8jet1_mass", "Leading AK8 jet m_{jet} [GeV]", 100, 0, 500);
    hist_ak8jet1_mSD = book<TH1F>("ak8jet1_mSD", "Leading AK8 jet m_{SD} [GeV]", 100, 0, 500);
    hist_ak8jet1_deepWvsQCD = book<TH1F>("ak8jet1_deepWvsQCD", "Leading AK8 jet #it{O}_{DeepAK8}(W vs. QCD)", 100, 0, 1);
    hist_ak8jet1_deepTvsQCD = book<TH1F>("ak8jet1_deepTvsQCD", "Leading AK8 jet #it{O}_{DeepAK8}(t vs. QCD)", 100, 0, 1);

    hist_ak8jet2_pt = book<TH1F>("ak8jet2_pt", "Sub-leading AK8 jet p_{T} [GeV]", 100, 0, 1000);
    hist_ak8jet2_pz = book<TH1F>("ak8jet2_pz", "Sub-leading AK8 jet p_{z} [GeV]", 100, 0, 1000);
    hist_ak8jet2_eta = book<TH1F>("ak8jet2_eta", "Sub-leading AK8 jet #eta", 100, -2.5, 2.5);
    hist_ak8jet2_phi = book<TH1F>("ak8jet2_phi", "Sub-leading AK8 jet #phi [rad]", 100, -M_PI, M_PI);
    hist_ak8jet2_mass = book<TH1F>("ak8jet2_mass", "Sub-leading AK8 jet m_{jet} [GeV]", 100, 0, 500);
    hist_ak8jet2_mSD = book<TH1F>("ak8jet2_mSD", "Sub-leading AK8 jet m_{SD} [GeV]", 100, 0, 500);
    hist_ak8jet2_deepWvsQCD = book<TH1F>("ak8jet2_deepWvsQCD", "Sub-leading AK8 jet #it{O}_{DeepAK8}(W vs. QCD)", 100, 0, 1);
    hist_ak8jet2_deepTvsQCD = book<TH1F>("ak8jet2_deepTvsQCD", "Sub-leading AK8 jet #it{O}_{DeepAK8}(t vs. QCD)", 100, 0, 1);
}


void MyAk8Hists::fill(const Event & event) {

    const double w = event.weight;

    const vector<TopJet> ak8jets = event.get(h_ak8jets);
    const FlavorParticle primlep = event.get(h_primlep);

    hist_number->Fill(ak8jets.size(), w);

    for(const auto & j : ak8jets) {
        hist_drLepton->Fill(deltaR(j.v4(), primlep.v4()), w);
    }

    if(ak8jets.size() >= 1) {
        hist_ak8jet1_pt->Fill(ak8jets.at(0).v4().Pt(), w);
        hist_ak8jet1_pz->Fill(ak8jets.at(0).v4().Pz(), w);
        hist_ak8jet1_eta->Fill(ak8jets.at(0).v4().Eta(), w);
        hist_ak8jet1_phi->Fill(ak8jets.at(0).v4().Phi(), w);
        hist_ak8jet1_mass->Fill(ak8jets.at(0).v4().M(), w);
        hist_ak8jet1_mSD->Fill(ak8jets.at(0).softdropmass(), w);
        hist_ak8jet1_deepWvsQCD->Fill(ak8jets.at(0).btag_DeepBoosted_WvsQCD(), w);
        hist_ak8jet1_deepTvsQCD->Fill(ak8jets.at(0).btag_DeepBoosted_TvsQCD(), w);
    }
    if(ak8jets.size() >= 2) {
        hist_ak8jet2_pt->Fill(ak8jets.at(1).v4().Pt(), w);
        hist_ak8jet2_pz->Fill(ak8jets.at(1).v4().Pz(), w);
        hist_ak8jet2_eta->Fill(ak8jets.at(1).v4().Eta(), w);
        hist_ak8jet2_phi->Fill(ak8jets.at(1).v4().Phi(), w);
        hist_ak8jet2_mass->Fill(ak8jets.at(1).v4().M(), w);
        hist_ak8jet2_mSD->Fill(ak8jets.at(1).softdropmass(), w);
        hist_ak8jet2_deepWvsQCD->Fill(ak8jets.at(1).btag_DeepBoosted_WvsQCD(), w);
        hist_ak8jet2_deepTvsQCD->Fill(ak8jets.at(1).btag_DeepBoosted_TvsQCD(), w);
    }
}
