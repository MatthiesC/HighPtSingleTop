#include "UHH2/HighPtSingleTop/include/MyUtils.h"
#include "UHH2/core/include/LorentzVector.h"
#include "UHH2/core/include/Event.h"

#include <limits>


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


// return HOTVR jet's min. m_ij of three leading subjets
double calcHOTVRmpair(const TopJet & hotvrjet) {

  std::vector<Jet> subjets = hotvrjet.subjets();

  if(subjets.size() < 3) throw std::runtime_error("calcHOTVRmpair: Using this function on a HOTVR jet with less than 3 subjets is not possible!");

  sort_by_pt<Jet>(subjets);

  double m01 = (subjets.at(0).v4() + subjets.at(1).v4()).M();
  double m12 = (subjets.at(1).v4() + subjets.at(2).v4()).M();
  double m02 = (subjets.at(0).v4() + subjets.at(2).v4()).M();

  return std::min(m01, std::min(m12, m02));
}


// return HOTVR jet's f_pt(subi) [sub1 is default]
double calcHOTVRfpt(const TopJet & hotvrjet, const unsigned int & i_sub) {

  std::vector<Jet> subjets = hotvrjet.subjets();

  sort_by_pt<Jet>(subjets);

  return subjets.at(i_sub).v4().pt() / hotvrjet.v4().pt();
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
