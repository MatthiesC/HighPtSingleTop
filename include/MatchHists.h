#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/HighPtSingleTop/include/SingleTopGen_tWch.h"


/** \brief Histograms to check if generator particles and reco decays overlap/match.
 *
 * To be used only with tW events!
 */


class MatchHists: public uhh2::Hists {

 public:

  MatchHists(uhh2::Context & ctx, const std::string & dirname, const std::string & object_name, double arg_MIN_PT = 0.0, double arg_MAX_PT = 99999.0);
  virtual void fill(const uhh2::Event & event) override;

 private:

  int get_number_of_matched_quarks(const TopJet & taggedjet, const std::vector<GenParticle> & daughters);

 protected:

  uhh2::Event::Handle<SingleTopGen_tWch> h_GENtW;
  uhh2::Event::Handle<TopJet> h_taggedjet;
  uhh2::Event::Handle<FlavorParticle> h_primlep;

  double m_MIN_PT, m_MAX_PT;

  bool is_TopTagRegion, is_WTagRegion;

  TH1F *hist_all_decayChannel;
  TH1F *hist_all_deltaR_taggedjet_gentop, *hist_all_deltaR_taggedjet_genwass, *hist_all_deltaR_taggedjet_genwtop, *hist_all_deltaR_lepton_gentop, *hist_all_deltaR_lepton_genwass, *hist_all_deltaR_lepton_genwtop;

  TH1F *hist_3matched_decayChannel;
  TH1F *hist_3matched_deltaR_taggedjet_gentop, *hist_3matched_deltaR_taggedjet_genwass, *hist_3matched_deltaR_taggedjet_genwtop, *hist_3matched_deltaR_lepton_gentop, *hist_3matched_deltaR_lepton_genwass, *hist_3matched_deltaR_lepton_genwtop;

  TH1F *hist_2matched_decayChannel;
  TH1F *hist_2matched_deltaR_taggedjet_gentop, *hist_2matched_deltaR_taggedjet_genwass, *hist_2matched_deltaR_taggedjet_genwtop, *hist_2matched_deltaR_lepton_gentop, *hist_2matched_deltaR_lepton_genwass, *hist_2matched_deltaR_lepton_genwtop;

  TH1F *hist_1matched_decayChannel;
  TH1F *hist_1matched_deltaR_taggedjet_gentop, *hist_1matched_deltaR_taggedjet_genwass, *hist_1matched_deltaR_taggedjet_genwtop, *hist_1matched_deltaR_lepton_gentop, *hist_1matched_deltaR_lepton_genwass, *hist_1matched_deltaR_lepton_genwtop;

  TH1F *hist_0matched_decayChannel;
  TH1F *hist_0matched_deltaR_taggedjet_gentop, *hist_0matched_deltaR_taggedjet_genwass, *hist_0matched_deltaR_taggedjet_genwtop, *hist_0matched_deltaR_lepton_gentop, *hist_0matched_deltaR_lepton_genwass, *hist_0matched_deltaR_lepton_genwtop;
};
