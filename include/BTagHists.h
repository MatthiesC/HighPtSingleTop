#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/JetIds.h"

#include "TH1F.h"
#include "TH2F.h"


class BTagHists: public uhh2::Hists {

 public:

  BTagHists(uhh2::Context & ctx, const std::string & dirname, const BTag::algo & algo = BTag::algo::DEEPCSV);
  virtual void fill(const uhh2::Event & event) override;

 private:
  BTag::algo m_btag_algo;

 protected:

  // uhh2::Event::Handle<std::vector<Jet>> m_h_all_bloose;
  // uhh2::Event::Handle<std::vector<Jet>> m_h_all_bmedium;
  // uhh2::Event::Handle<std::vector<Jet>> m_h_all_btight;

  TH1F *hist_n_all, *hist_n_all_bloose, *hist_n_all_bmedium, *hist_n_all_btight;
  TH2F *hist_n_all_b_loose_medium_2d, *hist_n_all_b_loose_tight_2d, *hist_n_all_b_medium_tight_2d;

  TH1F *hist_jet1btag_btag;
  TH1F *hist_jet1btag_pt;
  TH1F *hist_jet2btag_btag;
  TH1F *hist_jet2btag_pt;
  TH1F *hist_jet3btag_btag;
  TH1F *hist_jet3btag_pt;

  TH1F *hist_jet1pt_btag;
  TH1F *hist_jet1pt_pt;
  TH1F *hist_jet2pt_btag;
  TH1F *hist_jet2pt_pt;
  TH1F *hist_jet3pt_btag;
  TH1F *hist_jet3pt_pt;
};
