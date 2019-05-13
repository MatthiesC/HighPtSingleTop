#include "UHH2/HighPtSingleTop/include/DNNSetup.h"

using namespace std;
using namespace uhh2;


DNNSetup::DNNSetup(Context & ctx, const unsigned int & numberOfHotvrJets, const unsigned int & numberOfAk4Jets, const float & zero_padding_value = -100):
  m_n_hotvr(numberOfHotvrJets), m_n_jets(numberOfAk4Jets), m_zeropadding(zero_padding_value) {

  vector<string> template_event = {
    "met_pt",
    "met_phi",
    "ht_jet",
    "ht_lep",
    "st",
    "n_hotvr",
    "n_jets"
  };
  vector<string> template_hotvr = {
    "pt",
    "eta",
    "phi",
    "mass",
    "area",
    "nsubjets",
    "fpt",
    "mpair",
    "tau32",
    "sub1_pt",
    "sub1_eta",
    "sub1_phi",
    "sub1_mass",
    "sub1_area",
    "sub2_pt",
    "sub2_eta",
    "sub2_phi",
    "sub2_mass",
    "sub2_area",
    "sub3_pt",
    "sub3_eta",
    "sub3_phi",
    "sub3_mass",
    "sub3_area"
  };
  vector<string> template_jet = {
    "pt",
    "eta",
    "phi",
    "mass",
    "area",
    "csvv2"
  };
  vector<string> template_lepton = {
    "pt",
    "eta",
    "phi",
    "iso"
  };

  // Prepare vector with input name strings:
  for(auto s : template_event) m_inputs.push_back(s);
  for(unsigned int i = 1; i <= m_n_hotvr; i++) {
    for(auto s : template_hotvr) m_inputs.push_back(string("hotvr")+to_string(i)+string("_")+s);
  }
  for(unsigned int i = 1; i <= m_n_jets; i++) {
    for(auto s : template_jet) m_inputs.push_back(string("jet")+to_string(i)+string("_")+s);
  }
  for(auto s : template_lepton) m_inputs.push_back(s);
  cout << "Number of DNN input variables to set:  " << m_inputs.size() << endl;

  // Prepare vector with input values:
  m_values.reserve(m_inputs.size());

}


bool DNNSetup::process(Event & event) {

  return true;
}
