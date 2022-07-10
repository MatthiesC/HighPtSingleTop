#include "UHH2/common/include/Utils.h"

#include "UHH2/HighPtSingleTop/include/LeptonicHemisphereHists.h"

#include "TH1F.h"
// #include "TH2F.h"

using namespace std;
using namespace uhh2;
using namespace uhh2::btw;
using namespace uhh2::ltt;

namespace uhh2 { namespace btw {

LeptonicHemisphereHists::LeptonicHemisphereHists(Context & ctx, const string & dirname, const unsigned int default_nbins):
  Hists(ctx, dirname),
  // fHandle_Region_heavyTags(ctx.get_handle<ERegion_heavyTags>(kHandleName_Region_heavyTags)),
  fHandle_LeptonicHemisphere(ctx.get_handle<LeptonicHemisphere>(kHandleName_LeptonicHemisphere))
{
  hist_top_mass = book<TH1F>("top_mass", "Leptonic top quark #it{m} [GeV]", default_nbins, 0, 500);
  hist_w_mass = book<TH1F>("w_mass", "Leptonic W boson #it{m} [GeV]", default_nbins, 0, 200);

  hist_mtw_k = book<TH1F>("mtw_k", "Leptonic W boson #it{m}_{T} [GeV] (w/ kappa)", default_nbins, 0, 200);
  hist_mtw = book<TH1F>("mtw", "Leptonic W boson #it{m}_{T} [GeV] (w/o kappa)", default_nbins, 0, 200);

  hist_top_pt = book<TH1F>("top_pt", "Leptonic top quark #it{p}_{T} [GeV]", default_nbins, 0, 1000);
  hist_w_pt = book<TH1F>("w_pt", "Leptonic W boson #it{p}_{T} [GeV]", default_nbins, 0, 1000);

  hist_top_eta = book<TH1F>("top_eta", "Leptonic top quark #eta", default_nbins, -5.0, 5.0);
  hist_w_eta = book<TH1F>("w_eta", "Leptonic W boson #eta", default_nbins, -5.0, 5.0);

  hist_neutrino_pt = book<TH1F>("neutrino_pt", "Neutrino #it{p}_{T} [GeV]", default_nbins, 0, 500);
  hist_neutrino_pz = book<TH1F>("neutrino_pz", "Neutrino #it{p}_{z} [GeV]", default_nbins, 0, 500);

  hist_neutrino_eta = book<TH1F>("neutrino_eta", "Neutrino #eta", default_nbins, -5.0, 5.0);
  hist_neutrino_phi = book<TH1F>("neutrino_phi", "Neutrino #phi [rad]", default_nbins, -M_PI, M_PI);

  hist_lepton_pt = book<TH1F>("lepton_pt", "Lepton #it{p}_{T} [GeV]", default_nbins, 0, 500);
  hist_dr_leptonjet = book<TH1F>("dr_leptonjet", "#Delta#it{R}(lepton, jet)", default_nbins, 0.0, 5.0);

  hist_jet_pt = book<TH1F>("jet_pt", "Jet #it{p}_{T} [GeV]", default_nbins, 0, 500);
  hist_dr_max = book<TH1F>("dr_max", "max. #Delta#it{R} between lepton, neutrino and jet", default_nbins, 0.0, 5.0);

  hist_jet_deepjet = book<TH1F>("jet_deepjet", "Jet #it{O}(DeepJet)", default_nbins, 0, 1);
  hist_dr_puppi_chs = book<TH1F>("dr_puppi_chs", "#Delta#it{R}(PUPPI jet, matched CHS jet)", default_nbins, 0, 0.5);

  const unsigned int nx = kJetFindingMethods.size();
  hist_method = book<TH1F>("method", "Method", nx, -0.5, (float)nx-0.5);
  vector<string> method_labels;
  for(const auto & m : kJetFindingMethods) {
    method_labels.push_back(m.second.tlatex);
  }
  for(unsigned int i = 1; i <= nx; ++i) {
    hist_method->GetXaxis()->SetBinLabel(i, method_labels[i-1].c_str());
  }
  hist_nsolutions = book<TH1F>("nsolutions", "Number of solutions", 11, -0.5, 10.5);
}

void LeptonicHemisphereHists::fill(const Event & event) {
  const float w = event.weight;
  // const ERegion_heavyTags region_heavyTags = event.get(fHandle_Region_heavyTags);
  const LeptonicHemisphere hemi = event.get(fHandle_LeptonicHemisphere);

  hist_top_mass->Fill(hemi.v4().M(), w);
  hist_w_mass->Fill(hemi.wboson().v4().M(), w);

  hist_mtw_k->Fill(hemi.mtw(), w);
  hist_mtw->Fill(mTW(hemi.lepton(), *event.met), w);

  hist_top_pt->Fill(hemi.pt(), w);
  hist_w_pt->Fill(hemi.wboson().pt(), w);

  hist_top_eta->Fill(hemi.eta(), w);
  hist_w_eta->Fill(hemi.wboson().eta(), w);

  hist_neutrino_pt->Fill(hemi.neutrino().pt(), w);
  hist_neutrino_pz->Fill(hemi.neutrino().v4().pz(), w);

  hist_neutrino_eta->Fill(hemi.neutrino().eta(), w);
  hist_neutrino_phi->Fill(hemi.neutrino().phi(), w);

  hist_lepton_pt->Fill(hemi.lepton().pt(), w);
  hist_dr_leptonjet->Fill(hemi.dr_leptonjet(), w);

  hist_jet_pt->Fill(hemi.jet().pt(), w);
  hist_dr_max->Fill(hemi.dr_max(), w);

  hist_jet_deepjet->Fill(hemi.btag_DeepJet(), w);
  hist_dr_puppi_chs->Fill(uhh2::deltaR(hemi.jet().v4(), hemi.chsjet().v4()), w);

  hist_method->Fill((int)hemi.method(), w);
  hist_nsolutions->Fill(hemi.nsolutions(), w);
}

}}
