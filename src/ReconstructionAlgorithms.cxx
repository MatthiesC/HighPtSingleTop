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
  for(unsigned int i = 0; i < neutrinos.size(); i++) {
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


PseudoTopLeptonic::PseudoTopLeptonic(Context & ctx,
				     const string & h_name_wboson,
				     const string & h_name_bjets,
				     const string & h_name_pseudotop,
				     const double topmassMC):
  h_wboson(ctx.get_handle<LorentzVector>(h_name_wboson)),
  h_bjets(ctx.get_handle<vector<Jet>>(h_name_bjets)),
  h_pseudotop(ctx.get_handle<LorentzVector>(h_name_pseudotop)),
  m_topmassMC(topmassMC)
{}


bool PseudoTopLeptonic::process(Event & event) {
  assert(event.jets);

  const LorentzVector wboson = event.get(h_wboson);
  const vector<Jet> bjets = event.get(h_bjets);
  const vector<Jet> jets = *event.jets;

  vector<LorentzVector> solutions;

  if(bjets.size() > 0) {
    for(unsigned int i = 0; i < bjets.size(); ++i) {
      solutions.push_back(wboson + bjets.at(i).v4());
    }
  }
  else { // in case that there are no b quark candidates, loop over all AK4 jets
    for(unsigned int i = 0; i < jets.size(); ++i) {
      solutions.push_back(wboson + jets.at(i).v4());
    }
  }

  // take the solution that is closest to MC top quark mass (usually 172.5 GeV)
  LorentzVector pseudotop = solutions.at(0);
  for(unsigned int i = 1; i < solutions.size(); ++i) {
    if(abs(solutions.at(i).M() - m_topmassMC) < abs(pseudotop.M() - m_topmassMC)) {
      pseudotop = solutions.at(i);
    }
  }
  event.set(h_pseudotop, std::move(pseudotop));

  return true;
}
