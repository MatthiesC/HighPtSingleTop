#pragma once

#include <set>

#include "UHH2/common/include/Utils.h"

namespace uhh2 { namespace btw {

// enum class ELepChannel {
//   muo = 0,
//   ele = 1,
// };

enum class Channel {
  isEle,
  isMuo,
  notValid,
};

const std::map<Channel, int> kChannel_toInt = {
  { Channel::isEle,    1 },
  { Channel::isMuo,    2 },
  { Channel::notValid, 0 },
};

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

const std::map<ERegion_bTags, int> kRegion_bTags_toInt = {
  { ERegion_bTags::_0b, 1 },
  { ERegion_bTags::_1b, 2 },
  { ERegion_bTags::_2b, 3 },
};

//____________________________________________________________________________________________________
enum class ERegion_heavyTags {
  _1t,
  _0t1W,
  _0t0W,
  _else,
};

const std::map<ERegion_heavyTags, int> kRegion_heavyTags_toInt = {
  { ERegion_heavyTags::_1t,   1 },
  { ERegion_heavyTags::_0t1W, 2 },
  { ERegion_heavyTags::_0t0W, 3 },
  { ERegion_heavyTags::_else, 0 },
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

const std::string kHandleName_dnnClass_tW = "dnnClass_tW";

//____________________________________________________________________________________________________
enum class E_trueClass_tW {
  none, // non-tW backgrounds or data
  sig, // tW signal where we distinguish t(had)W(lep) and t(lep)W(had) explicitly
  bkg, // tW backgrounds
};

const std::string kHandleName_trueClass_tW = "trueClass_tW";
const std::string kHandleName_trueClass_tW_int = "trueClass_tW_int";

const std::map<E_trueClass_tW, int> k_trueClass_tW_toInt = {
  { E_trueClass_tW::none, 0 },
  { E_trueClass_tW::sig,  1 },
  { E_trueClass_tW::bkg,  2 },
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
