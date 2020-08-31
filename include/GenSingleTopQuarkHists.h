#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/HighPtSingleTop/include/SingleTopGen_tWch.h"

class GenSingleTopQuarkHists: public uhh2::Hists {

 public:

  GenSingleTopQuarkHists(uhh2::Context & ctx, const std::string & dirname, const std::string & channel = "");
  virtual void fill(const uhh2::Event & event) override;

 protected:

  std::string _channel;

  TH1F *hist_topQuarkPt;
  TH1F *hist_topQuarkPt_ggFusion;
  TH1F *hist_topQuarkPt_qqAnnihilation;
  TH1F *hist_topQuarkPt_gbFusion;
  TH1F *hist_topQuarkPt_else;

  TH1F *hist_tW_incl_top_pt;
  TH1F *hist_tW_incl_top_eta;
  TH1F *hist_tW_incl_Wass_pt;
  TH1F *hist_tW_incl_Wass_eta;
  TH1F *hist_tW_incl_dPhi_tW;
  TH1F *hist_tW_incl_bAss_pt;
  TH1F *hist_tW_incl_bAss_eta;

  TH1F *hist_tW_pt200_top_pt;
  TH1F *hist_tW_pt200_top_eta;
  TH1F *hist_tW_pt200_Wass_pt;
  TH1F *hist_tW_pt200_Wass_eta;
  TH1F *hist_tW_pt200_dPhi_tW;
  TH1F *hist_tW_pt200_bAss_pt;
  TH1F *hist_tW_pt200_bAss_eta;

  TH1F *hist_tW_pt400_top_pt;
  TH1F *hist_tW_pt400_top_eta;
  TH1F *hist_tW_pt400_Wass_pt;
  TH1F *hist_tW_pt400_Wass_eta;
  TH1F *hist_tW_pt400_dPhi_tW;
  TH1F *hist_tW_pt400_bAss_pt;
  TH1F *hist_tW_pt400_bAss_eta;

  uhh2::Event::Handle<SingleTopGen_tWch> h_GENtW;
};
