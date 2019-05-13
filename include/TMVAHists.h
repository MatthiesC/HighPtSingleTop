#pragma once
#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"


class TMVAHists: public uhh2::Hists {
 public:
  TMVAHists(uhh2::Context & ctx, const std::string & dirname);
  virtual void fill(const uhh2::Event & ev) override;
  
 private:
  uhh2::Event::Handle<float> h_BDTResponse;
  
 protected:
  TH1F *hist_BDT10, *hist_BDT20, *hist_BDT50, *hist_BDT100, *hist_nbjets;
  TH1F *hist_hotvr1_pt_bdt03, *hist_hotvr1_pt_bdt09, *hist_hotvr1_pt_bdt03_100GeVbins, *hist_hotvr1_pt_bdt09_100GeVbins, *hist_hotvr1_mass_bdt03, *hist_hotvr1_mass_bdt09;
  TH1F *hist_1b_hotvr1_pt_bdt03, *hist_1b_hotvr1_pt_bdt09, *hist_1b_hotvr1_pt_bdt03_100GeVbins, *hist_1b_hotvr1_pt_bdt09_100GeVbins, *hist_1b_hotvr1_mass_bdt03, *hist_1b_hotvr1_mass_bdt09;
  TH1F *hist_3b_hotvr1_pt_bdt03, *hist_3b_hotvr1_pt_bdt09, *hist_3b_hotvr1_pt_bdt03_100GeVbins, *hist_3b_hotvr1_pt_bdt09_100GeVbins, *hist_3b_hotvr1_mass_bdt03, *hist_3b_hotvr1_mass_bdt09;
};
