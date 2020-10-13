#include "UHH2/HighPtSingleTop/include/DNNSetup.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"

#include "UHH2/common/include/Utils.h"

using namespace std;
using namespace uhh2;


DNNInput::DNNInput(string arg_input_name, string arg_plot_label, double arg_xlow, double arg_xhigh, uint arg_nbins):
  _input_name(arg_input_name), _plot_label(arg_plot_label), _xlow(arg_xlow), _xhigh(arg_xhigh), _nbins(arg_nbins) {}


DNNSetup::DNNSetup(Context & ctx, const double & zero_padding_value) {

  m_zeropadding = zero_padding_value;

  m_h_which_region = ctx.get_handle<int>("which_region");

  m_h_tjet = ctx.get_handle<TopJet>("TopTaggedJet");
  m_h_wjet = ctx.get_handle<TopJet>("WTaggedJet");
  m_h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
  m_h_wboson = ctx.get_handle<LorentzVector>("WBosonLeptonic");
  m_h_pseudotop = ctx.get_handle<LorentzVector>("PseudoTop");
  m_h_xjets_tjet = ctx.get_handle<vector<Jet>>("ExJets_Top");
  m_h_ijets_tjet = ctx.get_handle<vector<Jet>>("InJets_Top");
  m_h_xjets_wjet = ctx.get_handle<vector<Jet>>("ExJets_W");
  m_h_ijets_wjet = ctx.get_handle<vector<Jet>>("InJets_W");

  m_h_event_weight = ctx.declare_event_output<double>("DNNinfo_event_weight");
  // h_toptag_pt = ctx.declare_event_output<double>("DNN_TopTagPt");

  /*
  * Define DNN inputs for t-tag region:
  */

  m_inputs_template_ttag.push_back(DNNInput("met_pt", "Missing p_{T} [GeV]", 0, 1000));
  m_inputs_template_ttag.push_back(DNNInput("lepton_pt", "Lepton p_{T} [GeV]", 0, 1000));
  m_inputs_template_ttag.push_back(DNNInput("lepton_eta", "Lepton #eta", -3, 3));
  m_inputs_template_ttag.push_back(DNNInput("lepton_reliso", "Rel. lepton isolation", 0, 0.2));
  m_inputs_template_ttag.push_back(DNNInput("lepton_charge", "Lepton charge [e]", -2.5, 2.5, 5));
  m_inputs_template_ttag.push_back(DNNInput("lepton_dr_nextjet", "#DeltaR(lepton, AK4 jet)", 0, 5));
  m_inputs_template_ttag.push_back(DNNInput("lepton_ptrel_nextjet", "Rel. p_{T}(lepton, AK4 jet)", 0, 500));
  m_inputs_template_ttag.push_back(DNNInput("dphi_lepton_met", "#Delta#phi(#vec{p}_{T}^{miss}, lepton)", 0, M_PI));
  m_inputs_template_ttag.push_back(DNNInput("mtw", "Transverse leptonic W boson mass [GeV]", 0, 500));
  m_inputs_template_ttag.push_back(DNNInput("pseudotop_m", "Leptonic pseudo t quark mass [GeV]", 0, 800));
  m_inputs_template_ttag.push_back(DNNInput("tjet_tau32", "t jet #tau_{3}/#tau_{2}", 0, 1));
  m_inputs_template_ttag.push_back(DNNInput("tjet_tau21", "t jet #tau_{2}/#tau_{1}", 0, 1));
  m_inputs_template_ttag.push_back(DNNInput("tjet_tau1", "t jet #tau_{1}", 0, 1));
  m_inputs_template_ttag.push_back(DNNInput("tjet_tau2", "t jet #tau_{2}", 0, 1));
  m_inputs_template_ttag.push_back(DNNInput("tjet_tau3", "t jet #tau_{3}", 0, 1));
  m_inputs_template_ttag.push_back(DNNInput("tjet_fpt", "t jet f_{p_{T}}^{sub1}", 0, 1));
  m_inputs_template_ttag.push_back(DNNInput("tjet_mjet", "t jet m_{jet}", 130, 230));
  m_inputs_template_ttag.push_back(DNNInput("tjet_mij", "t jet m_{ij}^{min}", 0, 200));
  m_inputs_template_ttag.push_back(DNNInput("dr_tjet_lepton", "#DeltaR(lepton, t jet)", 0, 5));
  m_inputs_template_ttag.push_back(DNNInput("dphi_tjet_met", "#Delta#phi(#vec{p}_{T}^{miss}, t jet)", 0, M_PI));
  m_inputs_template_ttag.push_back(DNNInput("ptbal_tjet_wboson", "[p_{T}(lep. W) #minus p_{T}(t jet)] / [p_{T}(lep. W) + p_{T}(t jet)]", -1.2, 0.8));
  m_inputs_template_ttag.push_back(DNNInput("ptbal_tjet_pseudotop", "[p_{T}(lep. t) #minus p_{T}(t jet)] / [p_{T}(lep. t) + p_{T}(t jet)]", -1.2, 0.8));
  m_inputs_template_ttag.push_back(DNNInput("n_xjets", "Number of xjets", -0.5, 10.5, 11));
  m_inputs_template_ttag.push_back(DNNInput("ht_xjets", "H_{T}(xjets) [GeV]", 0, 800));
  m_inputs_template_ttag.push_back(DNNInput("xjet1pt_m", "m_{jet}(p_{T}-leading xjet) [GeV]", 0, 100));
  m_inputs_template_ttag.push_back(DNNInput("xjet1pt_pt", "p_{T}(p_{T}-leading xjet) [GeV]", 0, 1000));
  m_inputs_template_ttag.push_back(DNNInput("xjet1pt_eta", "#eta(p_{T}-leading xjet)", -3, 3));
  m_inputs_template_ttag.push_back(DNNInput("xjet1pt_deepjet", "#it{O}_{DeepJet}(p_{T}-leading xjet)", 0, 1));
  m_inputs_template_ttag.push_back(DNNInput("mass_xjet1pt_lepton", "m(p_{T}-leading xjet, lepton) [GeV]", 0, 800));
  m_inputs_template_ttag.push_back(DNNInput("dr_xjet1pt_lepton", "#DeltaR(p_{T}-leading xjet, lepton)", 0, 5));
  m_inputs_template_ttag.push_back(DNNInput("dr_xjet1pt_tjet", "#DeltaR(p_{T}-leading xjet, t jet)", 0, 5));
  m_inputs_template_ttag.push_back(DNNInput("xjet1dj_m", "m_{jet}(#it{O}_{DeepJet}-leading xjet) [GeV]", 0, 100));
  m_inputs_template_ttag.push_back(DNNInput("xjet1dj_pt", "p_{T}(#it{O}_{DeepJet}-leading xjet) [GeV]", 0, 1000));
  m_inputs_template_ttag.push_back(DNNInput("xjet1dj_eta", "#eta(#it{O}_{DeepJet}-leading xjet)", -3, 3));
  m_inputs_template_ttag.push_back(DNNInput("xjet1dj_deepjet", "#it{O}_{DeepJet}(#it{O}_{DeepJet}-leading xjet)", 0, 1));
  m_inputs_template_ttag.push_back(DNNInput("mass_xjet1dj_lepton", "m(#it{O}_{DeepJet}-leading xjet, lepton) [GeV]", 0, 800));
  m_inputs_template_ttag.push_back(DNNInput("dr_xjet1dj_lepton", "#DeltaR(#it{O}_{DeepJet}-leading xjet, lepton)", 0, 5));
  m_inputs_template_ttag.push_back(DNNInput("dr_xjet1dj_tjet", "#DeltaR(#it{O}_{DeepJet}-leading xjet, t jet)", 0, 5));
  m_inputs_template_ttag.push_back(DNNInput("xjet2dj_deepjet", "#it{O}_{DeepJet}(#it{O}_{DeepJet}-subleading xjet)", 0, 1));
  m_inputs_template_ttag.push_back(DNNInput("ijet1dj_deepjet", "#it{O}_{DeepJet}(#it{O}_{DeepJet}-leading ijet)", 0, 1));
  m_inputs_template_ttag.push_back(DNNInput("ijet2dj_deepjet", "#it{O}_{DeepJet}(#it{O}_{DeepJet}-subleading ijet)", 0, 1));

  /*
  * Define DNN inputs for W-tag region:
  */

  m_inputs_template_wtag.push_back(DNNInput("met_pt", "Missing p_{T} [GeV]", 0, 1000));
  m_inputs_template_wtag.push_back(DNNInput("lepton_pt", "Lepton p_{T} [GeV]", 0, 1000));
  m_inputs_template_wtag.push_back(DNNInput("lepton_eta", "Lepton #eta", -3, 3));
  m_inputs_template_wtag.push_back(DNNInput("lepton_reliso", "Rel. lepton isolation", 0, 0.2));
  m_inputs_template_wtag.push_back(DNNInput("lepton_charge", "Lepton charge [e]", -2.5, 2.5, 5));
  m_inputs_template_wtag.push_back(DNNInput("lepton_dr_nextjet", "#DeltaR(lepton, AK4 jet)", 0, 5));
  m_inputs_template_wtag.push_back(DNNInput("lepton_ptrel_nextjet", "Rel. p_{T}(lepton, AK4 jet)", 0, 500));
  m_inputs_template_wtag.push_back(DNNInput("dphi_lepton_met", "#Delta#phi(#vec{p}_{T}^{miss}, lepton)", 0, M_PI));
  m_inputs_template_wtag.push_back(DNNInput("mtw", "Transverse leptonic W boson mass [GeV]", 0, 500));
  m_inputs_template_wtag.push_back(DNNInput("pseudotop_m", "Leptonic pseudo t quark mass [GeV]", 0, 800));
  m_inputs_template_wtag.push_back(DNNInput("wjet_tau32", "W jet #tau_{3}/#tau_{2}", 0, 1));
  m_inputs_template_wtag.push_back(DNNInput("wjet_tau21", "W jet #tau_{2}/#tau_{1}", 0, 1));
  m_inputs_template_wtag.push_back(DNNInput("wjet_tau1", "W jet #tau_{1}", 0, 1));
  m_inputs_template_wtag.push_back(DNNInput("wjet_tau2", "W jet #tau_{2}", 0, 1));
  m_inputs_template_wtag.push_back(DNNInput("wjet_tau3", "W jet #tau_{3}", 0, 1));
  m_inputs_template_wtag.push_back(DNNInput("wjet_mjet", "W jet m_{jet}", 0, 200));
  m_inputs_template_wtag.push_back(DNNInput("wjet_softdropmass", "W jet m_{SD}", 60, 110));


  string prefix = "DNNinput_";
  for(auto input : m_inputs_template_ttag) {
    string new_name = prefix+string("ttag__")+input.name();
    m_input_names_ttag.push_back(new_name);
    input.set_name(new_name);
    m_inputs_info_ttag.push_back(input);
  }
  for(auto input : m_inputs_template_wtag) {
    string new_name = prefix+string("wtag__")+input.name();
    m_input_names_wtag.push_back(new_name);
    input.set_name(new_name);
    m_inputs_info_wtag.push_back(input);
  }

  cout << "Number of t-tag DNN input variables: " << m_input_names_ttag.size() << endl;
  cout << "Number of W-tag DNN input variables: " << m_input_names_wtag.size() << endl;
  cout << "Double value for 'zero padding': " << m_zeropadding << endl;

  // Fill the event handle vector with the variable names:
  for(unsigned int i = 0; i < m_input_names_ttag.size(); i++) {
    m_h_dnn_inputs_ttag.push_back(ctx.declare_event_output<double>(m_input_names_ttag.at(i))); }
  for(unsigned int i = 0; i < m_input_names_wtag.size(); i++) {
    m_h_dnn_inputs_wtag.push_back(ctx.declare_event_output<double>(m_input_names_wtag.at(i))); }

  // it_works();
}

