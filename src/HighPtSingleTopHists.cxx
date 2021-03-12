#include "UHH2/HighPtSingleTop/include/HighPtSingleTopHists.h"
#include "UHH2/HighPtSingleTop/include/MyUtils.h"

#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/common/include/JetIds.h"

#include "TH1F.h"
#include "TH2F.h"
#include <iostream>


using namespace std;
using namespace uhh2;


RegionHist::RegionHist(Context & ctx, const string & dirname): Hists(ctx, dirname) {

  h_which_region = ctx.get_handle<int>("which_region");

  m_year = extract_year(ctx);
  m_year_int = -1;
  if(m_year == Year::is2016v3) m_year_int = 0;
  else if(m_year == Year::is2017v2) m_year_int = 1;
  else if(m_year == Year::is2018) m_year_int = 2;

  m_channel_int = -1;
  if(extract_channel(ctx) == Channel::isEle) m_channel_int = 0;
  else if(extract_channel(ctx) == Channel::isMuo) m_channel_int = 1;


  const int nx = 10;
  // const char *regions[nx] = {"0b1t", "0b0t1W", "0b0t0W", "1b1t", "1b0t1W", "1b0t0W", "#geq2b1t", "#geq2b0t1W", "#geq2b0t0W", "else"};
  vector<string> regions = {"0b1t", "0b0t1W", "0b0t0W", "1b1t", "1b0t1W", "1b0t0W", "#geq2b1t", "#geq2b0t1W", "#geq2b0t0W", "else"};
  vector<string> regions_per_year, regions_per_year_and_channel;
  for(auto & r : regions) {
    for(const string & y : {"2016", "2017", "2018"}) {
      regions_per_year.push_back(y+": "+r);
      for(const string & c : {"e", "#mu"}) {
        regions_per_year_and_channel.push_back(y+" "+c+": "+r);
      }
    }
  }


  hist_regions = book<TH1F>("regions", "Analysis regions", nx, 0.5, 10.5);
  hist_regions_per_year_and_channel = book<TH1F>("regions_per_year_and_channel", "Analysis regions", nx*6, 0.5, 60.5);
  hist_regions_per_year = book<TH1F>("regions_per_year", "Analysis regions", nx*3, 0.5, 30.5);
  for(int i = 1; i <= nx; ++i) hist_regions->GetXaxis()->SetBinLabel(i,regions[i-1].c_str());
  for(int i = 1; i <= nx*6; ++i) hist_regions_per_year_and_channel->GetXaxis()->SetBinLabel(i,regions_per_year_and_channel[i-1].c_str());
  for(int i = 1; i <= nx*3; ++i) hist_regions_per_year->GetXaxis()->SetBinLabel(i,regions_per_year[i-1].c_str());
}

void RegionHist::fill(const Event & event) {

  double w = event.weight;
  const int region = event.get(h_which_region);

  hist_regions->Fill(region, w);
  hist_regions_per_year_and_channel->Fill(1+(region-1)*6+m_year_int*2+m_channel_int, w);
  hist_regions_per_year->Fill(1+(region-1)*3+m_year_int, w);
}


