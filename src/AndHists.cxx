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
  hists_vector.push_back(new HOTVRHists(ctx, dirname +"_Hotvr"));
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


void AndHists::add_WTagHists(Context & ctx, const bool & var_binned) {

  hists_vector.push_back(new WTagHists(ctx, m_dirname + "_WTagHists_Full"));
  if(var_binned) {
    hists_vector.push_back(new WTagHists(ctx, m_dirname + "_WTagHists_Pt0to300", 0, 300));
    hists_vector.push_back(new WTagHists(ctx, m_dirname + "_WTagHists_Pt300toInf", 300));
  }
}


void AndHists::add_TopTagHists(Context & ctx, const bool & var_binned) {

  hists_vector.push_back(new TopTagHists(ctx, m_dirname + "_TopTagHists_Full"));
  if (var_binned) {
    hists_vector.push_back(new TopTagHists(ctx, m_dirname + "_TopTagHists_Pt0to300", 0, 300));
    hists_vector.push_back(new TopTagHists(ctx, m_dirname + "_TopTagHists_Pt300toInf", 300));
  }
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


BinnedDNNHists::BinnedDNNHists(Context & ctx, const string & dirname, const vector<string> inputs, const vector<DNNInput> inputs_info):
  Hists(ctx, dirname+"_Binning"), m_dirname(dirname) {

  h_toptag_pt = ctx.get_handle<double>("DNN_TopTagPt");

  hist_toptag_pt = book<TH1F>("toptag_pt", "t jet p_{T} [GeV]", MyConstants::pt_binning.size(), MyConstants::pt_binning_edges);

  hists_vector.push_back(new DNNHists(ctx, dirname+"_Full", inputs, inputs_info, "DNN_TopTagPt"));
  // hists_vector.push_back(new DNNHists(ctx, dirname+"_Pt200to350", inputs, inputs_info, "DNN_TopTagPt", 0, 350));
  // hists_vector.push_back(new DNNHists(ctx, dirname+"_Pt350toInf", inputs, inputs_info, "DNN_TopTagPt", 350));
  for(auto i : MyConstants::pt_binning) {
    hists_vector.push_back(new DNNHists(ctx, dirname+"_Pt"+to_string((int)(i.first))+"to"+to_string((int)(i.second)), inputs, inputs_info, "DNN_TopTagPt", i.first, i.second));
  }
}


void BinnedDNNHists::fill(const Event & event) {

  double toptag_pt = event.get(h_toptag_pt);

  hist_toptag_pt->Fill(toptag_pt, event.weight);

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
