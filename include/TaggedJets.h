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


// creates event handles for ak4 jets which have been filtered not to be inside the t-tagged jet

class NonTopAK4Jets: public uhh2::AnalysisModule {
 public:
  explicit NonTopAK4Jets(uhh2::Context & ctx,
			 BTag::algo btagalgo,
			 BTag::wp workingpoint,
			 const std::string & h_name_nontopjets="NonTopJets",
			 const std::string & h_name_bAnalysis="NonTopBJets",
			 const std::string & h_name_bLoose="NonTopBJetsLoose",
			 const std::string & h_name_bMedium="NonTopBJetsMedium",
			 const std::string & h_name_bTight="NonTopBJetsTight",
			 const std::string & h_name_TopTag="TopTaggedJet",
			 const double rho = 600);
  
  virtual bool process(uhh2::Event & event) override;

  virtual ~NonTopAK4Jets();

 private:
  uhh2::Event::Handle<std::vector<Jet>> h_nontopak4;
  uhh2::Event::Handle<std::vector<Jet>> h_nontopak4_bLoose;
  uhh2::Event::Handle<std::vector<Jet>> h_nontopak4_bMedium;
  uhh2::Event::Handle<std::vector<Jet>> h_nontopak4_bTight;
  uhh2::Event::Handle<std::vector<Jet>> h_nontopak4_bAnalysis;
  uhh2::Event::Handle<TopJet> h_toptaggedjet;

  double m_rho;

  BTag::algo m_btagalgo;
  BTag::wp m_workingpoint;
};
