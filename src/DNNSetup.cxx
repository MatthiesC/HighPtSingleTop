#include "UHH2/HighPtSingleTop/include/DNNSetup.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"

#include "UHH2/common/include/Utils.h"

using namespace std;
using namespace uhh2;


DNNSetup::DNNSetup(Context & ctx, vector<Event::Handle<double>> & h_dnn_inputs, const unsigned int & numberOfHotvrJets, const unsigned int & numberOfAk4Jets, const TopJetId & topJetId, const JetId & bJetId, const double & zero_padding_value):
  m_h_dnn_inputs(h_dnn_inputs), m_n_hotvr(numberOfHotvrJets), m_n_jets(numberOfAk4Jets), m_topjetid(topJetId), m_bjetid(bJetId), m_zeropadding(zero_padding_value) {

  h_toptaggedjet = ctx.get_handle<TopJet>("TopTaggedJet");
  h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
  h_pseudotop = ctx.get_handle<LorentzVector>("PseudoTop");
  h_wboson = ctx.get_handle<LorentzVector>("WBosonLeptonic");
  h_xjets = ctx.get_handle<vector<Jet>>("ExJets_Top");
  h_ijets = ctx.get_handle<vector<Jet>>("InJets_Top");

  h_event_weight = ctx.declare_event_output<double>("DNN_EventWeight");
  h_toptag_pt = ctx.declare_event_output<double>("DNN_TopTagPt");

  template_event.push_back(DNNInput("n_pv", "Number of primary vertices", 0, 100));
  template_event.push_back(DNNInput("met_px", "Missing p_{x} [GeV]", 0, 2000));
  template_event.push_back(DNNInput("met_py", "Missing p_{y} [GeV]", 0, 2000));
  template_event.push_back(DNNInput("met_pt", "Missing p_{T} [GeV]", 0, 1000));
  template_event.push_back(DNNInput("met_phi", "MET #phi", -M_PI, M_PI));
  template_event.push_back(DNNInput("ht_had", "Hadronic H_{T} [GeV]", 0, 2000));
  template_event.push_back(DNNInput("ht_lep", "Leptonic H_{T} [GeV]", 0, 2000));
  template_event.push_back(DNNInput("st", "Transverse activity [GeV]", 0, 2000));
  template_event.push_back(DNNInput("n_hotvr", "Number of HOTVR jets", -0.5, 10.5, 11));
  template_event.push_back(DNNInput("n_jets", "Number of AK4 jets", -0.5, 10.5, 11));
  template_event.push_back(DNNInput("n_btags", "Number of b-tagged AK4 jets", -0.5, 10.5, 11));

  template_hotvr.push_back(DNNInput("toptagged")); // all these inputs are not used right now. When/if the times comes, you have to insert meaningful arguments for xlow, xhigh, nbins, and label here
  template_hotvr.push_back(DNNInput("px"));
  template_hotvr.push_back(DNNInput("py"));
  template_hotvr.push_back(DNNInput("pz"));
  template_hotvr.push_back(DNNInput("e"));
  template_hotvr.push_back(DNNInput("pt"));
  template_hotvr.push_back(DNNInput("eta"));
  template_hotvr.push_back(DNNInput("phi"));
  template_hotvr.push_back(DNNInput("v4mass"));
  template_hotvr.push_back(DNNInput("area"));
  template_hotvr.push_back(DNNInput("nsubjets"));
  template_hotvr.push_back(DNNInput("sub1_px"));
  template_hotvr.push_back(DNNInput("sub1_py"));
  template_hotvr.push_back(DNNInput("sub1_pz"));
  template_hotvr.push_back(DNNInput("sub1_e"));
  template_hotvr.push_back(DNNInput("sub1_pt"));
  template_hotvr.push_back(DNNInput("sub1_eta"));
  template_hotvr.push_back(DNNInput("sub1_phi"));
  template_hotvr.push_back(DNNInput("sub1_v4mass"));
  template_hotvr.push_back(DNNInput("sub1_area"));
  template_hotvr.push_back(DNNInput("sub2_px"));
  template_hotvr.push_back(DNNInput("sub2_py"));
  template_hotvr.push_back(DNNInput("sub2_pz"));
  template_hotvr.push_back(DNNInput("sub2_e"));
  template_hotvr.push_back(DNNInput("sub2_pt"));
  template_hotvr.push_back(DNNInput("sub2_eta"));
  template_hotvr.push_back(DNNInput("sub2_phi"));
  template_hotvr.push_back(DNNInput("sub2_v4mass"));
  template_hotvr.push_back(DNNInput("sub2_area"));
  template_hotvr.push_back(DNNInput("sub3_px"));
  template_hotvr.push_back(DNNInput("sub3_py"));
  template_hotvr.push_back(DNNInput("sub3_pz"));
  template_hotvr.push_back(DNNInput("sub3_e"));
  template_hotvr.push_back(DNNInput("sub3_pt"));
  template_hotvr.push_back(DNNInput("sub3_eta"));
  template_hotvr.push_back(DNNInput("sub3_phi"));
  template_hotvr.push_back(DNNInput("sub3_v4mass"));
  template_hotvr.push_back(DNNInput("sub3_area"));
  template_hotvr.push_back(DNNInput("fpt"));
  template_hotvr.push_back(DNNInput("mpair"));
  template_hotvr.push_back(DNNInput("tau1"));
  template_hotvr.push_back(DNNInput("tau2"));
  template_hotvr.push_back(DNNInput("tau3"));
  template_hotvr.push_back(DNNInput("tau21"));
  template_hotvr.push_back(DNNInput("tau32"));

  template_jet.push_back(DNNInput("btagged"));
  template_jet.push_back(DNNInput("DeepJet"));
  template_jet.push_back(DNNInput("px"));
  template_jet.push_back(DNNInput("py"));
  template_jet.push_back(DNNInput("pz"));
  template_jet.push_back(DNNInput("e"));
  template_jet.push_back(DNNInput("pt"));
  template_jet.push_back(DNNInput("eta"));
  template_jet.push_back(DNNInput("phi"));
  template_jet.push_back(DNNInput("v4mass"));
  template_jet.push_back(DNNInput("area"));

  template_lepton.push_back(DNNInput("px", "Lepton p_{x} [GeV]", 0, 2000));
  template_lepton.push_back(DNNInput("py", "Lepton p_{y} [GeV]", 0, 2000));
  template_lepton.push_back(DNNInput("pz", "Lepton p_{z} [GeV]", 0, 2000));
  template_lepton.push_back(DNNInput("e", "Lepton energy [GeV]", 0, 2000));
  template_lepton.push_back(DNNInput("pt", "Lepton p_{T} [GeV]", 0, 1000));
  template_lepton.push_back(DNNInput("eta", "Lepton #eta", -3, 3));
  template_lepton.push_back(DNNInput("phi", "Lepton #phi", -M_PI, M_PI));
  template_lepton.push_back(DNNInput("v4mass", "Lepton mass [GeV]", -0.1, 0.2));
  template_lepton.push_back(DNNInput("reliso", "Rel. lepton isolation", 0, 0.2));
  template_lepton.push_back(DNNInput("charge", "Lepton charge [e]", -2.5, 2.5, 5));
  template_lepton.push_back(DNNInput("dr_jet", "#DeltaR(lepton, AK4 jet)", 0, 5));
  template_lepton.push_back(DNNInput("dphi_jet", "#Delta#phi(lepton, AK4 jet)", 0, M_PI));
  template_lepton.push_back(DNNInput("ptrel_jet", "Rel. p_{T}(lepton, AK4 jet)", 0, 500));

  template_custom.push_back(DNNInput("tjet_tau32", "t jet #tau_{3}/#tau_{2}", 0, 1));
  template_custom.push_back(DNNInput("tjet_tau21", "t jet #tau_{2}/#tau_{1}", 0, 1));
  template_custom.push_back(DNNInput("tjet_tau1", "t jet #tau_{1}", 0, 1));
  template_custom.push_back(DNNInput("tjet_tau2", "t jet #tau_{2}", 0, 1));
  template_custom.push_back(DNNInput("tjet_tau3", "t jet #tau_{3}", 0, 1));
  template_custom.push_back(DNNInput("tjet_fpt", "t jet f_{p_{T}}^{sub1}", 0, 1));
  template_custom.push_back(DNNInput("tjet_mjet", "t jet m_{jet}", 130, 230));
  template_custom.push_back(DNNInput("tjet_mij", "t jet m_{ij}^{min}", 0, 200));
  template_custom.push_back(DNNInput("dR_tl", "#DeltaR(lepton, t jet)", 0, 5));
  template_custom.push_back(DNNInput("dPhi_tm", "#Delta#phi(#vec{p}_{T}^{miss}, t jet)", 0, M_PI));
  template_custom.push_back(DNNInput("dPhi_lm", "#Delta#phi(#vec{p}_{T}^{miss}, lepton)", 0, M_PI));
  template_custom.push_back(DNNInput("pTbal_wt", "[p_{T}(lep. W) #minus p_{T}(t jet)] / [p_{T}(lep. W) + p_{T}(t jet)]", -1.2, 0.8));
  template_custom.push_back(DNNInput("pTbal_tlepthad", "[p_{T}(lep. t) #minus p_{T}(t jet)] / [p_{T}(lep. t) + p_{T}(t jet)]", -1.2, 0.8));
  template_custom.push_back(DNNInput("m_top", "Leptonic pseudo t quark mass [GeV]", 0, 800));
  template_custom.push_back(DNNInput("mt_w", "Transverse leptonic W boson mass [GeV]", 0, 500));
  template_custom.push_back(DNNInput("n_xjets", "Number of xjets", -0.5, 10.5, 11));
  template_custom.push_back(DNNInput("ht_xjets", "H_{T}(xjets) [GeV]", 0, 800));
  template_custom.push_back(DNNInput("xjet1pt_m", "m_{jet}(p_{T}-leading xjet) [GeV]", 0, 100));
  template_custom.push_back(DNNInput("xjet1pt_pt", "p_{T}(p_{T}-leading xjet) [GeV]", 0, 1000));
  template_custom.push_back(DNNInput("xjet1pt_eta", "#eta(p_{T}-leading xjet)", -3, 3));
  template_custom.push_back(DNNInput("xjet1pt_deepjet", "#it{O}_{DeepJet}(p_{T}-leading xjet)", 0, 1));
  template_custom.push_back(DNNInput("mass_xjet1pt_lep", "m(p_{T}-leading xjet, lepton) [GeV]", 0, 800));
  template_custom.push_back(DNNInput("dr_xjet1pt_lep", "#DeltaR(p_{T}-leading xjet, lepton)", 0, 5));
  template_custom.push_back(DNNInput("dr_xjet1pt_tjet", "#DeltaR(p_{T}-leading xjet, t jet)", 0, 5));
  template_custom.push_back(DNNInput("xjet1dj_m", "m_{jet}(#it{O}_{DeepJet}-leading xjet) [GeV]", 0, 100));
  template_custom.push_back(DNNInput("xjet1dj_pt", "p_{T}(#it{O}_{DeepJet}-leading xjet) [GeV]", 0, 1000));
  template_custom.push_back(DNNInput("xjet1dj_eta", "#eta(#it{O}_{DeepJet}-leading xjet)", -3, 3));
  template_custom.push_back(DNNInput("xjet1dj_deepjet", "#it{O}_{DeepJet}(#it{O}_{DeepJet}-leading xjet)", 0, 1));
  template_custom.push_back(DNNInput("mass_xjet1dj_lep", "m(#it{O}_{DeepJet}-leading xjet, lepton) [GeV]", 0, 800));
  template_custom.push_back(DNNInput("dr_xjet1dj_lep", "#DeltaR(#it{O}_{DeepJet}-leading xjet, lepton)", 0, 5));
  template_custom.push_back(DNNInput("dr_xjet1dj_tjet", "#DeltaR(#it{O}_{DeepJet}-leading xjet, t jet)", 0, 5));
  template_custom.push_back(DNNInput("xjet2dj_deepjet", "#it{O}_{DeepJet}(#it{O}_{DeepJet}-subleading xjet)", 0, 1));
  template_custom.push_back(DNNInput("ijet1dj_deepjet", "#it{O}_{DeepJet}(#it{O}_{DeepJet}-leading ijet)", 0, 1));
  template_custom.push_back(DNNInput("ijet2dj_deepjet", "#it{O}_{DeepJet}(#it{O}_{DeepJet}-subleading ijet)", 0, 1));

  // Prepare vector with input name strings:
  string prefix = "DNN__";
  for(auto s : template_event) {
    string new_name = prefix+string("event_")+s.name();
    m_input_names.push_back(new_name);
    s.set_name(new_name);
    m_inputs_info.push_back(s);
  }
  for(unsigned int i = 1; i <= m_n_hotvr; i++) {
    for(auto s : template_hotvr) {
      string new_name = prefix+string("hotvr")+to_string(i)+string("_")+s.name();
      m_input_names.push_back(new_name);
      s.set_name(new_name);
      m_inputs_info.push_back(s);
    }
  }
  for(unsigned int i = 1; i <= m_n_jets; i++) {
    for(auto s : template_jet) {
      string new_name = prefix+string("jet")+to_string(i)+string("_")+s.name();
      m_input_names.push_back(new_name);
      s.set_name(new_name);
      m_inputs_info.push_back(s);
    }
  }
  for(auto s : template_lepton) {
    string new_name = prefix+string("lepton_")+s.name();
    m_input_names.push_back(new_name);
    s.set_name(new_name);
    m_inputs_info.push_back(s);
  }
  for(auto s : template_custom) {
    string new_name = prefix+string("custom_")+s.name();
    m_input_names.push_back(new_name);
    s.set_name(new_name);
    m_inputs_info.push_back(s);
  }
  cout << "Number of DNN input variables to set:  " << m_input_names.size() << endl;
  cout << "Double value for 'zero padding':       " << m_zeropadding << endl;

  // Fill the event handle vector with the variable names:
  for(unsigned int i = 0; i < m_input_names.size(); i++) {
    m_h_dnn_inputs.push_back(ctx.declare_event_output<double>(m_input_names.at(i))); }

  it_works();
}


