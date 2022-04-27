#pragma once

#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/LegacyTopTagging/include/Constants.h"
#include "UHH2/LegacyTopTagging/include/Utils.h"

#include "UHH2/HighPtSingleTop/include/Utils.h"


namespace uhh2 { namespace btw {

class LeptonicHemisphereHists: public uhh2::Hists {
public:
  LeptonicHemisphereHists(uhh2::Context & ctx, const std::string & dirname, const unsigned int default_nbins = 100);
  virtual void fill(const uhh2::Event & event) override;

protected:
  TH1F *hist_top_mass;
  TH1F *hist_mtw;

  TH1F *hist_top_pt;
  TH1F *hist_w_pt;

  TH1F *hist_top_eta;
  TH1F *hist_w_eta;

  TH1F *hist_neutrino_pt;
  TH1F *hist_neutrino_pz;

  TH1F *hist_neutrino_eta;
  TH1F *hist_neutrino_phi;

  TH1F *hist_lepton_pt;
  TH1F *hist_dr_leptonjet;

  TH1F *hist_jet_pt;
  TH1F *hist_dr_max;

  TH1F *hist_jet_deepjet;
  TH1F *hist_dr_puppi_chs;

  TH1F *hist_method;
  TH1F *hist_nsolutions;

private:
  // const uhh2::Event::Handle<uhh2::btw::ERegion_heavyTags> fHandle_Region_heavyTags;
  const uhh2::Event::Handle<uhh2::btw::LeptonicHemisphere> fHandle_LeptonicHemisphere;
};

}}
