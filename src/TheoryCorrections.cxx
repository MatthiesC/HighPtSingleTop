#include "UHH2/HighPtSingleTop/include/TheoryCorrections.h"

using namespace std;
using namespace uhh2;


/*
 * According to this twiki:
 * https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopPtReweighting
 * one is free to apply "data-NLO" weights in a tW measurement
*/

TopPtReweighting::TopPtReweighting(Context & ctx, double a, double b, const string& weight_name):
  a_(a),
  b_(b),
  h_weight(ctx.declare_event_output<double>(weight_name)) {}

bool TopPtReweighting::process(Event & event) {

  double weight(1.);
  double pt_top(-1.);
  double pt_antitop(-1.);

  for(GenParticle gp : *event.genparticles) {
    if(gp.pdgId() == 6) pt_top = gp.v4().Pt();
    else if(gp.pdgId() == -6) pt_antitop = gp.v4().Pt();
  }
  if(pt_top < 0 || pt_antitop < 0) throw runtime_error("TopPtReweighting::process(): top or antitop pT is negative.");
  pt_top = min(pt_top, 500.);
  pt_antitop = min(pt_antitop, 500.);

  weight = sqrt(exp(a_+b_*pt_top)*exp(a_+b_*pt_antitop));

  event.weight *= weight;
  event.set(h_weight, weight);

  return true;
}

bool TopPtReweighting::process_dummy(Event & event) {

  event.set(h_weight, -1.);

  return true;
}
