#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"


class TaggedJetsHists: public uhh2::Hists {

 public:

  TaggedJetsHists(uhh2::Context & ctx, const std::string & dirname, const std::string & h_name_taggedjet, const std::string & h_name_postfix, double arg_MIN_PT = 0.0, double arg_MAX_PT = 99999.0);
  virtual void fill(const uhh2::Event & event) override;

 protected:

  uhh2::Event::Handle<TopJet> m_h_taggedjet;

  uhh2::Event::Handle<std::vector<Jet>> m_h_all_bloose;
  uhh2::Event::Handle<std::vector<Jet>> m_h_all_bmedium;
  uhh2::Event::Handle<std::vector<Jet>> m_h_all_btight;

  uhh2::Event::Handle<std::vector<Jet>> m_h_ex;
  uhh2::Event::Handle<std::vector<Jet>> m_h_ex_bloose;
  uhh2::Event::Handle<std::vector<Jet>> m_h_ex_bmedium;
  uhh2::Event::Handle<std::vector<Jet>> m_h_ex_btight;

  uhh2::Event::Handle<std::vector<Jet>> m_h_in;
  uhh2::Event::Handle<std::vector<Jet>> m_h_in_bloose;
  uhh2::Event::Handle<std::vector<Jet>> m_h_in_bmedium;
  uhh2::Event::Handle<std::vector<Jet>> m_h_in_btight;

  double m_MIN_PT, m_MAX_PT;

  TH1F *hist_n_all, *hist_n_all_bloose, *hist_n_all_bmedium, *hist_n_all_btight;
  TH2F *hist_n_all_b_loose_medium_2d, *hist_n_all_b_loose_tight_2d, *hist_n_all_b_medium_tight_2d;
  TH3F *hist_n_all_b_all_3d;
  TH1F *hist_n_ex, *hist_n_ex_bloose, *hist_n_ex_bmedium, *hist_n_ex_btight;
  TH1F *hist_n_in, *hist_n_in_bloose, *hist_n_in_bmedium, *hist_n_in_btight;
  TH2F *hist_n_inex_b_loose_2d, *hist_n_inex_b_medium_2d, *hist_n_inex_b_tight_2d;
};
