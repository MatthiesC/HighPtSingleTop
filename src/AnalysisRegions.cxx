#include "UHH2/HighPtSingleTop/include/AnalysisRegions.h"
#include "UHH2/HighPtSingleTop/include/Constants.h"

#include <TH1F.h>

using namespace std;
using namespace uhh2;
using namespace uhh2::btw;
using namespace uhh2::ltt;


namespace uhh2 { namespace btw {

//____________________________________________________________________________________________________
AnalysisRegionSetter::AnalysisRegionSetter(Context & ctx, const bool b_veto_like):
  bVetoLike(b_veto_like),

  fHandle_tJets(ctx.get_handle<vector<TopJet>>(kHandleName_tJets)),
  fHandle_WJets(ctx.get_handle<vector<TopJet>>(kHandleName_WJets)),

  fHandle_n_bJets(ctx.get_handle<int>(kHandleName_n_bJets)),
  // fHandle_n_bJets_loose(ctx.get_handle<int>(kHandleName_n_bJets_loose)),
  // fHandle_n_bJets_medium(ctx.get_handle<int>(kHandleName_n_bJets_medium)),
  // fHandle_n_bJets_tight(ctx.get_handle<int>(kHandleName_n_bJets_tight)),

  fHandle_n_bJets_hemi(ctx.get_handle<int>(kHandleName_n_bJets_hemi)),
  // fHandle_n_bJets_hemi_loose(ctx.get_handle<int>(kHandleName_n_bJets_hemi_loose)),
  // fHandle_n_bJets_hemi_medium(ctx.get_handle<int>(kHandleName_n_bJets_hemi_medium)),
  // fHandle_n_bJets_hemi_tight(ctx.get_handle<int>(kHandleName_n_bJets_hemi_tight)),

  fHandle_Region_bTags(ctx.get_handle<ERegion_bTags>(kHandleName_Region_bTags)),
  fHandle_Region_heavyTags(ctx.get_handle<ERegion_heavyTags>(kHandleName_Region_heavyTags)),
  fHandle_Region(ctx.get_handle<ERegion>(kHandleName_Region)),

