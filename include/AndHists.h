#pragma once

#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/HighPtSingleTop/include/DNNSetup.h"

#include <vector>
#include <TH1F.h>


namespace uhh2 {

  class CountingHist: public Hists {

  public:
    CountingHist(uhh2::Context & ctx, const std::string & dirname);

    virtual void fill(const uhh2::Event & event);
    virtual ~CountingHist() {};

  private:
    TH1F *hist_counting;
  };

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
    BinnedDNNHists(uhh2::Context &ctx, const std::string & dirname, const std::vector<std::string> inputs, const std::vector<DNNInput> inputs_info);

    virtual void fill(const uhh2::Event & event);
    virtual ~BinnedDNNHists();

  private:
    std::string m_dirname;

  protected:
    uhh2::Event::Handle<double> h_toptag_pt;
    std::vector<uhh2::Hists*> hists_vector;

    TH1F *hist_toptag_pt;
  };

}