HighPtSingleTopHists::HighPtSingleTopHists(Context & ctx, const string & dirname): Hists(ctx, dirname) {

  deltaR_lepton_nextjet = book<TH1F>("deltaR_lepton_nextjet", "#DeltaR(lepton, next AK4 jet)", 100, 0, 5);
  deltaR_lepton_leadhotvr = book<TH1F>("deltaR_lepton_leadhotvr", "#DeltaR(lepton, leading HOTVR jet)", 100, 0, 5);
  deltaPhi_lepton_nextjet = book<TH1F>("deltaPhi_lepton_nextjet", "#Delta#phi(lepton, next AK4 jet)", 100, 0, M_PI);
  deltaPhi_lepton_leadhotvr = book<TH1F>("deltaPhi_lepton_leadhotvr", "#Delta#phi(lepton, leading HOTVR jet)", 100, 0, M_PI);
  deltaPhiCos_lepton_nextjet = book<TH1F>("deltaPhiCos_lepton_nextjet", "cos(#Delta#phi)(lepton, next AK4 jet)", 100, -1, 1);
  deltaPhiCos_lepton_leadhotvr = book<TH1F>("deltaPhiCos_lepton_leadhotvr", "cos(#Delta#phi)(lepton, leading HOTVR jet)", 100, -1, 1);

  mtw = book<TH1F>("mtw", "m_{T}(lepton, missing p_{T}) [GeV]", 100, 0, 500);
  met = book<TH1F>("met", "missing p_{T} [GeV]", 100, 0, 1000);

  met_VS_mtw = book<TH2F>("met_vs_mtw", "missing p_{T} [GeV] vs. m_{T}(lepton, missing p_{T}) [GeV]", 100, 0, 500, 100, 0, 1000); // 2D

  deltaPhi_lepton_met = book<TH1F>("deltaPhi_lepton_met", "#Delta#phi(lepton, missing p_{T})", 100, 0, M_PI);
  deltaPhiCos_lepton_met = book<TH1F>("deltaPhiCos_lepton_met", "cos(#Delta#phi)(lepton, missing p_{T})", 100, -1, 1);
  deltaPhi_leadhotvr_met = book<TH1F>("deltaPhi_leadhotvr_met", "#Delta#phi(leading HOTVR jet, missing p_{T})", 100, 0, M_PI);
  deltaPhiCos_leadhotvr_met = book<TH1F>("deltaPhiCos_leadhotvr_met", "cos(#Delta#phi)(leading HOTVR jet, missing p_{T})", 100, -1, 1);
  deltaPhi_nextjet_met = book<TH1F>("deltaPhi_nextjet_met", "#Delta#phi(missing p_{T}, next AK4 jet)", 100, 0, M_PI);
  deltaPhiCos_nextjet_met = book<TH1F>("deltaPhiCos_nextjet_met", "cos(#Delta#phi)(missing p_{T}, next AK4 jet)", 100, -1, 1);

  deltaPhi_lepton_met_VS_met = book<TH2F>("deltaPhi_lepton_met_VS_met", "missing p_{T} [GeV] vs. #Delta#phi(lepton, missing p_{T})", 100, 0, 1000, 100, 0, M_PI); // 2D
  deltaPhi_leadhotvr_met_VS_met = book<TH2F>("deltaPhi_leadhotvr_met_VS_met", "missing p_{T} [GeV] vs. #Delta#phi(leading HOTVR jet, missing p_{T})", 100, 0, 1000, 100, 0, M_PI); // 2D
  deltaPhi_nextjet_met_VS_met = book<TH2F>("deltaPhi_nextjet_met_VS_met", "missing p_{T} [GeV] vs. #Delta#phi(missing p_{T}, next AK4 jet)", 100, 0, 1000, 100, 0, M_PI); // 2D

  jets_number = book<TH1F>("N_jets", "number of AK4 jets", 11, -0.5, 10.5);
  jets_Nbtags_loose = book<TH1F>("N_btags_loose", "number of loose b-tags", 11, -0.5, 10.5);
  jets_Nbtags_medium = book<TH1F>("N_btags_medium", "number of medium b-tags", 11, -0.5, 10.5);
  jets_Nbtags_tight = book<TH1F>("N_btags_tight", "number of tight b-tags", 11, -0.5, 10.5);
  jet1_eta = book<TH1F>("eta_jet1", "#eta(leading AK4 jet)", 60, -3, 3);
  jet2_eta = book<TH1F>("eta_jet2", "#eta(2nd leading AK4 jet)", 60, -3, 3);
  jet3_eta = book<TH1F>("eta_jet3", "#eta(3rd leading AK4 jet)", 60, -3, 3);
  jet4_eta = book<TH1F>("eta_jet4", "#eta(n-th leading AK4 jet, n #geq 4)", 60, -3, 3);
  jet1_pt = book<TH1F>("pt_jet1", "p_{T}(leading AK4 jet) [GeV]", 100, 0, 500);
  jet2_pt = book<TH1F>("pt_jet2", "p_{T}(2nd leading AK4 jet) [GeV]", 100, 0, 500);
  jet3_pt = book<TH1F>("pt_jet3", "p_{T}(3rd leading AK4 jet) [GeV]", 100, 0, 250);
  jet4_pt = book<TH1F>("pt_jet4", "p_{T}(n-th leading AK4 jet, n #geq 4) [GeV]", 100, 0, 250);
  jet1_deepjet = book<TH1F>("deepjet_jet1", "DeepJet discriminator (leading AK4 jet)", 50, 0 ,1);
  jet2_deepjet = book<TH1F>("deepjet_jet2", "DeepJet discriminator (2nd leading AK4 jet)", 50, 0 ,1);
  jet3_deepjet = book<TH1F>("deepjet_jet3", "DeepJet discriminator (3rd leading AK4 jet)", 50, 0 ,1);
  jet4_deepjet = book<TH1F>("deepjet_jet4", "DeepJet discriminator (n-th leading AK4 jet, n #geq 4)", 50, 0 ,1);
  jet1_deltaRlepton = book<TH1F>("deltaRlepton_jet1", "#DeltaR(lepton, leading AK4 jet)", 100, 0, 5);
  jet2_deltaRlepton = book<TH1F>("deltaRlepton_jet2", "#DeltaR(lepton, 2nd leading AK4 jet)", 100, 0, 5);
  jet3_deltaRlepton = book<TH1F>("deltaRlepton_jet3", "#DeltaR(lepton, 3rd leading AK4 jet)", 100, 0, 5);
  jet4_deltaRlepton = book<TH1F>("deltaRlepton_jet4", "#DeltaR(lepton, n-th leading AK4 jet, n #geq 4)", 100, 0, 5);

  hotvr_number = book<TH1F>("N_hotvr", "number of HOTVR jets", 11, -0.5, 10.5);
  hotvr1_eta = book<TH1F>("eta_hotvr1", "#eta(leading HOTVR jet)", 60, -3, 3);
  hotvr2_eta = book<TH1F>("eta_hotvr2", "#eta(2nd leading HOTVR jet)", 60, -3, 3);
  hotvr3_eta = book<TH1F>("eta_hotvr3", "#eta(3rd leading HOTVR jet)", 60, -3, 3);
  hotvr4_eta = book<TH1F>("eta_hotvr4", "#eta(n-th leading HOTVR jet, n #geq 4)", 60, -3, 3);
  hotvr1_phi = book<TH1F>("phi_hotvr1", "#phi(leading HOTVR jet)", 100, -M_PI, M_PI);
  hotvr2_phi = book<TH1F>("phi_hotvr2", "#phi(2nd leading HOTVR jet)", 100, -M_PI, M_PI);
  hotvr3_phi = book<TH1F>("phi_hotvr3", "#phi(3rd leading HOTVR jet)", 100, -M_PI, M_PI);
  hotvr4_phi = book<TH1F>("phi_hotvr4", "#phi(n-th leading HOTVR jet, n #geq 4)", 100, -M_PI, M_PI);
  hotvr1_pt = book<TH1F>("pt_hotvr1", "p_{T}(leading HOTVR jet) [GeV]", 100, 0, 1000);
  hotvr2_pt = book<TH1F>("pt_hotvr2", "p_{T}(2nd leading HOTVR jet) [GeV]", 100, 0, 1000);
  hotvr1_pt_lowpt = book<TH1F>("pt_hotvr1_lowpt", "p_{T}(leading HOTVR jet) [GeV]", 100, 0, 500);
  hotvr2_pt_lowpt = book<TH1F>("pt_hotvr2_lowpt", "p_{T}(2nd leading HOTVR jet) [GeV]", 100, 0, 500);
  hotvr1_pt_highpt = book<TH1F>("pt_hotvr1_highpt", "p_{T}(leading HOTVR jet) [GeV]", 100, 500, 1500);
  hotvr2_pt_highpt = book<TH1F>("pt_hotvr2_highpt", "p_{T}(2nd leading HOTVR jet) [GeV]", 100, 500, 1500);
  hotvr3_pt = book<TH1F>("pt_hotvr3", "p_{T}(3rd leading HOTVR jet) [GeV]", 100, 0, 500);
  hotvr4_pt = book<TH1F>("pt_hotvr4", "p_{T}(n-th leading HOTVR jet, n #geq 4) [GeV]", 100, 0, 500);
  hotvr1_mass = book<TH1F>("mass_hotvr1", "m_{jet}(leading HOTVR jet) [GeV]", 100, 0, 500);
  hotvr1_mass_ca173 = book<TH1F>("mass_hotvr1_ca173", "m_{jet}(leading HOTVR jet) [GeV]", 100, 140, 240);
  hotvr2_mass = book<TH1F>("mass_hotvr2", "m_{jet}(2nd leading HOTVR jet) [GeV]", 100, 0, 500);
  hotvr3_mass = book<TH1F>("mass_hotvr3", "m_{jet}(3rd leading HOTVR jet) [GeV]", 100, 0, 500);
  hotvr4_mass = book<TH1F>("mass_hotvr4", "m_{jet}(n-th leading HOTVR jet, n #geq 4) [GeV]", 100, 0, 500);

}


