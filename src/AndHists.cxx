#include "UHH2/HighPtSingleTop/include/AndHists.h"

#include "UHH2/common/include/LuminosityHists.h"
#include "UHH2/common/include/JetHists.h"

#include "UHH2/HOTVR/include/HOTVRHists.h"

#include "UHH2/HighPtSingleTop/include/Constants.h"
#include "UHH2/HighPtSingleTop/include/DNNHists.h"
#include "UHH2/HighPtSingleTop/include/MyEventHists.h"
#include "UHH2/HighPtSingleTop/include/MyElectronHists.h"
#include "UHH2/HighPtSingleTop/include/MyMuonHists.h"


using namespace std;
using namespace uhh2;


AndHists::AndHists(Context & ctx, const string & dirname):
  Hists(ctx, dirname+"_Counter"), m_dirname(dirname) {

  // Single histogram for event counting
  nevt = book<TH1F>("NEvt", "Number of weighted events", 0.5, 0, 1);
  // Single histogram for event weights
  wevt = book<TH1F>("WEvt", "Event weights (entries are unweighted)", 400, -2, 2);

  // Add common histogram classes
  hists_vector.push_back(new LuminosityHists(ctx, dirname + "_Lumi"));
  hists_vector.push_back(new MyEventHists(ctx, dirname + "_Event"));
  hists_vector.push_back(new MyElectronHists(ctx, dirname + "_Electron"));
  hists_vector.push_back(new MyMuonHists(ctx, dirname + "_Muon"));
  hists_vector.push_back(new JetHists(ctx, dirname + "_Jet"));
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


string AndHists::get_dirname() {

    return m_dirname;
}


AndHists::~AndHists() {

  for(uint i = 0; i < hists_vector.size(); ++i) {
    delete hists_vector[i];
  }
  hists_vector.clear();
}


BinnedDNNHists::BinnedDNNHists(Context & ctx, const string & dirname, const vector<string> inputs):
  Hists(ctx, dirname+"_Binning"), m_dirname(dirname) {

  h_toptag_pt = ctx.get_handle<double>("DNN_TopTagPt");

  hist_toptag_pt = book<TH1F>("toptag_pt", "t jet p_{T} [GeV]", MyConstants::pt_binning.size(), MyConstants::pt_binning_edges);

  hists_vector.push_back(new DNNHists(ctx, dirname+"_Full", inputs, "DNN_TopTagPt"));
  for(auto i : MyConstants::pt_binning) {
    hists_vector.push_back(new DNNHists(ctx, dirname+"_Pt"+to_string((int)(i.first))+"to"+to_string((int)(i.second)), inputs, "DNN_TopTagPt", i.first, i.second));
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
