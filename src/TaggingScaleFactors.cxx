#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/HighPtSingleTop/include/TaggingScaleFactors.h"
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

bool DeepAK8ScaleFactor::process_dummy(Event & event) {

  event.set(h_deepak8_weight, -1.);
  event.set(h_deepak8_weight_up, -1.);
  event.set(h_deepak8_weight_down, -1.);

  return true;
}


MyHOTVRScaleFactor::MyHOTVRScaleFactor(uhh2::Context & ctx, const TopJetId & id_topjet, const string & gen_handle_name, const string & xmlpathname):
  m_id_topjet(id_topjet),
  h_tophad(ctx.get_handle<vector<GenTop>>(gen_handle_name))
{
  h_toptag_weight      = ctx.declare_event_output<float>("weight_sfhotvr");
  h_toptag_weight_up   = ctx.declare_event_output<float>("weight_sfhotvr_up");
  h_toptag_weight_down = ctx.declare_event_output<float>("weight_sfhotvr_down");

  // open file and get scale factor histograms
  string path = ctx.get(xmlpathname);
  TFile *f = new TFile(path.c_str(), "READ");
  sf_merged = (TH1F*) f->Get("HOTVR/sf_mergedTop_nominal");
  sf_merged->SetDirectory(0);
  sf_merged_up = (TH1F*) f->Get("HOTVR/sf_mergedTop_up");
  sf_merged_up->SetDirectory(0);
  sf_merged_down = (TH1F*) f->Get("HOTVR/sf_mergedTop_down");
  sf_merged_down->SetDirectory(0);

  sf_semi = (TH1F*) f->Get("HOTVR/sf_semimerged_nominal")->Clone();
  sf_semi->SetDirectory(0);
  sf_semi_up = (TH1F*) f->Get("HOTVR/sf_semimerged_up")->Clone();
  sf_semi_up->SetDirectory(0);
  sf_semi_down = (TH1F*) f->Get("HOTVR/sf_semimerged_down")->Clone();
  sf_semi_down->SetDirectory(0);

  sf_not = (TH1F*) f->Get("HOTVR/sf_notmerged_nominal")->Clone();
  sf_not->SetDirectory(0);
  sf_not_up = (TH1F*) f->Get("HOTVR/sf_notmerged_up")->Clone();
  sf_not_up->SetDirectory(0);
  sf_not_down = (TH1F*) f->Get("HOTVR/sf_notmerged_down")->Clone();
  sf_not_down->SetDirectory(0);
  f->Close();

  string syst_direction_ = ctx.get("SystDirection_HOTVRTopTagSF", "nominal");
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

void MyHOTVRScaleFactor::get_sf(double pt, int category) {

  if(category == 3) {
    int bin = sf_merged->FindFixBin(pt);
    if(pt >= 5000.) bin = sf_merged->GetNbinsX();
    else if ( pt <= 200.) bin = 1;

    m_weight *= sf_merged->GetBinContent(bin);
    m_weight_up *= sf_merged_up->GetBinContent(bin);
    m_weight_down *= sf_merged_down->GetBinContent(bin);
  }
  else if(category == 2) {
    int bin = sf_semi->FindFixBin(pt);
    if(pt >= 5000.) bin = sf_semi->GetNbinsX();
    else if ( pt <= 200.) bin = 1;

    m_weight *= sf_semi->GetBinContent(bin);
    m_weight_up *= sf_semi_up->GetBinContent(bin);
    m_weight_down *= sf_semi_down->GetBinContent(bin);
  }
  else {
    int bin = sf_not->FindFixBin(pt);
    if(pt >= 5000.) bin = sf_not->GetNbinsX();
    else if ( pt <= 200.) bin = 1;

    m_weight *= sf_not->GetBinContent(bin);
    m_weight_up *= sf_not_up->GetBinContent(bin);
    m_weight_down *= sf_not_down->GetBinContent(bin);
  }
}

bool MyHOTVRScaleFactor::process(Event & event) {

  m_weight = 1.;
  m_weight_up = 1.;
  m_weight_down = 1.;

  if(event.isRealData) {
    event.set(h_toptag_weight, m_weight);
    event.set(h_toptag_weight_up, m_weight_up);
    event.set(h_toptag_weight_down, m_weight_down);
    return false;
  }

  vector<GenTop> gentops = event.get(h_tophad);
  if(gentops.size() == 0) {
    event.set(h_toptag_weight, m_weight);
    event.set(h_toptag_weight_up, m_weight_up);
    event.set(h_toptag_weight_down, m_weight_down);
    return false;
  }

  for(const auto & topjet : *event.topjets) {
    int nMatched = 0;
    bool bMatched = false;
    bool q1Matched = false;
    bool q2Matched = false;
    if(m_id_topjet(topjet, event)) {
      double dRmatch = min(1.5, max(0.1, 600.0 / (topjet.pt() * topjet.JEC_factor_raw()))); // calculate distance using clustering distance parameter
      // double dRmatch = sqrt(subjet.jetArea()/3.14);
      for(auto top : gentops) {
        if(deltaR(top.get_b(), topjet.v4()) < dRmatch) bMatched = true;
        if(deltaR(top.get_q1(), topjet.v4()) < dRmatch) q1Matched = true;
        if(deltaR(top.get_q2(), topjet.v4()) < dRmatch) q2Matched = true;
      }
      if(bMatched) ++nMatched;
      if(q1Matched) ++nMatched;
      if(q2Matched) ++nMatched;
      get_sf(topjet.pt(), nMatched);
    }
  }

  event.set(h_toptag_weight, m_weight);
  event.set(h_toptag_weight_up, m_weight_up);
  event.set(h_toptag_weight_down, m_weight_down);

  if (syst_direction == 1) {
    event.weight *= m_weight_up;
  } else if (syst_direction == -1) {
    event.weight *= m_weight_down;
  } else {
    event.weight *= m_weight;
  }

  return true;
}

bool MyHOTVRScaleFactor::process_dummy(Event & event) {

  event.set(h_toptag_weight, -1.);
  event.set(h_toptag_weight_up, -1.);
  event.set(h_toptag_weight_down, -1.);

  return true;
}