bool DNNSetup::process(Event & event) {

  const auto & topjet = event.get(h_toptaggedjet);
  const auto & lepton = event.get(h_primlep);
  const auto & pseudotop = event.get(h_pseudotop);
  const auto & wboson = event.get(h_wboson);
  auto & xjets = event.get(h_xjets);
  sort_by_pt<Jet>(xjets);
  auto & ijets = event.get(h_ijets);
  sort_by_pt<Jet>(ijets);

  vector<Jet> xjets_sortedByDj = xjets;
  sort_by_deepjet(xjets_sortedByDj);
  vector<Jet> ijets_sortedByDj = ijets;
  sort_by_deepjet(ijets_sortedByDj);

  vector<Jet> jets = *event.jets;
  sort_by_pt<Jet>(jets);
  vector<TopJet> hotvrjets = *event.topjets;
  sort_by_pt<TopJet>(hotvrjets);
  vector<Electron> electrons = *event.electrons;
  sort_by_pt<Electron>(electrons);
  vector<Muon> muons = *event.muons;
  sort_by_pt<Muon>(muons);
  MET met = *event.met;

  unsigned int i = 0;
  vector<double> values;
  values.resize(m_input_names.size(), m_zeropadding);

  // Event
  values.at(i++) = (*event.pvs).size();
  values.at(i++) = (met.v4()).px();
  values.at(i++) = (met.v4()).py();
  values.at(i++) = met.pt();
  values.at(i++) = met.phi();
  double ht_had = 0;
  for(auto j : jets) {
    ht_had += j.v4().Pt(); }
  values.at(i++) = ht_had;
  double ht_lep = met.pt();
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
  if(topjet.subjets().size() == 2) values.at(i++) = (topjet.subjets().at(0).v4() + topjet.subjets().at(0).v4()).M(); // Needed for 0toptag validation region, where we do not necessarily have 3 subjets
  else values.at(i++) = min((topjet.subjets().at(0).v4() + topjet.subjets().at(1).v4()).M(), min((topjet.subjets().at(0).v4() + topjet.subjets().at(2).v4()).M(), (topjet.subjets().at(1).v4() + topjet.subjets().at(2).v4()).M()));
  values.at(i++) = deltaR(topjet.v4(), lepton.v4());
  values.at(i++) = deltaPhi(topjet.v4(), met.v4());
  values.at(i++) = deltaPhi(lepton.v4(), met.v4());
  values.at(i++) = (wboson.pt() - topjet.v4().pt()) / (wboson.pt() + topjet.v4().pt());
  values.at(i++) = (pseudotop.pt() - topjet.v4().pt()) / (pseudotop.pt() + topjet.v4().pt());
  values.at(i++) = pseudotop.M();
  values.at(i++) = calcMTW(lepton, event);
  values.at(i++) = xjets.size();
  bool no_xjets = xjets.size() == 0;
  double ht_xjets(0);
  for(Jet xj : xjets) { ht_xjets += xj.v4().pt(); }
  values.at(i++) = no_xjets ? m_zeropadding : ht_xjets;
  values.at(i++) = no_xjets ? m_zeropadding : xjets.at(0).v4().M();
  values.at(i++) = no_xjets ? m_zeropadding : xjets.at(0).v4().Pt();
  values.at(i++) = no_xjets ? m_zeropadding : xjets.at(0).v4().Eta();
  values.at(i++) = no_xjets ? m_zeropadding : xjets.at(0).btag_DeepJet();
  values.at(i++) = no_xjets ? m_zeropadding : (xjets.at(0).v4() + lepton.v4()).M();
  values.at(i++) = no_xjets ? m_zeropadding : deltaR(xjets.at(0).v4(), lepton.v4());
  values.at(i++) = no_xjets ? m_zeropadding : deltaR(xjets.at(0).v4(), topjet.v4());
  values.at(i++) = no_xjets ? m_zeropadding : xjets_sortedByDj.at(0).v4().M();
  values.at(i++) = no_xjets ? m_zeropadding : xjets_sortedByDj.at(0).v4().Pt();
  values.at(i++) = no_xjets ? m_zeropadding : xjets_sortedByDj.at(0).v4().Eta();
  values.at(i++) = no_xjets ? m_zeropadding : xjets_sortedByDj.at(0).btag_DeepJet();
  values.at(i++) = no_xjets ? m_zeropadding : (xjets_sortedByDj.at(0).v4() + lepton.v4()).M();
  values.at(i++) = no_xjets ? m_zeropadding : deltaR(xjets_sortedByDj.at(0).v4(), lepton.v4());
  values.at(i++) = no_xjets ? m_zeropadding : deltaR(xjets_sortedByDj.at(0).v4(), topjet.v4());
  values.at(i++) = (xjets.size() < 2) ? m_zeropadding : xjets_sortedByDj.at(1).btag_DeepJet();
  values.at(i++) = (ijets.size() < 1) ? m_zeropadding : ijets_sortedByDj.at(0).btag_DeepJet();
  values.at(i++) = (ijets.size() < 2) ? m_zeropadding : ijets_sortedByDj.at(1).btag_DeepJet();

  if(values.size() != i) throw runtime_error("DNNSetup::process - Lengths of input and value vectors are not equal! Please check!");

  for(unsigned int j = 0; j < i; j++) {
    event.set(m_h_dnn_inputs.at(j), values.at(j)); }

  event.set(h_event_weight, event.weight);
  event.set(h_toptag_pt, topjet.v4().Pt());

  return true;
}


DNNInput::DNNInput(string arg_input_name, string arg_plot_label, double arg_xlow, double arg_xhigh, uint arg_nbins):
  _input_name(arg_input_name), _plot_label(arg_plot_label), _xlow(arg_xlow), _xhigh(arg_xhigh), _nbins(arg_nbins) {}
