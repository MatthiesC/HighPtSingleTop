#include "UHH2/HighPtSingleTop/include/GenSingleTopQuarkHists.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;


GenSingleTopQuarkHists::GenSingleTopQuarkHists(Context & ctx, const string & dirname):
  Hists(ctx, dirname) {

  hist_topQuarkPt = book<TH1F>("topQuarkPt", "top quark p_{T} [GeV]", 1500, 0, 1500);
  hist_topQuarkPt_ggFusion = book<TH1F>("topQuarkPt_ggFusion", "top quark p_{T} [GeV]", 1500, 0, 1500);
  hist_topQuarkPt_qqAnnihilation = book<TH1F>("topQuarkPt_qqAnnihilation", "top quark p_{T} [GeV]", 1500, 0, 1500);
  hist_topQuarkPt_gbFusion = book<TH1F>("topQuarkPt_gbFusion", "top quark p_{T} [GeV]", 1500, 0, 1500);
  hist_topQuarkPt_else = book<TH1F>("topQuarkPt_else", "top quark p_{T} [GeV]", 1500, 0, 1500);
}


void GenSingleTopQuarkHists::fill(const uhh2::Event & event) {

  GenParticle top, initial1, initial2;
  for(auto gp : *event.genparticles) {
    if(gp.index() == 0) initial1 = gp;
    else if(gp.index() == 1) initial2 = gp;
    if(abs(gp.pdgId()) == 6) { top = gp; break; };
  }

  hist_topQuarkPt->Fill(top.v4().pt(), event.weight);

  bool is_ggFusion = initial1.pdgId() == 21 && initial2.pdgId() == 21;
  bool is_qqAnnihilation = initial1.pdgId() == -initial2.pdgId() && abs(initial1.pdgId()) <= 6;
  bool is_gbFusion = (initial1.pdgId() == 21 && abs(initial2.pdgId()) == 5) || (abs(initial1.pdgId()) == 5 && initial2.pdgId() == 21);

  if(is_ggFusion) {
    hist_topQuarkPt_ggFusion->Fill(top.v4().pt(), event.weight);
  }
  else if(is_qqAnnihilation) {
    hist_topQuarkPt_qqAnnihilation->Fill(top.v4().pt(), event.weight);
  }
  else if(is_gbFusion) {
    hist_topQuarkPt_gbFusion->Fill(top.v4().pt(), event.weight);
  }
  else {
    hist_topQuarkPt_else->Fill(top.v4().pt(), event.weight);
  }
}
