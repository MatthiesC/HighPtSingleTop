#include "UHH2/HighPtSingleTop/include/MyUtils.h"
#include "UHH2/core/include/LorentzVector.h"


// credits to Torben
bool TopJetGroomer::process(uhh2::Event & event) { 

  assert(event.topjets);

  for(auto & topjet : *event.topjets){
    LorentzVector subjet_sum(0,0,0,0);

    for(const auto & subjet : topjet.subjets()){
      subjet_sum += subjet.v4();
          }
    topjet.set_v4(subjet_sum);
  }

  return true;
}

// returns the transverse W boson mass, using a given lepton and the MET in the event
double calcMTW(const FlavorParticle & l, const uhh2::Event & event) {
  
  assert(event.met);
  
  return sqrt(2*l.pt()*event.met->pt()*(1-cos(uhh2::deltaPhi(l, *event.met))));
}
