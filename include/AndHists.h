#pragma once

#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"

#include <vector>
#include <TH1F.h>


namespace uhh2 {

  class AndHists: public Hists {

  public:
    AndHists(uhh2::Context &ctx, const std::string & dirname);

    virtual void fill(const uhh2::Event & event);
    virtual ~AndHists();
    void add_hist(uhh2::Hists *hist);
    std::string get_dirname();

  private:
    std::string m_dirname;

  protected:
    std::vector<uhh2::Hists*> hists_vector;

    TH1F *nevt, *wevt;
  };

  class BinnedDNNHists: public Hists {

  public:
    BinnedDNNHists(uhh2::Context &ctx, const std::string & dirname);

    virtual void fill(const uhh2::Event & event);
    virtual ~BinnedDNNHists();

  private:
    std::string m_dirname;

  protected:
    uhh2::Event::Handle<TopJet> handle_tjet;
    std::vector<uhh2::Hists*> hists_vector;

    TH1F *h_tjet_pt;
  };

}
