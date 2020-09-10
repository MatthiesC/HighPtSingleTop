#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"


/** \brief Histograms for the HOTVR t-tag.
 *
 *
 */


class WTagHists: public uhh2::Hists {

 public:

  WTagHists(uhh2::Context & ctx, const std::string & dirname, double arg_MIN_PT = 0.0, double arg_MAX_PT = 99999.0);
  virtual void fill(const uhh2::Event & event) override;

 protected:

  TH1F *hist_w_pt, *hist_w_pt_1GeV, *hist_w_eta, *hist_w_mass, *hist_w_SDmass, *hist_w_phi, *hist_w_px, *hist_w_py, *hist_w_pz, *hist_w_energy, *hist_w_area, *hist_w_tau32, *hist_w_tau21, *hist_w_tau1, *hist_w_tau2, *hist_w_tau3, *hist_w_dr_lepton, *hist_w_dphi_lepton, *hist_w_dphi_met, *hist_met_dphi_lepton;

  uhh2::Event::Handle<std::vector<TopJet>> h_wtaggedjets;
  uhh2::Event::Handle<FlavorParticle> h_primlep;

  double m_MIN_PT, m_MAX_PT;
};
