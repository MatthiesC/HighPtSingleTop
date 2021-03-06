#pragma once

#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/FlavorParticle.h"
#include "UHH2/core/include/Jet.h"
#include "UHH2/core/include/Utils.h"
#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/Utils.h"


/** Match object p with genparticle from list of genparticles genparts. A
 * particle is matched to a genparticle if it is closest and is within
 * dR_min of the genparticle Returns NULL if no match was found
 */

template<typename T>
const T* match(const Particle &p, const std::vector<T> &genparts, double dR_min) {
  double closest_dR = DBL_MAX;
  const T* matched = NULL;
  for (auto &genp : genparts)
    {
      double dR = uhh2::deltaR(genp, p);
      if (dR < dR_min && dR < closest_dR)
	{
	  closest_dR = dR;
	  matched = &genp;
	}
    }
  return matched;
}


enum class Channel {
  isEle,
  isMuo,
  notValid,
};

Channel extract_channel(const uhh2::Context&);

// returns the transverse W boson mass, using a given lepton and the MET in the event
double calcMTW(const FlavorParticle&, const uhh2::Event&);

FlavorParticle returnPrimaryLepton(const uhh2::Event&);

const Jet * nextJetToMET(const uhh2::Event&, const std::vector<Jet>&);

double calcHOTVRmpair(const TopJet & hotvrjet);

double calcHOTVRfpt(const TopJet & hotvrjet, const unsigned int & i_sub=0);

// inverted MuonIso ID
class InvMuonIso {
public:
  InvMuonIso(double iso_);
  bool operator()(const Muon & muon, const uhh2::Event & event) const;
private:
  double iso;
};

class ElectronIso {
public:
  ElectronIso(double iso_);
  bool operator()(const Electron & ele, const uhh2::Event & event) const;
private:
  double iso;
};

class InvElectronIso {
public:
  InvElectronIso(double iso_);
  bool operator()(const Electron & ele, const uhh2::Event & event) const;
private:
  double iso;
};

bool inverted_ElectronID_Fall17_veto(const Electron&, const uhh2::Event&);

bool inverted_ElectronID_Fall17_tight(const Electron&, const uhh2::Event&);

double btagdisc(const Jet&, const BTag::algo&);

// Sort jets by DeepJet discriminant
inline void sort_by_deepjet(std::vector<Jet> & jets) {
  std::sort(jets.begin(), jets.end(), [](const Jet & j1, const Jet & j2){return j1.btag_DeepJet() > j2.btag_DeepJet();});
}

// Sort jets by DeepCSV discriminant
inline void sort_by_deepcsv(std::vector<Jet> & jets) {
  std::sort(jets.begin(), jets.end(), [](const Jet & j1, const Jet & j2){return j1.btag_DeepCSV() > j2.btag_DeepCSV();});
}

inline void sort_by_btagdisc(std::vector<Jet> & jets, const BTag::algo algo) {
  if(algo == BTag::DEEPJET) sort_by_deepjet(jets);
  else if(algo == BTag::DEEPCSV) sort_by_deepcsv(jets);
  else throw std::invalid_argument("sort_by_btagdisc(): invalid b-tagging algorithm passed");
}

void it_works();
