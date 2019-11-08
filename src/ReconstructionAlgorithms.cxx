#include "UHH2/HighPtSingleTop/include/ReconstructionAlgorithms.h"
#include "UHH2/common/include/TTbarReconstruction.h"


using namespace std;
using namespace uhh2;


WBosonLeptonic::WBosonLeptonic(Context & ctx,
			       const string & h_name_wboson,
			       const string & h_primlep_name):
  h_wboson(ctx.get_handle<LorentzVector>(h_name_wboson)),
  h_primlep(ctx.get_handle<FlavorParticle>(h_primlep_name))
{}


bool WBosonLeptonic::process(Event & event) {
  assert(event.met);
  
  const FlavorParticle & primlep = event.get(h_primlep);
  vector<LorentzVector> neutrinos = NeutrinoReconstruction(primlep.v4(), event.met->v4());

  vector<LorentzVector> solutions;
  for(int i = 0; i < neutrinos.size(); i++) {
    solutions.push_back(neutrinos.at(i) + primlep.v4());
  }

  // take the neutrino solution that has smaller absolute value of pZ
  LorentzVector wboson = solutions.at(0);
  if(neutrinos.size() > 1 && abs(solutions.at(1).Pz()) < abs(solutions.at(0).Pz())) {
    wboson = solutions.at(1);
  }
  event.set(h_wboson, wboson);

  return true;
}


