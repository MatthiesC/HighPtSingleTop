#pragma once

#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/LegacyTopTagging/include/Constants.h"
#include "UHH2/LegacyTopTagging/include/SingleTopGen_tWch.h"
#include "UHH2/LegacyTopTagging/include/Utils.h"

#include "UHH2/HighPtSingleTop/include/Utils.h"


namespace uhh2 { namespace btw {

class MatchingHists: public uhh2::Hists {
public:
  MatchingHists(uhh2::Context & ctx, const std::string & dirname, const unsigned int default_nbins = 100);

  virtual void fill(const uhh2::Event & event) override;

protected:
  TH1F *hist_decayChannel;

  //____________________________________________________________________________________________________
  // Leptonic W boson, lepton, neutrino

  TH2F* hist_parton_wlep_dr_vs_pt_rec;
  TH2F* hist_parton_wlep_pt_gen_vs_pt_rec;

  TH1F* hist_parton_pt_wlep_gen;
  TH1F* hist_parton_pt_wlep_rec;
  TH1F* hist_parton_pt_wlep_rec_over_gen_vs_gen;
  TH1F* hist_parton_pt_wlep_rec_over_gen_vs_rec;
  TH1F* hist_parton_pz_wlep_rec_over_gen_vs_gen;
  TH1F* hist_parton_pz_wlep_rec_over_gen_vs_rec;

  TH2F* hist_parton_lepton_dr_vs_pt_rec;
  TH2F* hist_parton_lepton_pt_gen_vs_pt_rec;

  TH1F* hist_parton_pt_lepton_gen;
  TH1F* hist_parton_pt_lepton_rec;
  TH1F* hist_parton_pt_lepton_rec_over_gen_vs_gen;
  TH1F* hist_parton_pt_lepton_rec_over_gen_vs_rec;
  TH1F* hist_parton_pz_lepton_rec_over_gen_vs_gen;
  TH1F* hist_parton_pz_lepton_rec_over_gen_vs_rec;

  TH2F* hist_parton_neutrino_dr_vs_pt_rec;
  TH2F* hist_parton_neutrino_pt_gen_vs_pt_rec;

  TH1F* hist_parton_pt_neutrino_gen;
  TH1F* hist_parton_pt_neutrino_rec;
  TH1F* hist_parton_pt_neutrino_rec_over_gen_vs_gen;
  TH1F* hist_parton_pt_neutrino_rec_over_gen_vs_rec;
  TH1F* hist_parton_pz_neutrino_rec_over_gen_vs_gen;
  TH1F* hist_parton_pz_neutrino_rec_over_gen_vs_rec;

  //____________________________________________________________________________________________________
  // Hadronic top quark, leptonic W boson (not the same as above!)

  TH2F* hist_1t_parton_top_dr_vs_pt_rec;
  TH2F* hist_1t_parton_top_pt_gen_vs_pt_rec;

  TH1F* hist_1t_parton_pt_top_gen;
  TH1F* hist_1t_parton_pt_top_rec;
  TH1F* hist_1t_parton_pt_top_rec_over_gen_vs_gen;
  TH1F* hist_1t_parton_pt_top_rec_over_gen_vs_rec;
  TH1F* hist_1t_parton_pz_top_rec_over_gen_vs_gen;
  TH1F* hist_1t_parton_pz_top_rec_over_gen_vs_rec;

  TH2F* hist_1t_parton_wass_dr_vs_pt_rec;
  TH2F* hist_1t_parton_wass_pt_gen_vs_pt_rec;

  TH1F* hist_1t_parton_pt_wass_gen;
  TH1F* hist_1t_parton_pt_wass_rec;
  TH1F* hist_1t_parton_pt_wass_rec_over_gen_vs_gen;
  TH1F* hist_1t_parton_pt_wass_rec_over_gen_vs_rec;
  TH1F* hist_1t_parton_pz_wass_rec_over_gen_vs_gen;
  TH1F* hist_1t_parton_pz_wass_rec_over_gen_vs_rec;

  TH2F* hist_1t_parton_tw_dr_vs_pt_rec;
  TH2F* hist_1t_parton_tw_pt_gen_vs_pt_rec;

