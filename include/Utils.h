#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Selection.h"
#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Utils.h"

#include "UHH2/common/include/Utils.h"

#include "UHH2/LegacyTopTagging/include/Constants.h"
#include "UHH2/LegacyTopTagging/include/SingleTopGen_tWch.h"
#include "UHH2/LegacyTopTagging/include/Utils.h"

#include "UHH2/HighPtSingleTop/include/AnalysisRegions.h"
#include "UHH2/HighPtSingleTop/include/Constants.h"


namespace uhh2 { namespace btw {

//____________________________________________________________________________________________________
class TWClassification_DNN: public uhh2::AnalysisModule {
public:
  TWClassification_DNN(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

private:
  const ltt::Channel fChannel;
  const uhh2::Event::Handle<ltt::SingleTopGen_tWch> fHandle_GENtW;
  const uhh2::Event::Handle<uhh2::btw::E_dnnClass_tW> fHandle_dnnClass_tW;
};

//____________________________________________________________________________________________________
class TWSignalSelection_DNN : public uhh2::Selection {
public:
  TWSignalSelection_DNN(uhh2::Context & ctx);
  virtual bool passes(const uhh2::Event & event) override;
private:
  const uhh2::Event::Handle<uhh2::btw::E_dnnClass_tW> fHandle_dnnClass_tW;
};

//____________________________________________________________________________________________________
class MttHist: public Hists {
public:
  MttHist(uhh2::Context & ctx, const std::string & dirname);
  virtual void fill(const uhh2::Event & event);
private:
  TH1F *hist_mtt;
  TH1F *hist_mtt_100;
};

//____________________________________________________________________________________________________
class BTWTriggerSelection : public uhh2::Selection {
public:
  BTWTriggerSelection(uhh2::Context & ctx, const bool low_pt);
  virtual bool passes(const uhh2::Event & event) override;
private:
  const Year fYear;
  const ltt::Channel fChannel;
  const bool fLowPt;
  enum class DataStream {
    isMC,
    isSingleMuon,
    isSingleElectron,
    isSinglePhoton,
    isEGamma,
  };
  DataStream fDataStream;
  // const double lumi_percentage_UL17_RunB = (41.5 - 36.7) / 41.5; // "note 1) this is only 36.7 out 41.5 fb-1 due to triggers not been included at start up" (https://twiki.cern.ch/twiki/bin/view/CMS/EgHLTRunIISummary#2017)
  const float lumi_percentage_UL17_RunB = 4.803 / 41.480; // = 11.580% ; Christopher's calculation with brilcalc tool
  // brilcalc lumi --normtag /cvmfs/cms-bril.cern.ch/cms-lumi-pog/Normtags/normtag_PHYSICS.json -i Cert_294927-306462_13TeV_UL2017_Collisions17_GoldenJSON.txt [--end 299329] (either with or without the last option)
  bool is_UL17_RunB(const Event & event) const { return fYear == Year::isUL17 && event.run <= 299329; };
  const float lumi_percentage_UL16preVFP_without_TkMu50 = 2.792 / 19.536; // = 14.290% ; Christopher's calculation with brilcalc tool
  // brilcalc lumi --normtag /cvmfs/cms-bril.cern.ch/cms-lumi-pog/Normtags/normtag_PHYSICS.json -i Cert_271036-284044_13TeV_Legacy2016_Collisions16_JSON_UL16preVFP.txt [--end 274888]
  bool is_UL16preVFP_without_TkMu50(const Event & event) const { return fYear == Year::isUL16preVFP && event.run < 274889; }; // < is correct, not <=, source: https://twiki.cern.ch/twiki/bin/view/CMS/MuonHLT2016#2016_Runs

  std::unique_ptr<uhh2::Selection> fTrigSel_IsoMu24;
  std::unique_ptr<uhh2::Selection> fTrigSel_IsoTkMu24;
  std::unique_ptr<uhh2::Selection> fTrigSel_IsoMu27;

  std::unique_ptr<uhh2::Selection> fTrigSel_Mu50;
  std::unique_ptr<uhh2::Selection> fTrigSel_TkMu50;
  std::unique_ptr<uhh2::Selection> fTrigSel_OldMu100;
  std::unique_ptr<uhh2::Selection> fTrigSel_TkMu100;

  std::unique_ptr<uhh2::Selection> fTrigSel_Ele27_WPTight_Gsf;
  std::unique_ptr<uhh2::Selection> fTrigSel_Ele35_WPTight_Gsf;
  std::unique_ptr<uhh2::Selection> fTrigSel_Ele32_WPTight_Gsf;

