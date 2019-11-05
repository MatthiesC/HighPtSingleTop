#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/HighPtSingleTop/include/SingleTopGen_tWch.h"


/** \brief Histograms to check if generator particles and reco decays overlap/match.
 *
 * To be used only with tW events!
 */


class MatchHists: public uhh2::Hists {

 public:

  MatchHists(uhh2::Context & ctx, const std::string & dirname, double arg_MIN_PT = 0.0, double arg_MAX_PT = 99999.0);
  virtual void fill(const uhh2::Event & event) override;

 protected:
  
  TH1F *hist_deltaR_topQuark_topRecoJet, *hist_deltaPhi_topQuark_topRecoJet, *hist_deltaR_topQuark_recoLepton, *hist_deltaPhi_topQuark_recoLepton;
  TH1F *hist_deltaR_WBoson_topRecoJet, *hist_deltaPhi_WBoson_topRecoJet, *hist_deltaR_WBoson_recoLepton, *hist_deltaPhi_WBoson_recoLepton;

  TH1F *hist_decayChannel;

  TH1F *hist_topRecoJet_pt, *hist_topRecoJet_eta, *hist_topRecoJet_mass, *hist_topRecoJet_phi, *hist_topRecoJet_px, *hist_topRecoJet_py, *hist_topRecoJet_pz, *hist_topRecoJet_energy, *hist_topRecoJet_area, *hist_topRecoJet_nsub, *hist_topRecoJet_fpt, *hist_topRecoJet_mpair, *hist_topRecoJet_tau32, *hist_topRecoJet_tau21, *hist_topRecoJet_tau1, *hist_topRecoJet_tau2, *hist_topRecoJet_tau3, *hist_topRecoJet_dr_lepton, *hist_topRecoJet_dphi_lepton, *hist_topRecoJet_dphi_met, *hist_met_dphi_lepton;

  uhh2::Event::Handle<SingleTopGen_tWch> h_GENtW;
  uhh2::Event::Handle<TopJet> h_toptaggedjet;
  uhh2::Event::Handle<FlavorParticle> h_primlep;

  double m_MIN_PT, m_MAX_PT;
};
