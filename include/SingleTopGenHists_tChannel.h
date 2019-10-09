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

    uhh2::Event::Handle<SingleTopGen_tChannel> h_singletopgen_tChannel;
};
