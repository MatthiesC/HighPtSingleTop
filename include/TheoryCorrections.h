#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "TFile.h"
#include "TH1F.h"


/*
According to this twiki:
https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopPtReweighting
one is free to apply "data-NLO" weights in a tW measurement
*/

class TopPtReweighting: public uhh2::AnalysisModule {
 public:
  explicit TopPtReweighting(uhh2::Context & ctx, double a, double b, const std::string& weight_name="weight_ttbar_toppt");
  virtual bool process(uhh2::Event & event) override;

 private:
  double a_, b_;
  uhh2::Event::Handle<double> h_weight;
  bool is_TTbar;
  bool apply;
};


/*
Taken from https://github.com/UHH2/VHResonances/blob/master/Analysis/python/PlotNLOCorrections.py

- EWK corrections are available for W+jets, Z+jets, gamma+jets samples in the "merged_kfactors_*.root" files under the name "kfactor_monojet_ewk"
- QCD NLO corrections are available for W+jets, Z+jets, gamma+jets in the same files under the name "kfactor_monojet_qcd". Those are calculated for 2016 samples.
- 2017 version of QCD NLO corrections are available for Z+jets (ll + nunu cases) in the "kfac_*_filter" files.
- QCD NNLO corrections are in the "lindert_qcd_nnlo_sf" file with the following convention:
    - eej -> Z(ll) +jets
    - vvj -> Z(nunu) +jets
    - evj -> W +jets
    - aj -> gamma +jets
- QCD NNLO corrections need to be applied on top of EWK corrections for NLO samples and on top of EWK + QCD NLO corrections for LO samples.
- According to Andreas "I do not apply the NNLO corrections. I have not seen any evidence that they actually improve data/MC agreement. I do not trust them."
- For W+Jets @LO for 2017 and 2018: wjet_dress_monojet or wjet_dress_inclusive in "2017_gen_v_pt_qcd_sf.root"
- In the "merged_kfactors_*.root" file, for Z and W + jets, the qcd_ewk histograms are also present: qcd_ewk = QCD * EWK
- taken from https://github.com/bu-cms/bucoffea/tree/master/bucoffea/data/sf/theory
- relative to those studies https://arxiv.org/abs/1705.04664

Example module can be found here: https://github.com/UHH2/VHResonances/blob/master/src/HiggsToWWModules.cxx
*/

class VJetsReweighting: public uhh2::AnalysisModule {
 public:
  explicit VJetsReweighting(uhh2::Context & ctx, const std::string& weight_name="weight_vjets");
  virtual bool process(uhh2::Event & event) override;

 private:
  std::unordered_map<std::string, std::unique_ptr<TH1F>> histos;

  void load_histo(TFile* file, const std::string& name, const std::string& histName);
  double get_v_pt(uhh2::Event & event);
  double evaluate(const std::string& name, double pt);

  bool is_2016_nonUL;
  bool is_WJets;
  bool is_DYJets;
  bool apply_EWK;
  bool apply_QCD_EWK;
  bool apply_QCD_NLO;
  bool apply_QCD_NNLO;

  uhh2::Event::Handle<double> h_weight_EWK;
  uhh2::Event::Handle<double> h_weight_QCD_EWK;
  uhh2::Event::Handle<double> h_weight_QCD_NLO;
  uhh2::Event::Handle<double> h_weight_QCD_NNLO;
};
