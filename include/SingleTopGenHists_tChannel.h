#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/HighPtSingleTop/include/SingleTopGen_tChannel.h"

/** \brief Histograms for single top t-channel quantities on generator level
 * 
 * SingleTopGen_tChannel container has to be filled before calling this histogram class
 */

class SingleTopGenHists_tChannel: public uhh2::Hists {
public:
    SingleTopGenHists_tChannel(uhh2::Context & ctx, const std::string & dirname);

    virtual void fill(const uhh2::Event & ev) override;

 protected:
    TH1F *hist_sHat, *hist_decayChannel;
    TH1F *hist_top_pt, *hist_top_eta, *hist_top_eta_pt0to100, *hist_top_eta_pt100to200, *hist_top_eta_pt200to500, *hist_top_eta_pt500to1000, *hist_top_eta_pt1000toInf;
    TH1F *hist_bSpec_pt, *hist_bSpec_eta, *hist_bSpec_eta_pt0to100, *hist_bSpec_eta_pt100to200, *hist_bSpec_eta_pt200to500, *hist_bSpec_eta_pt500to1000, *hist_bSpec_eta_pt1000toInf;
    TH1F *hist_scaQu_pt, *hist_scaQu_eta, *hist_scaQu_eta_pt0to100, *hist_scaQu_eta_pt100to200, *hist_scaQu_eta_pt200to500, *hist_scaQu_eta_pt500to1000, *hist_scaQu_eta_pt1000toInf;

    uhh2::Event::Handle<SingleTopGen_tChannel> h_singletopgen_tChannel;
};
