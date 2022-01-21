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







//// Cut-Based ID

bool MyElectron_CutBasedID_invertedIso(const Electron& ele_, const uhh2::Event& evt_, const std::string& tuning_, const std::string& wp_, const bool apply_iso_cut_){

  bool pass(false);

  int wp_idx(-1);
  if     (wp_ == "VETO"  ) wp_idx = 0;
  else if(wp_ == "LOOSE" ) wp_idx = 1;
  else if(wp_ == "MEDIUM") wp_idx = 2;
  else if(wp_ == "TIGHT" ) wp_idx = 3;
  else throw std::runtime_error("MyElectron_CutBasedID_invertedIso -- undefined working-point tag: "+wp_);

  const float abs_etaSC(fabs(ele_.supercluster_eta()));

  if     (                     abs_etaSC <= 1.479) pass = MyElectron_CutBasedID_invertedIso(ele_, evt_, tuning_, "barrel", wp_idx, apply_iso_cut_);
  else if(1.479 < abs_etaSC && abs_etaSC <  2.5  ) pass = MyElectron_CutBasedID_invertedIso(ele_, evt_, tuning_, "endcap", wp_idx, apply_iso_cut_);

  return pass;
}

bool MyElectron_CutBasedID_invertedIso(const Electron& ele_, const uhh2::Event& evt_, const std::string& tuning_, const std::string& eleSC_pos_, const int wp_idx_, const bool apply_iso_cut_){

  assert(evt_.pvs);
  if(!evt_.pvs->size()) return false;
  const auto& pv = (*evt_.pvs)[0];

  const float abs_d0 = fabs(ele_.gsfTrack_dxy_vertex(pv.x(), pv.y()));
  const float abs_dz = fabs(ele_.gsfTrack_dz_vertex (pv.x(), pv.y(), pv.z()));

  if(ele_.EcalEnergy() <= 0.) return false;
  const float abs_ooEmooP = fabs(1./ele_.EcalEnergy() - ele_.EoverPIn()/ele_.EcalEnergy());

  /*float C0 = -1.;
  const float C0_barrel = ele_.HoverE() - 1.12/ele_.EcalEnergy() - (0.0368*evt_.rho)/ele_.EcalEnergy();
  const float C0_endcap = ele_.HoverE() - 0.5/ele_.EcalEnergy() - (0.201*evt_.rho)/ele_.EcalEnergy();
  if(eleSC_pos_ == "barrel") C0 = C0_barrel;
  else if(eleSC_pos_ == "endcap") C0 = C0_endcap;
  else throw std::runtime_error("Invalid value for variable eleSC_pos_. May be 'barrel' or 'endcap'.");
  */
  float C0 = ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("HoverE_C0").at(wp_idx_);
  float C1 = ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("HoverE_C1").at(wp_idx_);
  float C2 = ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("HoverE_C2").at(wp_idx_);
  float HoverE_cut = C0+C1/ele_.EcalEnergy()+C2*evt_.rho/ele_.EcalEnergy();

  const int expMissingHits = ele_.gsfTrack_trackerExpectedHitsInner_numberOfLostHits();

  const int passConvVeto = int(ele_.passconversionveto());

  if(!( ele_.sigmaIEtaIEta() < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("sigmaIetaIeta").at(wp_idx_)  )) return false;// sigmaIetaIeta
  if(!( fabs(ele_.dEtaIn())  < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("|dEtaIn|")     .at(wp_idx_)  )) return false;// |dEtaIn|
  if(!( fabs(ele_.dPhiIn())  < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("|dPhiIn|")     .at(wp_idx_)  )) return false;// |dPhiIn|
  if(!( ele_.HoverE()        < HoverE_cut)) return false;// HoverE
  if(!( abs_ooEmooP          < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("|ooEmooP|")    .at(wp_idx_)  )) return false;// |ooEmooP|
  if(!( abs_d0               < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("|d0|")         .at(wp_idx_)  )) return false;// |d0|
  if(!( abs_dz               < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("|dz|")         .at(wp_idx_)  )) return false;// |dz|
  if(!( expMissingHits  <= int(ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("missingHits")  .at(wp_idx_)) )) return false;// expected missing inner hits
  if(!( passConvVeto    >= int(ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("passConvVeto") .at(wp_idx_)) )) return false;// conversion veto

  if(apply_iso_cut_){
    float relIso_C0 = ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("relIsoEA_C0").at(wp_idx_);
    float relIso_C1 = ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("relIsoEA_C1").at(wp_idx_);
    float relIso_cut = relIso_C0 + relIso_C1/ele_.pt();

    const float pfIsoEA = ele_.relIsorho(evt_.rho);
    if(!( pfIsoEA            > relIso_cut )) return false;// pfIso (PU correction: effective-area)
  }

  return true;
}

bool MyElectronID_Fall17_tight_invertedIso (const Electron& ele, const uhh2::Event& evt){ return MyElectron_CutBasedID_invertedIso(ele, evt, "Fall17", "TIGHT" , true) ; }







double btagdisc(const Jet& jet, const BTag::algo& algo) {
  double result;
  if(algo == BTag::DEEPCSV) result = jet.btag_DeepCSV();
  else if(algo == BTag::DEEPJET) result = jet.btag_DeepJet();
  // else if(algo == BTag::CSVV2) result = jet.btag_combinedSecondaryVertex(); // not anymore present in 106X_v2
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
