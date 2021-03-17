#pragma once
#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"


class MyAk8Hists: public uhh2::Hists {
public:
    MyAk8Hists(uhh2::Context & ctx, const std::string & dirname, const std::string & ak8_handle_name="Ak8Jets");

    virtual void fill(const uhh2::Event & event) override;

protected:
    TH1F *hist_number;
    TH1F *hist_drLepton;

    TH1F *hist_ak8jet1_pt;
    TH1F *hist_ak8jet1_pz;
    TH1F *hist_ak8jet1_eta;
    TH1F *hist_ak8jet1_phi;
    TH1F *hist_ak8jet1_mass;
    TH1F *hist_ak8jet1_mSD;
    TH1F *hist_ak8jet1_deepWvsQCD;
    TH1F *hist_ak8jet1_deepTvsQCD;

    TH1F *hist_ak8jet2_pt;
    TH1F *hist_ak8jet2_pz;
    TH1F *hist_ak8jet2_eta;
    TH1F *hist_ak8jet2_phi;
    TH1F *hist_ak8jet2_mass;
    TH1F *hist_ak8jet2_mSD;
    TH1F *hist_ak8jet2_deepWvsQCD;
    TH1F *hist_ak8jet2_deepTvsQCD;

    TH2F *hist_n_ak8_vs_n_hotvr;

private:
    uhh2::Event::Handle<std::vector<TopJet>> h_ak8jets;
    uhh2::Event::Handle<FlavorParticle> h_primlep;
};