void DNNSetup::calculate_inputs_for_ttag_dnn(Event & event) {

  const auto & topjet = event.get(m_h_tjet);
  const auto & lepton = event.get(m_h_primlep);
  const auto & wboson = event.get(m_h_wboson);
  const auto & pseudotop = event.get(m_h_pseudotop);
  const auto & xjets = event.get(m_h_xjets_tjet);
  const auto & ijets = event.get(m_h_ijets_tjet);

  vector<Jet> xjets_sortedByDj = xjets;
  sort_by_deepjet(xjets_sortedByDj);
  vector<Jet> ijets_sortedByDj = ijets;
  sort_by_deepjet(ijets_sortedByDj);

  MET met = *event.met;
  const vector<Electron> electrons = *event.electrons;
  const vector<Muon> muons = *event.muons;

  unsigned int i = 0;
  vector<double> values;
  values.resize(m_input_names_ttag.size(), m_zeropadding);

  /*
  * Calculate the DNN inputs. Make sure to follow the same order as in the constructor.
  */

  values.at(i++) = met.pt();
  values.at(i++) = lepton.v4().Pt();
  values.at(i++) = lepton.v4().Eta();
  values.at(i++) = muons.size() > 0 ? muons.at(0).relIso() : (electrons.size() > 0 ? electrons.at(0).relIso() : m_zeropadding);
  values.at(i++) = lepton.charge();
  auto nj = nextJet(lepton, *event.jets);
  values.at(i++) = deltaR(lepton, *nj);
  values.at(i++) = pTrel(lepton, nj);
  values.at(i++) = deltaPhi(lepton.v4(), met.v4());
  values.at(i++) = calcMTW(lepton, event);
  values.at(i++) = pseudotop.M();
  values.at(i++) = topjet.tau3_groomed() / topjet.tau2_groomed();
  values.at(i++) = topjet.tau2_groomed() / topjet.tau1_groomed();
  values.at(i++) = topjet.tau1_groomed();
  values.at(i++) = topjet.tau2_groomed();
  values.at(i++) = topjet.tau3_groomed();
  values.at(i++) = calcHOTVRfpt(topjet);
  values.at(i++) = topjet.v4().M();
  if(topjet.subjets().size() == 2) values.at(i++) = (topjet.subjets().at(0).v4() + topjet.subjets().at(1).v4()).M(); // Needed for 0toptag validation region, where we do not necessarily have 3 subjets
  else values.at(i++) = calcHOTVRmpair(topjet);
  values.at(i++) = deltaR(topjet.v4(), lepton.v4());
  values.at(i++) = deltaPhi(topjet.v4(), met.v4());
  values.at(i++) = (wboson.pt() - topjet.v4().pt()) / (wboson.pt() + topjet.v4().pt());
  values.at(i++) = (pseudotop.pt() - topjet.v4().pt()) / (pseudotop.pt() + topjet.v4().pt());
  values.at(i++) = xjets.size();
  bool no_xjets = (xjets.size() == 0);
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
  values.at(i++) = (xjets.size() > 1) ? xjets_sortedByDj.at(1).btag_DeepJet() : m_zeropadding;
  values.at(i++) = (ijets.size() > 0) ? ijets_sortedByDj.at(0).btag_DeepJet() : m_zeropadding;
  values.at(i++) = (ijets.size() > 1) ? ijets_sortedByDj.at(1).btag_DeepJet() : m_zeropadding;

  if(values.size() != i) throw runtime_error("DNNSetup::calculate_inputs_for_ttag_dnn(): Lengths of input and value vectors are not equal. Please check!");

  for(unsigned int j = 0; j < i; j++) {
    event.set(m_h_dnn_inputs_ttag.at(j), values.at(j)); }
}

