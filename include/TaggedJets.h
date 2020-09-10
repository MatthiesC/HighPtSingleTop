#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/TopJetIds.h"
#include "UHH2/common/include/JetIds.h"


class TopTaggedJet: public uhh2::AnalysisModule {
 public:
  explicit TopTaggedJet(uhh2::Context & ctx,
			TopJetId topjetid,
			const std::string & h_name="TopTaggedJet");

  virtual bool process(uhh2::Event & event) override;

  virtual ~TopTaggedJet();

 private:
  uhh2::Event::Handle<TopJet> h_toptaggedjet;
  TopJetId m_topjetid;
};


class Ak8Jets: public uhh2::AnalysisModule {
public:
  explicit Ak8Jets(uhh2::Context & ctx,
           const std::string & h_name_ak8jets="Ak8Jets");

  virtual bool process(uhh2::Event & event) override;

  virtual ~Ak8Jets();

private:
  uhh2::Event::Handle<std::vector<TopJet>> h_ak8jets_rec;
  uhh2::Event::Handle<std::vector<TopJet>> h_ak8jets;
};


class WTaggedJets: public uhh2::AnalysisModule {
public:
  explicit WTaggedJets(uhh2::Context & ctx,
           const std::string & h_name_analysis="WJets",
           const std::string & h_name_loose="WJetsLoose",
           const std::string & h_name_medium="WJetsMedium",
           const std::string & h_name_tight="WJetsTight",
           // loose, medium, tight, deepAk8, tau-only cuts, etc.
				   const std::string & h_name_ak8jets="Ak8Jets");

  virtual bool process(uhh2::Event & event) override;

  virtual ~WTaggedJets();

private:
	uhh2::Event::Handle<std::vector<TopJet>> h_ak8jets;
  uhh2::Event::Handle<std::vector<TopJet>> h_wtaggedjets_loose, h_wtaggedjets_medium, h_wtaggedjets_tight, h_wtaggedjets_analysis;
};


class BTaggedJets: public uhh2::AnalysisModule {
 public:
  explicit BTaggedJets(uhh2::Context & ctx,
		       BTag::algo btagalgo,
		       BTag::wp workingpoint,
		       const std::string & h_name_analysis="BJets",
		       const std::string & h_name_loose="BJetsLoose",
		       const std::string & h_name_medium="BJetsMedium",
		       const std::string & h_name_tight="BJetsTight");

  virtual bool process(uhh2::Event & event) override;

  virtual ~BTaggedJets();

 private:
  uhh2::Event::Handle<std::vector<Jet>> h_btaggedjets_loose;
  uhh2::Event::Handle<std::vector<Jet>> h_btaggedjets_medium;
  uhh2::Event::Handle<std::vector<Jet>> h_btaggedjets_tight;
  uhh2::Event::Handle<std::vector<Jet>> h_btaggedjets_analysis;

  BTag::algo m_btagalgo;
  BTag::wp m_workingpoint;
};


// creates event handles for ak4 jets which have been filtered not to be inside/outside the t-tagged jet

class NonTopAK4Jets: public uhh2::AnalysisModule {
 public:
  explicit NonTopAK4Jets(uhh2::Context & ctx,
			 BTag::algo btagalgo,
			 BTag::wp workingpoint,
			 double toptag_radius,
			 const std::string & h_name_topEx_jets="TopExJets",
			 const std::string & h_name_topEx_bAnalysis="TopExBJets",
			 const std::string & h_name_topEx_bLoose="TopExBJetsLoose",
			 const std::string & h_name_topEx_bMedium="TopExBJetsMedium",
			 const std::string & h_name_topEx_bTight="TopExBJetsTight",
			 const std::string & h_name_topIn_jets="TopInJets",
			 const std::string & h_name_topIn_bAnalysis="TopInBJets",
			 const std::string & h_name_topIn_bLoose="TopInBJetsLoose",
			 const std::string & h_name_topIn_bMedium="TopInBJetsMedium",
			 const std::string & h_name_topIn_bTight="TopInBJetsTight",
			 const std::string & h_name_TopTag="TopTaggedJet",
			 const double rho = 600);

  virtual bool process(uhh2::Event & event) override;

  virtual ~NonTopAK4Jets();

 private:
  uhh2::Event::Handle<std::vector<Jet>> h_topEx_ak4;
  uhh2::Event::Handle<std::vector<Jet>> h_topEx_ak4_bLoose;
  uhh2::Event::Handle<std::vector<Jet>> h_topEx_ak4_bMedium;
  uhh2::Event::Handle<std::vector<Jet>> h_topEx_ak4_bTight;
  uhh2::Event::Handle<std::vector<Jet>> h_topEx_ak4_bAnalysis;
  uhh2::Event::Handle<std::vector<Jet>> h_topIn_ak4;
  uhh2::Event::Handle<std::vector<Jet>> h_topIn_ak4_bLoose;
  uhh2::Event::Handle<std::vector<Jet>> h_topIn_ak4_bMedium;
  uhh2::Event::Handle<std::vector<Jet>> h_topIn_ak4_bTight;
  uhh2::Event::Handle<std::vector<Jet>> h_topIn_ak4_bAnalysis;
  uhh2::Event::Handle<TopJet> h_toptaggedjet;

  double m_rho;

  BTag::algo m_btagalgo;
  BTag::wp m_workingpoint;
  double m_toptag_radius;
};
