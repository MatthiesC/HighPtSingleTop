#include "UHH2/core/include/Utils.h"

#include "UHH2/common/include/Utils.h"

#include "UHH2/HighPtSingleTop/include/TheoryCorrections.h"

using namespace std;
using namespace uhh2;


TopPtReweighting::TopPtReweighting(Context & ctx, double a, double b, const string& weight_name):
  a_(a),
  b_(b),
  h_weight(ctx.declare_event_output<double>(weight_name)),
  is_TTbar(ctx.get("dataset_version").find("TTbar") == 0),
  apply(string2bool(ctx.get("TopPtReweighting"))) {}

bool TopPtReweighting::process(Event & event) {

  double weight(1.);

  if(is_TTbar) {
    double pt_top(-1.);
    double pt_antitop(-1.);

    for(const GenParticle & gp : *event.genparticles) {
      if(gp.pdgId() == 6) pt_top = gp.v4().Pt();
      else if(gp.pdgId() == -6) pt_antitop = gp.v4().Pt();
    }
    if(pt_top < 0 || pt_antitop < 0) throw runtime_error("TopPtReweighting::process(): top or antitop pT is negative.");
    pt_top = min(pt_top, 500.);
    pt_antitop = min(pt_antitop, 500.);

    weight = sqrt(exp(a_+b_*pt_top)*exp(a_+b_*pt_antitop));

    if(apply) event.weight *= weight;
  }

  event.set(h_weight, weight);

  return true;
}


VJetsReweighting::VJetsReweighting(Context & ctx, const string& weight_name):
  is_2016_nonUL(extract_year(ctx) == Year::is2016v3),
  is_WJets(ctx.get("dataset_version").find("WJets") == 0),
  is_DYJets(ctx.get("dataset_version").find("DYJets") == 0),
  apply_EWK(string2bool(ctx.get("VJetsReweighting_do_EWK"))),
  apply_QCD_EWK(string2bool(ctx.get("VJetsReweighting_do_QCD_EWK"))),
  apply_QCD_NLO(string2bool(ctx.get("VJetsReweighting_do_QCD_NLO"))),
  apply_QCD_NNLO(string2bool(ctx.get("VJetsReweighting_do_QCD_NNLO"))),
  h_weight_EWK(ctx.declare_event_output<double>(weight_name+"_EWK")),
  h_weight_QCD_EWK(ctx.declare_event_output<double>(weight_name+"_QCD_EWK")),
  h_weight_QCD_NLO(ctx.declare_event_output<double>(weight_name+"_QCD_NLO")),
  h_weight_QCD_NNLO(ctx.declare_event_output<double>(weight_name+"_QCD_NNLO"))
{
  if((apply_QCD_EWK && (apply_EWK || apply_QCD_NLO)) || (apply_QCD_NNLO && !(apply_QCD_EWK || (apply_EWK && apply_QCD_NLO)))) {
    throw invalid_argument("VJetsReweighting: You are not allowed to use the specified combination of correction scale factors.");
  }

  string filesDir = ctx.get("uhh2Dir")+"HighPtSingleTop/data/ScaleFactors/VJetsCorrections/";
  for(const string& proc : {"w", "z"}) {
    TFile* file = new TFile((filesDir+"merged_kfactors_"+proc+"jets.root").c_str());
    for(const string& corr : {"ewk", "qcd", "qcd_ewk"}) load_histo(file, (string)(proc+"_"+corr), (string)("kfactor_monojet_"+corr));
    file->Close();
  }
  for(const string& proc : {"dy", "znn"}) {
    TFile* file = new TFile((filesDir+"kfac_"+proc+"_filter.root").c_str());
    load_histo(file, (string)(proc+"_qcd_2017"), (string)("kfac_"+proc+"_filter"));
    file->Close();
  }
  TFile* file = new TFile((filesDir+"2017_gen_v_pt_qcd_sf.root").c_str());
  load_histo(file, "w_qcd_2017", "wjet_dress_inclusive");
  file->Close();
  file = new TFile((filesDir+"lindert_qcd_nnlo_sf.root").c_str());
  for(const string& proc : {"eej", "evj", "vvj"}) load_histo(file, (string)(proc+"_qcd_nnlo"), (string)(proc));
  file->Close();
}

