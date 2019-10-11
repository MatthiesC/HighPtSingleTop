#pragma once
#include "UHH2/core/include/Hists.h"

/** \brief Common muon histograms such as multiplicity, pt, eta, isolation, etc.
 *
 *
 * Differences to SFrameAnalysis version:
 *   - does not sort muons in pt; do that outside this class if desired
 *   - no log-y version of the histogram; this is a plotting choice.
 */
class MyMuonHists: public uhh2::Hists {
public:
    MyMuonHists(uhh2::Context & ctx, const std::string & dirname, bool gen_plots=true);
    
    virtual void fill(const uhh2::Event & ev) override;
    
protected:
    
    // declare all histograms as members. Note that one could also use get_hist
    // as in the example's ExampleHists instead of saving the histograms here. However,
    // that would entail quite a runtime overhead and it is much faster to declare the histograms
    // here directly.
    TH1F *number, *pt, *eta, *phi, *deltaPhiMET, *isolation, *charge, *ptrel, *deltaRmin, *deltaPhimin;
    TH1F *eff_sub, *eff_tot, *pt_response;
    
    TH2F *deltaRmin_ptrel;
};
