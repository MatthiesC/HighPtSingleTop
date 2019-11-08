#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"


/** \brief Histograms for the HOTVR t-tag.
 *
 *
 */


class TopTagHists: public uhh2::Hists {

 public:

  TopTagHists(uhh2::Context & ctx, const std::string & dirname, double arg_MIN_PT = 0.0, double arg_MAX_PT = 99999.0);
  virtual void fill(const uhh2::Event & event) override;

 protected:

  TH1F *hist_top_pt, *hist_top_eta, *hist_top_mass, *hist_top_phi, *hist_top_px, *hist_top_py, *hist_top_pz, *hist_top_energy, *hist_top_area, *hist_top_nsub, *hist_top_fpt, *hist_top_mpair, *hist_top_tau32, *hist_top_tau21, *hist_top_tau1, *hist_top_tau2, *hist_top_tau3, *hist_top_dr_lepton, *hist_top_dphi_lepton, *hist_top_dphi_met, *hist_met_dphi_lepton;

  uhh2::Event::Handle<TopJet> h_toptaggedjet;
  uhh2::Event::Handle<FlavorParticle> h_primlep;
  
  double m_MIN_PT, m_MAX_PT;
};