  std::unique_ptr<uhh2::Selection> fTrigSel_Ele115_CaloIdVT_GsfTrkIdT;
  std::unique_ptr<uhh2::Selection> fTrigSel_Photon175;
  std::unique_ptr<uhh2::Selection> fTrigSel_Photon200;
};

//____________________________________________________________________________________________________
class WTag {
public:
  enum algo {ParticleNet};
  // enum wp {WP_LOOSE, WP_MEDIUM, WP_TIGHT};
  enum wp {WP_DUMMY, WP_BkgEff0p050, WP_BkgEff0p010, WP_BkgEff0p005};
  explicit WTag(const WTag::algo & algorithm, const WTag::wp & working_point);
  bool operator()(const TopJet & jet, const uhh2::Event & event) const;
private:
  const algo fAlgo;
  const wp fWP;
};

//____________________________________________________________________________________________________
class FatJetTagger: public uhh2::AnalysisModule {
public:
  FatJetTagger(uhh2::Context & ctx, const WTag::algo & wtag_algo, const WTag::wp & wtag_wp);
  virtual bool process(uhh2::Event & event) override;

private:
  const TopJetId fWTagID;
  const TopJetId fHOTVRTopTagID = ltt::HOTVRTopTag(); // does not include tau32groomed cut

  const uhh2::Event::Handle<std::vector<TopJet>> fHandle_tJets;
  const uhh2::Event::Handle<std::vector<TopJet>> fHandle_AK8Collection_rec;
  const uhh2::Event::Handle<std::vector<TopJet>> fHandle_WJets;
};

//____________________________________________________________________________________________________
class TopTagSetter: public uhh2::AnalysisModule {
public:
  TopTagSetter(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

private:
  const uhh2::Event::Handle<uhh2::btw::ERegion> fHandle_Region;
  const uhh2::Event::Handle<uhh2::btw::ERegion_heavyTags> fHandle_Region_heavyTags;
  const uhh2::Event::Handle<std::vector<TopJet>> fHandle_tJets;
  const uhh2::Event::Handle<TopJet> fHandle_TheTopJet;
  // const bool doWarning;
};

//____________________________________________________________________________________________________
class WTagSetter: public uhh2::AnalysisModule {
public:
  WTagSetter(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

private:
  const uhh2::Event::Handle<uhh2::btw::ERegion> fHandle_Region;
  const uhh2::Event::Handle<uhh2::btw::ERegion_heavyTags> fHandle_Region_heavyTags;
  const uhh2::Event::Handle<std::vector<TopJet>> fHandle_WJets;
  const uhh2::Event::Handle<TopJet> fHandle_TheWJet;
  const uhh2::Event::Handle<std::vector<TopJet>> fHandle_AK8Collection_rec;
  // const bool doWarning;
};

//____________________________________________________________________________________________________
std::vector<Particle> NeutrinoReconstruction(const FlavorParticle & lepton, const MET & met);

//____________________________________________________________________________________________________
enum class JetFindingMethod {
  bJetsDR,
  bScoreLeadingJetDR,
  bJets,
  allJets,
  None,
};

typedef struct {
  int index;
  std::string name;
  std::string tlatex;
} JetFindingMethodInfo;

const std::map<JetFindingMethod, JetFindingMethodInfo> kJetFindingMethods = {
  { JetFindingMethod::bJetsDR, { 0, "bJetsDR", "b jets within hemisphere" }},
  { JetFindingMethod::bScoreLeadingJetDR, { 1, "bScoreLeadingJetDR", "score-leading b jet within hemisphere" }},
  { JetFindingMethod::bJets, { 2, "bJets", "all b jets" }},
  { JetFindingMethod::allJets, { 3, "allJets", "all jets" }},
  { JetFindingMethod::None, { 4, "None", "none" }},
};

class LeptonicHemisphere: public Particle {
public:
  LeptonicHemisphere() {
    m_jet = Jet();
    m_chsjet = Jet();
    m_neutrino = Particle();
    m_lepton = FlavorParticle();
    m_method = JetFindingMethod::None;
  };

  LeptonicHemisphere(
    const Jet & jet,
    const Jet & chsjet,
    const Particle & neutrino,
    const FlavorParticle & lepton
  ) {
    m_jet = jet;
    m_chsjet = chsjet;
    m_neutrino = neutrino;
    m_lepton = lepton;
    set_charge(m_jet.charge() + m_lepton.charge());
    set_v4(m_jet.v4() + m_neutrino.v4() + m_lepton.v4());
  };

  void set_jet(const Jet & x) { m_jet = x; };
  void set_chsjet(const Jet & x) { m_chsjet = x; };
  void set_neutrino(const Particle & x) { m_neutrino = x; };
  void set_lepton(const FlavorParticle & x) { m_lepton = x; };
  void set_method(const JetFindingMethod & x) { m_method = x; };
  void set_nsolutions(const unsigned int & x) { m_nsolutions = x; };

