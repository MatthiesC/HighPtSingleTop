#include "UHH2/common/include/Utils.h"

#include "UHH2/HighPtSingleTop/include/MatchingHists.h"

#include "TH1F.h"
#include "TH2F.h"

using namespace std;
using namespace uhh2;
using namespace uhh2::btw;
using namespace uhh2::ltt;

namespace uhh2 { namespace btw {

MatchingHists::MatchingHists(Context & ctx, const string & dirname, const unsigned int default_nbins):
  Hists(ctx, dirname),
  fHandle_GENtW(ctx.get_handle<ltt::SingleTopGen_tWch>(kHandleName_SingleTopGen_tWch)),
  fHandle_Region_heavyTags(ctx.get_handle<ERegion_heavyTags>(kHandleName_Region_heavyTags)),

  fHandle_TheTopJet(ctx.get_handle<TopJet>(kHandleName_TheTopJet)),
  fHandle_TheWJet(ctx.get_handle<TopJet>(kHandleName_TheWJet)),
  // fHandle_PrimaryLepton(ctx.get_handle<FlavorParticle>(kHandleName_PrimaryLepton)),
  fHandle_LeptonicHemisphere(ctx.get_handle<LeptonicHemisphere>(kHandleName_LeptonicHemisphere)),

  fHandle_Parton_Top(ctx.get_handle<GenLevelDef>(kHandleName_Parton_Top)),
  fHandle_Parton_WAss(ctx.get_handle<GenLevelDef>(kHandleName_Parton_WAss)),
  // fHandle_Parton_WTop(ctx.get_handle<GenLevelDef>(kHandleName_Parton_WTop)),
  fHandle_Parton_tW_system(ctx.get_handle<GenLevelDef>(kHandleName_Parton_tW_system)),
  fHandle_Parton_LeptonicW(ctx.get_handle<GenLevelDef>(kHandleName_Parton_LeptonicW)),
  fHandle_Parton_SingleLepton(ctx.get_handle<GenLevelDef>(kHandleName_Parton_SingleLepton)),
  fHandle_Parton_SingleNeutrino(ctx.get_handle<GenLevelDef>(kHandleName_Parton_SingleNeutrino))
{
  (void) default_nbins;

  const int nx = 16;
  // same order as enums in SingleTopGen_tWch class
  const char *decayChannels[nx] = {"t_{e}W_{e}", "t_{#mu}W_{e}", "t_{#tau}W_{e}", "t_{h}W_{e}", "t_{e}W_{#mu}", "t_{#mu}W_{#mu}", "t_{#tau}W_{#mu}", "t_{h}W_{#mu}", "t_{e}W_{#tau}", "t_{#mu}W_{#tau}", "t_{#tau}W_{#tau}", "t_{h}W_{#tau}", "t_{e}W_{h}", "t_{#mu}W_{h}", "t_{#tau}W_{h}", "t_{h}W_{h}"};
  hist_decayChannel = book<TH1F>("decayChannel", "tW decay channel", nx, -0.5, 15.5);
  for(int i = 1; i <= nx; ++i) hist_decayChannel->GetXaxis()->SetBinLabel(i, decayChannels[i-1]);

  const int n_bins = 2000;
  const float x_max = 2000.f;

  //____________________________________________________________________________________________________
  // Leptonic W boson, lepton, neutrino

  hist_parton_wlep_dr_vs_pt_rec = book<TH2F>("parton_wlep_dr_vs_pt_rec", "#Delta#it{R}(partonic W_{lep}, reco W_{lep})", 20, 0, 1000, 50, 0, 5);
  hist_parton_wlep_pt_gen_vs_pt_rec = book<TH2F>("parton_wlep_pt_gen_vs_pt_rec", "partonic W_{lep} #it{p}_{T} [GeV] VS. reco W_{lep} #it{p}_{T} [GeV]", 20, 0, 1000, 20, 0, 1000);

  hist_parton_pt_wlep_gen = book<TH1F>("parton_pt_wlep_gen", "partonic W_{lep}: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_parton_pt_wlep_rec = book<TH1F>("parton_pt_wlep_rec", "reco W_{lep}: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_parton_pt_wlep_rec_over_gen_vs_gen = book<TH1F>("parton_pt_wlep_rec_over_gen_vs_gen", "partonic W_{lep}: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_parton_pt_wlep_rec_over_gen_vs_rec = book<TH1F>("parton_pt_wlep_rec_over_gen_vs_rec", "reco W_{lep}: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_parton_pz_wlep_rec_over_gen_vs_gen = book<TH1F>("parton_pz_wlep_rec_over_gen_vs_gen", "partonic W_{lep}: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_parton_pz_wlep_rec_over_gen_vs_rec = book<TH1F>("parton_pz_wlep_rec_over_gen_vs_rec", "reco W_{lep}: #it{p}_{T} [GeV]", n_bins, 0, x_max);

  hist_parton_lepton_dr_vs_pt_rec = book<TH2F>("parton_lepton_dr_vs_pt_rec", "#Delta#it{R}(partonic #ell, reco #ell)", 20, 0, 1000, 50, 0, 5);
  hist_parton_lepton_pt_gen_vs_pt_rec = book<TH2F>("parton_lepton_pt_gen_vs_pt_rec", "partonic #ell #it{p}_{T} [GeV] VS. reco #ell #it{p}_{T} [GeV]", 20, 0, 1000, 20, 0, 1000);

  hist_parton_pt_lepton_gen = book<TH1F>("parton_pt_lepton_gen", "partonic #ell: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_parton_pt_lepton_rec = book<TH1F>("parton_pt_lepton_rec", "reco #ell: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_parton_pt_lepton_rec_over_gen_vs_gen = book<TH1F>("parton_pt_lepton_rec_over_gen_vs_gen", "partonic #ell: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_parton_pt_lepton_rec_over_gen_vs_rec = book<TH1F>("parton_pt_lepton_rec_over_gen_vs_rec", "reco #ell: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_parton_pz_lepton_rec_over_gen_vs_gen = book<TH1F>("parton_pz_lepton_rec_over_gen_vs_gen", "partonic #ell: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_parton_pz_lepton_rec_over_gen_vs_rec = book<TH1F>("parton_pz_lepton_rec_over_gen_vs_rec", "reco #ell: #it{p}_{T} [GeV]", n_bins, 0, x_max);

  hist_parton_neutrino_dr_vs_pt_rec = book<TH2F>("parton_neutrino_dr_vs_pt_rec", "#Delta#it{R}(partonic #nu, reco #nu)", 20, 0, 1000, 50, 0, 5);
  hist_parton_neutrino_pt_gen_vs_pt_rec = book<TH2F>("parton_neutrino_pt_gen_vs_pt_rec", "partonic #nu #it{p}_{T} [GeV] VS. reco #nu #it{p}_{T} [GeV]", 20, 0, 1000, 20, 0, 1000);

  hist_parton_pt_neutrino_gen = book<TH1F>("parton_pt_neutrino_gen", "partonic #nu: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_parton_pt_neutrino_rec = book<TH1F>("parton_pt_neutrino_rec", "reco #nu: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_parton_pt_neutrino_rec_over_gen_vs_gen = book<TH1F>("parton_pt_neutrino_rec_over_gen_vs_gen", "partonic #nu: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_parton_pt_neutrino_rec_over_gen_vs_rec = book<TH1F>("parton_pt_neutrino_rec_over_gen_vs_rec", "reco #nu: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_parton_pz_neutrino_rec_over_gen_vs_gen = book<TH1F>("parton_pz_neutrino_rec_over_gen_vs_gen", "partonic #nu: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_parton_pz_neutrino_rec_over_gen_vs_rec = book<TH1F>("parton_pz_neutrino_rec_over_gen_vs_rec", "reco #nu: #it{p}_{T} [GeV]", n_bins, 0, x_max);

  //____________________________________________________________________________________________________
  // Hadronic top quark, leptonic W boson (not the same as above!)

  hist_1t_parton_top_dr_vs_pt_rec = book<TH2F>("1t_parton_top_dr_vs_pt_rec", "#Delta#it{R}(partonic top, reco top)", 20, 0, 1000, 50, 0, 5);
  hist_1t_parton_top_pt_gen_vs_pt_rec = book<TH2F>("1t_parton_top_pt_gen_vs_pt_rec", "partonic top #it{p}_{T} [GeV] VS. reco top #it{p}_{T} [GeV]", 20, 0, 1000, 20, 0, 1000);

  hist_1t_parton_pt_top_gen = book<TH1F>("1t_parton_pt_top_gen", "partonic top: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_1t_parton_pt_top_rec = book<TH1F>("1t_parton_pt_top_rec", "reco top: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_1t_parton_pt_top_rec_over_gen_vs_gen = book<TH1F>("1t_parton_pt_top_rec_over_gen_vs_gen", "partonic top: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_1t_parton_pt_top_rec_over_gen_vs_rec = book<TH1F>("1t_parton_pt_top_rec_over_gen_vs_rec", "reco top: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_1t_parton_pz_top_rec_over_gen_vs_gen = book<TH1F>("1t_parton_pz_top_rec_over_gen_vs_gen", "partonic top: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_1t_parton_pz_top_rec_over_gen_vs_rec = book<TH1F>("1t_parton_pz_top_rec_over_gen_vs_rec", "reco top: #it{p}_{T} [GeV]", n_bins, 0, x_max);

  hist_1t_parton_wass_dr_vs_pt_rec = book<TH2F>("1t_parton_wass_dr_vs_pt_rec", "#Delta#it{R}(partonic W_{as}, reco W_{as})", 20, 0, 1000, 50, 0, 5);
  hist_1t_parton_wass_pt_gen_vs_pt_rec = book<TH2F>("1t_parton_wass_pt_gen_vs_pt_rec", "partonic W_{as} #it{p}_{T} [GeV] VS. reco W_{as} #it{p}_{T} [GeV]", 20, 0, 1000, 20, 0, 1000);

  hist_1t_parton_pt_wass_gen = book<TH1F>("1t_parton_pt_wass_gen", "partonic W_{as}: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_1t_parton_pt_wass_rec = book<TH1F>("1t_parton_pt_wass_rec", "reco W_{as}: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_1t_parton_pt_wass_rec_over_gen_vs_gen = book<TH1F>("1t_parton_pt_wass_rec_over_gen_vs_gen", "partonic W_{as}: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_1t_parton_pt_wass_rec_over_gen_vs_rec = book<TH1F>("1t_parton_pt_wass_rec_over_gen_vs_rec", "reco W_{as}: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_1t_parton_pz_wass_rec_over_gen_vs_gen = book<TH1F>("1t_parton_pz_wass_rec_over_gen_vs_gen", "partonic W_{as}: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_1t_parton_pz_wass_rec_over_gen_vs_rec = book<TH1F>("1t_parton_pz_wass_rec_over_gen_vs_rec", "reco W_{as}: #it{p}_{T} [GeV]", n_bins, 0, x_max);

  hist_1t_parton_tw_dr_vs_pt_rec = book<TH2F>("1t_parton_tw_dr_vs_pt_rec", "#Delta#it{R}(partonic tW, reco tW)", 20, 0, 400, 50, 0, 5);
  hist_1t_parton_tw_pt_gen_vs_pt_rec = book<TH2F>("1t_parton_tw_pt_gen_vs_pt_rec", "partonic tW #it{p}_{T} [GeV] VS. reco tW #it{p}_{T} [GeV]", 20, 0, 400, 20, 0, 400);

  hist_1t_parton_pt_tw_gen = book<TH1F>("1t_parton_pt_tw_gen", "partonic tW: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_1t_parton_pt_tw_rec = book<TH1F>("1t_parton_pt_tw_rec", "reco tW: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_1t_parton_pt_tw_rec_over_gen_vs_gen = book<TH1F>("1t_parton_pt_tw_rec_over_gen_vs_gen", "partonic tW: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_1t_parton_pt_tw_rec_over_gen_vs_rec = book<TH1F>("1t_parton_pt_tw_rec_over_gen_vs_rec", "reco tW: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_1t_parton_pz_tw_rec_over_gen_vs_gen = book<TH1F>("1t_parton_pz_tw_rec_over_gen_vs_gen", "partonic tW: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_1t_parton_pz_tw_rec_over_gen_vs_rec = book<TH1F>("1t_parton_pz_tw_rec_over_gen_vs_rec", "reco tW: #it{p}_{T} [GeV]", n_bins, 0, x_max);

  hist_1t_parton_tw_mass_gen_vs_mass_rec = book<TH2F>("1t_parton_tw_mass_gen_vs_mass_rec", "partonic tW #it{m} [GeV] VS. reco tW #it{m} [GeV]", 20, 0, 1000, 20, 0, 1000);

  hist_1t_parton_mass_tw_gen = book<TH1F>("1t_parton_mass_tw_gen", "partonic tW: #it{m} [GeV]", n_bins, 0, x_max);
  hist_1t_parton_mass_tw_rec = book<TH1F>("1t_parton_mass_tw_rec", "reco tW: #it{m} [GeV]", n_bins, 0, x_max);
  hist_1t_parton_mass_tw_rec_over_gen_vs_gen = book<TH1F>("1t_parton_mass_tw_rec_over_gen_vs_gen", "partonic tW: #it{m} [GeV]", n_bins, 0, x_max);
  hist_1t_parton_mass_tw_rec_over_gen_vs_rec = book<TH1F>("1t_parton_mass_tw_rec_over_gen_vs_rec", "reco tW: #it{m} [GeV]", n_bins, 0, x_max);

  //____________________________________________________________________________________________________
  // Leptonic top quark, hadronic W boson

  hist_0t1W_parton_top_dr_vs_pt_rec = book<TH2F>("0t1W_parton_top_dr_vs_pt_rec", "#Delta#it{R}(partonic top, reco top)", 20, 0, 1000, 50, 0, 5);
  hist_0t1W_parton_top_pt_gen_vs_pt_rec = book<TH2F>("0t1W_parton_top_pt_gen_vs_pt_rec", "partonic top #it{p}_{T} [GeV] VS. reco top #it{p}_{T} [GeV]", 20, 0, 1000, 20, 0, 1000);

  hist_0t1W_parton_pt_top_gen = book<TH1F>("0t1W_parton_pt_top_gen", "partonic top: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_0t1W_parton_pt_top_rec = book<TH1F>("0t1W_parton_pt_top_rec", "reco top: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_0t1W_parton_pt_top_rec_over_gen_vs_gen = book<TH1F>("0t1W_parton_pt_top_rec_over_gen_vs_gen", "partonic top: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_0t1W_parton_pt_top_rec_over_gen_vs_rec = book<TH1F>("0t1W_parton_pt_top_rec_over_gen_vs_rec", "reco top: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_0t1W_parton_pz_top_rec_over_gen_vs_gen = book<TH1F>("0t1W_parton_pz_top_rec_over_gen_vs_gen", "partonic top: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_0t1W_parton_pz_top_rec_over_gen_vs_rec = book<TH1F>("0t1W_parton_pz_top_rec_over_gen_vs_rec", "reco top: #it{p}_{T} [GeV]", n_bins, 0, x_max);

  hist_0t1W_parton_wass_dr_vs_pt_rec = book<TH2F>("0t1W_parton_wass_dr_vs_pt_rec", "#Delta#it{R}(partonic W_{as}, reco W_{as})", 20, 0, 1000, 50, 0, 5);
  hist_0t1W_parton_wass_pt_gen_vs_pt_rec = book<TH2F>("0t1W_parton_wass_pt_gen_vs_pt_rec", "partonic W_{as} #it{p}_{T} [GeV] VS. reco W_{as} #it{p}_{T} [GeV]", 20, 0, 1000, 20, 0, 1000);

  hist_0t1W_parton_pt_wass_gen = book<TH1F>("0t1W_parton_pt_wass_gen", "partonic W_{as}: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_0t1W_parton_pt_wass_rec = book<TH1F>("0t1W_parton_pt_wass_rec", "reco W_{as}: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_0t1W_parton_pt_wass_rec_over_gen_vs_gen = book<TH1F>("0t1W_parton_pt_wass_rec_over_gen_vs_gen", "partonic W_{as}: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_0t1W_parton_pt_wass_rec_over_gen_vs_rec = book<TH1F>("0t1W_parton_pt_wass_rec_over_gen_vs_rec", "reco W_{as}: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_0t1W_parton_pz_wass_rec_over_gen_vs_gen = book<TH1F>("0t1W_parton_pz_wass_rec_over_gen_vs_gen", "partonic W_{as}: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_0t1W_parton_pz_wass_rec_over_gen_vs_rec = book<TH1F>("0t1W_parton_pz_wass_rec_over_gen_vs_rec", "reco W_{as}: #it{p}_{T} [GeV]", n_bins, 0, x_max);

  hist_0t1W_parton_tw_dr_vs_pt_rec = book<TH2F>("0t1W_parton_tw_dr_vs_pt_rec", "#Delta#it{R}(partonic tW, reco tW)", 20, 0, 400, 50, 0, 5);
  hist_0t1W_parton_tw_pt_gen_vs_pt_rec = book<TH2F>("0t1W_parton_tw_pt_gen_vs_pt_rec", "partonic tW #it{p}_{T} [GeV] VS. reco tW #it{p}_{T} [GeV]", 20, 0, 400, 20, 0, 400);

  hist_0t1W_parton_pt_tw_gen = book<TH1F>("0t1W_parton_pt_tw_gen", "partonic tW: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_0t1W_parton_pt_tw_rec = book<TH1F>("0t1W_parton_pt_tw_rec", "reco tW: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_0t1W_parton_pt_tw_rec_over_gen_vs_gen = book<TH1F>("0t1W_parton_pt_tw_rec_over_gen_vs_gen", "partonic tW: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_0t1W_parton_pt_tw_rec_over_gen_vs_rec = book<TH1F>("0t1W_parton_pt_tw_rec_over_gen_vs_rec", "reco tW: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_0t1W_parton_pz_tw_rec_over_gen_vs_gen = book<TH1F>("0t1W_parton_pz_tw_rec_over_gen_vs_gen", "partonic tW: #it{p}_{T} [GeV]", n_bins, 0, x_max);
  hist_0t1W_parton_pz_tw_rec_over_gen_vs_rec = book<TH1F>("0t1W_parton_pz_tw_rec_over_gen_vs_rec", "reco tW: #it{p}_{T} [GeV]", n_bins, 0, x_max);

  hist_0t1W_parton_tw_mass_gen_vs_mass_rec = book<TH2F>("0t1W_parton_tw_mass_gen_vs_mass_rec", "partonic tW #it{m} [GeV] VS. reco tW #it{m} [GeV]", 20, 0, 1000, 20, 0, 1000);

  hist_0t1W_parton_mass_tw_gen = book<TH1F>("0t1W_parton_mass_tw_gen", "partonic tW: #it{m} [GeV]", n_bins, 0, x_max);
  hist_0t1W_parton_mass_tw_rec = book<TH1F>("0t1W_parton_mass_tw_rec", "reco tW: #it{m} [GeV]", n_bins, 0, x_max);
  hist_0t1W_parton_mass_tw_rec_over_gen_vs_gen = book<TH1F>("0t1W_parton_mass_tw_rec_over_gen_vs_gen", "partonic tW: #it{m} [GeV]", n_bins, 0, x_max);
  hist_0t1W_parton_mass_tw_rec_over_gen_vs_rec = book<TH1F>("0t1W_parton_mass_tw_rec_over_gen_vs_rec", "reco tW: #it{m} [GeV]", n_bins, 0, x_max);
}


void MatchingHists::fill(const Event & event) {

  if(!event.is_valid(fHandle_GENtW)) return; // MC tW event?
  const auto & GENtW = event.get(fHandle_GENtW);
  const double w = event.weight;
  const ERegion_heavyTags & region_heavyTags = event.get(fHandle_Region_heavyTags);

  hist_decayChannel->Fill(GENtW.DecayChannel(), w);

  // const FlavorParticle & primlep = event.get(fHandle_PrimaryLepton);
  const LeptonicHemisphere & hemi = event.get(fHandle_LeptonicHemisphere);

  if(GENtW.IsSemiLeptonic(false)) {

    const GenLevelDef & Parton_Top = event.get(fHandle_Parton_Top);
    const GenLevelDef & Parton_WAss = event.get(fHandle_Parton_WAss);
    // const GenLevelDef & Parton_WTop = event.get(fHandle_Parton_WTop);
    const GenLevelDef & Parton_tW_system = event.get(fHandle_Parton_tW_system);
    const GenLevelDef & Parton_LeptonicW = event.get(fHandle_Parton_LeptonicW);
    const GenLevelDef & Parton_SingleLepton = event.get(fHandle_Parton_SingleLepton);
    const GenLevelDef & Parton_SingleNeutrino = event.get(fHandle_Parton_SingleNeutrino);

    //____________________________________________________________________________________________________
    // Leptonic W boson, lepton, neutrino

    //____________________________________________________________________________________________________
    hist_parton_wlep_dr_vs_pt_rec->Fill(hemi.wboson().pt(), uhh2::deltaR(Parton_LeptonicW.v4(), hemi.wboson().v4()), w); // 2D
    hist_parton_wlep_pt_gen_vs_pt_rec->Fill(hemi.wboson().pt(), Parton_LeptonicW.pt(), w); // 2D

    hist_parton_pt_wlep_gen->Fill(Parton_LeptonicW.pt(), w);
    hist_parton_pt_wlep_rec->Fill(hemi.wboson().pt(), w);
    hist_parton_pt_wlep_rec_over_gen_vs_gen->Fill(Parton_LeptonicW.pt(), w * hemi.wboson().pt() / Parton_LeptonicW.pt());
    hist_parton_pt_wlep_rec_over_gen_vs_rec->Fill(hemi.wboson().pt(), w * hemi.wboson().pt() / Parton_LeptonicW.pt());
    hist_parton_pz_wlep_rec_over_gen_vs_gen->Fill(Parton_LeptonicW.pt(), w * hemi.wboson().v4().pz() / Parton_LeptonicW.v4().pz());
    hist_parton_pz_wlep_rec_over_gen_vs_rec->Fill(hemi.wboson().pt(), w * hemi.wboson().v4().pz() / Parton_LeptonicW.v4().pz());

    //____________________________________________________________________________________________________
    hist_parton_lepton_dr_vs_pt_rec->Fill(hemi.lepton().pt(), uhh2::deltaR(Parton_SingleLepton.v4(), hemi.lepton().v4()), w); // 2D
    hist_parton_lepton_pt_gen_vs_pt_rec->Fill(hemi.lepton().pt(), Parton_SingleLepton.pt(), w); // 2D

    hist_parton_pt_lepton_gen->Fill(Parton_SingleLepton.pt(), w);
    hist_parton_pt_lepton_rec->Fill(hemi.lepton().pt(), w);
    hist_parton_pt_lepton_rec_over_gen_vs_gen->Fill(Parton_SingleLepton.pt(), w * hemi.lepton().pt() / Parton_SingleLepton.pt());
    hist_parton_pt_lepton_rec_over_gen_vs_rec->Fill(hemi.lepton().pt(), w * hemi.lepton().pt() / Parton_SingleLepton.pt());
    hist_parton_pz_lepton_rec_over_gen_vs_gen->Fill(Parton_SingleLepton.pt(), w * hemi.lepton().v4().pz() / Parton_SingleLepton.v4().pz());
    hist_parton_pz_lepton_rec_over_gen_vs_rec->Fill(hemi.lepton().pt(), w * hemi.lepton().v4().pz() / Parton_SingleLepton.v4().pz());

    //____________________________________________________________________________________________________
    hist_parton_neutrino_dr_vs_pt_rec->Fill(hemi.neutrino().pt(), uhh2::deltaR(Parton_SingleNeutrino.v4(), hemi.neutrino().v4()), w); // 2D
    hist_parton_neutrino_pt_gen_vs_pt_rec->Fill(hemi.neutrino().pt(), Parton_SingleNeutrino.pt(), w); // 2D

    hist_parton_pt_neutrino_gen->Fill(Parton_SingleNeutrino.pt(), w);
    hist_parton_pt_neutrino_rec->Fill(hemi.neutrino().pt(), w);
    hist_parton_pt_neutrino_rec_over_gen_vs_gen->Fill(Parton_SingleNeutrino.pt(), w * hemi.neutrino().pt() / Parton_SingleNeutrino.pt());
    hist_parton_pt_neutrino_rec_over_gen_vs_rec->Fill(hemi.neutrino().pt(), w * hemi.neutrino().pt() / Parton_SingleNeutrino.pt());
    hist_parton_pz_neutrino_rec_over_gen_vs_gen->Fill(Parton_SingleNeutrino.pt(), w * hemi.neutrino().v4().pz() / Parton_SingleNeutrino.v4().pz());
    hist_parton_pz_neutrino_rec_over_gen_vs_rec->Fill(hemi.neutrino().pt(), w * hemi.neutrino().v4().pz() / Parton_SingleNeutrino.v4().pz());

    //____________________________________________________________________________________________________
    // Hadronic top quark, leptonic W boson (not the same as above!)

    if(region_heavyTags == ERegion_heavyTags::_1t) {
      const TopJet & topjet = event.get(fHandle_TheTopJet);
      const Particle tW_system = add_Particles(topjet, hemi.wboson());

      //____________________________________________________________________________________________________
      hist_1t_parton_top_dr_vs_pt_rec->Fill(topjet.pt(), uhh2::deltaR(Parton_Top.v4(), topjet.v4()), w); // 2D
      hist_1t_parton_top_pt_gen_vs_pt_rec->Fill(topjet.pt(), Parton_Top.pt(), w); // 2D

      hist_1t_parton_pt_top_gen->Fill(Parton_Top.pt(), w);
      hist_1t_parton_pt_top_rec->Fill(topjet.pt(), w);
      hist_1t_parton_pt_top_rec_over_gen_vs_gen->Fill(Parton_Top.pt(), w * topjet.pt() / Parton_Top.pt());
      hist_1t_parton_pt_top_rec_over_gen_vs_rec->Fill(topjet.pt(), w * topjet.pt() / Parton_Top.pt());
      hist_1t_parton_pz_top_rec_over_gen_vs_gen->Fill(Parton_Top.pt(), w * topjet.v4().pz() / Parton_Top.v4().pz());
      hist_1t_parton_pz_top_rec_over_gen_vs_rec->Fill(topjet.pt(), w * topjet.v4().pz() / Parton_Top.v4().pz());

      //____________________________________________________________________________________________________
      hist_1t_parton_wass_dr_vs_pt_rec->Fill(hemi.wboson().pt(), uhh2::deltaR(Parton_WAss.v4(), hemi.wboson().v4()), w); // 2D
      hist_1t_parton_wass_pt_gen_vs_pt_rec->Fill(hemi.wboson().pt(), Parton_WAss.pt(), w); // 2D

      hist_1t_parton_pt_wass_gen->Fill(Parton_WAss.pt(), w);
      hist_1t_parton_pt_wass_rec->Fill(hemi.wboson().pt(), w);
      hist_1t_parton_pt_wass_rec_over_gen_vs_gen->Fill(Parton_WAss.pt(), w * hemi.wboson().pt() / Parton_WAss.pt());
      hist_1t_parton_pt_wass_rec_over_gen_vs_rec->Fill(hemi.wboson().pt(), w * hemi.wboson().pt() / Parton_WAss.pt());
      hist_1t_parton_pz_wass_rec_over_gen_vs_gen->Fill(Parton_WAss.pt(), w * hemi.wboson().v4().pz() / Parton_WAss.v4().pz());
      hist_1t_parton_pz_wass_rec_over_gen_vs_rec->Fill(hemi.wboson().pt(), w * hemi.wboson().v4().pz() / Parton_WAss.v4().pz());

      //____________________________________________________________________________________________________
      hist_1t_parton_tw_dr_vs_pt_rec->Fill(tW_system.pt(), uhh2::deltaR(Parton_tW_system.v4(), tW_system.v4()), w); // 2D
      hist_1t_parton_tw_pt_gen_vs_pt_rec->Fill(tW_system.pt(), Parton_tW_system.pt(), w); // 2D

      hist_1t_parton_pt_tw_gen->Fill(Parton_tW_system.pt(), w);
      hist_1t_parton_pt_tw_rec->Fill(tW_system.pt(), w);
      hist_1t_parton_pt_tw_rec_over_gen_vs_gen->Fill(Parton_tW_system.pt(), w * tW_system.pt() / Parton_tW_system.pt());
      hist_1t_parton_pt_tw_rec_over_gen_vs_rec->Fill(tW_system.pt(), w * tW_system.pt() / Parton_tW_system.pt());
      hist_1t_parton_pz_tw_rec_over_gen_vs_gen->Fill(Parton_tW_system.pt(), w * tW_system.v4().pz() / Parton_tW_system.v4().pz());
      hist_1t_parton_pz_tw_rec_over_gen_vs_rec->Fill(tW_system.pt(), w * tW_system.v4().pz() / Parton_tW_system.v4().pz());

      hist_1t_parton_tw_mass_gen_vs_mass_rec->Fill(tW_system.v4().M(), Parton_tW_system.v4().M(), w); // 2D

      hist_1t_parton_mass_tw_gen->Fill(Parton_tW_system.v4().M(), w);
      hist_1t_parton_mass_tw_rec->Fill(tW_system.v4().M(), w);
      hist_1t_parton_mass_tw_rec_over_gen_vs_gen->Fill(Parton_tW_system.v4().M(), w * tW_system.v4().M() / Parton_tW_system.v4().M());
      hist_1t_parton_mass_tw_rec_over_gen_vs_rec->Fill(tW_system.v4().M(), w * tW_system.v4().M() / Parton_tW_system.v4().M());

      // if(GENtW.IsTopHadronicDecay()) {
      //   // fill the same hists as above
      // }
    }

    //____________________________________________________________________________________________________
    // Leptonic top quark, hadronic W boson

    else if(region_heavyTags == ERegion_heavyTags::_0t1W) {
      const TopJet & wjet = event.get(fHandle_TheWJet);
      const Particle tW_system = add_Particles(wjet, hemi);

      //____________________________________________________________________________________________________
      hist_0t1W_parton_top_dr_vs_pt_rec->Fill(hemi.pt(), uhh2::deltaR(Parton_Top.v4(), hemi.v4()), w); // 2D
      hist_0t1W_parton_top_pt_gen_vs_pt_rec->Fill(hemi.pt(), Parton_Top.pt(), w); // 2D

      hist_0t1W_parton_pt_top_gen->Fill(Parton_Top.pt(), w);
      hist_0t1W_parton_pt_top_rec->Fill(hemi.pt(), w);
      hist_0t1W_parton_pt_top_rec_over_gen_vs_gen->Fill(Parton_Top.pt(), w * hemi.pt() / Parton_Top.pt());
      hist_0t1W_parton_pt_top_rec_over_gen_vs_rec->Fill(hemi.pt(), w * hemi.pt() / Parton_Top.pt());
      hist_0t1W_parton_pz_top_rec_over_gen_vs_gen->Fill(Parton_Top.pt(), w * hemi.v4().pz() / Parton_Top.v4().pz());
      hist_0t1W_parton_pz_top_rec_over_gen_vs_rec->Fill(hemi.pt(), w * hemi.v4().pz() / Parton_Top.v4().pz());

      //____________________________________________________________________________________________________
      hist_0t1W_parton_wass_dr_vs_pt_rec->Fill(wjet.pt(), uhh2::deltaR(Parton_WAss.v4(), wjet.v4()), w); // 2D
      hist_0t1W_parton_wass_pt_gen_vs_pt_rec->Fill(wjet.pt(), Parton_WAss.pt(), w); // 2D

      hist_0t1W_parton_pt_wass_gen->Fill(Parton_WAss.pt(), w);
      hist_0t1W_parton_pt_wass_rec->Fill(wjet.pt(), w);
      hist_0t1W_parton_pt_wass_rec_over_gen_vs_gen->Fill(Parton_WAss.pt(), w * wjet.pt() / Parton_WAss.pt());
      hist_0t1W_parton_pt_wass_rec_over_gen_vs_rec->Fill(wjet.pt(), w * wjet.pt() / Parton_WAss.pt());
      hist_0t1W_parton_pz_wass_rec_over_gen_vs_gen->Fill(Parton_WAss.pt(), w * wjet.v4().pz() / Parton_WAss.v4().pz());
      hist_0t1W_parton_pz_wass_rec_over_gen_vs_rec->Fill(wjet.pt(), w * wjet.v4().pz() / Parton_WAss.v4().pz());

      //____________________________________________________________________________________________________
      hist_0t1W_parton_tw_dr_vs_pt_rec->Fill(tW_system.pt(), uhh2::deltaR(Parton_tW_system.v4(), tW_system.v4()), w); // 2D
      hist_0t1W_parton_tw_pt_gen_vs_pt_rec->Fill(tW_system.pt(), Parton_tW_system.pt(), w); // 2D

      hist_0t1W_parton_pt_tw_gen->Fill(Parton_tW_system.pt(), w);
      hist_0t1W_parton_pt_tw_rec->Fill(tW_system.pt(), w);
      hist_0t1W_parton_pt_tw_rec_over_gen_vs_gen->Fill(Parton_tW_system.pt(), w * tW_system.pt() / Parton_tW_system.pt());
      hist_0t1W_parton_pt_tw_rec_over_gen_vs_rec->Fill(tW_system.pt(), w * tW_system.pt() / Parton_tW_system.pt());
      hist_0t1W_parton_pz_tw_rec_over_gen_vs_gen->Fill(Parton_tW_system.pt(), w * tW_system.v4().pz() / Parton_tW_system.v4().pz());
      hist_0t1W_parton_pz_tw_rec_over_gen_vs_rec->Fill(tW_system.pt(), w * tW_system.v4().pz() / Parton_tW_system.v4().pz());

      hist_0t1W_parton_tw_mass_gen_vs_mass_rec->Fill(tW_system.v4().M(), Parton_tW_system.v4().M(), w); // 2D

      hist_0t1W_parton_mass_tw_gen->Fill(Parton_tW_system.v4().M(), w);
      hist_0t1W_parton_mass_tw_rec->Fill(tW_system.v4().M(), w);
      hist_0t1W_parton_mass_tw_rec_over_gen_vs_gen->Fill(Parton_tW_system.v4().M(), w * tW_system.v4().M() / Parton_tW_system.v4().M());
      hist_0t1W_parton_mass_tw_rec_over_gen_vs_rec->Fill(tW_system.v4().M(), w * tW_system.v4().M() / Parton_tW_system.v4().M());

      // if(GENtW.IsAssHadronicDecay()) {
      //   // fill the same hists as above
      // }
    }
  }
}

}}
