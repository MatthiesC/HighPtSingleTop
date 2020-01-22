#include "UHH2/HighPtSingleTop/include/DNNSetup.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"

using namespace std;
using namespace uhh2;


DNNSetup::DNNSetup(Context & ctx, vector<Event::Handle<float>> & h_dnn_inputs, const unsigned int & numberOfHotvrJets, const unsigned int & numberOfAk4Jets, const TopJetId & topJetId, const JetId & bJetId, const float & zero_padding_value):
  m_h_dnn_inputs(h_dnn_inputs), m_n_hotvr(numberOfHotvrJets), m_n_jets(numberOfAk4Jets), m_topjetid(topJetId), m_bjetid(bJetId), m_zeropadding(zero_padding_value) {

  h_toptaggedjet = ctx.get_handle<TopJet>("TopTaggedJet");
  h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
  h_pseudotop = ctx.get_handle<LorentzVector>("PseudoTop");
  h_wboson = ctx.get_handle<LorentzVector>("WBosonLeptonic");
  h_xjets = ctx.get_handle<vector<Jet>>("NonTopJets");

  template_event = {
    "n_pv",
    "met_px",
    "met_py",
    "met_pt",
    "met_phi",
    "ht_had",
    "ht_lep",
    "st",
    "n_hotvr",
    "n_jets",
    "n_btags" };
  template_hotvr = {
    "toptagged",
    "px",
    "py",
    "pz",
    "e",
    "pt",
    "eta",
    "phi",
    "v4mass",
    "area",
    "nsubjets",
    "sub1_px",
    "sub1_py",
    "sub1_pz",
    "sub1_e",
    "sub1_pt",
    "sub1_eta",
    "sub1_phi",
    "sub1_v4mass",
    "sub1_area",
    "sub2_px",
    "sub2_py",
    "sub2_pz",
    "sub2_e",
    "sub2_pt",
    "sub2_eta",
    "sub2_phi",
    "sub2_v4mass",
    "sub2_area",
    "sub3_px",
    "sub3_py",
    "sub3_pz",
    "sub3_e",
    "sub3_pt",
    "sub3_eta",
    "sub3_phi",
    "sub3_v4mass",
    "sub3_area",
    "fpt",
    "mpair",
    "tau1",
    "tau2",
    "tau3",
    "tau21",
    "tau32" };
  template_jet = {
    "btagged",
    "DeepJet",
    "px",
    "py",
    "pz",
    "e",
    "pt",
    "eta",
    "phi",
    "v4mass",
    "area" };
  template_lepton = {
    "px",
    "py",
    "pz",
    "e",
    "pt",
    "eta",
    "phi",
    "v4mass",
    "reliso",
    "charge",
    "dr_jet",
    "dphi_jet",
    "ptrel_jet" };
  template_custom = {
    "tjet_tau32",
    "tjet_tau21",
    "tjet_tau1",
    "tjet_tau2",
    "tjet_tau3",
    "tjet_fpt",
    "tjet_mjet",
    "tjet_mij",
    "dR_tl",
    "dPhi_tm",
    "dPhi_lm",
    "pTbal_wt",
    "pTbal_tlepthad",
    "m_top",
    "mt_w",
    "n_xjets",
    "ht_xjets",
    "xjet1_m",
    "xjet1_pt",
    "xjet1_eta",
    "xjet1_deepjet",
    "mass_xjet1_lep",
    "dr_xjet1_lep" };

  // Prepare vector with input name strings:
  string prefix = "DNN__";
  for(auto s : template_event) m_inputs.push_back(prefix+string("event_")+s);
  for(unsigned int i = 1; i <= m_n_hotvr; i++) {
    for(auto s : template_hotvr) m_inputs.push_back(prefix+string("hotvr")+to_string(i)+string("_")+s); }
  for(unsigned int i = 1; i <= m_n_jets; i++) {
    for(auto s : template_jet) m_inputs.push_back(prefix+string("jet")+to_string(i)+string("_")+s); }
  for(auto s : template_lepton) m_inputs.push_back(prefix+string("lepton_")+s);
  for(auto s : template_custom) m_inputs.push_back(prefix+string("custom_")+s);
  cout << "Number of DNN input variables to set:  " << m_inputs.size() << endl;
  cout << "Float value for 'zero padding':        " << m_zeropadding << endl;

  // Fill the event handle vector with the variable names:
  for(unsigned int i = 0; i < m_inputs.size(); i++) {
    m_h_dnn_inputs.push_back(ctx.declare_event_output<float>(m_inputs.at(i))); }

  it_works();
}


