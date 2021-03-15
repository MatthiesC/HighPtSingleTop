#include "UHH2/HighPtSingleTop/include/MyUtils.h"
#include "UHH2/core/include/LorentzVector.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/ElectronIds.h"

#include <limits>


Channel extract_channel(const uhh2::Context & ctx) {

  Channel channel = Channel::notValid;

  std::string config = uhh2::string2lowercase(ctx.get("analysis_channel"));

  if(config.find("ele") != std::string::npos) channel = Channel::isEle;
  if(config.find("muo") != std::string::npos) channel = Channel::isMuo;

  if(channel == Channel::notValid) {
    std::runtime_error("extract_channel(): Invalid channel string in xml file. Please check.");
  }

  return channel;
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


// inverted MuonIso ID
InvMuonIso::InvMuonIso(double iso_): iso(iso_) {}

bool InvMuonIso::operator()(const Muon & muon, const uhh2::Event &) const {
  if(muon.relIso()<iso) return false;
  return true;
}


ElectronIso::ElectronIso(double iso_): iso(iso_) {}

bool ElectronIso::operator()(const Electron & ele, const uhh2::Event &) const {
  if(ele.relIso()>iso) return false;
  return true;
}


InvElectronIso::InvElectronIso(double iso_): iso(iso_) {}

bool InvElectronIso::operator()(const Electron & ele, const uhh2::Event &) const {
  if(ele.relIso()<iso) return false;
  return true;
}


bool inverted_ElectronID_Fall17_veto(const Electron& ele, const uhh2::Event& evt) { return !Electron_CutBasedID(ele, evt, "Fall17", "VETO", true); }

bool inverted_ElectronID_Fall17_tight(const Electron& ele, const uhh2::Event& evt) { return !Electron_CutBasedID(ele, evt, "Fall17", "TIGHT", true); }

double btagdisc(const Jet& jet, const BTag::algo& algo) {
  double result;
  if(algo == BTag::DEEPCSV) result = jet.btag_DeepCSV();
  else if(algo == BTag::DEEPJET) result = jet.btag_DeepJet();
  else if(algo == BTag::CSVV2) result = jet.btag_combinedSecondaryVertex();
  else throw std::invalid_argument("btagdisc(): invalid b-tagging algorithm passed");
  return result;
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
