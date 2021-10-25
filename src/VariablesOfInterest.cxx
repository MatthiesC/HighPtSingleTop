#include "UHH2/HighPtSingleTop/include/VariablesOfInterest.h"

using namespace std;
using namespace uhh2;


VariablesOfInterest::VariablesOfInterest(Context & ctx) {

  m_h_generic_region = ctx.get_handle<GenericRegion>("generic_region");

  m_h_tjet = ctx.get_handle<TopJet>("TopTaggedJet");
  m_h_wjet = ctx.get_handle<TopJet>("WTaggedJet");
  m_h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
  m_h_wboson = ctx.get_handle<LorentzVector>("WBosonLeptonic");
  m_h_pseudotop = ctx.get_handle<LorentzVector>("PseudoTop");

  /*
  * Observables in t-tag region
  */
  m_h_voi_ttag__tjet_pt = ctx.declare_event_output<double>("VOI_ttag__tjet_pt");
  m_h_voi_ttag__tjet_eta = ctx.declare_event_output<double>("VOI_ttag__tjet_eta");
  m_h_voi_ttag__lepton_pt = ctx.declare_event_output<double>("VOI_ttag__lepton_pt");
  m_h_voi_ttag__tW_dphi = ctx.declare_event_output<double>("VOI_ttag__tW_dphi");
  m_h_voi_ttag__tW_mass = ctx.declare_event_output<double>("VOI_ttag__tW_mass");

  /*
  * Observables in W-tag region
  */
  m_h_voi_wtag__pseudotop_pt = ctx.declare_event_output<double>("VOI_wtag__pseudotop_pt");
  m_h_voi_wtag__pseudotop_eta = ctx.declare_event_output<double>("VOI_wtag__pseudotop_eta");
  m_h_voi_wtag__lepton_pt = ctx.declare_event_output<double>("VOI_wtag__lepton_pt");
  m_h_voi_wtag__tW_dphi = ctx.declare_event_output<double>("VOI_wtag__tW_dphi");
  m_h_voi_wtag__tW_mass = ctx.declare_event_output<double>("VOI_wtag__tW_mass");
}


void VariablesOfInterest::set_ttag_variables(Event & event) {

  const TopJet & tjet = event.get(m_h_tjet);
  const FlavorParticle & lepton = event.get(m_h_primlep);
  const LorentzVector & wboson = event.get(m_h_wboson);

  event.set(m_h_voi_ttag__tjet_pt, tjet.v4().Pt());
  event.set(m_h_voi_ttag__tjet_eta, tjet.v4().Eta());
  event.set(m_h_voi_ttag__lepton_pt, lepton.v4().Pt());
  event.set(m_h_voi_ttag__tW_dphi, deltaPhi(tjet.v4(), wboson));
  event.set(m_h_voi_ttag__tW_mass, (tjet.v4() + wboson).M());

  return;
}


void VariablesOfInterest::set_ttag_dummies(Event & event) {

  event.set(m_h_voi_ttag__tjet_pt, 0.);
  event.set(m_h_voi_ttag__tjet_eta, 0.);
  event.set(m_h_voi_ttag__lepton_pt, 0.);
  event.set(m_h_voi_ttag__tW_dphi, 0.);
  event.set(m_h_voi_ttag__tW_mass, 0.);

  return;
}


void VariablesOfInterest::set_wtag_variables(Event & event) {

  const TopJet & wjet = event.get(m_h_wjet);
  const FlavorParticle & lepton = event.get(m_h_primlep);
  const LorentzVector & pseudotop = event.get(m_h_pseudotop);

  event.set(m_h_voi_wtag__pseudotop_pt, pseudotop.Pt());
  event.set(m_h_voi_wtag__pseudotop_eta, pseudotop.Eta());
  event.set(m_h_voi_wtag__lepton_pt, lepton.v4().Pt());
  event.set(m_h_voi_wtag__tW_dphi, deltaPhi(pseudotop, wjet.v4()));
  event.set(m_h_voi_wtag__tW_mass, (pseudotop + wjet.v4()).M());

  return;
}


void VariablesOfInterest::set_wtag_dummies(Event & event) {

  event.set(m_h_voi_wtag__pseudotop_pt, 0.);
  event.set(m_h_voi_wtag__pseudotop_eta, 0.);
  event.set(m_h_voi_wtag__lepton_pt, 0.);
  event.set(m_h_voi_wtag__tW_dphi, 0.);
  event.set(m_h_voi_wtag__tW_mass, 0.);

  return;
}


bool VariablesOfInterest::process(Event & event) {

  const GenericRegion generic_region = event.get(m_h_generic_region);

  if(generic_region == GenericRegion::TopTag) {
    set_ttag_variables(event);
    set_wtag_dummies(event);
  }
  else if(generic_region == GenericRegion::WTag) {
    set_wtag_variables(event);
    set_ttag_dummies(event);
  }
  else if(generic_region == GenericRegion::Veto) {
    set_ttag_variables(event);
    set_wtag_variables(event);
  }
  else {
    throw runtime_error("VariablesOfInterest::process(): Calling this function on event that does not belong to a valid analysis region. Make sure to discard invalid events beforehand.");
  }

  return true;
}
