#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/LorentzVector.h"

#include "UHH2/HighPtSingleTop/include/MyElectronHists.h"

#include "TH1F.h"
#include "TH2F.h"

#include <vector>

using namespace uhh2;
using namespace std;

MyElectronHists::MyElectronHists(Context & ctx, const std::string & dname, bool gen_plots): Hists(ctx, dname){
    number      = book<TH1F>("number",      "electron multiplicity",7,-0.5,6.5);

    pt          = book<TH1F>("pt",          "electron p_{T} [GeV]",           40,0,2000);
    eta         = book<TH1F>("eta",         "electron #eta",            30,-3,3);
    phi         = book<TH1F>("phi",         "electron #phi",            50,-M_PI,M_PI);
    isolation   = book<TH1F>("isolation",   "electron isolation",          50,0,0.2);
    charge      = book<TH1F>("charge",      "electron charge [e]",          3,-1.5,1.5);
    ptrel       = book<TH1F>("ptrel",       "p_{T}^{rel}(e, jet)",       40, 0, 200.);
    deltaRmin   = book<TH1F>("deltaRmin",   "#DeltaR(e, jet)",    40, 0, 2.0);
    deltaRmin_ptrel
                = book<TH2F>("deltaRmin_ptrel",
                                            "x=#DeltaR(e, jet) y=p_{T}^{rel}(e, jet)",
                                                                        40, 0, 2.0, 40, 0, 200.);
    deltaPhimin = book<TH1F>("deltaPhimin", "#Delta#phi(e, jet)", 30, 0, M_PI);
    
    if (gen_plots) {
        eff_sub     = book<TH1F>("eff_sub",     "electron p_{T} [GeV]",                100,0,500);
        eff_tot     = book<TH1F>("eff_tot",     "electron p_{T} [GeV]",                100,0,500);
        pt_response = book<TH1F>("pt_response", "electron p_{T} response: (p_{T, gen} - p_{T, reco}) / p_{T, gen}",
                                                                        800,-1.,1.);
    } else {
        eff_sub     = 0;
        eff_tot     = 0;
        pt_response = 0;
    }
}

void MyElectronHists::fill(const Event & event){
    auto w = event.weight;
    assert(event.electrons);
    number->Fill(event.electrons->size(), w);

    if (eff_sub && event.genparticles) {
        for (const auto & gp: *event.genparticles) {
            if (abs(gp.pdgId()) == 11) {
                auto gp_pt = gp.pt();
                eff_tot->Fill(gp_pt, w);
                auto ele = closestParticle(gp, *event.electrons);
                if (ele && deltaR(gp, *ele) < 0.1) {
                    eff_sub->Fill(gp_pt, w);
                    pt_response->Fill((gp_pt - ele->pt()) / gp_pt, w);
                }
            }
        }
    }

    // buffer values for ptrel and drmin to avoid recomputation:
    vector<float> drmin_buf;
    vector<float> ptrel_buf;
    for(const auto & ele : *event.electrons){
        pt->Fill(ele.pt(), w);
        eta->Fill(ele.eta(), w);
        phi->Fill(ele.phi(), w);
        isolation->Fill(ele.relIso(), w);
        charge->Fill(ele.charge(), w);
        
        if(event.jets){
            auto nj = nextJet(ele, *event.jets);
            auto drmin_val = nj ? deltaR(ele, *nj) : numeric_limits<float>::infinity();
            drmin_buf.push_back(drmin_val);
            ptrel_buf.push_back(pTrel(ele, nj));
            ptrel->Fill(ptrel_buf.back(), w);
            deltaRmin->Fill(drmin_buf.back(), w);
            deltaRmin_ptrel->Fill(drmin_buf.back(), ptrel_buf.back(), w);
        }
    }
    
 
}