void DNNSetup::set_dummy_inputs_for_ttag_dnn(Event & event) {

  for(unsigned int i = 0; i < m_input_names_ttag.size(); i++) {
    event.set(m_h_dnn_inputs_ttag.at(i), m_zeropadding); }
}

void DNNSetup::calculate_inputs_for_wtag_dnn(Event & event) {

  const auto & wjet = event.get(m_h_wjet);
  const auto & lepton = event.get(m_h_primlep);
  const auto & wboson = event.get(m_h_wboson);
  const auto & pseudotop = event.get(m_h_pseudotop);
  const auto & xjets = event.get(m_h_xjets_wjet);
  const auto & ijets = event.get(m_h_ijets_wjet);

  vector<Jet> xjets_sortedByDj = xjets;
  sort_by_deepjet(xjets_sortedByDj);
  vector<Jet> ijets_sortedByDj = ijets;
  sort_by_deepjet(ijets_sortedByDj);

  MET met = *event.met;
  const vector<Electron> electrons = *event.electrons;
  const vector<Muon> muons = *event.muons;

  unsigned int i = 0;
  vector<double> values;
  values.resize(m_input_names_wtag.size(), m_zeropadding);

  /*
  * Calculate the DNN inputs. Make sure to follow the same order as in the constructor.
  */

  values.at(i++) = met.pt();
  values.at(i++) = lepton.v4().Pt();
  values.at(i++) = lepton.v4().Eta();
  values.at(i++) = muons.size() > 0 ? muons.at(0).relIso() : (electrons.size() > 0 ? electrons.at(0).relIso() : m_zeropadding);
  values.at(i++) = lepton.charge();
  auto nj = nextJet(lepton, *event.jets);
  values.at(i++) = deltaR(lepton, *nj);
  values.at(i++) = pTrel(lepton, nj);
  values.at(i++) = calcMTW(lepton, event);
  values.at(i++) = pseudotop.M();
  values.at(i++) = wjet.tau3() / wjet.tau2();
  values.at(i++) = wjet.tau2() / wjet.tau1();
  values.at(i++) = wjet.tau1();
  values.at(i++) = wjet.tau2();
  values.at(i++) = wjet.tau3();
  values.at(i++) = wjet.v4().M();
  values.at(i++) = wjet.softdropmass();
  // values.at(i++) = topjet.subjets().at(0).v4().Pt() / topjet.v4().Pt();
  // values.at(i++) = topjet.v4().M();
  // if(topjet.subjets().size() == 2) values.at(i++) = (topjet.subjets().at(0).v4() + topjet.subjets().at(0).v4()).M(); // Needed for 0toptag validation region, where we do not necessarily have 3 subjets
  // else values.at(i++) = min((topjet.subjets().at(0).v4() + topjet.subjets().at(1).v4()).M(), min((topjet.subjets().at(0).v4() + topjet.subjets().at(2).v4()).M(), (topjet.subjets().at(1).v4() + topjet.subjets().at(2).v4()).M()));
  // values.at(i++) = deltaR(topjet.v4(), lepton.v4());
  // values.at(i++) = deltaPhi(topjet.v4(), met.v4());
  // values.at(i++) = deltaPhi(lepton.v4(), met.v4());
  // values.at(i++) = (wboson.pt() - topjet.v4().pt()) / (wboson.pt() + topjet.v4().pt());
  // values.at(i++) = (pseudotop.pt() - topjet.v4().pt()) / (pseudotop.pt() + topjet.v4().pt());
  // values.at(i++) = xjets.size();
  // bool no_xjets = xjets.size() == 0;
  // double ht_xjets(0);
  // for(Jet xj : xjets) { ht_xjets += xj.v4().pt(); }
  // values.at(i++) = no_xjets ? m_zeropadding : ht_xjets;
  // values.at(i++) = no_xjets ? m_zeropadding : xjets.at(0).v4().M();
  // values.at(i++) = no_xjets ? m_zeropadding : xjets.at(0).v4().Pt();
  // values.at(i++) = no_xjets ? m_zeropadding : xjets.at(0).v4().Eta();
  // values.at(i++) = no_xjets ? m_zeropadding : xjets.at(0).btag_DeepJet();
  // values.at(i++) = no_xjets ? m_zeropadding : (xjets.at(0).v4() + lepton.v4()).M();
  // values.at(i++) = no_xjets ? m_zeropadding : deltaR(xjets.at(0).v4(), lepton.v4());
  // values.at(i++) = no_xjets ? m_zeropadding : deltaR(xjets.at(0).v4(), topjet.v4());
  // values.at(i++) = no_xjets ? m_zeropadding : xjets_sortedByDj.at(0).v4().M();
  // values.at(i++) = no_xjets ? m_zeropadding : xjets_sortedByDj.at(0).v4().Pt();
  // values.at(i++) = no_xjets ? m_zeropadding : xjets_sortedByDj.at(0).v4().Eta();
  // values.at(i++) = no_xjets ? m_zeropadding : xjets_sortedByDj.at(0).btag_DeepJet();
  // values.at(i++) = no_xjets ? m_zeropadding : (xjets_sortedByDj.at(0).v4() + lepton.v4()).M();
  // values.at(i++) = no_xjets ? m_zeropadding : deltaR(xjets_sortedByDj.at(0).v4(), lepton.v4());
  // values.at(i++) = no_xjets ? m_zeropadding : deltaR(xjets_sortedByDj.at(0).v4(), topjet.v4());
  // values.at(i++) = (xjets.size() < 2) ? m_zeropadding : xjets_sortedByDj.at(1).btag_DeepJet();
  // values.at(i++) = (ijets.size() < 1) ? m_zeropadding : ijets_sortedByDj.at(0).btag_DeepJet();
  // values.at(i++) = (ijets.size() < 2) ? m_zeropadding : ijets_sortedByDj.at(1).btag_DeepJet();

  if(values.size() != i) throw runtime_error("DNNSetup::calculate_inputs_for_wtag_dnn(): Lengths of input and value vectors are not equal. Please check!");

  for(unsigned int j = 0; j < i; j++) {
    event.set(m_h_dnn_inputs_wtag.at(j), values.at(j)); }
}

void DNNSetup::set_dummy_inputs_for_wtag_dnn(Event & event) {

  for(unsigned int i = 0; i < m_input_names_wtag.size(); i++) {
    event.set(m_h_dnn_inputs_wtag.at(i), m_zeropadding); }
}

bool DNNSetup::process(Event & event) {

  const int region = event.get(m_h_which_region);

  if(region == 1) {
    calculate_inputs_for_ttag_dnn(event);
    set_dummy_inputs_for_wtag_dnn(event);
  }
  else if(region == 2) {
    calculate_inputs_for_wtag_dnn(event);
    set_dummy_inputs_for_ttag_dnn(event);
  }
  else if(region == 3) {
    calculate_inputs_for_ttag_dnn(event);
    calculate_inputs_for_wtag_dnn(event);
  }
  else {
    throw runtime_error("DNNSetup::process(): Calling this function on event that does not belong to a valid analysis region. Make sure to discard invalid events beforehand.");
  }

  event.set(m_h_event_weight, (double)event.weight);

  return true;
}
