#include "UHH2/HighPtSingleTop/include/DNNInputs.h"

#include "UHH2/LegacyTopTagging/include/Utils.h"

using namespace std;
using namespace uhh2;
using namespace uhh2::btw;
using namespace uhh2::ltt;

namespace uhh2 { namespace btw {

//____________________________________________________________________________________________________
DNNInputs::DNNInputs(Context & ctx):
  fYear(extract_year(ctx)),
  fChannel(extract_channel(ctx)),
  // fHandle_Region(ctx.get_handle<ERegion>(kHandleName_Region)),
  fHandle_Region_heavyTags(ctx.get_handle<ERegion_heavyTags>(kHandleName_Region_heavyTags)),
  fHandle_PrimaryLepton(ctx.get_handle<FlavorParticle>(kHandleName_PrimaryLepton)),
  fHandle_LeptonicHemisphere(ctx.get_handle<LeptonicHemisphere>(kHandleName_LeptonicHemisphere)),
  fHandle_pairedPUPPIjets(ctx.get_handle<vector<Jet>>(kHandleName_pairedPUPPIjets)),
  fHandle_CHSjets(ctx.get_handle<vector<Jet>>(kCollectionName_AK4CHS)),
  fHandle_TheTopJet(ctx.get_handle<TopJet>(kHandleName_TheTopJet)),
  fHandle_TheWJet(ctx.get_handle<TopJet>(kHandleName_TheWJet))
{
  cout << "Hello World from DNNInputs!" << endl;

  vector<DNNInput> inputs;
  uint i(0);

  //__________________________________________________
  // Meta
  inputs.push_back(DNNInput{i++, "year", "Year encoded as integer", -0.5, 4.5, 5});
  inputs.push_back(DNNInput{i++, "channel", "Channel (= lepton flavor) encoded as integer", -0.5, 2.5, 3});
  //__________________________________________________
  // Lepton
  inputs.push_back(DNNInput{i++, "lepton_pt_log", "log_{10}(1 + #it{p}_{T}^{lepton} / GeV)", 0, 4});
  inputs.push_back(DNNInput{i++, "lepton_eta", "#eta^{lepton}", -5, 5});
  inputs.push_back(DNNInput{i++, "lepton_phi", "#phi^{lepton} [rad]", -M_PI, M_PI});
  inputs.push_back(DNNInput{i++, "lepton_energy_log", "log_{10}(1 + #it{E}^{lepton} / GeV)", 0, 4});
  inputs.push_back(DNNInput{i++, "lepton_charge", "#it{Q}^{lepton} [#it{e}]", -1.5, 1.5, 3});
  //__________________________________________________
  // MET
  inputs.push_back(DNNInput{i++, "met_pt_log", "log_{10}(1 + #it{p}_{T}^{miss} / GeV)", 0, 4});
  inputs.push_back(DNNInput{i++, "met_phil", "#phi_{l=0}^{miss} [rad]", -M_PI, M_PI});
  inputs.push_back(DNNInput{i++, "met_phil_cos", "cos(#phi_{l=0}^{miss})", -1, 1});
  inputs.push_back(DNNInput{i++, "met_phil_sin", "sin(#phi_{l=0}^{miss})", -1, 1});
  //__________________________________________________
  // Neutrino
  inputs.push_back(DNNInput{i++, "nu_pt_log", "log_{10}(1 + #it{p}_{T}^{#nu} / GeV)", 0, 4});
  inputs.push_back(DNNInput{i++, "nu_eta", "#eta^{#nu}", -5, 5});
  // phi values are the same as for MET
  inputs.push_back(DNNInput{i++, "nu_energy_log", "log_{10}(1 + #it{E}^{#nu} / GeV)", 0, 4});
  //__________________________________________________
  // Jet of leptonic hemisphere
  inputs.push_back(DNNInput{i++, "hemijet_pt_log", "log_{10}(1 + #it{p}_{T}^{hemijet} / GeV)", 0, 4});
  inputs.push_back(DNNInput{i++, "hemijet_eta", "#eta^{hemijet}", -5, 5});
  inputs.push_back(DNNInput{i++, "hemijet_phil", "#phi_{l=0}^{hemijet} [rad]", -M_PI, M_PI});
  inputs.push_back(DNNInput{i++, "hemijet_phil_cos", "cos(#phi_{l=0}^{hemijet})", -1, 1});
  inputs.push_back(DNNInput{i++, "hemijet_phil_sin", "sin(#phi_{l=0}^{hemijet})", -1, 1});
  inputs.push_back(DNNInput{i++, "hemijet_energy_log", "log_{10}(1 + #it{E}^{hemijet} / GeV)", 0, 4});
  inputs.push_back(DNNInput{i++, "hemijet_deepjet", "#it{O}_{DeepJet}^{hemijet}", 0, 1});
  //__________________________________________________
  // Leptonic hemisphere total
  inputs.push_back(DNNInput{i++, "hemi_pt_log", "log_{10}(1 + #it{p}_{T}^{hemi} / GeV)", 0, 4});
  inputs.push_back(DNNInput{i++, "hemi_eta", "#eta^{hemi}", -5, 5});
  inputs.push_back(DNNInput{i++, "hemi_phil", "#phi_{l=0}^{hemi} [rad]", -M_PI, M_PI});
  inputs.push_back(DNNInput{i++, "hemi_phil_cos", "cos(#phi_{l=0}^{hemi})", -1, 1});
  inputs.push_back(DNNInput{i++, "hemi_phil_sin", "sin(#phi_{l=0}^{hemi})", -1, 1});
  inputs.push_back(DNNInput{i++, "hemi_energy_log", "log_{10}(1 + #it{E}^{hemi} / GeV)", 0, 4});
  inputs.push_back(DNNInput{i++, "hemi_mass_log", "log_{10}(1 + #it{m}^{hemi} / GeV)", 0, 4});
  //__________________________________________________
  // Tagged fat jet
  inputs.push_back(DNNInput{i++, "thetag_pt_log", "log_{10}(1 + #it{p}_{T}^{the tag} / GeV)", 0, 4});
  inputs.push_back(DNNInput{i++, "thetag_eta", "#eta^{the tag}", -5, 5});
  inputs.push_back(DNNInput{i++, "thetag_phil", "#phi_{l=0}^{the tag} [rad]", -M_PI, M_PI});
  inputs.push_back(DNNInput{i++, "thetag_phil_cos", "cos(#phi_{l=0}^{the tag})", -1, 1});
  inputs.push_back(DNNInput{i++, "thetag_phil_sin", "sin(#phi_{l=0}^{the tag})", -1, 1});
  inputs.push_back(DNNInput{i++, "thetag_energy_log", "log_{10}(1 + #it{E}^{the tag} / GeV)", 0, 4});
  //__________________________________________________
  // Hadronic recoil = vectorial sum of AK4 jets
  inputs.push_back(DNNInput{i++, "hadrecoil_pt_log", "log_{10}(1 + #it{p}_{T}^{had. recoil} / GeV)", 0, 4});
  inputs.push_back(DNNInput{i++, "hadrecoil_eta", "#eta^{had. recoil}", -5, 5});
  inputs.push_back(DNNInput{i++, "hadrecoil_phil", "#phi_{l=0}^{had. recoil} [rad]", -M_PI, M_PI});
  inputs.push_back(DNNInput{i++, "hadrecoil_phil_cos", "cos(#phi_{l=0}^{had. recoil})", -1, 1});
  inputs.push_back(DNNInput{i++, "hadrecoil_phil_sin", "sin(#phi_{l=0}^{had. recoil})", -1, 1});
  inputs.push_back(DNNInput{i++, "hadrecoil_energy_log", "log_{10}(1 + #it{E}^{had. recoil} / GeV)", 0, 4});
  //__________________________________________________
  // All AK4 jets
  inputs.push_back(DNNInput{i++, "ak4_pt_sum_log", "log_{10}(1 + #Sigma_{AK4 jets}#it{p}_{T} / GeV)", 0, 4});
  inputs.push_back(DNNInput{i++, "ak4_multiplicity", "AK4 jet multiplicity", -0.5, 10.5, 11});
  //__________________________________________________
  // Individual AK4 jets
  for(uint j = 1; j <= fNJetsLimit; j++) {
    inputs.push_back(DNNInput{i++, "ak4jet"+to_string(j)+"_pt_log", "log_{10}(1 + #it{p}_{T}^{ak4jet"+to_string(j)+"} / GeV)", 0, 4});
    inputs.push_back(DNNInput{i++, "ak4jet"+to_string(j)+"_eta", "#eta^{ak4jet"+to_string(j)+"}", -5, 5});
    inputs.push_back(DNNInput{i++, "ak4jet"+to_string(j)+"_phil", "#phi_{l=0}^{ak4jet"+to_string(j)+"} [rad]", -M_PI, M_PI});
    inputs.push_back(DNNInput{i++, "ak4jet"+to_string(j)+"_phil_cos", "cos(#phi_{l=0}^{ak4jet"+to_string(j)+"})", -1, 1});
    inputs.push_back(DNNInput{i++, "ak4jet"+to_string(j)+"_phil_sin", "sin(#phi_{l=0}^{ak4jet"+to_string(j)+"})", -1, 1});
    inputs.push_back(DNNInput{i++, "ak4jet"+to_string(j)+"_energy_log", "log_{10}(1 + #it{E}^{ak4jet"+to_string(j)+"} / GeV)", 0, 4});
    inputs.push_back(DNNInput{i++, "ak4jet"+to_string(j)+"_deepjet", "#it{O}_{DeepJet}^{ak4jet"+to_string(j)+"}", 0, 1});
  }

  cout << "Number of prepared DNN inputs: " << inputs.size() << endl;
  cout << "Float value for 'zero padding': " << fZeroPaddingValue << endl;

  for(auto & input : inputs) {
    input.name = (string)"dnnInput__"+input.name;
    fMap_Inputs[input.name] = input;
    fInputHandles.push_back(ctx.declare_event_output<float>(input.name));
  }
}

void DNNInputs::set_inputs(Event & event, const ERegion_heavyTags & region_heavyTags) {
  const FlavorParticle & primlep = event.get(fHandle_PrimaryLepton);
  const LeptonicHemisphere & hemi = event.get(fHandle_LeptonicHemisphere);
  const vector<Jet> & central_jets = event.get(fHandle_pairedPUPPIjets); // sorted by pt
  TopJet the_tag = TopJet();
  if(region_heavyTags == ERegion_heavyTags::_1t) the_tag = event.get(fHandle_TheTopJet);
  else if(region_heavyTags == ERegion_heavyTags::_0t1W) the_tag = event.get(fHandle_TheWJet);
  Particle hadronic_recoil;
  float jet_pt_sum(0.f);
  float jet_energy_sum(0.f);
  for(const Jet & jet : central_jets) {
    hadronic_recoil.set_v4(hadronic_recoil.v4() + jet.v4());
    hadronic_recoil.set_charge(hadronic_recoil.charge() + jet.charge());
    jet_pt_sum += jet.pt();
    jet_energy_sum += jet.energy();
  }

  vector<float> inputs;
  inputs.resize(fInputHandles.size(), fZeroPaddingValue);
  uint i(0);
  //__________________________________________________
  // Meta
  inputs.at(i++) = kYears.at(fYear).index;
  inputs.at(i++) = kChannels.at(fChannel).index;
  //__________________________________________________
  // Lepton
  inputs.at(i++) = log10( 1 + primlep.pt() );
  inputs.at(i++) = primlep.eta();
  inputs.at(i++) = primlep.phi();
  inputs.at(i++) = log10( 1 + primlep.energy() );
  inputs.at(i++) = primlep.charge();
  //__________________________________________________
  // MET
  inputs.at(i++) = log10( 1 + event.met->pt() );
  inputs.at(i++) = signedDeltaPhi( *event.met, primlep );
  inputs.at(i++) = cos( signedDeltaPhi( *event.met, primlep ) );
  inputs.at(i++) = sin( signedDeltaPhi( *event.met, primlep ) );
  //__________________________________________________
  // Neutrino
  inputs.at(i++) = log10( 1 + hemi.neutrino().pt() ); // not the same as MET pt if kappa != 1
  inputs.at(i++) = hemi.neutrino().eta();
  // phi values are the same as for MET
  inputs.at(i++) = log10( 1 + hemi.neutrino().energy() );
  //__________________________________________________
  // Jet of leptonic hemisphere
  inputs.at(i++) = log10( 1 + hemi.jet().pt() );
  inputs.at(i++) = hemi.jet().eta();
  inputs.at(i++) = signedDeltaPhi( hemi.jet(), primlep );
  inputs.at(i++) = cos( signedDeltaPhi( hemi.jet(), primlep ) );
  inputs.at(i++) = sin( signedDeltaPhi( hemi.jet(), primlep ) );
  inputs.at(i++) = log10( 1 + hemi.jet().energy() );
  inputs.at(i++) = hemi.btag_DeepJet();
  //__________________________________________________
  // Leptonic hemisphere total
  inputs.at(i++) = log10( 1 + hemi.pt() );
  inputs.at(i++) = hemi.eta();
  inputs.at(i++) = signedDeltaPhi( hemi, primlep );
  inputs.at(i++) = cos( signedDeltaPhi( hemi, primlep ) );
  inputs.at(i++) = sin( signedDeltaPhi( hemi, primlep ) );
  inputs.at(i++) = log10( 1 + hemi.energy() );
  inputs.at(i++) = log10( 1 + hemi.v4().M() ); // top mass
  //__________________________________________________
  // Tagged fat jet
  inputs.at(i++) = log10( 1 + the_tag.pt() );
  inputs.at(i++) = the_tag.eta();
  inputs.at(i++) = signedDeltaPhi( the_tag, primlep );
  inputs.at(i++) = cos( signedDeltaPhi( the_tag, primlep ) );
  inputs.at(i++) = sin( signedDeltaPhi( the_tag, primlep ) );
  inputs.at(i++) = log10( 1 + the_tag.energy() );
  //__________________________________________________
  // Hadronic recoil = vectorial sum of AK4 jets
  inputs.at(i++) = log10( 1 + hadronic_recoil.pt() );
  inputs.at(i++) = hadronic_recoil.eta();
  inputs.at(i++) = signedDeltaPhi( hadronic_recoil, primlep );
  inputs.at(i++) = cos( signedDeltaPhi( hadronic_recoil, primlep ) );
  inputs.at(i++) = sin( signedDeltaPhi( hadronic_recoil, primlep ) );
  inputs.at(i++) = log10( 1 + hadronic_recoil.energy() );
  //__________________________________________________
  // All AK4 jets
  inputs.at(i++) = log10( 1 + jet_pt_sum );
  inputs.at(i++) = central_jets.size();
  //__________________________________________________
  // Individual AK4 jets
  const uint fNJetsLimit = 6; // maximum number of jets that may be considered for inputs
  uint j_max = min( fNJetsLimit, (uint)central_jets.size() );
  for(uint j = 0; j < j_max; j++) {
    const Jet & jet = central_jets.at(j);
    inputs.at(i++) = log10( 1 + jet.pt() );
    inputs.at(i++) = jet.eta();
    inputs.at(i++) = signedDeltaPhi( jet, primlep );
    inputs.at(i++) = cos( signedDeltaPhi( jet, primlep ) );
    inputs.at(i++) = sin( signedDeltaPhi( jet, primlep ) );
    inputs.at(i++) = log10( 1 + jet.energy() );
    inputs.at(i++) = getCHSmatch(jet, event, fHandle_CHSjets)->btag_DeepJet();
  }
  // uint n_jets_missing = max( (uint)0, fNJetsLimit - (uint)central_jets.size() );
  // for(uint j = 0; j < n_jets_missing*7; j++) {
  //   inputs.at(i++) = fZeroPaddingValue;
  // }

  // if(inputs.size() != fInputHandles.size()) throw logic_error("DNNInputs::set_inputs(): values vector and handles vector do not have the same size");

  for(uint h = 0; h < fInputHandles.size(); h++) {
    event.set(fInputHandles.at(h), inputs.at(h));
  }
}

void DNNInputs::set_dummy_inputs(Event & event) {
  for(uint h = 0; h < fInputHandles.size(); h++) {
    event.set(fInputHandles.at(h), fZeroPaddingValue);
  }
}

bool DNNInputs::process(Event & event) {
  const ERegion_heavyTags & region_heavyTags = event.get(fHandle_Region_heavyTags);
  if(region_heavyTags == ERegion_heavyTags::_1t || region_heavyTags == ERegion_heavyTags::_0t1W) {
    set_inputs(event, region_heavyTags);
  }
  else {
    set_dummy_inputs(event);
  }
  return true;
}

}}