  const Jet & jet() const { return m_jet; };
  const Jet & chsjet() const { return m_chsjet; };
  const Particle & neutrino() const { return m_neutrino; };
  const FlavorParticle & lepton() const { return m_lepton; };
  JetFindingMethod method() const { return m_method; };
  unsigned int nsolutions() const { return m_nsolutions; };

  Particle wboson() const {
    Particle w;
    w.set_charge(m_lepton.charge());
    w.set_v4(m_neutrino.v4() + m_lepton.v4());
    return w;
  };
  float btag_DeepJet() const { return m_chsjet.btag_DeepJet(); };
  float dr_max() const { return std::max(uhh2::deltaR(m_lepton.v4(), m_neutrino.v4()), std::max(uhh2::deltaR(m_lepton.v4(), m_jet.v4()), uhh2::deltaR(m_neutrino.v4(), m_jet.v4()))); };
  float dr_leptonjet() const { return uhh2::deltaR(m_lepton.v4(), m_jet.v4()); };
  float mtw() const { return std::sqrt(2. * m_lepton.pt() * m_neutrino.pt() * (1 - std::cos(uhh2::deltaPhi(m_lepton.v4(), m_neutrino.v4())))); };

private:
  Jet m_jet;
  Jet m_chsjet;
  Particle m_neutrino;
  FlavorParticle m_lepton;
  JetFindingMethod m_method = JetFindingMethod::None;
  unsigned int m_nsolutions = -1; // total number of solutions for leptonic hemisphere
};

//____________________________________________________________________________________________________
class LeptonicHemisphereReco: public uhh2::AnalysisModule {
public:
  LeptonicHemisphereReco(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

private:
  const uhh2::Event::Handle<std::vector<Jet>> fHandle_CHSjets;
  const uhh2::Event::Handle<std::vector<Jet>> fHandle_pairedPUPPIjets;
  const uhh2::Event::Handle<std::vector<Jet>> fHandle_bJets;
  const uhh2::Event::Handle<FlavorParticle> fHandle_PrimaryLepton;
  const uhh2::Event::Handle<uhh2::btw::ERegion_heavyTags> fHandle_Region_heavyTags;
  const uhh2::Event::Handle<uhh2::btw::LeptonicHemisphere> fHandle_LeptonicHemisphere;
};

//____________________________________________________________________________________________________
class LeptonicTopQuarkMassSelection : public uhh2::Selection {
public:
  LeptonicTopQuarkMassSelection(uhh2::Context & ctx, const float m_min, const float m_max);
  virtual bool passes(const uhh2::Event & event) override;
private:
  const float fMassMin;
  const float fMassMax;
  const uhh2::Event::Handle<uhh2::btw::ERegion_heavyTags> fHandle_Region_heavyTags;
  const uhh2::Event::Handle<uhh2::btw::LeptonicHemisphere> fHandle_LeptonicHemisphere;
};

//____________________________________________________________________________________________________
class TWClassification_TrueDecay: public uhh2::AnalysisModule {
public:
  TWClassification_TrueDecay(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

private:
  const ltt::Channel fChannel;
  const uhh2::Event::Handle<ltt::SingleTopGen_tWch> fHandle_GENtW;
  const uhh2::Event::Handle<uhh2::btw::ERegion_heavyTags> fHandle_Region_heavyTags;
  const uhh2::Event::Handle<uhh2::btw::E_trueClass_tW> fHandle_trueClass_tW;
  const uhh2::Event::Handle<int> fHandle_trueClass_tW_int;
};

//____________________________________________________________________________________________________
class TWSignalSelection_TrueDecay : public uhh2::Selection {
public:
  TWSignalSelection_TrueDecay(uhh2::Context & ctx);
  virtual bool passes(const uhh2::Event & event) override;
private:
  const uhh2::Event::Handle<uhh2::btw::E_trueClass_tW> fHandle_trueClass_tW;
};

//____________________________________________________________________________________________________
typedef Particle GenLevelDef;

//____________________________________________________________________________________________________
class GenLevelDefinitions: public uhh2::AnalysisModule {
public:
  GenLevelDefinitions(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

private:
  const ltt::Channel fChannel;
  const uhh2::Event::Handle<ltt::SingleTopGen_tWch> fHandle_GENtW;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Parton_Top;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Parton_WAss;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Parton_WTop;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Parton_tW_system;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Parton_LeptonicW;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Parton_SingleLepton;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Parton_SingleNeutrino;
};

}}
