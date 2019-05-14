#include "UHH2/HighPtSingleTop/include/MyUtils.h"
#include "UHH2/core/include/LorentzVector.h"
#include "UHH2/core/include/Event.h"

#include <limits>

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


// based on uhh2::PrimaryLepton
FlavorParticle returnPrimaryLepton(const uhh2::Event & event) {

  assert(event.muons || event.electrons);

  double ptmax = -1;
  FlavorParticle primlep;

  if(event.electrons) {
    for(const auto & ele : *event.electrons) {
      float ele_pt = ele.pt();
      if(ele_pt > ptmax) {
	ptmax = ele_pt;
	primlep = ele;
      }
    }
  }
  if(event.muons) {
    for(const auto & mu : *event.muons) {
      float mu_pt = mu.pt();
      if(mu_pt > ptmax) {
	ptmax = mu_pt;
	primlep = mu;
      }
    }
  }

  return primlep;
}


// based on uhh2::closestParticle
const Jet * nextJetToMET(const uhh2::Event & event, const std::vector<Jet> & jets) {

  assert(event.met);

  double deltarmin = std::numeric_limits<double>::infinity();
  const Jet *next = 0;
  for(unsigned int i = 0; i < jets.size(); i++) {
    const Jet & ji = jets[i];
    double dr = uhh2::deltaR(ji.v4(), event.met->v4());
    if(dr < deltarmin) {
      deltarmin = dr;
      next = &ji;
    }
  }

  return next;
}


// for fun
void it_works() {
  std::cout << "IT HELLA WORKS! YEAH!" << std::endl;
  std::cout << "░░░░░░░░░░▄▄█▀▀▀▀▀▀▀▀█▄▄░░░░░░░░" << std::endl;
  std::cout << "░░░░░░░▄▄▀▀░░░░░░░░░░░░▀▀▄▄░░░░░" << std::endl;
  std::cout << "░░░░░▄█▀░░░░▄▀░░░░▄░░░░░░░▀█░░░░" << std::endl;
  std::cout << "░░░░██▄▄████░░░░░░▀▄░░░░░░░░█▄░░" << std::endl;
  std::cout << "░░▄████▀███▄▀▄░░░░░░███▄▄▄▄░░█░░" << std::endl;
  std::cout << "░▄█████▄████░██░░░▄███▄▄░▀█▀░░█░" << std::endl;
  std::cout << "▄███████▀▀░█░▄█░▄███▀█████░█░░▀▄" << std::endl;
  std::cout << "█░█▀██▄▄▄▄█▀░█▀█▀██████▀░██▀█░░█" << std::endl;
  std::cout << "█░█░▀▀▀▀▀░░░█▀░█░███▀▀░░▄█▀░█░░█" << std::endl;
  std::cout << "█░░█▄░░░░▄▄▀░░░█░▀██▄▄▄██▀░░█▄░█" << std::endl;
  std::cout << "█░░░░▀█▀▀▀░░░░░░█░░▀▀▀▀░░░░▄█░░█" << std::endl;
  std::cout << "█░░░░░░░░░░░░░░░░▀▄░░░░░░▄█▀░░░█" << std::endl;
  std::cout << "░█░░░░░░░░░░░░░░░░▀▀▀▀▀▀▀▄░░░░█░" << std::endl;
  std::cout << "░░█░░░░░░▄▄▄▄▄▄▄░░░░░░░░░░░░░▄▀░" << std::endl;
  std::cout << "░░░▀▄░░░░░▀█▄░░░▀▀██▄░░░░░░░▄▀░░" << std::endl;
  std::cout << "░░░░░▀▄▄░░░░░▀▀▀▀▀░░░░░░░░▄▀░░░░" << std::endl;
  std::cout << "░░░░░░░░▀▀▄▄▄░░░░░░░░▄▄▄▀▀█░░░░░" << std::endl;
  std::cout << "░░░░░░░░░░▄▀▀█████▀▀▀▀░░░░██░░░░" << std::endl;
  std::cout << "░░░░░░░░░█░░░██░░░█▀▀▀▀▀▀▀▀█░░░░" << std::endl;
  return;
}
