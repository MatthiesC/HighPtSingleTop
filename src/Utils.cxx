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
TWClassification_DNN::TWClassification_DNN(Context & ctx):
  fChannel(extract_channel(ctx)),
  fHandle_GENtW(ctx.get_handle<ltt::SingleTopGen_tWch>(kHandleName_SingleTopGen_tWch)),
  fHandle_dnnClass_tW(ctx.get_handle<btw::E_dnnClass_tW>(kHandleName_dnnClass_tW)),
  fHandle_dnnClass_tW_int(ctx.declare_event_output<int>(kHandleName_dnnClass_tW_int))
{}

bool TWClassification_DNN::process(Event & event) {
  if(!event.is_valid(fHandle_GENtW)) {
    event.set(fHandle_dnnClass_tW, E_dnnClass_tW::none);
    event.set(fHandle_dnnClass_tW_int, k_dnnClasses_tW.at(E_dnnClass_tW::none).index);
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
  event.set(fHandle_dnnClass_tW_int, k_dnnClasses_tW.at(dnnClass_tW).index);
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
        case WTag::wp::WP_CUSTOMPT200 : // BkgEff0p030, derived privately
          if(year == "UL16preVFP") x = 0.871;
          else if(year == "UL16postVFP") x = 0.868;
          else if(year == "UL17") x = 0.868;
          else if(year == "UL18") x = 0.864;
          else runtime_error((string)"WTag::operator()(): Year '"+year+"' not implemented");
          break;
        case WTag::wp::WP_CUSTOMPT400 : // BkgEff0p030, derived privately
          if(year == "UL16preVFP") x = 0.737;
          else if(year == "UL16postVFP") x = 0.729;
          else if(year == "UL17") x = 0.746;
          else if(year == "UL18") x = 0.734;
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
  fHandle_bool_reco_sel(ctx.get_handle<bool>(kHandleName_bool_reco_sel)),
  fHandle_CHSjets(ctx.get_handle<vector<Jet>>(kCollectionName_AK4CHS)),
  fHandle_pairedPUPPIjets(ctx.get_handle<vector<Jet>>(kHandleName_pairedPUPPIjets)),
  fHandle_bJets(ctx.get_handle<vector<Jet>>(kHandleName_bJets)),
  fHandle_PrimaryLepton(ctx.get_handle<FlavorParticle>(kHandleName_PrimaryLepton)),
  fHandle_Region_heavyTags(ctx.get_handle<ERegion_heavyTags>(kHandleName_Region_heavyTags)),
  fHandle_LeptonicHemisphere(ctx.get_handle<LeptonicHemisphere>(kHandleName_LeptonicHemisphere))
{}

bool LeptonicHemisphereReco::process(Event & event) {
  const bool passes_reco_sel_so_far = event.get(fHandle_bool_reco_sel);
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
  if(considered_jets.size() == 0) {
    if(passes_reco_sel_so_far) throw runtime_error("LeptonicHemisphereReco::process(): No jets found which could be considered for the top quark reconstruction");
    else considered_jets.push_back(Jet()); // add dummy Jet for events that do not pass reco level selection (more specifically: do not pass >=1 AK4 requirement)
  }

  const ERegion_heavyTags region_heavyTags = event.get(fHandle_Region_heavyTags);
  vector<LeptonicHemisphere> solutions;
  for(const Jet & jet : considered_jets) {
    const Jet *chsjetPtr = passes_reco_sel_so_far ? getCHSmatch(jet, event, fHandle_CHSjets) : &jet;
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
    event.set(fHandle_trueClass_tW_int, k_trueClasses_tW.at(E_trueClass_tW::none).index);
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
  event.set(fHandle_trueClass_tW_int, k_trueClasses_tW.at(trueClass_tW).index);
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
// This class will need to be adjusted once I have the proper Particle and Parton Level definitions available
GenLevelDefinitions::GenLevelDefinitions(Context & ctx):
  // fChannel(extract_channel(ctx)),
  fHandle_GENtW(ctx.get_handle<ltt::SingleTopGen_tWch>(kHandleName_SingleTopGen_tWch)),
  fHandle_Matrix_Top(ctx.get_handle<GenLevelDef>(kHandleName_Matrix_Top)),
  fHandle_Matrix_WAss(ctx.get_handle<GenLevelDef>(kHandleName_Matrix_WAss)),
  fHandle_Matrix_WTop(ctx.get_handle<GenLevelDef>(kHandleName_Matrix_WTop)),
  fHandle_Matrix_tW_system(ctx.get_handle<GenLevelDef>(kHandleName_Matrix_tW_system)),
  fHandle_Matrix_LeptonicW(ctx.get_handle<GenLevelDef>(kHandleName_Matrix_LeptonicW)),
  fHandle_Matrix_SingleLepton(ctx.get_handle<GenLevelDef>(kHandleName_Matrix_SingleLepton)),
  fHandle_Matrix_SingleNeutrino(ctx.get_handle<GenLevelDef>(kHandleName_Matrix_SingleNeutrino))
{}

bool GenLevelDefinitions::process(Event & event) {
  if(!event.is_valid(fHandle_GENtW)) return true;
  const auto & GENtW = event.get(fHandle_GENtW);
  event.set(fHandle_Matrix_Top, GENtW.Top());
  event.set(fHandle_Matrix_WAss, GENtW.WAss());
  event.set(fHandle_Matrix_WTop, GENtW.WTop());
  const Particle tW_system = add_Particles(GENtW.Top(), GENtW.WAss());
  event.set(fHandle_Matrix_tW_system, tW_system);

  // This part will crash for non-semileptonic tW events:
  if(GENtW.IsSemiLeptonic(false)) { // no taus
    event.set(fHandle_Matrix_LeptonicW, GENtW.LeptonicW());
    event.set(fHandle_Matrix_SingleLepton, GENtW.SingleLepton());
    event.set(fHandle_Matrix_SingleNeutrino, GENtW.SingleNeutrino());
  }
  return true;
}

//____________________________________________________________________________________________________
MatrixLevelSelection::MatrixLevelSelection(Context & ctx, const string & sel, const boost::optional<ltt::Channel> & channel):
  fSel(sel),
  fChannel(channel ? *channel : extract_channel(ctx)),
  fHandle_Region_heavyTags(ctx.get_handle<ERegion_heavyTags>(kHandleName_Region_heavyTags)),
  fHandle_GENtW(ctx.get_handle<ltt::SingleTopGen_tWch>(kHandleName_SingleTopGen_tWch)),
  fHandle_Matrix_Top(ctx.get_handle<GenLevelDef>(kHandleName_Matrix_Top)),
  fHandle_Matrix_WAss(ctx.get_handle<GenLevelDef>(kHandleName_Matrix_WAss)),
  fHandle_Matrix_SingleLepton(ctx.get_handle<GenLevelDef>(kHandleName_Matrix_SingleLepton))
{}

bool MatrixLevelSelection::passes(const Event & event) {
  if(!event.is_valid(fHandle_GENtW)) return false;
  const auto & GENtW = event.get(fHandle_GENtW);
  const GenLevelDef & Matrix_Top = event.get(fHandle_Matrix_Top);
  const GenLevelDef & Matrix_WAss = event.get(fHandle_Matrix_WAss);
  const GenLevelDef * Matrix_SingleLepton = event.is_valid(fHandle_Matrix_SingleLepton) ? &event.get(fHandle_Matrix_SingleLepton) : nullptr;

  if(fSel == "presel") {
    if(
      // (Matrix_Top.pt() > 200. && fabs(Matrix_Top.eta()) < 2.5) ||
      // (Matrix_WAss.pt() > 200. && fabs(Matrix_WAss.eta()) < 2.5)
      (Matrix_Top.pt() > 200.) ||
      (Matrix_WAss.pt() > 200.)
    ) return true;
  }

  else if(fSel == "mainsel") {
    const ERegion_heavyTags & region_heavyTags = event.get(fHandle_Region_heavyTags);
    if(fChannel == Channel::isMuo) {
      if(
        (
          (GENtW.IsTopHadronicDecay() && GENtW.IsAssToMuonDecay()) ||
          (GENtW.IsAssHadronicDecay() && GENtW.IsTopToMuonDecay())
        )
        // &&
        // (
        //   (region_heavyTags == ERegion_heavyTags::_1t && Matrix_Top.pt() > 200.) ||
        //   (region_heavyTags == ERegion_heavyTags::_0t1W && Matrix_WAss.pt() > 200.)
        // )
        &&
        (
          Matrix_SingleLepton->pt() > 40.
        )
      ) return true;
    }
    else if(fChannel == Channel::isEle) {
      if(
        (
          (GENtW.IsTopHadronicDecay() && GENtW.IsAssToElectronDecay()) ||
          (GENtW.IsAssHadronicDecay() && GENtW.IsTopToElectronDecay())
        )
        // &&
        // (
        //   (region_heavyTags == ERegion_heavyTags::_1t && Matrix_Top.pt() > 200.) ||
        //   (region_heavyTags == ERegion_heavyTags::_0t1W && Matrix_WAss.pt() > 200.)
        // )
        &&
        (
          Matrix_SingleLepton->pt() > 40.
        )
      ) return true;
    }
    // const ERegion_heavyTags & region_heavyTags = event.get(fHandle_Region_heavyTags);
    // if(fChannel == Channel::isMuo) {
    //   if(
    //     (
    //       GENtW.IsTopHadronicDecay() && GENtW.IsAssToMuonDecay() && region_heavyTags == ERegion_heavyTags::_1t
    //       &&
    //       Matrix_Top.pt() > 200. && fabs(Matrix_Top.eta()) < 2.5
    //       &&
    //       Matrix_SingleLepton->pt() > 40. && fabs(Matrix_SingleLepton->eta()) < 2.4
    //     )
    //     ||
    //     (
    //       GENtW.IsAssHadronicDecay() && GENtW.IsTopToMuonDecay() && region_heavyTags == ERegion_heavyTags::_0t1W
    //       &&
    //       Matrix_WAss.pt() > 200. && fabs(Matrix_WAss.eta()) < 2.5
    //       &&
    //       Matrix_SingleLepton->pt() > 40. && fabs(Matrix_SingleLepton->eta()) < 2.4
    //     )
    //   ) return true;
    // }
    // else if(fChannel == Channel::isEle) {
    //   if(
    //     (
    //       GENtW.IsTopHadronicDecay() && GENtW.IsAssToElectronDecay() && region_heavyTags == ERegion_heavyTags::_1t
    //       &&
    //       Matrix_Top.pt() > 200. && fabs(Matrix_Top.eta()) < 2.5
    //       &&
    //       Matrix_SingleLepton->pt() > 40. && fabs(Matrix_SingleLepton->eta()) < 2.4
    //     )
    //     ||
    //     (
    //       GENtW.IsAssHadronicDecay() && GENtW.IsTopToElectronDecay() && region_heavyTags == ERegion_heavyTags::_0t1W
    //       &&
    //       Matrix_WAss.pt() > 200. && fabs(Matrix_WAss.eta()) < 2.5
    //       &&
    //       Matrix_SingleLepton->pt() > 40. && fabs(Matrix_SingleLepton->eta()) < 2.4
    //     )
    //   ) return true;
    // }
  }

  return false;
}

//____________________________________________________________________________________________________
VariablesOfInterest::VariablesOfInterest(Context & ctx):
  fHandle_Region_heavyTags(ctx.get_handle<ERegion_heavyTags>(kHandleName_Region_heavyTags)),

  fHandle_TheTopJet(ctx.get_handle<TopJet>(kHandleName_TheTopJet)),
  fHandle_TheWJet(ctx.get_handle<TopJet>(kHandleName_TheWJet)),
  // fHandle_PrimaryLepton(ctx.get_handle<FlavorParticle>(kHandleName_PrimaryLepton)),
  fHandle_LeptonicHemisphere(ctx.get_handle<LeptonicHemisphere>(kHandleName_LeptonicHemisphere)),

  fHandle_GENtW(ctx.get_handle<ltt::SingleTopGen_tWch>(kHandleName_SingleTopGen_tWch)),
  fHandle_Matrix_Top(ctx.get_handle<GenLevelDef>(kHandleName_Matrix_Top)),
  fHandle_Matrix_WAss(ctx.get_handle<GenLevelDef>(kHandleName_Matrix_WAss)),
  // fHandle_Matrix_WTop(ctx.get_handle<GenLevelDef>(kHandleName_Matrix_WTop)),
  fHandle_Matrix_tW_system(ctx.get_handle<GenLevelDef>(kHandleName_Matrix_tW_system)),
  // fHandle_Matrix_LeptonicW(ctx.get_handle<GenLevelDef>(kHandleName_Matrix_LeptonicW)),
  // fHandle_Matrix_SingleLepton(ctx.get_handle<GenLevelDef>(kHandleName_Matrix_SingleLepton)),
  // fHandle_Matrix_SingleNeutrino(ctx.get_handle<GenLevelDef>(kHandleName_Matrix_SingleNeutrino)),

  fHandle_VOI_Reco_TopQuarkPt(ctx.declare_event_output<float>(kHandleName_VOI_Reco_TopQuarkPt)),
  fHandle_VOI_Reco_TopQuarkEta(ctx.declare_event_output<float>(kHandleName_VOI_Reco_TopQuarkEta)),
  fHandle_VOI_Reco_TopQuarkY(ctx.declare_event_output<float>(kHandleName_VOI_Reco_TopQuarkY)),
  fHandle_VOI_Reco_WBosonPt(ctx.declare_event_output<float>(kHandleName_VOI_Reco_WBosonPt)),
  fHandle_VOI_Reco_WBosonEta(ctx.declare_event_output<float>(kHandleName_VOI_Reco_WBosonEta)),
  fHandle_VOI_Reco_WBosonY(ctx.declare_event_output<float>(kHandleName_VOI_Reco_WBosonY)),
  fHandle_VOI_Reco_tWSystemPt(ctx.declare_event_output<float>(kHandleName_VOI_Reco_tWSystemPt)),
  fHandle_VOI_Reco_tWSystemEta(ctx.declare_event_output<float>(kHandleName_VOI_Reco_tWSystemEta)),
  fHandle_VOI_Reco_tWSystemY(ctx.declare_event_output<float>(kHandleName_VOI_Reco_tWSystemY)),
  fHandle_VOI_Reco_tWSystemMass(ctx.declare_event_output<float>(kHandleName_VOI_Reco_tWSystemMass)),
  fHandle_VOI_Reco_tWSystemDPhi(ctx.declare_event_output<float>(kHandleName_VOI_Reco_tWSystemDPhi)),

  fHandle_VOI_Matrix_TopQuarkPt(ctx.declare_event_output<float>(kHandleName_VOI_Matrix_TopQuarkPt)),
  fHandle_VOI_Matrix_TopQuarkEta(ctx.declare_event_output<float>(kHandleName_VOI_Matrix_TopQuarkEta)),
  fHandle_VOI_Matrix_TopQuarkY(ctx.declare_event_output<float>(kHandleName_VOI_Matrix_TopQuarkY)),
  fHandle_VOI_Matrix_WBosonPt(ctx.declare_event_output<float>(kHandleName_VOI_Matrix_WBosonPt)),
  fHandle_VOI_Matrix_WBosonEta(ctx.declare_event_output<float>(kHandleName_VOI_Matrix_WBosonEta)),
  fHandle_VOI_Matrix_WBosonY(ctx.declare_event_output<float>(kHandleName_VOI_Matrix_WBosonY)),
  fHandle_VOI_Matrix_tWSystemPt(ctx.declare_event_output<float>(kHandleName_VOI_Matrix_tWSystemPt)),
  fHandle_VOI_Matrix_tWSystemEta(ctx.declare_event_output<float>(kHandleName_VOI_Matrix_tWSystemEta)),
  fHandle_VOI_Matrix_tWSystemY(ctx.declare_event_output<float>(kHandleName_VOI_Matrix_tWSystemY)),
  fHandle_VOI_Matrix_tWSystemMass(ctx.declare_event_output<float>(kHandleName_VOI_Matrix_tWSystemMass)),
  fHandle_VOI_Matrix_tWSystemDPhi(ctx.declare_event_output<float>(kHandleName_VOI_Matrix_tWSystemDPhi))
{}

bool VariablesOfInterest::process(Event & event) {
  const ERegion_heavyTags region_heavyTags = event.get(fHandle_Region_heavyTags);

  const TopJet & theTopJet = event.get(fHandle_TheTopJet);
  const TopJet & theWJet = event.get(fHandle_TheWJet);
  const LeptonicHemisphere & hemi = event.get(fHandle_LeptonicHemisphere);

  // const auto & GENtW = event.get(fHandle_GENtW);
  const bool bGENtW = event.is_valid(fHandle_GENtW);
  const GenLevelDef * Matrix_Top = bGENtW ? &event.get(fHandle_Matrix_Top) : nullptr;
  const GenLevelDef * Matrix_WAss = bGENtW ? &event.get(fHandle_Matrix_WAss) : nullptr;
  const GenLevelDef * Matrix_tW_system = bGENtW ? &event.get(fHandle_Matrix_tW_system) : nullptr;

  const float dummy_value = -1.f;

  //__________________________________________________
  if(region_heavyTags == ERegion_heavyTags::_1t) {
    event.set(fHandle_VOI_Reco_TopQuarkPt, theTopJet.pt());
    event.set(fHandle_VOI_Reco_TopQuarkEta, theTopJet.eta());
    event.set(fHandle_VOI_Reco_TopQuarkY, ltt::rapidity(theTopJet));
    event.set(fHandle_VOI_Reco_WBosonPt, hemi.wboson().pt());
    event.set(fHandle_VOI_Reco_WBosonEta, hemi.wboson().eta());
    event.set(fHandle_VOI_Reco_WBosonY, ltt::rapidity(hemi.wboson()));
    const Particle tW_system = add_Particles(theTopJet, hemi.wboson());
    event.set(fHandle_VOI_Reco_tWSystemPt, tW_system.pt());
    event.set(fHandle_VOI_Reco_tWSystemEta, tW_system.eta());
    event.set(fHandle_VOI_Reco_tWSystemY, ltt::rapidity(tW_system));
    event.set(fHandle_VOI_Reco_tWSystemMass, tW_system.v4().M());
    event.set(fHandle_VOI_Reco_tWSystemDPhi, fabs(ltt::signedDeltaPhi(theTopJet, hemi.wboson())));
  }
  else if(region_heavyTags == ERegion_heavyTags::_0t1W) {
    event.set(fHandle_VOI_Reco_TopQuarkPt, hemi.pt());
    event.set(fHandle_VOI_Reco_TopQuarkEta, hemi.eta());
    event.set(fHandle_VOI_Reco_TopQuarkY, ltt::rapidity(hemi));
    event.set(fHandle_VOI_Reco_WBosonPt, theWJet.pt());
    event.set(fHandle_VOI_Reco_WBosonEta, theWJet.eta());
    event.set(fHandle_VOI_Reco_WBosonY, ltt::rapidity(theWJet));
    const Particle tW_system = add_Particles(theWJet, hemi);
    event.set(fHandle_VOI_Reco_tWSystemPt, tW_system.pt());
    event.set(fHandle_VOI_Reco_tWSystemEta, tW_system.eta());
    event.set(fHandle_VOI_Reco_tWSystemY, ltt::rapidity(tW_system));
    event.set(fHandle_VOI_Reco_tWSystemMass, tW_system.v4().M());
    event.set(fHandle_VOI_Reco_tWSystemDPhi, fabs(ltt::signedDeltaPhi(theWJet, hemi)));
  }
  else {
    event.set(fHandle_VOI_Reco_TopQuarkPt, dummy_value);
    event.set(fHandle_VOI_Reco_TopQuarkEta, dummy_value);
    event.set(fHandle_VOI_Reco_TopQuarkY, dummy_value);
    event.set(fHandle_VOI_Reco_WBosonPt, dummy_value);
    event.set(fHandle_VOI_Reco_WBosonEta, dummy_value);
    event.set(fHandle_VOI_Reco_WBosonY, dummy_value);
    event.set(fHandle_VOI_Reco_tWSystemPt, dummy_value);
    event.set(fHandle_VOI_Reco_tWSystemEta, dummy_value);
    event.set(fHandle_VOI_Reco_tWSystemY, dummy_value);
    event.set(fHandle_VOI_Reco_tWSystemMass, dummy_value);
    event.set(fHandle_VOI_Reco_tWSystemDPhi, dummy_value);
  }

  //__________________________________________________
  if(bGENtW) {
    event.set(fHandle_VOI_Matrix_TopQuarkPt, Matrix_Top->pt());
    event.set(fHandle_VOI_Matrix_TopQuarkEta, Matrix_Top->eta());
    event.set(fHandle_VOI_Matrix_TopQuarkY, ltt::rapidity(*Matrix_Top));
    event.set(fHandle_VOI_Matrix_WBosonPt, Matrix_WAss->pt());
    event.set(fHandle_VOI_Matrix_WBosonEta, Matrix_WAss->eta());
    event.set(fHandle_VOI_Matrix_WBosonY, ltt::rapidity(*Matrix_WAss));
    event.set(fHandle_VOI_Matrix_tWSystemPt, Matrix_tW_system->pt());
    event.set(fHandle_VOI_Matrix_tWSystemEta, Matrix_tW_system->eta());
    event.set(fHandle_VOI_Matrix_tWSystemY, ltt::rapidity(*Matrix_tW_system));
    event.set(fHandle_VOI_Matrix_tWSystemMass, Matrix_tW_system->v4().M());
    event.set(fHandle_VOI_Matrix_tWSystemDPhi, fabs(ltt::signedDeltaPhi(*Matrix_Top, *Matrix_WAss)));
  }
  else {
    event.set(fHandle_VOI_Matrix_TopQuarkPt, dummy_value);
    event.set(fHandle_VOI_Matrix_TopQuarkEta, dummy_value);
    event.set(fHandle_VOI_Matrix_TopQuarkY, dummy_value);
    event.set(fHandle_VOI_Matrix_WBosonPt, dummy_value);
    event.set(fHandle_VOI_Matrix_WBosonEta, dummy_value);
    event.set(fHandle_VOI_Matrix_WBosonY, dummy_value);
    event.set(fHandle_VOI_Matrix_tWSystemPt, dummy_value);
    event.set(fHandle_VOI_Matrix_tWSystemEta, dummy_value);
    event.set(fHandle_VOI_Matrix_tWSystemY, dummy_value);
    event.set(fHandle_VOI_Matrix_tWSystemMass, dummy_value);
    event.set(fHandle_VOI_Matrix_tWSystemDPhi, dummy_value);
  }

  return true;
}

}}
