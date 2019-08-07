#include "UHH2/HighPtSingleTop/include/DNNSetup.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"

using namespace std;
using namespace uhh2;


DNNSetup::DNNSetup(Context & ctx, vector<Event::Handle<float>> & h_dnn_inputs, const unsigned int & numberOfHotvrJets, const unsigned int & numberOfAk4Jets, const float & zero_padding_value):
  m_h_dnn_inputs(h_dnn_inputs), m_n_hotvr(numberOfHotvrJets), m_n_jets(numberOfAk4Jets), m_zeropadding(zero_padding_value) {

  template_event = {
    "met_pt",
    "met_phi",
    "ht_had",
    "ht_lep",
    "st",
    "n_hotvr",
    "n_jets" };
  template_hotvr = {
    "pt",
    "eta",
    "phi",
    "v4mass",
    "area",
    "nsubjets",
    "sub1_pt",
    "sub1_eta",
    "sub1_phi",
    "sub1_v4mass",
    "sub1_area",
    "sub2_pt",
    "sub2_eta",
    "sub2_phi",
    "sub2_v4mass",
    "sub2_area",
    "sub3_pt",
    "sub3_eta",
    "sub3_phi",
    "sub3_v4mass",
    "sub3_area",
    "fpt",
    "mpair",
    "tau32" };
  template_jet = {
    "pt",
    "eta",
    "phi",
    "v4mass",
    "area",
    "csvv2" };
  template_lepton = {
    "pt",
    "eta",
    "phi",
    "reliso",
    "charge" };

  // Prepare vector with input name strings:
  for(auto s : template_event) m_inputs.push_back(string("DNN__")+s);
  for(unsigned int i = 1; i <= m_n_hotvr; i++) {
    for(auto s : template_hotvr) m_inputs.push_back(string("DNN__")+string("hotvr")+to_string(i)+string("_")+s); }
  for(unsigned int i = 1; i <= m_n_jets; i++) {
    for(auto s : template_jet) m_inputs.push_back(string("DNN__")+string("jet")+to_string(i)+string("_")+s); }
  for(auto s : template_lepton) m_inputs.push_back(string("DNN__")+string("lepton_")+s);
  cout << "Number of DNN input variables to set:  " << m_inputs.size() << endl;
  cout << "Float value for 'zero padding':        " << m_zeropadding << endl;

  // Fill the event handle vector with the variable names:
  for(unsigned int i = 0; i < m_inputs.size(); i++) {
    m_h_dnn_inputs.push_back(ctx.declare_event_output<float>(m_inputs.at(i))); }

  it_works();
}


bool DNNSetup::process(Event & event) {

  const vector<Jet> jets = *event.jets;
  const vector<TopJet> hotvrjets = *event.topjets;
  const vector<Electron> electrons = *event.electrons;
  const vector<Muon> muons = *event.muons;
  const MET met = *event.met;
  //const auto lepton = returnPrimaryLepton(event); // need to fix this later! .relIso not available for FlavorParticle
  const Muon lepton = muons.at(0);

  unsigned int i = 0;
  vector<float> values;
  values.resize(m_inputs.size(), m_zeropadding);

  // Event
  values.at(i++) = met.pt();
  values.at(i++) = met.phi();
  float ht_had = 0;
  for(auto j : jets) {
    ht_had += j.v4().Pt(); }
  values.at(i++) = ht_had;
  float ht_lep = met.pt();
  for(auto e : electrons) {
    ht_lep += e.v4().Pt(); }
  for(auto m : muons) {
    ht_lep += m.v4().Pt(); }
  values.at(i++) = ht_lep;
  values.at(i++) = ht_had + ht_lep;
  values.at(i++) = hotvrjets.size();
  values.at(i++) = jets.size();

  // HOTVR
  bool need_hotvrVector_resize = hotvrjets.size() < m_n_hotvr;
  unsigned int n_valid_hotvr = need_hotvrVector_resize ? hotvrjets.size() : m_n_hotvr;
  for(unsigned int j = 0; j < n_valid_hotvr; j++) {
    values.at(i++) = hotvrjets.at(j).v4().Pt();
    values.at(i++) = hotvrjets.at(j).v4().Eta();
    values.at(i++) = hotvrjets.at(j).v4().Phi();
    values.at(i++) = hotvrjets.at(j).v4().M();
    values.at(i++) = hotvrjets.at(j).jetArea();
    vector<Jet> subjets = hotvrjets.at(j).subjets();
    values.at(i++) = subjets.size();
    bool no_3rd_subjet = subjets.size() < 3;
    for(unsigned int k = 0; k < 3; k++) {
      values.at(i++) = no_3rd_subjet && k==2 ? m_zeropadding : subjets.at(k).v4().Pt();
      values.at(i++) = no_3rd_subjet && k==2 ? m_zeropadding : subjets.at(k).v4().Eta();
      values.at(i++) = no_3rd_subjet && k==2 ? m_zeropadding : subjets.at(k).v4().Phi();
      values.at(i++) = no_3rd_subjet && k==2 ? m_zeropadding : subjets.at(k).v4().M();
      values.at(i++) = no_3rd_subjet && k==2 ? m_zeropadding : subjets.at(k).jetArea(); }
    values.at(i++) = subjets.at(0).v4().Pt() / hotvrjets.at(j).v4().Pt();
    values.at(i++) = no_3rd_subjet ? m_zeropadding : min((subjets.at(0).v4() + subjets.at(1).v4()).M(), min((subjets.at(0).v4() + subjets.at(2).v4()).M(), (subjets.at(1).v4() + subjets.at(2).v4()).M()));
    values.at(i++) = hotvrjets.at(j).tau3_groomed() / hotvrjets.at(j).tau2_groomed(); }
  if(need_hotvrVector_resize) { for(unsigned int j = 0; j < (m_n_hotvr-hotvrjets.size())*template_hotvr.size(); j++) values.at(i++) = m_zeropadding; }; // zero padding for non-existent HOTVR jets

  // Jets
  bool need_jetsVector_resize = jets.size() < m_n_jets;
  unsigned int n_valid_jets = need_jetsVector_resize ? jets.size() : m_n_jets;
  for(unsigned int j = 0; j < n_valid_jets; j++) {
    values.at(i++) = jets.at(j).v4().Pt();
    values.at(i++) = jets.at(j).v4().Eta();
    values.at(i++) = jets.at(j).v4().Phi();
    values.at(i++) = jets.at(j).v4().M();
    values.at(i++) = jets.at(j).jetArea();
    values.at(i++) = jets.at(j).btag_combinedSecondaryVertex(); }
  if(need_jetsVector_resize) { for(unsigned int j = 0; j < (m_n_jets-jets.size())*template_jet.size(); j++) values.at(i++) = m_zeropadding; }; // zero padding for non-existent AK4 jets

  // Lepton
  values.at(i++) = lepton.v4().Pt();
  values.at(i++) = lepton.v4().Eta();
  values.at(i++) = lepton.v4().Phi();
  values.at(i++) = lepton.relIso();
  values.at(i++) = lepton.charge();

  if(values.size() != i) throw runtime_error("DNNSetup::process - Lengths of input and value vectors are not equal! Please check!");

  for(unsigned int j = 0; j < i; j++) {
    event.set(m_h_dnn_inputs.at(j), values.at(j)); }

  return true;
}