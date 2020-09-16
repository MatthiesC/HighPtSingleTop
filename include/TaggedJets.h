#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/TopJetIds.h"
#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/Utils.h"


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


// TODO: Could be extended to be used with top tags as well
class WTaggedJets: public uhh2::AnalysisModule {
public:
  explicit WTaggedJets(uhh2::Context & ctx,
           const std::string & h_name_analysis="WJets",
           const std::string & h_name_veryloose="WJetsVeryLoose",
           const std::string & h_name_loose="WJetsLoose",
           const std::string & h_name_medium="WJetsMedium",
           const std::string & h_name_tight="WJetsTight",
           const std::string & h_name_verytight="WJetsVeryTight",
           // loose, medium, tight, deepAk8, tau-only cuts, etc.
				   const std::string & h_name_ak8jets="Ak8Jets",
           const bool & b_massDecorrelated_=false);

  virtual bool process(uhh2::Event & event) override;

  virtual ~WTaggedJets();

private:
	uhh2::Event::Handle<std::vector<TopJet>> h_ak8jets;
  uhh2::Event::Handle<std::vector<TopJet>> h_wtaggedjets_veryloose, h_wtaggedjets_loose, h_wtaggedjets_medium, h_wtaggedjets_tight, h_wtaggedjets_verytight, h_wtaggedjets_analysis;
  Year year;
  bool b_massDecorrelated;

  // https://twiki.cern.ch/twiki/bin/viewauth/CMS/DeepAK8Tagging2018WPsSFs
  // W tagging
  std::vector<double> wps_W_2016    { .475 , .763 , .918 , .960 , -1 }; // there is not very tight WP for W bosons, thus choosing value "-1"
  std::vector<double> wps_W_2016_MD { .274 , .506 , .731 , .828 , -1 };
  std::vector<double> wps_W_2017    { .467 , .772 , .925 , .964 , -1 };
  std::vector<double> wps_W_2017_MD { .258 , .506 , .739 , .838 , -1 };
  std::vector<double> wps_W_2018    { .458 , .762 , .918 , .961 , -1 };
  std::vector<double> wps_W_2018_MD { .245 , .479 , .704 , .806 , -1 };
  // t tagging
  std::vector<double> wps_t_2016    { -1 , .528 , .834 , .929 , .988 }; // there is not very loose WP for t quarks, thus choosing value "-1"
  std::vector<double> wps_t_2016_MD { -1 , .177 , .435 , .632 , .889 };
  std::vector<double> wps_t_2017    { -1 , .333 , .725 , .884 , .983 };
  std::vector<double> wps_t_2017_MD { -1 , .117 , .344 , .554 , .863 };
  std::vector<double> wps_t_2018    { -1 , .436 , .802 , .922 , .989 };
  std::vector<double> wps_t_2018_MD { -1 , .174 , .470 , .685 , .920 };

  std::vector<double> wps;
};


// class to tag the W ak8 jet; if there is no w tag, the leading ak8 jet is used
class WTaggedJet: public uhh2::AnalysisModule {
public:
  explicit WTaggedJet(uhh2::Context & ctx,
           const std::string & h_name="WTaggedJet");

  virtual bool process(uhh2::Event & event) override;

  virtual ~WTaggedJet();

private:
  uhh2::Event::Handle<std::vector<TopJet>> h_ak8jets;
  uhh2::Event::Handle<std::vector<TopJet>> h_wtaggedjets;
  uhh2::Event::Handle<TopJet> h_wtaggedjet;
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

class InExAK4Jets: public uhh2::AnalysisModule {
 public:
  explicit InExAK4Jets(uhh2::Context & ctx,
			 BTag::algo btagalgo,
			 BTag::wp workingpoint,
			 const std::string & h_name_postfix,
			 const std::string & h_name_FatJet,
       const bool is_hotvr,
			 const double rho = 600);

  virtual bool process(uhh2::Event & event) override;

  virtual ~InExAK4Jets();

 private:
  uhh2::Event::Handle<std::vector<Jet>> h_Ex_ak4;
  uhh2::Event::Handle<std::vector<Jet>> h_Ex_ak4_bLoose;
  uhh2::Event::Handle<std::vector<Jet>> h_Ex_ak4_bMedium;
  uhh2::Event::Handle<std::vector<Jet>> h_Ex_ak4_bTight;
  uhh2::Event::Handle<std::vector<Jet>> h_Ex_ak4_bAnalysis;
  uhh2::Event::Handle<std::vector<Jet>> h_In_ak4;
  uhh2::Event::Handle<std::vector<Jet>> h_In_ak4_bLoose;
  uhh2::Event::Handle<std::vector<Jet>> h_In_ak4_bMedium;
  uhh2::Event::Handle<std::vector<Jet>> h_In_ak4_bTight;
  uhh2::Event::Handle<std::vector<Jet>> h_In_ak4_bAnalysis;
  uhh2::Event::Handle<TopJet> h_taggedfatjet;

  bool m_is_hotvr;
  double m_rho;

  BTag::algo m_btagalgo;
  BTag::wp m_workingpoint;
};
