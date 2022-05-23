#pragma once

#include <set>

#include "UHH2/common/include/Utils.h"

#include "UHH2/LegacyTopTagging/include/Utils.h"

namespace uhh2 { namespace btw {

// enum class ELepChannel {
//   muo = 0,
//   ele = 1,
// };

const std::string kHandleName_tJets = "btw_tJets";
const std::string kHandleName_TheTopJet = "btw_TheTopJet";
const std::string kHandleName_WJets = "btw_WJets";
const std::string kHandleName_TheWJet = "btw_TheWJet";

const std::string kHandleName_Region_bTags = "btw_Region_bTags";
const std::string kHandleName_Region_heavyTags = "btw_Region_heavyTags";
const std::string kHandleName_Region = "btw_Region";

const std::string kHandleName_Region_bTags_int = "btw_Region_bTags_int";
const std::string kHandleName_Region_heavyTags_int = "btw_Region_heavyTags_int";
const std::string kHandleName_Region_int = "btw_Region_int";

const std::string kHandleName_LeptonicHemisphere = "LeptonicHemisphere";

const std::string kHandleName_Parton_Top = "Parton_Top";
const std::string kHandleName_Parton_WAss = "Parton_WAss";
const std::string kHandleName_Parton_WTop = "Parton_WTop";
const std::string kHandleName_Parton_tW_system = "Parton_tW_system";
const std::string kHandleName_Parton_LeptonicW = "Parton_LeptonicW";
const std::string kHandleName_Parton_SingleLepton = "Parton_SingleLepton";
const std::string kHandleName_Parton_SingleNeutrino = "Parton_SingleNeutrino";


//____________________________________________________________________________________________________
enum class ERegion_bTags {
  _0b,
  _1b,
  _2b, // Actually, this is >= 2 b jets; so this is kind of "else"
  _else,
};

typedef struct {
  int index;
  std::string name;
  std::string tlatex; // using TLatex syntax
} RegionInfo_bTags;

const std::map<ERegion_bTags, RegionInfo_bTags> kRegions_bTags = {
  { ERegion_bTags::_0b,   { .index=1, .name="0b",   .tlatex="0b" }},
  { ERegion_bTags::_1b,   { .index=2, .name="1b",   .tlatex="1b" }},
  { ERegion_bTags::_2b,   { .index=3, .name="2b",   .tlatex="2b" }},
  { ERegion_bTags::_else, { .index=0, .name="else", .tlatex="else" }},
};

//____________________________________________________________________________________________________
enum class ERegion_heavyTags {
  _1t,
  _0t1W,
  _0t0W,
  _else,
};

const std::set<ERegion_heavyTags> kRelevantRegions_heavyTags = {
  ERegion_heavyTags::_1t,
  ERegion_heavyTags::_0t1W,
};

typedef struct {
  int index;
  std::string name;
  std::string tlatex; // using TLatex syntax
} RegionInfo_heavyTags;

const std::map<ERegion_heavyTags, RegionInfo_heavyTags> kRegions_heavyTags = {
  { ERegion_heavyTags::_1t,   { .index=1, .name="1t",   .tlatex="1t" }},
  { ERegion_heavyTags::_0t1W, { .index=2, .name="0t1W", .tlatex="0t 1W" }},
  { ERegion_heavyTags::_0t0W, { .index=3, .name="0t0W", .tlatex="0t 0W" }},
  { ERegion_heavyTags::_else, { .index=0, .name="else", .tlatex="else" }},
};

//____________________________________________________________________________________________________
enum class ERegion {
  _0b1t,
  _0b0t1W,
  _0b0t0W,
  _1b1t,
  _1b0t1W,
  _1b0t0W,
  _2b1t,
  _2b0t1W,
  _2b0t0W,
  _else,
};

const std::set<ERegion> kRelevantRegions = {
  ERegion::_0b1t,
  ERegion::_0b0t1W,
  ERegion::_1b1t,
  ERegion::_1b0t1W,
  ERegion::_2b1t,
  ERegion::_2b0t1W,
};

typedef struct {
  int index;
  std::string name;
  std::string tlatex; // using TLatex syntax
  ERegion_heavyTags region_heavyTags;
  ERegion_bTags region_bTags;
} RegionInfo;

const std::map<ERegion, RegionInfo> kRegions = {
  { ERegion::_0b1t,   RegionInfo{ .index=11, .name="0b1t",   .tlatex="0b 1t",    .region_heavyTags=ERegion_heavyTags::_1t,   .region_bTags=ERegion_bTags::_0b }},
  { ERegion::_0b0t1W, RegionInfo{ .index=12, .name="0b0t1W", .tlatex="0b 0t 1W", .region_heavyTags=ERegion_heavyTags::_0t1W, .region_bTags=ERegion_bTags::_0b }},
  { ERegion::_0b0t0W, RegionInfo{ .index=13, .name="0b0t0W", .tlatex="0b 0t 0W", .region_heavyTags=ERegion_heavyTags::_0t0W, .region_bTags=ERegion_bTags::_0b }},
  { ERegion::_1b1t,   RegionInfo{ .index=21, .name="1b1t",   .tlatex="1b 1t",    .region_heavyTags=ERegion_heavyTags::_1t,   .region_bTags=ERegion_bTags::_1b }},
  { ERegion::_1b0t1W, RegionInfo{ .index=22, .name="1b0t1W", .tlatex="1b 0t 1W", .region_heavyTags=ERegion_heavyTags::_0t1W, .region_bTags=ERegion_bTags::_1b }},
  { ERegion::_1b0t0W, RegionInfo{ .index=23, .name="1b0t0W", .tlatex="1b 0t 0W", .region_heavyTags=ERegion_heavyTags::_0t0W, .region_bTags=ERegion_bTags::_1b }},
  { ERegion::_2b1t,   RegionInfo{ .index=31, .name="2b1t",   .tlatex="2b 1t",    .region_heavyTags=ERegion_heavyTags::_1t,   .region_bTags=ERegion_bTags::_2b }},
  { ERegion::_2b0t1W, RegionInfo{ .index=32, .name="2b0t1W", .tlatex="2b 0t 1W", .region_heavyTags=ERegion_heavyTags::_0t1W, .region_bTags=ERegion_bTags::_2b }},
  { ERegion::_2b0t0W, RegionInfo{ .index=33, .name="2b0t0W", .tlatex="2b 0t 0W", .region_heavyTags=ERegion_heavyTags::_0t0W, .region_bTags=ERegion_bTags::_2b }},
  { ERegion::_else,   RegionInfo{ .index=0,  .name="else",   .tlatex="else",     .region_heavyTags=ERegion_heavyTags::_else, .region_bTags=ERegion_bTags::_else }},
};

//____________________________________________________________________________________________________
enum class E_dnnClass_tW {
  none, // non-tW backgrounds or data
  sig, // tW signal used for DNN training
  bkg, // tW backgrounds
};

typedef struct {
  int index;
} dnnClass_tW_Info;

const std::string kHandleName_dnnClass_tW = "btw_dnnClass_tW";
const std::string kHandleName_dnnClass_tW_int = "btw_dnnClass_tW_int";

const std::map<E_dnnClass_tW, dnnClass_tW_Info> k_dnnClasses_tW = {
  { E_dnnClass_tW::none, dnnClass_tW_Info{ .index=0 }},
  { E_dnnClass_tW::sig,  dnnClass_tW_Info{ .index=1 }},
  { E_dnnClass_tW::bkg,  dnnClass_tW_Info{ .index=2 }},
};

//____________________________________________________________________________________________________
enum class E_trueClass_tW {
  none, // non-tW backgrounds or data
  sig, // tW signal where we distinguish t(had)W(lep) and t(lep)W(had) explicitly
  bkg, // tW backgrounds
};

typedef struct {
  int index;
} trueClass_tW_Info;

const std::string kHandleName_trueClass_tW = "btw_trueClass_tW";
const std::string kHandleName_trueClass_tW_int = "btw_trueClass_tW_int";

const std::map<E_trueClass_tW, trueClass_tW_Info> k_trueClasses_tW = {
  { E_trueClass_tW::none, trueClass_tW_Info{ .index=0 }},
  { E_trueClass_tW::sig,  trueClass_tW_Info{ .index=1 }},
  { E_trueClass_tW::bkg,  trueClass_tW_Info{ .index=2 }},
};

//____________________________________________________________________________________________________
// enum class E_unfClass_tW {
//   none,
//   sig,
//   bkg,
// };
//
// const std::string kHandleName_unfClass_tW = "unfClass_tW";





// const std::map<ERegion, int> kRegion_toInt = {
//   { ERegion::_0b1t,   11 },
//   { ERegion::_0b0t1W, 12 },
//   { ERegion::_0b0t0W, 13 },
//   { ERegion::_1b1t,   21 },
//   { ERegion::_1b0t1W, 22 },
//   { ERegion::_1b0t0W, 23 },
//   { ERegion::_2b1t,   31 },
//   { ERegion::_2b0t1W, 32 },
//   { ERegion::_2b0t0W, 33 },
//   { ERegion::_else,    0 },
// };
//
// const std::map<ERegion, std::string> kRegion_toString = {
//   { ERegion::_0b1t,   "0b1t" },
//   { ERegion::_0b0t1W, "0b0t1W" },
//   { ERegion::_0b0t0W, "0b0t0W" },
//   { ERegion::_1b1t,   "1b1t" },
//   { ERegion::_1b0t1W, "1b0t1W" },
//   { ERegion::_1b0t0W, "1b0t0W" },
//   { ERegion::_2b1t,   "2b1t" },
//   { ERegion::_2b0t1W, "2b0t1W" },
//   { ERegion::_2b0t0W, "2b0t0W" },
//   { ERegion::_else,   "else" },
// };
//
// const std::map<ERegion, std::string> kRegion_toNiceString = {
//   { ERegion::_0b1t,   "0b 1t" },
//   { ERegion::_0b0t1W, "0b 0t 1W" },
//   { ERegion::_0b0t0W, "0b 0t 0W" },
//   { ERegion::_1b1t,   "1b 1t" },
//   { ERegion::_1b0t1W, "1b 0t 1W" },
//   { ERegion::_1b0t0W, "1b 0t 0W" },
//   { ERegion::_2b1t,   "2b 1t" },
//   { ERegion::_2b0t1W, "2b 0t 1W" },
//   { ERegion::_2b0t0W, "2b 0t 0W" }, // #geq
//   { ERegion::_else,   "else" },
// };




/*const uint n_pt_bins = 11;
const double pt_bins[n_pt_bins+1] = {
  200,
  250,
  300,
  350,
  400,
  450,
  500,
  550,
  600,
  680,
  800,
  1000
};
*/

typedef std::vector<std::pair<double, double>> binning;

// binning pt_binning = {
//   {200, 250},
//   {250, 300},
//   {300, 350},
//   {350, 400},
//   {400, 450},
//   {450, 500},
//   {500, 1000},
//   // {550, 600},
//   // {600, 680},
//   // {680, 800},
//   // {800, 1000}
// };

// binning pt_binning = {
//   {200, 220},
//   {220, 250},
//   {250, 290},
//   {290, 340},
//   {340, 400},
//   {400, 470},
//   {470, 550},
//   {550, 640},
//   {640, 740},
//   {740, 1500},
// };

inline const binning kBinning__tjet_pt = {
  {200, 220},
  {220, 250},
  {250, 290},
  {290, 340},
  {340, 400},
  {400, 470},
  {470, 550},
  {550, 640},
  {640, 740},
  {740, 1500},
};

inline const binning kBinning__wjet_pt = {
  {200, 220},
  {220, 250},
  {250, 290},
  {290, 340},
  {340, 400},
  {400, 470},
  {470, 550},
  {550, 640},
  {640, 740},
  {740, 1500},
};

inline const binning kBinning__lepton_pt = {
  {50, 60},
  {60, 70},
  {70, 80},
  {80, 100},
  {100, 120},
  {120, 140},
  {140, 170},
  {170, 200},
  {200, 250},
  {250, 1000},
};

// const double pt_binning_edges[8] = {200, 250, 300, 350, 400, 450, 500, 1000};
inline const double kBinning_edges__tjet_pt[11] = {200, 220, 250, 290, 340, 400, 470, 550, 640, 740, 1500};
inline const double kBinning_edges__wjet_pt[11] = {200, 220, 250, 290, 340, 400, 470, 550, 640, 740, 1500};
inline const double kBinning_edges__lepton_pt[11] = {50, 60, 70, 80, 100, 120, 140, 170, 200, 250, 1000};

}}
