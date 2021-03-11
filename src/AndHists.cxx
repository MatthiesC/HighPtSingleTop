#include "UHH2/HighPtSingleTop/include/AndHists.h"

#include "UHH2/common/include/LuminosityHists.h"
#include "UHH2/common/include/JetHists.h"

#include "UHH2/HOTVR/include/HOTVRHists.h"

#include "UHH2/HighPtSingleTop/include/Constants.h"
#include "UHH2/HighPtSingleTop/include/DNNHists.h"
#include "UHH2/HighPtSingleTop/include/MyEventHists.h"
#include "UHH2/HighPtSingleTop/include/MyElectronHists.h"
#include "UHH2/HighPtSingleTop/include/MyMuonHists.h"
#include "UHH2/HighPtSingleTop/include/WTagHists.h"
#include "UHH2/HighPtSingleTop/include/TopTagHists.h"
#include "UHH2/HighPtSingleTop/include/MyAk8Hists.h"
#include "UHH2/HighPtSingleTop/include/TaggedJetsHists.h"
#include "UHH2/HighPtSingleTop/include/DeepJetHists.h"


using namespace std;
using namespace uhh2;


CountingHist::CountingHist(Context & ctx, const string & dirname):
  Hists(ctx, dirname) {

  hist_counting = book<TH1F>("count", "Number of weighted events", 1, -1, 1);
}


void CountingHist::fill(const Event & event) {

  hist_counting->Fill(0., event.weight);
}


MttHist::MttHist(Context & ctx, const string & dirname):
  Hists(ctx, dirname) {

  hist_mtt = book<TH1F>("mtt", "Generated m_{t#bar{t}}", 2000, 0, 2000);
  hist_mtt_100 = book<TH1F>("mtt_100", "Generated m_{t#bar{t}}", 100, 0, 2000);
}


void MttHist::fill(const Event & event) {

  double mtt = -1;
  LorentzVector top = {0.,0.,0.,0.}, antitop = {0.,0.,0.,0.};
  for (const auto & genpart : *event.genparticles){
    if (genpart.pdgId() == 6) {
      top = genpart.v4();
    }
    else if(genpart.pdgId() == -6) {
      antitop = genpart.v4();
    }
  }
  mtt = (top+antitop).M();

  hist_mtt->Fill(mtt, event.weight);
  hist_mtt_100->Fill(mtt, event.weight);
}


AndHists::AndHists(Context & ctx, const string & dirname):
  Hists(ctx, dirname+"_Counter"), m_dirname(dirname) {

  // Single histogram for event counting
  nevt = book<TH1F>("NEvt", "Number of weighted events", 1, -1, 1);
  // Single histogram for event weights
  wevt = book<TH1F>("WEvt", "Event weights (entries are unweighted)", 400, -2, 2);

  // Add common histogram classes
  hists_vector.push_back(new LuminosityHists(ctx, dirname + "_Lumi"));
  hists_vector.push_back(new MyEventHists(ctx, dirname + "_Event"));
  hists_vector.push_back(new MyElectronHists(ctx, dirname + "_Electron"));
  hists_vector.push_back(new MyMuonHists(ctx, dirname + "_Muon"));
  hists_vector.push_back(new JetHists(ctx, dirname + "_Jets"));
  // hists_vector.push_back(new DeepJetHists(ctx, dirname + "_DeepJet"));
  hists_vector.push_back(new HOTVRHists(ctx, dirname +"_HOTVR"));
}


void AndHists::fill(const Event & event) {

  nevt->Fill(0., event.weight);
  wevt->Fill(event.weight, 1.);
  for(Hists *hist : hists_vector) {
    hist->fill(event);
  }
}


void AndHists::add_hist(Hists *hist) {

  hists_vector.push_back(hist);
}


void AndHists::add_Ak8Hists(Context & ctx, const string & ak8_handle_name) {

  hists_vector.push_back(new MyAk8Hists(ctx, m_dirname + "_AK8", ak8_handle_name));
}


void AndHists::add_DeepJetHists(Context & ctx) {

  hists_vector.push_back(new DeepJetHists(ctx, m_dirname + "_DeepJet"));
}


void AndHists::add_WTagHists(Context & ctx, const bool & var_binned) {

  hists_vector.push_back(new WTagHists(ctx, m_dirname + "_WTagHists"));
  if(var_binned) {
    // hists_vector.push_back(new WTagHists(ctx, m_dirname + "_WTagHists_Pt0to300", 0, 300));
    // hists_vector.push_back(new WTagHists(ctx, m_dirname + "_WTagHists_Pt300toInf", 300));
  }
}


