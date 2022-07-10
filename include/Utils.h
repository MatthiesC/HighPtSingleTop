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
  const uhh2::Event::Handle<int> fHandle_dnnClass_tW_int;
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
class WTag {
public:
  enum algo {ParticleNet};
  // enum wp {WP_LOOSE, WP_MEDIUM, WP_TIGHT};
  enum wp {WP_DUMMY, WP_CUSTOM, WP_BkgEff0p050, WP_BkgEff0p010, WP_BkgEff0p005};
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
  const uhh2::Event::Handle<bool> fHandle_bool_reco_sel;
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
  // const ltt::Channel fChannel;
  const uhh2::Event::Handle<ltt::SingleTopGen_tWch> fHandle_GENtW;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Matrix_Top;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Matrix_WAss;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Matrix_WTop;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Matrix_tW_system;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Matrix_LeptonicW;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Matrix_SingleLepton;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Matrix_SingleNeutrino;
};

//____________________________________________________________________________________________________
class MatrixLevelSelection: public uhh2::Selection {
public:
  MatrixLevelSelection(uhh2::Context & ctx, const std::string & sel, const boost::optional<ltt::Channel> & channel = boost::none);
  virtual bool passes(const uhh2::Event & event) override;
private:
  const std::string fSel;
  const ltt::Channel fChannel;
  const uhh2::Event::Handle<uhh2::btw::ERegion_heavyTags> fHandle_Region_heavyTags;
  const uhh2::Event::Handle<ltt::SingleTopGen_tWch> fHandle_GENtW;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Matrix_Top;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Matrix_WAss;
  const uhh2::Event::Handle<GenLevelDef> fHandle_Matrix_SingleLepton;
};

//____________________________________________________________________________________________________
class VariablesOfInterest: public uhh2::AnalysisModule {
public:
  VariablesOfInterest(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

  private:
    const uhh2::Event::Handle<uhh2::btw::ERegion_heavyTags> fHandle_Region_heavyTags;

    const uhh2::Event::Handle<TopJet> fHandle_TheTopJet;
    const uhh2::Event::Handle<TopJet> fHandle_TheWJet;
    const uhh2::Event::Handle<uhh2::btw::LeptonicHemisphere> fHandle_LeptonicHemisphere;

    const uhh2::Event::Handle<ltt::SingleTopGen_tWch> fHandle_GENtW;
    const uhh2::Event::Handle<GenLevelDef> fHandle_Matrix_Top;
    const uhh2::Event::Handle<GenLevelDef> fHandle_Matrix_WAss;
    const uhh2::Event::Handle<GenLevelDef> fHandle_Matrix_tW_system;

    uhh2::Event::Handle<float> fHandle_VOI_Reco_TopQuarkPt;
    uhh2::Event::Handle<float> fHandle_VOI_Reco_TopQuarkEta;
    uhh2::Event::Handle<float> fHandle_VOI_Reco_TopQuarkY;
    uhh2::Event::Handle<float> fHandle_VOI_Reco_WBosonPt;
    uhh2::Event::Handle<float> fHandle_VOI_Reco_WBosonEta;
    uhh2::Event::Handle<float> fHandle_VOI_Reco_WBosonY;
    uhh2::Event::Handle<float> fHandle_VOI_Reco_tWSystemPt;
    uhh2::Event::Handle<float> fHandle_VOI_Reco_tWSystemEta;
    uhh2::Event::Handle<float> fHandle_VOI_Reco_tWSystemY;
    uhh2::Event::Handle<float> fHandle_VOI_Reco_tWSystemMass;
    uhh2::Event::Handle<float> fHandle_VOI_Reco_tWSystemDPhi;

    uhh2::Event::Handle<float> fHandle_VOI_Matrix_TopQuarkPt;
    uhh2::Event::Handle<float> fHandle_VOI_Matrix_TopQuarkEta;
    uhh2::Event::Handle<float> fHandle_VOI_Matrix_TopQuarkY;
    uhh2::Event::Handle<float> fHandle_VOI_Matrix_WBosonPt;
    uhh2::Event::Handle<float> fHandle_VOI_Matrix_WBosonEta;
    uhh2::Event::Handle<float> fHandle_VOI_Matrix_WBosonY;
    uhh2::Event::Handle<float> fHandle_VOI_Matrix_tWSystemPt;
    uhh2::Event::Handle<float> fHandle_VOI_Matrix_tWSystemEta;
    uhh2::Event::Handle<float> fHandle_VOI_Matrix_tWSystemY;
    uhh2::Event::Handle<float> fHandle_VOI_Matrix_tWSystemMass;
    uhh2::Event::Handle<float> fHandle_VOI_Matrix_tWSystemDPhi;
};

}}