void VJetsReweighting::load_histo(TFile* file, const string& name, const string& histName) {

  histos[name].reset((TH1F*)file->Get(histName.c_str()));
  histos[name]->SetDirectory(0);
}

double VJetsReweighting::get_v_pt(Event & event) {

  if(!(is_DYJets || is_WJets)) throw runtime_error("VJetsReweighting::get_v_pt(): Calling this function on non-WJets/DYJets sample makes no sense.");
  double pt(-1.);
  bool v_found(false);
  for(const GenParticle & gp : *event.genparticles) {
    if(is_WJets && gp.status() == 22 && abs(gp.pdgId()) == 24) {
      pt = gp.v4().Pt();
      v_found = true;
    }
    else if(is_DYJets && gp.status() == 22 && abs(gp.pdgId()) == 23) {
      pt = gp.v4().Pt();
      v_found = true;
    }
  }
  if(!v_found) {
    int n_status23_leptons(0);
    GenParticle d1, d2; // daughters of V boson
    for(const GenParticle & gp : *event.genparticles) {
      if(gp.status() == 23 && abs(gp.pdgId()) >= 11 && abs(gp.pdgId()) <= 16) {
        n_status23_leptons++;
        if(gp.pdgId() > 0) d1 = gp;
        else d2 = gp;
      }
    }
    if(n_status23_leptons != 2) throw runtime_error("VJetsReweighting::get_v_pt(): Did not find exactly two V daughter candidates.");
    pt = (d1.v4() + d2.v4()).Pt();
  }

  return pt;
}

double VJetsReweighting::evaluate(const string& name, double pt) {

  int firstBin = 1;
  int lastBin = histos[name]->GetNbinsX();
  double h_min = histos[name]->GetBinCenter(firstBin)-0.5*histos[name]->GetBinWidth(firstBin);
  double h_max = histos[name]->GetBinCenter(lastBin)+0.5*histos[name]->GetBinWidth(lastBin);
  double pt_for_eval = pt;
  pt_for_eval = (pt_for_eval > h_min) ? pt_for_eval : h_min+0.001;
  pt_for_eval = (pt_for_eval < h_max) ? pt_for_eval : h_max-0.001;

  return histos[name]->GetBinContent(histos[name]->FindBin(pt_for_eval));
}

bool VJetsReweighting::process(Event & event) {

  double weight_EWK(1.);
  double weight_QCD_EWK(1.);
  double weight_QCD_NLO(1.);
  double weight_QCD_NNLO(1.);

  if(is_WJets || is_DYJets) {
    double pt = get_v_pt(event);
    string process = "";

    // QCD EWK
    if(is_WJets) process = "w";
    if(is_DYJets) process = "z";
    weight_QCD_EWK = evaluate(process+"_qcd_ewk", pt);
    if(apply_QCD_EWK) event.weight *= weight_QCD_EWK;

    // EWK
    weight_EWK = evaluate(process+"_ewk", pt);
    if(apply_EWK) event.weight *= weight_EWK;

    // QCD NLO
    if(!is_2016_nonUL && is_DYJets) process = "dy";
    weight_QCD_NLO = evaluate(process+"_qcd"+(is_2016_nonUL ? "" : "_2017"), pt);
    if(apply_QCD_NLO) event.weight *= weight_QCD_NLO;

    // QCD NNLO
    if(is_DYJets) process = "eej";
    if(is_WJets) process = "evj";
    weight_QCD_NNLO = evaluate(process+"_qcd_nnlo", pt);
    if(apply_QCD_NNLO) event.weight *= weight_QCD_NNLO;
  }

  event.set(h_weight_EWK, weight_EWK);
  event.set(h_weight_QCD_EWK, weight_QCD_EWK);
  event.set(h_weight_QCD_NLO, weight_QCD_NLO);
  event.set(h_weight_QCD_NNLO, weight_QCD_NNLO);

  return true;
}