void AndHists::add_TopTagHists(Context & ctx, const bool & var_binned) {

  hists_vector.push_back(new TopTagHists(ctx, m_dirname + "_TopTagHists"));
  if (var_binned) {
    // hists_vector.push_back(new TopTagHists(ctx, m_dirname + "_TopTagHists_Pt0to300", 0, 300));
    // hists_vector.push_back(new TopTagHists(ctx, m_dirname + "_TopTagHists_Pt300toInf", 300));
  }
}


void AndHists::add_TaggedJetsHists(Context & ctx, const string & h_name_taggedjet, const string & h_name_postfix) {

  hists_vector.push_back(new TaggedJetsHists(ctx, m_dirname + "_TaggedJetsHists" + h_name_postfix, h_name_taggedjet, h_name_postfix));
}


string AndHists::get_dirname() {

    return m_dirname;
}


AndHists::~AndHists() {

  for(uint i = 0; i < hists_vector.size(); ++i) {
    delete hists_vector[i];
  }
  hists_vector.clear();
}


BinnedDNNHists::BinnedDNNHists(Context & ctx, const string & dirname, const vector<string> inputs, const vector<DNNInput> inputs_info, const vector<string> outputs):
  Hists(ctx, dirname+"_Binning"), m_dirname(dirname) {

  h_tjet_pt = ctx.get_handle<double>("DNNinfo_tjet_pt");
  h_wjet_pt = ctx.get_handle<double>("DNNinfo_wjet_pt");
  h_lepton_pt = ctx.get_handle<double>("DNNinfo_lepton_pt");
  // h_tquark_pt = ctx.get_handle<double>("VOI_tquark_pt");

  hist_tjet_pt = book<TH1F>("tjet_pt", "t jet p_{T} [GeV]", MyConstants::binning__tjet_pt.size(), MyConstants::binning_edges__tjet_pt);
  hist_wjet_pt = book<TH1F>("wjet_pt", "W jet p_{T} [GeV]", MyConstants::binning__wjet_pt.size(), MyConstants::binning_edges__wjet_pt);
  hist_lepton_pt = book<TH1F>("lepton_pt", "Lepton p_{T} [GeV]", MyConstants::binning__lepton_pt.size(), MyConstants::binning_edges__lepton_pt);
  // hist_tquark_pt = book<TH1F>("tquark_pt", "t quark candidate p_{T} [GeV]", MyConstants::binning__tquark_pt.size(), MyConstants::binning_edges__tquark_pt);

  hists_vector.push_back(new DNNHists(ctx, dirname+"_Full", inputs, inputs_info, outputs));
  // hists_vector.push_back(new DNNHists(ctx, dirname+"_Pt200to350", inputs, inputs_info, "DNN_TopTagPt", 0, 350));
  // hists_vector.push_back(new DNNHists(ctx, dirname+"_Pt350toInf", inputs, inputs_info, "DNN_TopTagPt", 350));
  for(auto i : MyConstants::binning__tjet_pt) {
    hists_vector.push_back(new DNNHists(ctx, dirname+"_tjet_pt"+to_string((int)(i.first))+"to"+to_string((int)(i.second)), inputs, inputs_info, outputs, "DNNinfo_tjet_pt", i.first, i.second));
  }
  // for(auto i : MyConstants::binning__wjet_pt) {
  //   hists_vector.push_back(new DNNHists(ctx, dirname+"_wjet_pt"+to_string((int)(i.first))+"to"+to_string((int)(i.second)), inputs, inputs_info, outputs, "DNNinfo_wjet_pt", i.first, i.second));
  // }
  // for(auto i : MyConstants::binning__lepton_pt) {
  //   hists_vector.push_back(new DNNHists(ctx, dirname+"_lepton_pt"+to_string((int)(i.first))+"to"+to_string((int)(i.second)), inputs, inputs_info, outputs, "DNNinfo_lepton_pt", i.first, i.second));
  // }

  // for(auto i : MyConstants::binning__tquark_pt) {
  //   hists_vector.push_back(new DNNHists(ctx, dirname+"_tquark_pt"+to_string((int)(i.first))+"to"+to_string((int)(i.second)), inputs, inputs_info, outputs, "VOI_tquark_pt", i.first, i.second));
  // }
}


void BinnedDNNHists::fill(const Event & event) {

  double tjet_pt = event.get(h_tjet_pt);
  double wjet_pt = event.get(h_wjet_pt);
  double lepton_pt = event.get(h_lepton_pt);

  hist_tjet_pt->Fill(tjet_pt, event.weight);
  hist_wjet_pt->Fill(wjet_pt, event.weight);
  hist_lepton_pt->Fill(lepton_pt, event.weight);

  for(Hists *hist: hists_vector) {
    hist->fill(event);
  }
}


BinnedDNNHists::~BinnedDNNHists() {

  for(uint i = 0; i < hists_vector.size(); ++i) {
    delete hists_vector[i];
  }
  hists_vector.clear();
}
