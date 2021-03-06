#pragma once

#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Hists.h"

#include "UHH2/common/include/Utils.h"

namespace uhh2 {


  class RegionHist: public uhh2::Hists {
  public:
    RegionHist(uhh2::Context & ctx, const std::string & dirname);

    virtual void fill(const uhh2::Event & ev) override;

  private:
    uhh2::Event::Handle<int> h_which_region;
    Year m_year;
    int m_year_int;
    int m_channel_int;

  protected:
    TH1F *hist_regions;
    TH1F *hist_regions_per_year_and_channel;
    TH1F *hist_regions_per_year;
  };


  class HighPtSingleTopHists: public uhh2::Hists {
  public:
    HighPtSingleTopHists(uhh2::Context & ctx, const std::string & dirname);

    virtual void fill(const uhh2::Event & ev) override;
    virtual ~HighPtSingleTopHists();

    // private:
    //uhh2::Event::Handle<FlavorParticle> h_primlep;

  protected:
    TH1F *deltaR_lepton_nextjet, *deltaR_lepton_leadhotvr, *deltaPhi_lepton_nextjet, *deltaPhi_lepton_leadhotvr, *deltaPhiCos_lepton_nextjet, *deltaPhiCos_lepton_leadhotvr;

    TH1F *mtw, *met;
    TH2F *met_VS_mtw;

    TH1F *deltaPhi_lepton_met, *deltaPhiCos_lepton_met, *deltaPhi_leadhotvr_met, *deltaPhiCos_leadhotvr_met, *deltaPhi_nextjet_met, *deltaPhiCos_nextjet_met;

    TH2F *deltaPhi_lepton_met_VS_met, *deltaPhi_leadhotvr_met_VS_met, *deltaPhi_nextjet_met_VS_met;

    TH1F *jets_number, *jets_Nbtags_loose, *jets_Nbtags_medium, *jets_Nbtags_tight, *jet1_eta, *jet2_eta, *jet3_eta, *jet4_eta, *jet1_pt, *jet2_pt, *jet3_pt, *jet4_pt;
    TH1F *jet1_deepjet, *jet2_deepjet, *jet3_deepjet, *jet4_deepjet, *jet1_deltaRlepton, *jet2_deltaRlepton, *jet3_deltaRlepton, *jet4_deltaRlepton;
    TH1F *hotvr_number, *hotvr1_eta, *hotvr1_phi, *hotvr2_eta, *hotvr2_phi, *hotvr3_eta, *hotvr3_phi, *hotvr4_eta, *hotvr4_phi, *hotvr1_pt, *hotvr1_pt_lowpt, *hotvr1_pt_highpt, *hotvr2_pt, *hotvr2_pt_lowpt, *hotvr2_pt_highpt, *hotvr3_pt, *hotvr4_pt;
    TH1F *hotvr1_mass, *hotvr1_mass_ca173, *hotvr2_mass, *hotvr3_mass, *hotvr4_mass;
  };

}