  fHandle_Region_bTags_int(ctx.declare_event_output<int>(kHandleName_Region_bTags_int)),
  fHandle_Region_heavyTags_int(ctx.declare_event_output<int>(kHandleName_Region_heavyTags_int)),
  fHandle_Region_int(ctx.declare_event_output<int>(kHandleName_Region_int))
{}

bool AnalysisRegionSetter::process(Event & event) {
  //__________________________________________________
  ERegion_heavyTags region_heavyTags;
  const uint n_tJets = event.get(fHandle_tJets).size();
  const uint n_WJets = event.get(fHandle_WJets).size();
  if(n_tJets == 1) {
    region_heavyTags = ERegion_heavyTags::_1t;
  }
  else if(n_tJets == 0 && n_WJets == 1) {
    region_heavyTags = ERegion_heavyTags::_0t1W;
  }
  else if(n_tJets == 0 && n_WJets == 0) {
    region_heavyTags = ERegion_heavyTags::_0t0W;
  }
  else {
    region_heavyTags = ERegion_heavyTags::_else;
  }
  event.set(fHandle_Region_heavyTags, region_heavyTags);
  event.set(fHandle_Region_heavyTags_int, kRegions_heavyTags.at(region_heavyTags).index);

  //__________________________________________________
  ERegion_bTags region_bTags;
  const uint n_bJets = event.get(fHandle_n_bJets);
  // const uint n_bJets_loose = event.get(fHandle_n_bJets_loose);
  // const uint n_bJets_medium = event.get(fHandle_n_bJets_medium);
  // const uint n_bJets_tight = event.get(fHandle_n_bJets_tight);
  if(bVetoLike && region_heavyTags == ERegion_heavyTags::_1t) {
    const uint n_bJets_vetoable = event.get(fHandle_n_bJets_hemi);
    // const uint n_bJets_vetoable_loose = event.get(fHandle_n_bJets_hemi_loose);
    // const uint n_bJets_vetoable_medium = event.get(fHandle_n_bJets_hemi_medium);
    // const uint n_bJets_vetoable_tight = event.get(fHandle_n_bJets_hemi_tight);
    if(n_bJets == 0) {
      region_bTags = ERegion_bTags::_0b;
    }
    else if(n_bJets_vetoable == 0) {
      region_bTags = ERegion_bTags::_1b;
    }
    else {
      region_bTags = ERegion_bTags::_2b;
    }
  }
  else if(bVetoLike && region_heavyTags == ERegion_heavyTags::_0t1W) {
    const uint n_bJets_vetoable = n_bJets - event.get(fHandle_n_bJets_hemi);
    // const uint n_bJets_vetoable_loose = n_bJets_loose - event.get(fHandle_n_bJets_hemi_loose);
    // const uint n_bJets_vetoable_medium = n_bJets_medium - event.get(fHandle_n_bJets_hemi_medium);
    // const uint n_bJets_vetoable_tight = n_bJets_tight - event.get(fHandle_n_bJets_hemi_tight);
    if(n_bJets == 0) {
      region_bTags = ERegion_bTags::_0b;
    }
    else if(n_bJets_vetoable == 0) {
      region_bTags = ERegion_bTags::_1b;
    }
    else {
      region_bTags = ERegion_bTags::_2b;
    }
  }
  else {
    if(n_bJets >= 2) {
      region_bTags = ERegion_bTags::_2b;
    }
    else if(n_bJets == 1) {
      region_bTags = ERegion_bTags::_1b;
    }
    else {
      region_bTags = ERegion_bTags::_0b;
    }
    // if(n_bJets >= 2 && n_bJets_tight >= 1) {
    //   region_bTags = ERegion_bTags::_2b;
    // }
    // else if(n_bJets >= 1) {
    //   region_bTags = ERegion_bTags::_1b;
    // }
    // else {
    //   region_bTags = ERegion_bTags::_0b;
    // }
  }
  event.set(fHandle_Region_bTags, region_bTags);
  event.set(fHandle_Region_bTags_int, kRegions_bTags.at(region_bTags).index);

  //__________________________________________________
  ERegion region;
  if(region_heavyTags == ERegion_heavyTags::_1t) {
    if(region_bTags == ERegion_bTags::_0b) {
      region = ERegion::_0b1t;
    }
    else if(region_bTags == ERegion_bTags::_1b) {
      region = ERegion::_1b1t;
    }
    else {
      region = ERegion::_2b1t;
    }
  }
  else if(region_heavyTags == ERegion_heavyTags::_0t1W) {
    if(region_bTags == ERegion_bTags::_0b) {
      region = ERegion::_0b0t1W;
    }
    else if(region_bTags == ERegion_bTags::_1b) {
      region = ERegion::_1b0t1W;
    }
    else {
      region = ERegion::_2b0t1W;
    }
  }
  else if(region_heavyTags == ERegion_heavyTags::_0t0W) {
    if(region_bTags == ERegion_bTags::_0b) {
      region = ERegion::_0b0t0W;
    }
    else if(region_bTags == ERegion_bTags::_1b) {
      region = ERegion::_1b0t0W;
    }
    else {
      region = ERegion::_2b0t0W;
    }
  }
  else {
    region = ERegion::_else;
  }
  event.set(fHandle_Region, region);
  event.set(fHandle_Region_int, kRegions.at(region).index);

  return true;
}

//____________________________________________________________________________________________________
AnalysisRegionHists::AnalysisRegionHists(Context & ctx, const string & dirname):
  Hists(ctx, dirname),

