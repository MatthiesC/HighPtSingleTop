#pragma once

#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/FlavorParticle.h"
#include "UHH2/core/include/Jet.h"
#include "UHH2/common/include/JetCorrections.h"
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



// credits to Torben
class TopJetGroomer : public uhh2::AnalysisModule {
 public:

  explicit TopJetGroomer(bool corrected = true) : _corrected(corrected) {}
  virtual bool process(uhh2::Event & event) override;
 
 private:
  bool _corrected;

};


// returns the transverse W boson mass, using a given lepton and the MET in the event
double calcMTW(const FlavorParticle&, const uhh2::Event&);

FlavorParticle returnPrimaryLepton(const uhh2::Event&);

const Jet * nextJetToMET(const uhh2::Event&, const std::vector<Jet>&);

void it_works();


// Sort jets by DeepJet discriminant
inline void sort_by_deepjet(std::vector<Jet> & jets) {
  std::sort(jets.begin(), jets.end(), [](const Jet & j1, const Jet & j2){return j1.btag_DeepJet() > j2.btag_DeepJet();});
}