bool DNNSetup::process(Event & event) {

  const auto & topjet = event.get(h_toptaggedjet);
  const auto & lepton = event.get(h_primlep);
  const auto & pseudotop = event.get(h_pseudotop);
  const auto & wboson = event.get(h_wboson);
  const auto & xjets = event.get(h_xjets);

  const vector<Jet> jets = *event.jets;
  const vector<TopJet> hotvrjets = *event.topjets;
  const vector<Electron> electrons = *event.electrons;
  const vector<Muon> muons = *event.muons;
  MET met = *event.met;

  unsigned int i = 0;
  vector<float> values;
  values.resize(m_inputs.size(), m_zeropadding);

  // Event
  values.at(i++) = (*event.pvs).size();
  values.at(i++) = (met.v4()).px();
  values.at(i++) = (met.v4()).py();
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
  int n_btags = 0;
  for(Jet j : jets) {
    if(m_bjetid && (m_bjetid)(j, event)) ++n_btags; }
  values.at(i++) = n_btags;

  // HOTVR
  bool need_hotvrVector_resize = hotvrjets.size() < m_n_hotvr;
  unsigned int n_valid_hotvr = need_hotvrVector_resize ? hotvrjets.size() : m_n_hotvr;
  for(unsigned int j = 0; j < n_valid_hotvr; j++) {
    bool is_toptagged = false;
    if(m_topjetid && (m_topjetid)(hotvrjets.at(j), event)) is_toptagged = true;
    values.at(i++) = is_toptagged ? -m_zeropadding+1 : 0; // can have three values: m_zeropadding (= no j-th HOTVR jet), 0 (= not tagged), -m_zeropadding+1 (= tagged)
    values.at(i++) = hotvrjets.at(j).v4().px();
    values.at(i++) = hotvrjets.at(j).v4().py();
    values.at(i++) = hotvrjets.at(j).v4().pz();
    values.at(i++) = hotvrjets.at(j).v4().e();
    values.at(i++) = hotvrjets.at(j).v4().Pt();
    values.at(i++) = hotvrjets.at(j).v4().Eta();
    values.at(i++) = hotvrjets.at(j).v4().Phi();
    values.at(i++) = hotvrjets.at(j).v4().M();
    values.at(i++) = hotvrjets.at(j).jetArea();
    vector<Jet> subjets = hotvrjets.at(j).subjets();
    values.at(i++) = subjets.size();
    bool no_3rd_subjet = subjets.size() < 3;
    for(unsigned int k = 0; k < 3; k++) {
      values.at(i++) = no_3rd_subjet && k==2 ? m_zeropadding : subjets.at(k).v4().px();
      values.at(i++) = no_3rd_subjet && k==2 ? m_zeropadding : subjets.at(k).v4().py();
      values.at(i++) = no_3rd_subjet && k==2 ? m_zeropadding : subjets.at(k).v4().pz();
      values.at(i++) = no_3rd_subjet && k==2 ? m_zeropadding : subjets.at(k).v4().e();
      values.at(i++) = no_3rd_subjet && k==2 ? m_zeropadding : subjets.at(k).v4().Pt();
      values.at(i++) = no_3rd_subjet && k==2 ? m_zeropadding : subjets.at(k).v4().Eta();
      values.at(i++) = no_3rd_subjet && k==2 ? m_zeropadding : subjets.at(k).v4().Phi();
      values.at(i++) = no_3rd_subjet && k==2 ? m_zeropadding : subjets.at(k).v4().M();
      values.at(i++) = no_3rd_subjet && k==2 ? m_zeropadding : subjets.at(k).jetArea(); }
    values.at(i++) = subjets.at(0).v4().Pt() / hotvrjets.at(j).v4().Pt();
    values.at(i++) = no_3rd_subjet ? m_zeropadding : min((subjets.at(0).v4() + subjets.at(1).v4()).M(), min((subjets.at(0).v4() + subjets.at(2).v4()).M(), (subjets.at(1).v4() + subjets.at(2).v4()).M()));
    values.at(i++) = hotvrjets.at(j).tau1_groomed(); // need to take groomed values; plane tau values don't make sense since HOTVR jets are groomed (combination of actual subjets)!
    values.at(i++) = hotvrjets.at(j).tau2_groomed();
    values.at(i++) = hotvrjets.at(j).tau3_groomed();
    values.at(i++) = hotvrjets.at(j).tau2_groomed() / hotvrjets.at(j).tau1_groomed();
    values.at(i++) = hotvrjets.at(j).tau3_groomed() / hotvrjets.at(j).tau2_groomed(); }
  if(need_hotvrVector_resize) { for(unsigned int j = 0; j < (m_n_hotvr-hotvrjets.size())*template_hotvr.size(); j++) values.at(i++) = m_zeropadding; }; // zero padding for non-existent HOTVR jets

  // Jets
  bool need_jetsVector_resize = jets.size() < m_n_jets;
  unsigned int n_valid_jets = need_jetsVector_resize ? jets.size() : m_n_jets;
  for(unsigned int j = 0; j < n_valid_jets; j++) {
    bool is_btagged = false;
    if(m_bjetid && (m_bjetid)(jets.at(j), event)) is_btagged = true;
    values.at(i++) = is_btagged ? -m_zeropadding+1 : 0; // can have three values: m_zeropadding (= no j-th AK4 jet), 0 (= not tagged), -m_zeropadding+1 (= tagged)
    values.at(i++) = jets.at(j).btag_DeepJet();
    values.at(i++) = jets.at(j).v4().px();
    values.at(i++) = jets.at(j).v4().py();
    values.at(i++) = jets.at(j).v4().pz();
    values.at(i++) = jets.at(j).v4().e();
    values.at(i++) = jets.at(j).v4().Pt();
    values.at(i++) = jets.at(j).v4().Eta();
    values.at(i++) = jets.at(j).v4().Phi();
    values.at(i++) = jets.at(j).v4().M();
    values.at(i++) = jets.at(j).jetArea(); }
  if(need_jetsVector_resize) { for(unsigned int j = 0; j < (m_n_jets-jets.size())*template_jet.size(); j++) values.at(i++) = m_zeropadding; }; // zero padding for non-existent AK4 jets

  // Lepton
  values.at(i++) = lepton.v4().px();
  values.at(i++) = lepton.v4().py();
  values.at(i++) = lepton.v4().pz();
  values.at(i++) = lepton.v4().e();
  values.at(i++) = lepton.v4().Pt();
  values.at(i++) = lepton.v4().Eta();
  values.at(i++) = lepton.v4().Phi();
  values.at(i++) = lepton.v4().M();
  values.at(i++) = muons.size() > 0 ? muons.at(0).relIso() : (electrons.size() > 0 ? electrons.at(0).relIso() : m_zeropadding);
  values.at(i++) = lepton.charge();
  auto nj = nextJet(lepton, *event.jets);
  values.at(i++) = deltaR(lepton, *nj);
  values.at(i++) = deltaPhi(lepton, *nj);
  values.at(i++) = pTrel(lepton, nj);

  // Custom variables
  values.at(i++) = topjet.tau3_groomed() / topjet.tau2_groomed();
  values.at(i++) = topjet.tau2_groomed() / topjet.tau1_groomed();
  values.at(i++) = topjet.tau1_groomed();
  values.at(i++) = topjet.tau2_groomed();
  values.at(i++) = topjet.tau3_groomed();
  values.at(i++) = topjet.subjets().at(0).v4().Pt() / topjet.v4().Pt();
  values.at(i++) = topjet.v4().M();
  values.at(i++) = min((topjet.subjets().at(0).v4() + topjet.subjets().at(1).v4()).M(), min((topjet.subjets().at(0).v4() + topjet.subjets().at(2).v4()).M(), (topjet.subjets().at(1).v4() + topjet.subjets().at(2).v4()).M()));
  values.at(i++) = deltaR(topjet.v4(), lepton.v4());
  values.at(i++) = deltaPhi(topjet.v4(), met.v4());
  values.at(i++) = deltaPhi(lepton.v4(), met.v4());
  values.at(i++) = (wboson.pt() - topjet.v4().pt()) / (wboson.pt() + topjet.v4().pt());
  values.at(i++) = (pseudotop.pt() - topjet.v4().pt()) / (pseudotop.pt() + topjet.v4().pt());
  values.at(i++) = pseudotop.M();
  values.at(i++) = calcMTW(lepton, event);
  values.at(i++) = xjets.size();
  bool no_xjets = xjets.size() == 0;
  float ht_xjets(0);
  for(Jet xj : xjets) { ht_xjets += xj.v4().pt(); }
  values.at(i++) = no_xjets ? m_zeropadding : ht_xjets;
  values.at(i++) = no_xjets ? m_zeropadding : xjets.at(0).v4().M();
  values.at(i++) = no_xjets ? m_zeropadding : xjets.at(0).v4().Pt();
  values.at(i++) = no_xjets ? m_zeropadding : xjets.at(0).v4().Eta();
  values.at(i++) = no_xjets ? m_zeropadding : xjets.at(0).btag_DeepJet();
  values.at(i++) = no_xjets ? m_zeropadding : (xjets.at(0).v4() + lepton.v4()).M();
  values.at(i++) = no_xjets ? m_zeropadding : deltaR(xjets.at(0).v4(), lepton.v4());

  if(values.size() != i) throw runtime_error("DNNSetup::process - Lengths of input and value vectors are not equal! Please check!");

  for(unsigned int j = 0; j < i; j++) {
    event.set(m_h_dnn_inputs.at(j), values.at(j)); }

  return true;
}
