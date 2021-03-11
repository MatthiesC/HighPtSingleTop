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


  class MttHist: public Hists {

  public:
    MttHist(uhh2::Context & ctx, const std::string & dirname);

    virtual void fill(const uhh2::Event & event);
    virtual ~MttHist() {};

  private:
    TH1F *hist_mtt;
    TH1F *hist_mtt_100;
  };


  class AndHists: public Hists {

  public:
    AndHists(uhh2::Context & ctx, const std::string & dirname);

    virtual void fill(const uhh2::Event & event);
    virtual ~AndHists();
    void add_hist(uhh2::Hists *hist);
    void add_Ak8Hists(uhh2::Context & ctx, const std::string & ak8_handle_name="Ak8Jets");
    void add_DeepJetHists(uhh2::Context & ctx);
    void add_WTagHists(uhh2::Context & ctx, const bool & var_binned=true);
    void add_TopTagHists(uhh2::Context & ctx, const bool & var_binned=true);
    void add_TaggedJetsHists(uhh2::Context & ctx, const std::string & h_name_taggedjet, const std::string & h_name_postfix);
    std::string get_dirname();

  private:
    std::string m_dirname;

  protected:
    std::vector<uhh2::Hists*> hists_vector;

    TH1F *nevt, *wevt;
  };


  class BinnedDNNHists: public Hists {

  public:
    BinnedDNNHists(uhh2::Context & ctx, const std::string & dirname, const std::vector<std::string> inputs, const std::vector<DNNInput> inputs_info, const std::vector<std::string> outputs);

    virtual void fill(const uhh2::Event & event);
    virtual ~BinnedDNNHists();

  private:
    std::string m_dirname;

  protected:
    uhh2::Event::Handle<double> h_tjet_pt, h_wjet_pt, h_lepton_pt;
    std::vector<uhh2::Hists*> hists_vector;

    TH1F *hist_tjet_pt, *hist_wjet_pt, *hist_lepton_pt;
  };

}