void HighPtSingleTopHists::fill(const Event & event) {

  double weight = event.weight;

  const auto lepton = returnPrimaryLepton(event);

  vector<Jet> jets = *event.jets;
  unsigned int Njets = jets.size();
  vector<TopJet> hotvrjets = *event.topjets;
  unsigned int Nhotvr = hotvrjets.size();


  //------------------------------//
  // Some DeltaR's and DeltaPhi's //
  //------------------------------//

  deltaR_lepton_nextjet->Fill(uhh2::deltaR(lepton.v4(), (nextJet(lepton, jets))->v4()), weight);
  deltaR_lepton_leadhotvr->Fill(uhh2::deltaR(lepton.v4(), hotvrjets.at(0).v4()), weight);
  deltaPhi_lepton_nextjet->Fill(uhh2::deltaPhi(lepton.v4(), (nextJet(lepton, jets))->v4()), weight);
  deltaPhi_lepton_leadhotvr->Fill(uhh2::deltaPhi(lepton.v4(), hotvrjets.at(0).v4()), weight);
  deltaPhiCos_lepton_nextjet->Fill(TMath::Cos(uhh2::deltaPhi(lepton.v4(), (nextJet(lepton, jets))->v4())), weight);
  deltaPhiCos_lepton_leadhotvr->Fill(TMath::Cos(uhh2::deltaPhi(lepton.v4(), hotvrjets.at(0).v4())), weight);

  auto MTW = calcMTW(lepton, event);
  auto MET = *event.met;

  mtw->Fill(MTW, weight);
  met->Fill(MET.pt(), weight);
  met_VS_mtw->Fill(MTW, MET.pt(), weight); // 2D

  deltaPhi_lepton_met->Fill(uhh2::deltaPhi(lepton.v4(), MET.v4()), weight);
  deltaPhiCos_lepton_met->Fill(TMath::Cos(uhh2::deltaPhi(lepton.v4(), MET.v4())), weight);
  deltaPhi_leadhotvr_met->Fill(uhh2::deltaPhi(hotvrjets.at(0).v4(), MET.v4()), weight);
  deltaPhiCos_leadhotvr_met->Fill(TMath::Cos(uhh2::deltaPhi(hotvrjets.at(0).v4(), MET.v4())), weight);
  deltaPhi_nextjet_met->Fill(uhh2::deltaPhi(MET.v4(), (nextJetToMET(event, jets))->v4()), weight);
  deltaPhiCos_nextjet_met->Fill(TMath::Cos(uhh2::deltaPhi(MET.v4(), (nextJetToMET(event, jets))->v4())), weight);

  deltaPhi_lepton_met_VS_met->Fill(MET.pt(), uhh2::deltaPhi(lepton.v4(), MET.v4()), weight); // 2D
  deltaPhi_leadhotvr_met_VS_met->Fill(MET.pt(), uhh2::deltaPhi(hotvrjets.at(0).v4(), MET.v4()), weight); // 2D
  deltaPhi_nextjet_met_VS_met->Fill(MET.pt(), uhh2::deltaPhi(MET.v4(), (nextJetToMET(event, jets))->v4()), weight); // 2D


  //----------//
  // AK4 jets //
  //----------//

  jets_number->Fill(Njets, weight);

  int n_btags_loose(0), n_btags_medium(0), n_btags_tight(0);

  BTag::algo btag_algo = BTag::DEEPJET;
  JetId BJetID_loose = BTag(btag_algo, BTag::WP_LOOSE);
  JetId BJetID_medium = BTag(btag_algo, BTag::WP_MEDIUM);
  JetId BJetID_tight = BTag(btag_algo, BTag::WP_TIGHT);

  for(auto j : jets) {
    if(BJetID_loose(j, event)) n_btags_loose++;
    if(BJetID_medium(j, event)) n_btags_medium++;
    if(BJetID_tight(j, event)) n_btags_tight++;
  }

  jets_Nbtags_loose->Fill(n_btags_loose, weight);
  jets_Nbtags_medium->Fill(n_btags_medium, weight);
  jets_Nbtags_tight->Fill(n_btags_tight, weight);

  if(Njets >= 1) {
    jet1_eta->Fill(jets.at(0).eta(), weight);
    jet1_pt->Fill(jets.at(0).pt(), weight);
    jet1_deepjet->Fill(jets.at(0).btag_DeepJet(), weight);
    jet1_deltaRlepton->Fill(uhh2::deltaR(lepton.v4(), jets.at(0).v4()), weight);
  }
  if(Njets >= 2) {
    jet2_eta->Fill(jets.at(1).eta(), weight);
    jet2_pt->Fill(jets.at(1).pt(), weight);
    jet2_deepjet->Fill(jets.at(1).btag_DeepJet(), weight);
    jet2_deltaRlepton->Fill(uhh2::deltaR(lepton.v4(), jets.at(1).v4()), weight);
  }
  if(Njets >= 3) {
    jet3_eta->Fill(jets.at(2).eta(), weight);
    jet3_pt->Fill(jets.at(2).pt(), weight);
    jet3_deepjet->Fill(jets.at(2).btag_DeepJet(), weight);
    jet3_deltaRlepton->Fill(uhh2::deltaR(lepton.v4(), jets.at(2).v4()), weight);
  }
  if(Njets >= 4) {
    for(unsigned int i = 3; i < Njets; i++) {
      jet4_eta->Fill(jets.at(i).eta(), weight);
      jet4_pt->Fill(jets.at(i).pt(), weight);
      jet4_deepjet->Fill(jets.at(i).btag_DeepJet(), weight);
      jet4_deltaRlepton->Fill(uhh2::deltaR(lepton.v4(), jets.at(i).v4()), weight);
    }
  }


  //------------//
  // HOTVR jets //
  //------------//

  hotvr_number->Fill(Nhotvr, weight);
  if(Nhotvr >= 1) {
    hotvr1_eta->Fill(hotvrjets.at(0).eta(), weight);
    hotvr1_phi->Fill(hotvrjets.at(0).phi(), weight);
    hotvr1_pt->Fill(hotvrjets.at(0).pt(), weight);
    hotvr1_pt_lowpt->Fill(hotvrjets.at(0).pt(), weight);
    hotvr1_pt_highpt->Fill(hotvrjets.at(0).pt(), weight);
    hotvr1_mass->Fill(hotvrjets.at(0).v4().M(), weight);
    hotvr1_mass_ca173->Fill(hotvrjets.at(0).v4().M(), weight);
  }
  if(Nhotvr >= 2) {
    hotvr2_eta->Fill(hotvrjets.at(1).eta(), weight);
    hotvr2_phi->Fill(hotvrjets.at(1).phi(), weight);
    hotvr2_pt->Fill(hotvrjets.at(1).pt(), weight);
    hotvr2_pt_lowpt->Fill(hotvrjets.at(1).pt(), weight);
    hotvr2_pt_highpt->Fill(hotvrjets.at(1).pt(), weight);
    hotvr2_mass->Fill(hotvrjets.at(1).v4().M(), weight);
  }
  if(Nhotvr >= 3) {
    hotvr3_eta->Fill(hotvrjets.at(2).eta(), weight);
    hotvr3_phi->Fill(hotvrjets.at(2).phi(), weight);
    hotvr3_pt->Fill(hotvrjets.at(2).pt(), weight);
    hotvr3_mass->Fill(hotvrjets.at(2).v4().M(), weight);
  }
  if(Nhotvr >= 4) {
    for(unsigned int i = 3; i < Nhotvr; i++) {
      hotvr4_eta->Fill(hotvrjets.at(i).eta(), weight);
      hotvr4_phi->Fill(hotvrjets.at(i).phi(), weight);
      hotvr4_pt->Fill(hotvrjets.at(i).pt(), weight);
      hotvr4_mass->Fill(hotvrjets.at(i).v4().M(), weight);
    }
  }

}

HighPtSingleTopHists::~HighPtSingleTopHists() {}
