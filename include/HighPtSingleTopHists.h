#pragma once

#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Hists.h"

namespace uhh2 {

  class HighPtSingleTopHists: public uhh2::Hists {
  public:
    HighPtSingleTopHists(uhh2::Context & ctx, const std::string & dirname);

    virtual void fill(const uhh2::Event & ev) override;
    virtual ~HighPtSingleTopHists();

  protected:

    uhh2::Event::Handle<FlavorParticle> h_primlep;

    TH1F *deltaR_lepton_nextjet, *deltaR_lepton_leadhotvr, *deltaPhi_lepton_nextjet, *deltaPhi_lepton_leadhotvr, *deltaPhiCos_lepton_nextjet, *deltaPhiCos_lepton_leadhotvr;

    TH1F *mtw, *met;
    TH2F *met_VS_mtw;

    TH1F *deltaPhi_lepton_met, *deltaPhiCos_lepton_met, *deltaPhi_leadhotvr_met, *deltaPhiCos_leadhotvr_met, *deltaPhi_nextjet_met, *deltaPhiCos_nextjet_met;

    TH2F *deltaPhi_lepton_met_VS_met, *deltaPhi_leadhotvr_met_VS_met, *deltaPhi_nextjet_met_VS_met;

    TH1F *jets_number, *jet1_eta, *jet2_eta, *jet3_eta, *jet4_eta, *jet1_pt, *jet2_pt, *jet3_pt, *jet4_pt;
    TH1F *hotvr_number, *hotvr1_eta, *hotvr1_phi, *hotvr2_eta, *hotvr2_phi, *hotvr3_eta, *hotvr3_phi, *hotvr4_eta, *hotvr4_phi, *hotvr1_pt, *hotvr1_pt_lowpt, *hotvr1_pt_highpt, *hotvr2_pt, *hotvr2_pt_lowpt, *hotvr2_pt_highpt, *hotvr3_pt, *hotvr4_pt;
    TH1F *hotvr1_mass, *hotvr1_mass_ca173, *hotvr2_mass, *hotvr3_mass, *hotvr4_mass;
  };

}
