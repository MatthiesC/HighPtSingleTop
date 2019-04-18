#pragma once

#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/AnalysisModule.h"
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
