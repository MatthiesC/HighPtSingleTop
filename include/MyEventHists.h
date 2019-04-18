#pragma once
#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"


class MyEventHists: public uhh2::Hists {
public:
    MyEventHists(uhh2::Context & ctx, const std::string & dirname);

    virtual void fill(const uhh2::Event & ev) override;

protected:
    TH1F *N_PrimVertices, *N_TrueInteractions, *Weights, *MET, *HThad, *HTlep, *ST;

};
