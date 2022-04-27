#include "UHH2/core/include/LorentzVector.h"

#include "UHH2/common/include/TriggerSelection.h"

#include "UHH2/HighPtSingleTop/include/Utils.h"

#include <TH1F.h>
#include "TRandom3.h"

using namespace std;
using namespace uhh2;
using namespace uhh2::btw;
using namespace uhh2::ltt;


namespace uhh2 { namespace btw {

//____________________________________________________________________________________________________
Channel extract_channel(const Context & ctx) {
  Channel channel = Channel::notValid;
  const TString config = string2lowercase(ctx.get("analysis_channel"));
  if(config.Contains("ele")) channel = Channel::isEle;
  if(config.Contains("muo")) channel = Channel::isMuo;
  if(channel == Channel::notValid) {
    std::runtime_error("extract_channel(): Invalid channel string in xml file. Please check.");
  }
  return channel;
}

//____________________________________________________________________________________________________
TWClassification_DNN::TWClassification_DNN(Context & ctx):
  fChannel(extract_channel(ctx)),
  fHandle_GENtW(ctx.get_handle<ltt::SingleTopGen_tWch>(kHandleName_SingleTopGen_tWch)),
  fHandle_dnnClass_tW(ctx.get_handle<btw::E_dnnClass_tW>(kHandleName_dnnClass_tW))
{}

bool TWClassification_DNN::process(Event & event) {
  if(!event.is_valid(fHandle_GENtW)) {
    event.set(fHandle_dnnClass_tW, E_dnnClass_tW::none);
    return true;
  }
  const auto & GENtW = event.get(fHandle_GENtW);
  E_dnnClass_tW dnnClass_tW = E_dnnClass_tW::bkg;
  if(fChannel == Channel::isMuo) {
    if(
      (GENtW.IsTopHadronicDecay() && GENtW.IsAssToMuonDecay()) ||
      (GENtW.IsAssHadronicDecay() && GENtW.IsTopToMuonDecay())
    ) dnnClass_tW = E_dnnClass_tW::sig;
  }
  else if(fChannel == Channel::isEle) {
    if(
      (GENtW.IsTopHadronicDecay() && GENtW.IsAssToElectronDecay()) ||
      (GENtW.IsAssHadronicDecay() && GENtW.IsTopToElectronDecay())
    ) dnnClass_tW = E_dnnClass_tW::sig;
  }
  event.set(fHandle_dnnClass_tW, dnnClass_tW);
  return true;
}

//____________________________________________________________________________________________________
TWSignalSelection_DNN::TWSignalSelection_DNN(Context & ctx):
  fHandle_dnnClass_tW(ctx.get_handle<btw::E_dnnClass_tW>(kHandleName_dnnClass_tW))
{}

bool TWSignalSelection_DNN::passes(const Event & event) {
  if(event.get(fHandle_dnnClass_tW) == E_dnnClass_tW::sig) return true;
  else return false;
}

//____________________________________________________________________________________________________
MttHist::MttHist(Context & ctx, const string & dirname):
  Hists(ctx, dirname)
{
  hist_mtt = book<TH1F>("mtt", "Generated m_{t#bar{t}}", 2000, 0, 2000);
  hist_mtt_100 = book<TH1F>("mtt_100", "Generated m_{t#bar{t}}", 100, 0, 2000);
}

void MttHist::fill(const Event & event) {
  double mtt = -1;
  LorentzVector top = {0.,0.,0.,0.}, antitop = {0.,0.,0.,0.};
  for (const auto & genpart : *event.genparticles){
    if (genpart.pdgId() == 6) {
      top = genpart.v4();
    }
    else if(genpart.pdgId() == -6) {
      antitop = genpart.v4();
    }
  }
  mtt = (top+antitop).M();
  hist_mtt->Fill(mtt, event.weight);
  hist_mtt_100->Fill(mtt, event.weight);
}

//____________________________________________________________________________________________________
// Information on EGamma triggers taken from https://twiki.cern.ch/twiki/bin/view/CMS/EgHLTRunIISummary
BTWTriggerSelection::BTWTriggerSelection(Context & ctx, const bool low_pt):
  fYear(extract_year(ctx)),
  fChannel(extract_channel(ctx)),
  fLowPt(low_pt)
{
  const TString dataset_version = ctx.get("dataset_version");
  if(dataset_version.Contains("SingleMuon")) fDataStream = DataStream::isSingleMuon;
  else if(dataset_version.Contains("SingleElectron")) fDataStream = DataStream::isSingleElectron;
  else if(dataset_version.Contains("SinglePhoton")) fDataStream = DataStream::isSinglePhoton;
  else if(dataset_version.Contains("EGamma")) fDataStream = DataStream::isEGamma;
  else fDataStream = DataStream::isMC;

  fTrigSel_IsoMu24.reset(new TriggerSelection("HLT_IsoMu24_v*"));
  fTrigSel_IsoTkMu24.reset(new TriggerSelection("HLT_IsoTkMu24_v*"));
  fTrigSel_IsoMu27.reset(new TriggerSelection("HLT_IsoMu27_v*"));

  fTrigSel_Mu50.reset(new TriggerSelection("HLT_Mu50_v*"));
  fTrigSel_TkMu50.reset(new TriggerSelection("HLT_TkMu50_v*"));
  fTrigSel_OldMu100.reset(new TriggerSelection("HLT_OldMu100_v*"));
  fTrigSel_TkMu100.reset(new TriggerSelection("HLT_TkMu100_v*"));

  fTrigSel_Ele27_WPTight_Gsf.reset(new TriggerSelection("HLT_Ele27_WPTight_Gsf_v*"));
  fTrigSel_Ele35_WPTight_Gsf.reset(new TriggerSelection("HLT_Ele35_WPTight_Gsf_v*"));
  fTrigSel_Ele32_WPTight_Gsf.reset(new TriggerSelection("HLT_Ele32_WPTight_Gsf_v*"));

  fTrigSel_Ele115_CaloIdVT_GsfTrkIdT.reset(new TriggerSelection("HLT_Ele115_CaloIdVT_GsfTrkIdT_v*"));
  fTrigSel_Photon175.reset(new TriggerSelection("HLT_Photon175_v*"));
  fTrigSel_Photon200.reset(new TriggerSelection("HLT_Photon200_v*"));
}

bool BTWTriggerSelection::passes(const Event & event) {
  if(fDataStream == DataStream::isMC && event.isRealData) throw runtime_error("BTWTriggerSelection::passes(): Conflict with event.isRealData and dataset_version");
  if(fYear == Year::isUL16preVFP || fYear == Year::isUL16postVFP) {
    if(fChannel == Channel::isMuo) {
      if(fLowPt) return fTrigSel_IsoMu24->passes(event) || fTrigSel_IsoTkMu24->passes(event);
      else return fTrigSel_Mu50->passes(event) || fTrigSel_TkMu50->passes(event);
    }
    else if(fChannel == Channel::isEle) {
      if(fDataStream == DataStream::isMC) {
        if(fLowPt) return fTrigSel_Ele27_WPTight_Gsf->passes(event);
        else return fTrigSel_Ele115_CaloIdVT_GsfTrkIdT->passes(event) || fTrigSel_Photon175->passes(event);
      }
      else if(fDataStream == DataStream::isSingleElectron) {
        if(fLowPt) return fTrigSel_Ele27_WPTight_Gsf->passes(event);
        else return fTrigSel_Ele115_CaloIdVT_GsfTrkIdT->passes(event);
      }
      else if(fDataStream == DataStream::isSinglePhoton) {
        // Veto Ele115 since those events will be in the SingleElectron stream already
        if(fLowPt) return false;
        else return !fTrigSel_Ele115_CaloIdVT_GsfTrkIdT->passes(event) && fTrigSel_Photon175->passes(event);
      }
      else return false;
    }
    else return false;
  }
  else if(fYear == Year::isUL17) {
    if(fChannel == Channel::isMuo) {
      // if(fLowPt) return fTrigSel_IsoMu27->passes(event);
      // else return fTrigSel_Mu50->passes(event) || fTrigSel_OldMu100->passes(event) || fTrigSel_TkMu100->passes(event);
      if(fDataStream == DataStream::isMC) {
        if(fLowPt) return fTrigSel_IsoMu27->passes(event);
        else {
          // Use random number generator with eta-dependent seed for reproducibility
          TRandom3 *random = new TRandom3((int)(fabs(event.muons->at(0).v4().eta()*1000))); // TRandom3 is the same generator as used for gRandom
          if(random->Rndm() > lumi_percentage_UL17_RunB) { // Run C-F emulation
            return fTrigSel_Mu50->passes(event) || fTrigSel_OldMu100->passes(event) || fTrigSel_TkMu100->passes(event);
          }
          else { // Run B emulation
            return fTrigSel_Mu50->passes(event);
          }
        }
      }
      else if(fDataStream == DataStream::isSingleMuon) {
        if(fLowPt) return fTrigSel_IsoMu27->passes(event);
        else {
          if(is_UL17_RunB(event)) return fTrigSel_Mu50->passes(event);
          else return fTrigSel_Mu50->passes(event) || fTrigSel_OldMu100->passes(event) || fTrigSel_TkMu100->passes(event);
        }
      }
      else return false;
    }
    else if(fChannel == Channel::isEle) {
      if(fDataStream == DataStream::isMC) {
        // Use random number generator with eta-dependent seed for reproducibility
        TRandom3 *random = new TRandom3((int)(fabs(event.electrons->at(0).v4().eta()*1000))); // TRandom3 is the same generator as used for gRandom
        if(random->Rndm() > lumi_percentage_UL17_RunB) { // Run C-F emulation
          if(fLowPt) return fTrigSel_Ele35_WPTight_Gsf->passes(event);
          else return fTrigSel_Ele115_CaloIdVT_GsfTrkIdT->passes(event) || fTrigSel_Photon200->passes(event);
        }
        else { // Run B emulation
          return fTrigSel_Ele35_WPTight_Gsf->passes(event) || fTrigSel_Photon200->passes(event);
        }
      }
      else if(fDataStream == DataStream::isSingleElectron) {
        if(is_UL17_RunB(event)) {
          return fTrigSel_Ele35_WPTight_Gsf->passes(event);
        }
        else {
          if(fLowPt) return fTrigSel_Ele35_WPTight_Gsf->passes(event);
          else return fTrigSel_Ele115_CaloIdVT_GsfTrkIdT->passes(event);
        }
      }
      else if(fDataStream == DataStream::isSinglePhoton) {
        if(is_UL17_RunB(event)) {
          // Veto Ele35 since those events will be in the SingleElectron stream already
          return !fTrigSel_Ele35_WPTight_Gsf->passes(event) && fTrigSel_Photon200->passes(event);
        }
        else {
          // Veto Ele115 since those events will be in the SingleElectron stream already
          if(fLowPt) return false;
          else return !fTrigSel_Ele115_CaloIdVT_GsfTrkIdT->passes(event) && fTrigSel_Photon200->passes(event);
        }
      }
      else return false;
    }
    else return false;
  }
  else if(fYear == Year::isUL18)  {
    if(fChannel == Channel::isMuo) {
      if(fLowPt) return fTrigSel_IsoMu24->passes(event);
      else return fTrigSel_Mu50->passes(event) || fTrigSel_OldMu100->passes(event) || fTrigSel_TkMu100->passes(event);
    }
    else if(fChannel == Channel::isEle) {
      // No need for differentiation between SingleElectron and SinglePhoton streams since we have EGamma in 2018
      // According to https://twiki.cern.ch/twiki/bin/view/CMS/EgHLTRunIISummary#2018 there is no need for a photon trigger
      if(fLowPt) return fTrigSel_Ele32_WPTight_Gsf->passes(event);
      else return fTrigSel_Ele115_CaloIdVT_GsfTrkIdT->passes(event) || fTrigSel_Photon200->passes(event);
    }
    else return false;
  }
  else return false;
}

//____________________________________________________________________________________________________
WTag::WTag(const WTag::algo & algorithm, const WTag::wp & working_point): fAlgo(algorithm), fWP(working_point) {}

bool WTag::operator()(const TopJet & jet, const Event & event) const {
  const string year = event.year;

  double x(-1.); // default value for most tags is -2. as defined in core/include/TopJet.h; this x value here should be higher than that value
  switch(fAlgo) {
    case WTag::algo::ParticleNet :
      switch(fWP) {
        case WTag::wp::WP_DUMMY :
          if(year == "UL16preVFP") x = 0.9;
          else if(year == "UL16postVFP") x = 0.9;
          else if(year == "UL17") x = 0.9;
          else if(year == "UL18") x = 0.9;
          else runtime_error((string)"WTag::operator()(): Year '"+year+"' not implemented");
          break;
        // Taken from https://indico.cern.ch/event/1152827/contributions/4840404/attachments/2428856/4162159/ParticleNet_SFs_ULNanoV9_JMAR_25April2022_PK.pdf
        case WTag::wp::WP_BkgEff0p050 :
          if(year == "UL16preVFP") x = 0.68;
          else if(year == "UL16postVFP") x = 0.67;
          else if(year == "UL17") x = 0.71;
          else if(year == "UL18") x = 0.70;
          else runtime_error((string)"WTag::operator()(): Year '"+year+"' not implemented");
          break;
        case WTag::wp::WP_BkgEff0p010 :
          if(year == "UL16preVFP") x = 0.94;
          else if(year == "UL16postVFP") x = 0.93;
          else if(year == "UL17") x = 0.94;
          else if(year == "UL18") x = 0.94;
          else runtime_error((string)"WTag::operator()(): Year '"+year+"' not implemented");
          break;
        case WTag::wp::WP_BkgEff0p005 :
          if(year == "UL16preVFP") x = 0.97;
          else if(year == "UL16postVFP") x = 0.97;
          else if(year == "UL17") x = 0.98;
          else if(year == "UL18") x = 0.98;
          else runtime_error((string)"WTag::operator()(): Year '"+year+"' not implemented");
          break;
        default :
          throw runtime_error((string)"WTag::operator()(): Unknown working point");
      } break;
    default :
    throw runtime_error((string)"WTag::operator()(): Unknown tagging algorithm");
  }

  if(fAlgo == WTag::algo::ParticleNet) return ltt::mSD(jet) > 65. && ltt::mSD(jet) < 105. && jet.btag_ParticleNetDiscriminatorsJetTags_WvsQCD() > x;
  else return false;
}

//____________________________________________________________________________________________________
FatJetTagger::FatJetTagger(Context & ctx, const WTag::algo & wtag_algo, const WTag::wp & wtag_wp):
  fWTagID(WTag(wtag_algo, wtag_wp)),
  fHandle_tJets(ctx.get_handle<vector<TopJet>>(kHandleName_tJets)),
  fHandle_AK8Collection_rec(ctx.get_handle<vector<TopJet>>(kCollectionName_AK8_rec)),
  fHandle_WJets(ctx.get_handle<vector<TopJet>>(kHandleName_WJets))
{}

bool FatJetTagger::process(Event & event) {
  //__________________________________________________
  vector<TopJet> tjets;
  for(const auto & j : *event.topjets) {
    if(fHOTVRTopTagID(j, event) && ltt::tau32groomed(j) < ltt::kWorkingPointsHOTVR.at(ltt::WorkingPoint::Standard)) tjets.push_back(j);
  }
  event.set(fHandle_tJets, move(tjets));
  //__________________________________________________
  vector<TopJet> wjets;
  for(const auto & j : event.get(fHandle_AK8Collection_rec)) {
    if(fWTagID(j, event)) wjets.push_back(j);
  }
  event.set(fHandle_WJets, move(wjets));
  //__________________________________________________
  return true;
}

//____________________________________________________________________________________________________
TopTagSetter::TopTagSetter(Context & ctx):
  fHandle_Region(ctx.get_handle<ERegion>(kHandleName_Region)),
  fHandle_Region_heavyTags(ctx.get_handle<ERegion_heavyTags>(kHandleName_Region_heavyTags)),
  fHandle_tJets(ctx.get_handle<vector<TopJet>>(kHandleName_tJets)),
  fHandle_TheTopJet(ctx.get_handle<TopJet>(kHandleName_TheTopJet))
  // doWarning(string2bool(ctx.get("debug")))
{}

bool TopTagSetter::process(Event & event) {
  const ERegion_heavyTags region_heavyTags = event.get(fHandle_Region_heavyTags);
  TopJet theTopJet = TopJet();
  if(region_heavyTags == ERegion_heavyTags::_1t) {
    theTopJet = event.get(fHandle_tJets).at(0);
  }
  // else if(event.topjets->size() > 0) {
  //   theTopJet = event.topjets->at(0);
  // }
  // else if(doWarning) {
  //   cout << "TopTagSetter::process(): Warning: Event belongs to non-1t region (" << kRegion_toString.at(event.get(fHandle_Region)) << ") and no leading HOTVR jet could be found as replacement" << endl;
  // }
  event.set(fHandle_TheTopJet, theTopJet);
  return true;
}

//____________________________________________________________________________________________________
WTagSetter::WTagSetter(Context & ctx):
  fHandle_Region(ctx.get_handle<ERegion>(kHandleName_Region)),
  fHandle_Region_heavyTags(ctx.get_handle<ERegion_heavyTags>(kHandleName_Region_heavyTags)),
  fHandle_WJets(ctx.get_handle<vector<TopJet>>(kHandleName_WJets)),
  fHandle_TheWJet(ctx.get_handle<TopJet>(kHandleName_TheWJet)),
  fHandle_AK8Collection_rec(ctx.get_handle<vector<TopJet>>(kCollectionName_AK8_rec))
  // doWarning(string2bool(ctx.get("debug")))
{}

bool WTagSetter::process(Event & event) {
  const ERegion_heavyTags region_heavyTags = event.get(fHandle_Region_heavyTags);
  TopJet theWJet = TopJet();
  if(region_heavyTags == ERegion_heavyTags::_0t1W) {
    theWJet = event.get(fHandle_WJets).at(0);
  }
  // else if(event.get(fHandle_AK8Collection_rec).size() > 0) {
  //   theWJet = event.get(fHandle_AK8Collection_rec).at(0);
  // }
  // else if(doWarning) {
  //   cout << "WTagSetter::process(): Warning: Event belongs to non-0t1W region (" << kRegion_toString.at(event.get(fHandle_Region)) << ") and no leading AK8 jet could be found as replacement" << endl;
  // }
  event.set(fHandle_TheWJet, theWJet);
  return true;
}

//____________________________________________________________________________________________________
vector<Particle> NeutrinoReconstruction(const FlavorParticle & lepton_, const MET & met_) {
  const float W_mass = 80.379; // +/- 0.012 (PDG value of 2021)
  LorentzVector lepton = lepton_.v4();
  lepton.SetE(lepton.P()); // set mass of lepton to zero (negligible compared to W mass and its uncertainty)
  LorentzVector met(met_.pt(), 0., met_.phi(), met_.pt()); // [PtEtaPhiE4D] // LorentzVector met = met_.v4(); // does not work with const MET since MET.h is missing a const qualifier for v4() member function, causing compiler error
  TVector3 lepton_pT = uhh2::toVector(lepton); // [PxPyPz]
  lepton_pT.SetZ(0);
  TVector3 met_pT = uhh2::toVector(met); // [PxPyPz]
  met_pT.SetZ(0);
  const float alpha = 0.5 * W_mass * W_mass + lepton_pT * met_pT; // scalar product of TVector3
  const float pT2 = lepton_pT * lepton_pT;
  const float A = alpha * lepton.Pz() / pT2; // lepton.Pz() indeed returns Pz and not the 3rd component (Phi)
  const float B = (lepton.E() * lepton.E() * (met_pT * met_pT) - alpha * alpha) / pT2;
  float discriminant = A * A - B;
  vector<Particle> solutions;
  if(discriminant <= 0) {
    const float K = 0.5 * W_mass * W_mass / (lepton_pT.Mag() * met_pT.Mag() - lepton_pT * met_pT);
    const float pz = A + (K - 1) * lepton.Pz() * (lepton_pT * met_pT) / pT2;
    LorentzVectorXYZE solution(K * met.Px(), K * met.Py(), pz, 0);
    solution.SetE(solution.P());
    Particle solution_;
    solution_.set_v4(uhh2::toPtEtaPhi(solution));
    solutions.push_back(solution_);
  }
  else {
    discriminant = sqrt(discriminant);
    LorentzVectorXYZE solution1(met.Px(), met.Py(), A + discriminant, 0);
    solution1.SetE(solution1.P());
    Particle solution1_;
    solution1_.set_v4(uhh2::toPtEtaPhi(solution1));
    solutions.push_back(solution1_);
    LorentzVectorXYZE solution2(met.Px(), met.Py(), A - discriminant, 0);
    solution2.SetE(solution2.P());
    Particle solution2_;
    solution2_.set_v4(uhh2::toPtEtaPhi(solution2));
    solutions.push_back(solution2_);
  }
  return solutions;
}

//____________________________________________________________________________________________________
LeptonicHemisphereReco::LeptonicHemisphereReco(Context & ctx):
  fHandle_CHSjets(ctx.get_handle<vector<Jet>>(kCollectionName_AK4CHS)),
  fHandle_pairedPUPPIjets(ctx.get_handle<vector<Jet>>(kHandleName_pairedPUPPIjets)),
  fHandle_bJets(ctx.get_handle<vector<Jet>>(kHandleName_bJets)),
  fHandle_PrimaryLepton(ctx.get_handle<FlavorParticle>(kHandleName_PrimaryLepton)),
  fHandle_Region_heavyTags(ctx.get_handle<ERegion_heavyTags>(kHandleName_Region_heavyTags)),
  fHandle_LeptonicHemisphere(ctx.get_handle<LeptonicHemisphere>(kHandleName_LeptonicHemisphere))
{}

bool LeptonicHemisphereReco::process(Event & event) {
  const FlavorParticle & primlep = event.get(fHandle_PrimaryLepton);
  const vector<Particle> neutrino_solutions = btw::NeutrinoReconstruction(primlep, *event.met);
  const vector<Jet> bjets = event.get(fHandle_bJets);
  const vector<Jet> jets = event.get(fHandle_pairedPUPPIjets);
  const float dr_max = kDeltaRLeptonicHemisphere;
  const float dr_min = 0.4;
  vector<Jet> considered_jets;
  // first consider all b jets inside dr_max
  JetFindingMethod method = JetFindingMethod::bJetsDR;
  for(const Jet & jet : bjets) {
    const float dr = deltaR(primlep.v4(), jet.v4());
    if(dr > dr_max || dr < dr_min) continue;
    considered_jets.push_back(jet);
  }
  // if no b jets inside dr_max are found, use the jet with highest btag score within dr_max
  if(considered_jets.size() == 0) {
    method = JetFindingMethod::bScoreLeadingJetDR;
    vector<Jet> jets_dj;
    for(const Jet & jet : jets) {
      const float dr = deltaR(primlep.v4(), jet.v4());
      if(dr > dr_max || dr < dr_min) continue;
      jets_dj.push_back(jet);
    }
    if(jets_dj.size() > 0) {
      sort_by_deepjet_from_matches(jets_dj, event, fHandle_CHSjets);
      considered_jets.push_back(jets_dj.at(0));
    }
  }
  // if this also fails, just use all b jets in the event not overlapping with lepton
  if(considered_jets.size() == 0) {
    method = JetFindingMethod::bJets;
    for(const Jet & jet : bjets) {
      const float dr = deltaR(primlep.v4(), jet.v4());
      if(dr < dr_min) continue;
      considered_jets.push_back(jet);
    }
  }
  // if this fails too, just use all jets in the event not overlapping with lepton
  if(considered_jets.size() == 0) {
    method = JetFindingMethod::allJets;
    for(const Jet & jet : jets) {
      const float dr = deltaR(primlep.v4(), jet.v4());
      if(dr < dr_min) continue;
      considered_jets.push_back(jet);
    }
  }
  // throw an error if event has no central jets
  if(considered_jets.size() == 0) throw runtime_error("LeptonicHemisphereReco::process(): No jets found which could be considered for the top quark reconstruction");

  const ERegion_heavyTags region_heavyTags = event.get(fHandle_Region_heavyTags);
  vector<LeptonicHemisphere> solutions;
  for(const Jet & jet : considered_jets) {
    const Jet *chsjetPtr = getCHSmatch(jet, event, fHandle_CHSjets);
    if(region_heavyTags != ERegion_heavyTags::_1t) {
      // Iterate over neutrino solutions and find best neutrino (and thus best W boson) from top quark mass constraint
      for(const Particle & neutrino : neutrino_solutions) {
        solutions.push_back(LeptonicHemisphere(jet, *chsjetPtr, neutrino, primlep));
      }
    }
    else {
      Particle neutrino(neutrino_solutions.at(0));
      if(neutrino_solutions.size() > 1) {
        // Do not find best neutrino solution via top quark mass constraint but always take neutrino solution with highest abs(pz) in order to have
        // consistent leptonic W boson reconstruction for t(had)W(lep) where correct leptonic top quark reco for ttbar events is not too important.
        // Using highest abs(pz) is recommended here: https://twiki.cern.ch/twiki/bin/view/LHCPhysics/ParticleLevelTopDefinitions
        // const bool use_other_solution = fabs(neutrino_solutions.at(1).v4().Pz()) > fabs(neutrino_solutions.at(0).v4().Pz());
        //
        // Alternatively, use neutrino solution which is closest to lepton in pz:
        const bool use_other_solution = fabs(neutrino_solutions.at(1).v4().Pz() - primlep.v4().Pz()) < fabs(neutrino_solutions.at(0).v4().Pz() - primlep.v4().Pz());
        if(use_other_solution) neutrino = neutrino_solutions.at(1);
      }
      solutions.push_back(LeptonicHemisphere(jet, *chsjetPtr, neutrino, primlep));
    }
  }

  for(LeptonicHemisphere & solution : solutions) {
    solution.set_method(method);
    solution.set_nsolutions(solutions.size());
  }

  // sort solutions by difference to MC top mass and take solution with minimal value
  const float mtop_MC = 172.5;
  std::sort(solutions.begin(), solutions.end(), [&mtop_MC](const LeptonicHemisphere & s1, const LeptonicHemisphere & s2) {
    return fabs(s1.v4().M() - mtop_MC) < fabs(s2.v4().M() - mtop_MC);
  });
  event.set(fHandle_LeptonicHemisphere, solutions.at(0));

  return true;
}

//____________________________________________________________________________________________________
LeptonicTopQuarkMassSelection::LeptonicTopQuarkMassSelection(Context & ctx, const float m_min, const float m_max):
  fMassMin(m_min),
  fMassMax(m_max),
  fHandle_Region_heavyTags(ctx.get_handle<btw::ERegion_heavyTags>(kHandleName_Region_heavyTags)),
  fHandle_LeptonicHemisphere(ctx.get_handle<LeptonicHemisphere>(kHandleName_LeptonicHemisphere))
{}

bool LeptonicTopQuarkMassSelection::passes(const Event & event) {
  const ERegion_heavyTags & region_heavyTags = event.get(fHandle_Region_heavyTags);
  if(region_heavyTags != ERegion_heavyTags::_0t1W) return true;
  const LeptonicHemisphere & hemi = event.get(fHandle_LeptonicHemisphere);
  if(hemi.v4().M() < fMassMin || hemi.v4().M() > fMassMax) return false;
  else return true;
}

//____________________________________________________________________________________________________
TWClassification_TrueDecay::TWClassification_TrueDecay(Context & ctx):
  fChannel(extract_channel(ctx)),
  fHandle_GENtW(ctx.get_handle<ltt::SingleTopGen_tWch>(kHandleName_SingleTopGen_tWch)),
  fHandle_Region_heavyTags(ctx.get_handle<btw::ERegion_heavyTags>(kHandleName_Region_heavyTags)),
  fHandle_trueClass_tW(ctx.get_handle<btw::E_trueClass_tW>(kHandleName_trueClass_tW)),
  fHandle_trueClass_tW_int(ctx.declare_event_output<int>(kHandleName_trueClass_tW_int))
{}

bool TWClassification_TrueDecay::process(Event & event) {
  if(!event.is_valid(fHandle_GENtW)) {
    event.set(fHandle_trueClass_tW, E_trueClass_tW::none);
    event.set(fHandle_trueClass_tW_int, k_trueClass_tW_toInt.at(E_trueClass_tW::none));
    return true;
  }
  const auto & GENtW = event.get(fHandle_GENtW);
  const ERegion_heavyTags & region_heavyTags = event.get(fHandle_Region_heavyTags);
  E_trueClass_tW trueClass_tW = E_trueClass_tW::bkg;
  if(fChannel == Channel::isMuo) {
    if(
      (GENtW.IsTopHadronicDecay() && GENtW.IsAssToMuonDecay() && region_heavyTags == ERegion_heavyTags::_1t) ||
      (GENtW.IsAssHadronicDecay() && GENtW.IsTopToMuonDecay() && region_heavyTags == ERegion_heavyTags::_0t1W)
    ) trueClass_tW = E_trueClass_tW::sig;
  }
  else if(fChannel == Channel::isEle) {
    if(
      (GENtW.IsTopHadronicDecay() && GENtW.IsAssToElectronDecay() && region_heavyTags == ERegion_heavyTags::_1t) ||
      (GENtW.IsAssHadronicDecay() && GENtW.IsTopToElectronDecay() && region_heavyTags == ERegion_heavyTags::_0t1W)
    ) trueClass_tW = E_trueClass_tW::sig;
  }
  event.set(fHandle_trueClass_tW, trueClass_tW);
  event.set(fHandle_trueClass_tW_int, k_trueClass_tW_toInt.at(trueClass_tW));
  return true;
}

//____________________________________________________________________________________________________
TWSignalSelection_TrueDecay::TWSignalSelection_TrueDecay(Context & ctx):
  fHandle_trueClass_tW(ctx.get_handle<btw::E_trueClass_tW>(kHandleName_trueClass_tW))
{}

bool TWSignalSelection_TrueDecay::passes(const Event & event) {
  if(event.get(fHandle_trueClass_tW) == E_trueClass_tW::sig) return true;
  else return false;
}

//____________________________________________________________________________________________________
// Only usable with tW Sig events!
GenLevelDefinitions::GenLevelDefinitions(Context & ctx):
  fChannel(extract_channel(ctx)),
  fHandle_GENtW(ctx.get_handle<ltt::SingleTopGen_tWch>(kHandleName_SingleTopGen_tWch)),
  fHandle_Parton_Top(ctx.get_handle<GenLevelDef>(kHandleName_Parton_Top)),
  fHandle_Parton_WAss(ctx.get_handle<GenLevelDef>(kHandleName_Parton_WAss)),
  fHandle_Parton_WTop(ctx.get_handle<GenLevelDef>(kHandleName_Parton_WTop)),
  fHandle_Parton_tW_system(ctx.get_handle<GenLevelDef>(kHandleName_Parton_tW_system)),
  fHandle_Parton_LeptonicW(ctx.get_handle<GenLevelDef>(kHandleName_Parton_LeptonicW)),
  fHandle_Parton_SingleLepton(ctx.get_handle<GenLevelDef>(kHandleName_Parton_SingleLepton)),
  fHandle_Parton_SingleNeutrino(ctx.get_handle<GenLevelDef>(kHandleName_Parton_SingleNeutrino))
{}

bool GenLevelDefinitions::process(Event & event) {
  if(!event.is_valid(fHandle_GENtW)) return true;
  const auto & GENtW = event.get(fHandle_GENtW);
  event.set(fHandle_Parton_Top, GENtW.Top());
  event.set(fHandle_Parton_WAss, GENtW.WAss());
  event.set(fHandle_Parton_WTop, GENtW.WTop());
  Particle tW_system;
  tW_system.set_v4(GENtW.Top().v4() + GENtW.WAss().v4());
  tW_system.set_charge(GENtW.Top().charge() + GENtW.WAss().charge());
  event.set(fHandle_Parton_tW_system, tW_system);

  // This part will crash for non-semileptonic tW events:
  if(GENtW.IsSemiLeptonic(false)) { // no taus
    event.set(fHandle_Parton_LeptonicW, GENtW.LeptonicW());
    event.set(fHandle_Parton_SingleLepton, GENtW.SingleLepton());
    event.set(fHandle_Parton_SingleNeutrino, GENtW.SingleNeutrino());
  }
  return true;
}

}}
