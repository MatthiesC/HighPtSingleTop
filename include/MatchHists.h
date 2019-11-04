#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/HighPtSingleTop/include/SingleTopGen_tWch.h"


/** \brief Histograms to check if generator particles and reco decays overlap/match.
 *
 * To be used only with tW events!
 */


class MatchHists: public uhh2::Hists {

 public:

  MatchHists(uhh2::Context & ctx, const std::string & dirname);
  virtual void fill(const uhh2::Event & event) override;

 protected:
  
  TH1F *hist_deltaR_topQuark_topRecoJet, *hist_deltaPhi_topQuark_topRecoJet, *hist_deltaR_topQuark_recoLepton, *hist_deltaPhi_topQuark_recoLepton;
  TH1F *hist_deltaR_WBoson_topRecoJet, *hist_deltaPhi_WBoson_topRecoJet, *hist_deltaR_WBoson_recoLepton, *hist_deltaPhi_WBoson_recoLepton;
  TH1F *hist_deltaR_topQuark_topRecoJet_Pt0to400, *hist_deltaPhi_topQuark_topRecoJet_Pt0to400, *hist_deltaR_topQuark_recoLepton_Pt0to400, *hist_deltaPhi_topQuark_recoLepton_Pt0to400;
  TH1F *hist_deltaR_WBoson_topRecoJet_Pt0to400, *hist_deltaPhi_WBoson_topRecoJet_Pt0to400, *hist_deltaR_WBoson_recoLepton_Pt0to400, *hist_deltaPhi_WBoson_recoLepton_Pt0to400;
  TH1F *hist_deltaR_topQuark_topRecoJet_Pt400toInf, *hist_deltaPhi_topQuark_topRecoJet_Pt400toInf, *hist_deltaR_topQuark_recoLepton_Pt400toInf, *hist_deltaPhi_topQuark_recoLepton_Pt400toInf;
  TH1F *hist_deltaR_WBoson_topRecoJet_Pt400toInf, *hist_deltaPhi_WBoson_topRecoJet_Pt400toInf, *hist_deltaR_WBoson_recoLepton_Pt400toInf, *hist_deltaPhi_WBoson_recoLepton_Pt400toInf;

  TH1F *hist_decayChannel;

  TH1F *hist_topRecoJet_pt, *hist_topRecoJet_eta, *hist_topRecoJet_mass, *hist_topRecoJet_phi;

  uhh2::Event::Handle<SingleTopGen_tWch> h_GENtW;
  uhh2::Event::Handle<TopJet> h_toptaggedjet;
  uhh2::Event::Handle<FlavorParticle> h_primlep;
};