  // fHandle_Region_bTags(ctx.get_handle<ERegion_bTags>(kHandleName_Region_bTags)),
  // fHandle_Region_heavyTags(ctx.get_handle<ERegion_heavyTags>(kHandleName_Region_heavyTags)),
  fHandle_Region(ctx.get_handle<ERegion>(kHandleName_Region))
{
  fYear = extract_year(ctx);
  switch(fYear) {
    case Year::isUL16preVFP :
    fYearInt = 0; break;
    case Year::isUL16postVFP :
    fYearInt = 1; break;
    case Year::isUL17 :
    fYearInt = 2; break;
    case Year::isUL18 :
    fYearInt = 3; break;
    default :
    throw invalid_argument("AnalysisRegionHists: Can only handle UL years!");
  }
  fChannel = extract_channel(ctx);
  switch(fChannel) {
    case Channel::isEle :
    fChannelInt = 0; break;
    case Channel::isMuo :
    fChannelInt = 1; break;
    default :
    throw invalid_argument("AnalysisRegionHists: Invalid channel!");
  }

  const int nx = kRegions.size();

  hist_region = book<TH1F>("region", "Analysis region", nx, -0.5, (float)nx-0.5);
  hist_region_rel = book<TH1F>("region_rel", "Analysis region", nx, -0.5, (float)nx-0.5);

  hist_region_year_channel = book<TH1F>("region_year_channel", "Analysis region", nx*8, -0.5, (float)nx*8-0.5);
  hist_region_year_channel_rel = book<TH1F>("region_year_channel_rel", "Analysis region", nx*8, -0.5, (float)nx*8-0.5);

  hist_region_year = book<TH1F>("region_year", "Analysis region", nx*4, -0.5, (float)nx*4-0.5);
  hist_region_year_rel = book<TH1F>("region_year_rel", "Analysis region", nx*4, -0.5, (float)nx*4-0.5);

  hist_region_channel = book<TH1F>("region_channel", "Analysis region", nx*2, -0.5, (float)nx*2-0.5);
  hist_region_channel_rel = book<TH1F>("region_channel_rel", "Analysis region", nx*2, -0.5, (float)nx*2-0.5);

  const vector<string> channel_labels = {"e", "#mu"};
  vector<string> labels_region;
  vector<string> labels_region_year_channel;
  vector<string> labels_region_year;
  vector<string> labels_region_channel;
  for(const auto & r : kRegions) {
    labels_region.push_back(r.second.tlatex);
    for(const auto & y : kYears) {
      labels_region_year.push_back(y.second.nice_name+": "+r.second.tlatex);
      for(const auto & c : channel_labels) {
        labels_region_year_channel.push_back(y.second.nice_name+" "+c+": "+r.second.tlatex);
      }
    }
    for(const auto & c : channel_labels) {
      labels_region_channel.push_back(c+": "+r.second.tlatex);
    }
  }
  for(int i = 1; i <= nx; ++i) {
    hist_region->GetXaxis()->SetBinLabel(i, labels_region[i-1].c_str());
    hist_region_rel->GetXaxis()->SetBinLabel(i, labels_region[i-1].c_str());
  }
  for(int i = 1; i <= nx*8; ++i) {
    hist_region_year_channel->GetXaxis()->SetBinLabel(i, labels_region_year_channel[i-1].c_str());
    hist_region_year_channel_rel->GetXaxis()->SetBinLabel(i, labels_region_year_channel[i-1].c_str());
  }
  for(int i = 1; i <= nx*4; ++i) {
    hist_region_year->GetXaxis()->SetBinLabel(i, labels_region_year[i-1].c_str());
    hist_region_year_rel->GetXaxis()->SetBinLabel(i, labels_region_year[i-1].c_str());
  }
  for(int i = 1; i <= nx*2; ++i) {
    hist_region_channel->GetXaxis()->SetBinLabel(i, labels_region_channel[i-1].c_str());
    hist_region_channel_rel->GetXaxis()->SetBinLabel(i, labels_region_channel[i-1].c_str());
  }
}

void AnalysisRegionHists::fill(const Event & event) {
  const double w = event.weight;
  // const int i_region_bTags = (int)event.get(fHandle_Region_bTags);
  // const int i_region_heavyTags = (int)event.get(fHandle_Region_heavyTags);
  const ERegion region = event.get(fHandle_Region);
  const int i_region = (int)region;

  hist_region->Fill(i_region, w);
  hist_region_year_channel->Fill(i_region*8+fYearInt*2+fChannelInt, w);
  hist_region_year->Fill(i_region*4+fYearInt, w);
  hist_region_channel->Fill(i_region*2+fChannelInt, w);

  if(kRelevantRegions.find(region) != kRelevantRegions.end()) {
    hist_region_rel->Fill(i_region, w);
    hist_region_year_channel_rel->Fill(i_region*8+fYearInt*2+fChannelInt, w);
    hist_region_year_rel->Fill(i_region*4+fYearInt, w);
    hist_region_channel_rel->Fill(i_region*2+fChannelInt, w);
  }
}

}}