  TH1F* hist_1t_parton_pt_tw_gen;
  TH1F* hist_1t_parton_pt_tw_rec;
  TH1F* hist_1t_parton_pt_tw_rec_over_gen_vs_gen;
  TH1F* hist_1t_parton_pt_tw_rec_over_gen_vs_rec;
  TH1F* hist_1t_parton_pz_tw_rec_over_gen_vs_gen;
  TH1F* hist_1t_parton_pz_tw_rec_over_gen_vs_rec;

  TH2F* hist_1t_parton_tw_mass_gen_vs_mass_rec;

  TH1F* hist_1t_parton_mass_tw_gen;
  TH1F* hist_1t_parton_mass_tw_rec;
  TH1F* hist_1t_parton_mass_tw_rec_over_gen_vs_gen;
  TH1F* hist_1t_parton_mass_tw_rec_over_gen_vs_rec;

  //____________________________________________________________________________________________________
  // Leptonic top quark, hadronic W boson

  TH2F* hist_0t1W_parton_top_dr_vs_pt_rec;
  TH2F* hist_0t1W_parton_top_pt_gen_vs_pt_rec;

  TH1F* hist_0t1W_parton_pt_top_gen;
  TH1F* hist_0t1W_parton_pt_top_rec;
  TH1F* hist_0t1W_parton_pt_top_rec_over_gen_vs_gen;
  TH1F* hist_0t1W_parton_pt_top_rec_over_gen_vs_rec;
  TH1F* hist_0t1W_parton_pz_top_rec_over_gen_vs_gen;
  TH1F* hist_0t1W_parton_pz_top_rec_over_gen_vs_rec;

  TH2F* hist_0t1W_parton_wass_dr_vs_pt_rec;
  TH2F* hist_0t1W_parton_wass_pt_gen_vs_pt_rec;

  TH1F* hist_0t1W_parton_pt_wass_gen;
  TH1F* hist_0t1W_parton_pt_wass_rec;
  TH1F* hist_0t1W_parton_pt_wass_rec_over_gen_vs_gen;
  TH1F* hist_0t1W_parton_pt_wass_rec_over_gen_vs_rec;
  TH1F* hist_0t1W_parton_pz_wass_rec_over_gen_vs_gen;
  TH1F* hist_0t1W_parton_pz_wass_rec_over_gen_vs_rec;

  TH2F* hist_0t1W_parton_tw_dr_vs_pt_rec;
  TH2F* hist_0t1W_parton_tw_pt_gen_vs_pt_rec;

  TH1F* hist_0t1W_parton_pt_tw_gen;
  TH1F* hist_0t1W_parton_pt_tw_rec;
  TH1F* hist_0t1W_parton_pt_tw_rec_over_gen_vs_gen;
  TH1F* hist_0t1W_parton_pt_tw_rec_over_gen_vs_rec;
  TH1F* hist_0t1W_parton_pz_tw_rec_over_gen_vs_gen;
  TH1F* hist_0t1W_parton_pz_tw_rec_over_gen_vs_rec;

  TH2F* hist_0t1W_parton_tw_mass_gen_vs_mass_rec;

  TH1F* hist_0t1W_parton_mass_tw_gen;
  TH1F* hist_0t1W_parton_mass_tw_rec;
  TH1F* hist_0t1W_parton_mass_tw_rec_over_gen_vs_gen;
  TH1F* hist_0t1W_parton_mass_tw_rec_over_gen_vs_rec;

private:
  const uhh2::Event::Handle<ltt::SingleTopGen_tWch> fHandle_GENtW;
  const uhh2::Event::Handle<uhh2::btw::ERegion_heavyTags> fHandle_Region_heavyTags;

  const uhh2::Event::Handle<TopJet> fHandle_TheTopJet;
  const uhh2::Event::Handle<TopJet> fHandle_TheWJet;
  // const uhh2::Event::Handle<FlavorParticle> fHandle_PrimaryLepton;
  const uhh2::Event::Handle<uhh2::btw::LeptonicHemisphere> fHandle_LeptonicHemisphere;

  const uhh2::Event::Handle<GenLevelDef> fHandle_Parton_Top;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Parton_WAss;
  // const uhh2::Event::Handle<GenLevelDef> fHandle_Parton_WTop;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Parton_tW_system;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Parton_LeptonicW;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Parton_SingleLepton;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Parton_SingleNeutrino;
};

}}
