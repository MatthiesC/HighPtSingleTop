#include "UHH2/HighPtSingleTop/include/AndHists.h"

#include "UHH2/common/include/LuminosityHists.h"
#include "UHH2/common/include/JetHists.h"

#include "UHH2/HighPtSingleTop/include/MyEventHists.h"
#include "UHH2/HighPtSingleTop/include/MyElectronHists.h"
#include "UHH2/HighPtSingleTop/include/MyMuonHists.h"


using namespace std;
using namespace uhh2;

AndHists::AndHists(Context & ctx, const string & dirname):
  Hists(ctx, dirname+"_Counter")
{
  // Single histogram for event counting
  nevt = book<TH1F>("NEvt", "", 0.5, 0, 1);

  // Add common histogram classes
  hists_vector.push_back(new LuminosityHists(ctx, dirname + "_Lumi"));
  hists_vector.push_back(new MyEventHists(ctx, dirname + "_Event"));
  if(ctx.get("analysis_channel") == "ELECTRON") hists_vector.push_back(new MyElectronHists(ctx, dirname + "_Electron"));
  else if(ctx.get("analysis_channel") == "MUON") hists_vector.push_back(new MyMuonHists(ctx, dirname + "_Muon"));
  hists_vector.push_back(new JetHists(ctx, dirname + "_Jet"));
}

void AndHists::fill(const Event & event) {
  nevt->Fill(0., event.weight);
  for (Hists *hist : hists_vector)
    {
      hist->fill(event);
    }
}

void AndHists::add_hist(Hists *hist) {
  hists_vector.push_back(hist);
}

AndHists::~AndHists() {
  for (unsigned int i = 0; i < hists_vector.size(); ++i)
    {
      delete hists_vector[i];
    }
  hists_vector.clear();
}
