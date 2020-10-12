#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/HighPtSingleTop/include/DeepAK8ScaleFactor.h"
#include "UHH2/HighPtSingleTop/include/TaggedJets.h"

#include <TFile.h>

using namespace std;
using namespace uhh2;


DeepAK8ScaleFactor::DeepAK8ScaleFactor(Context & ctx, const string & object_name, const bool & mass_decorrelated, const WTaggedJets::wp & working_point) {

  if(!(object_name == "Top" || object_name == "W")) throw runtime_error("DeepAK8ScaleFactor: Invalid object name. Only W/Top are valid.");

  h_deepak8_weight      = ctx.declare_event_output<float>("weight_sfdeepak8_"+object_name);
  h_deepak8_weight_up   = ctx.declare_event_output<float>("weight_sfdeepak8_"+object_name+"_up");
  h_deepak8_weight_down = ctx.declare_event_output<float>("weight_sfdeepak8_"+object_name+"_down");

  auto dataset_type = ctx.get("dataset_type");
  bool is_mc = dataset_type == "MC";
  if(!is_mc) {
    cout << "Warning: DeepAK8ScaleFactor will not have an effect on this non-MC sample (dataset_type = '" + dataset_type + "')" << endl;
    return;
  }
  year = extract_year(ctx);
  TString yeartag = "";
  if(year == Year::is2016v3 || year == Year::is2016v2) yeartag = "2016";
  else if(year == Year::is2017v1 || year == Year::is2017v2) yeartag = "2017";
  else if(year == Year::is2018) yeartag = "2018";
  else throw runtime_error("DeepAK8ScaleFactor: invalid year");

  h_taggedjet = ctx.get_handle<TopJet>("WTaggedJet");

  unique_ptr<TFile> sf_file;
  TString path = ctx.get("uhh2Dir")+"/HighPtSingleTop/data/ScaleFactors/";
  sf_file.reset(new TFile(path+"201X/DeepAK8V2_Top_W_SFs.csv.root","READ"));
  TString wp_string = "";
  if(working_point == WTaggedJets::WP_VERYLOOSE) wp_string = "5p0";
  else if(working_point == WTaggedJets::WP_LOOSE) wp_string = "2p5";
  else if(working_point == WTaggedJets::WP_MEDIUM) wp_string = "1p0";
  else if(working_point == WTaggedJets::WP_TIGHT) wp_string = "0p5";
  else if(working_point == WTaggedJets::WP_VERYTIGHT) wp_string = "0p1";
  TString graph_name = object_name+"_"+yeartag+"_"+(mass_decorrelated ? "MassDecorr" : "Nominal")+"_"+wp_string;
  sf_graph.reset((TGraphAsymmErrors*)sf_file->Get(graph_name));

  double dummy_y_min, dummy_y_max;
  sf_graph->GetPoint(0, pt_minimum, dummy_y_min);
  sf_graph->GetPoint(sf_graph->GetN()-1, pt_maximum, dummy_y_max);
  pt_minimum = pt_minimum - sf_graph->GetErrorXlow(0);
  pt_maximum = pt_maximum + sf_graph->GetErrorXhigh(sf_graph->GetN()-1);

  string syst_direction_ = ctx.get("SystDirection_DeepAK8WTagSF", "nominal");
  if(syst_direction_ == "up") {
    syst_direction = 1;
  }
  else if(syst_direction_ == "down") {
    syst_direction = -1;
  }
  else {
    syst_direction = 0;
  }
}

bool DeepAK8ScaleFactor::process(Event & event) {

  if(event.isRealData) {
    event.set(h_deepak8_weight, 1.);
    event.set(h_deepak8_weight_up, 1.);
    event.set(h_deepak8_weight_down, 1.);
    return true;
  }

  const TopJet taggedjet = event.get(h_taggedjet);
  double pt = taggedjet.v4().Pt();
  bool overflow_pt(false);
  if(pt < pt_minimum) throw runtime_error("DeepAK8ScaleFactor::process(): jet pt too small, not supported. Make sure to properly clean the collection of tagged AK8 jets beforehand.");
  else if(pt > pt_maximum) overflow_pt = true;

  int idx(0);
  if(!overflow_pt) {
    bool keep_going(true);
    while(keep_going) {
      double x, dummy_y;
      sf_graph->GetPoint(idx, x, dummy_y);
      keep_going = (pt > x + sf_graph->GetErrorXhigh(idx));
      if(keep_going) idx++;
    }
  }
  else {
    idx = sf_graph->GetN() - 1;
  }

  double dummy_x, sf;
  sf_graph->GetPoint(idx, dummy_x, sf);
  double sf_up = sf + sf_graph->GetErrorYhigh(idx)*(overflow_pt ? 2. : 1.);
  double sf_down = sf - sf_graph->GetErrorYlow(idx)*(overflow_pt ? 2. : 1.);

  event.set(h_deepak8_weight, sf);
  event.set(h_deepak8_weight_up, sf_up);
  event.set(h_deepak8_weight_down, sf_down);

  if (syst_direction == 1) {
    event.weight *= sf_up;
  } else if (syst_direction == -1) {
    event.weight *= sf_down;
  } else {
    event.weight *= sf;
  }

  return true;
}
