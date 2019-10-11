#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/LorentzVector.h"

#include "UHH2/HighPtSingleTop/include/MyMuonHists.h"

#include "TH1F.h"
#include "TH2F.h"

#include <vector>

using namespace uhh2;
using namespace std;

MyMuonHists::MyMuonHists(Context & ctx, const std::string & dname, bool gen_plots): Hists(ctx, dname){
    number = book<TH1F>("number","muon multiplicity",7,-0.5,6.5);
    
    pt = book<TH1F>("pt","muon p_{T} [GeV]",50,0,1000);
    eta = book<TH1F>("eta","muon #eta",50,-3,3);
    phi = book<TH1F>("phi","muon #phi",50,-M_PI,M_PI);
    deltaPhiMET = book<TH1F>("deltaPhiMET","#Delta#phi(#mu, p_{T}^{miss})", 50, 0, M_PI);
    isolation = book<TH1F>("isolation","muon isolation",50,0,0.2);
    charge = book<TH1F>("charge", "muon charge [e]",3,-1.5,1.5);
    ptrel = book<TH1F>("ptrel", "p_{T}^{rel}(#mu, jet)", 40, 0, 200.);
    deltaRmin = book<TH1F>("deltaRmin", "#DeltaR(#mu, jet)", 40, 0, 2.0);
    deltaRmin_ptrel = book<TH2F>("deltaRmin_ptrel", "x=#DeltaR(#mu, jet) y=p_{T}^{rel}(#mu, jet)", 40, 0, 2.0, 40, 0, 200.);
    // next-jet hists by Chris
    deltaPhimin = book<TH1F>("deltaPhimin", "#Delta#phi(#mu, jet)", 30, 0, M_PI);

    if (gen_plots) {
        eff_sub     = book<TH1F>("eff_sub",     "muon p_{T} [GeV]",                100,0,500);
        eff_tot     = book<TH1F>("eff_tot",     "muon p_{T} [GeV]",                100,0,500);
        pt_response = book<TH1F>("pt_response", "muon p_{T} response: (p_{T, gen} - p_{T, reco}) / p_{T, gen}",
                                                                        800,-1.,1.);
    } else {
        eff_sub     = 0;
        eff_tot     = 0;
        pt_response = 0;
    }
}

void MyMuonHists::fill(const Event & event){
    auto w = event.weight;
    assert(event.muons);
    number->Fill(event.muons->size(), w);

    if (eff_sub && event.genparticles) {
        for (const auto & gp: *event.genparticles) {
            if (abs(gp.pdgId()) == 13) {
                auto gp_pt = gp.pt();
                eff_tot->Fill(gp_pt, w);
                auto mu = closestParticle(gp, *event.muons);
                if (mu && deltaR(gp, *mu) < 0.1) {
                    eff_sub->Fill(gp_pt, w);
                    pt_response->Fill((gp_pt - mu->pt()) / gp_pt, w);
                }
            }
        }
    }

    // buffer values for ptrel and drmin to avoid recomputation:
    vector<float> drmin_buf;
    vector<float> ptrel_buf;
    vector<float> deltaPhi_buf;
    for(const auto & muon : *event.muons){
        pt->Fill(muon.pt(), w);
        eta->Fill(muon.eta(), w);
        phi->Fill(muon.phi(), w);
	deltaPhiMET->Fill(uhh2::deltaPhi(muon.v4(), event.met->v4()), w);
        isolation->Fill(muon.relIso(), w);
        charge->Fill(muon.charge(), w);
        
        if(event.jets){
            auto nj = nextJet(muon, *event.jets);
            auto drmin_val = nj ? deltaR(muon, *nj) : numeric_limits<float>::infinity();
	    auto deltaPhi_val = nj ? deltaPhi(muon, *nj) : numeric_limits<float>::infinity();
            drmin_buf.push_back(drmin_val);
            ptrel_buf.push_back(pTrel(muon, nj));
	    deltaPhi_buf.push_back(deltaPhi_val);
            ptrel->Fill(ptrel_buf.back(), w);
            deltaRmin->Fill(drmin_buf.back(), w);
            deltaRmin_ptrel->Fill(drmin_buf.back(), ptrel_buf.back(), w);
	    deltaPhimin->Fill(deltaPhi_buf.back(), w);
        }
    }
    

}
