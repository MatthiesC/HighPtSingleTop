#pragma once

#include <set>

namespace macros {

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

  _1t,
  _0t1W,
  _0t0W,
  _unfold,
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
  { ERegion::_0b1t,   RegionInfo{ .index=11, .name="0b1t",   .tlatex="0b1t",    .region_heavyTags=ERegion_heavyTags::_1t,   .region_bTags=ERegion_bTags::_0b }},
  { ERegion::_0b0t1W, RegionInfo{ .index=12, .name="0b0t1W", .tlatex="0b0t1W", .region_heavyTags=ERegion_heavyTags::_0t1W, .region_bTags=ERegion_bTags::_0b }},
  { ERegion::_0b0t0W, RegionInfo{ .index=13, .name="0b0t0W", .tlatex="0b0t0W", .region_heavyTags=ERegion_heavyTags::_0t0W, .region_bTags=ERegion_bTags::_0b }},
  { ERegion::_1b1t,   RegionInfo{ .index=21, .name="1b1t",   .tlatex="1b1t",    .region_heavyTags=ERegion_heavyTags::_1t,   .region_bTags=ERegion_bTags::_1b }},
  { ERegion::_1b0t1W, RegionInfo{ .index=22, .name="1b0t1W", .tlatex="1b0t1W", .region_heavyTags=ERegion_heavyTags::_0t1W, .region_bTags=ERegion_bTags::_1b }},
  { ERegion::_1b0t0W, RegionInfo{ .index=23, .name="1b0t0W", .tlatex="1b0t0W", .region_heavyTags=ERegion_heavyTags::_0t0W, .region_bTags=ERegion_bTags::_1b }},
  { ERegion::_2b1t,   RegionInfo{ .index=31, .name="2b1t",   .tlatex="2b1t",    .region_heavyTags=ERegion_heavyTags::_1t,   .region_bTags=ERegion_bTags::_2b }},
  { ERegion::_2b0t1W, RegionInfo{ .index=32, .name="2b0t1W", .tlatex="2b0t1W", .region_heavyTags=ERegion_heavyTags::_0t1W, .region_bTags=ERegion_bTags::_2b }},
  { ERegion::_2b0t0W, RegionInfo{ .index=33, .name="2b0t0W", .tlatex="2b0t0W", .region_heavyTags=ERegion_heavyTags::_0t0W, .region_bTags=ERegion_bTags::_2b }},
  { ERegion::_else,   RegionInfo{ .index=0,  .name="else",   .tlatex="else",     .region_heavyTags=ERegion_heavyTags::_else, .region_bTags=ERegion_bTags::_else }},

  { ERegion::_1t,     RegionInfo{ .index=-1, .name="1t",     .tlatex="1t",       .region_heavyTags=ERegion_heavyTags::_1t,   .region_bTags=ERegion_bTags::_else }},
  { ERegion::_0t1W,   RegionInfo{ .index=-2, .name="0t1W",   .tlatex="0t1W",     .region_heavyTags=ERegion_heavyTags::_0t1W, .region_bTags=ERegion_bTags::_else }},
  { ERegion::_0t0W,   RegionInfo{ .index=-3, .name="0t0W",   .tlatex="0t0W",     .region_heavyTags=ERegion_heavyTags::_0t0W, .region_bTags=ERegion_bTags::_else }},
  { ERegion::_unfold, RegionInfo{ .index=-999, .name="unfold", .tlatex="1t + 0t1W", .region_heavyTags=ERegion_heavyTags::_else, .region_bTags=ERegion_bTags::_else }}, // region_heavyTags and region_bTags here are just dummy values
};

}
