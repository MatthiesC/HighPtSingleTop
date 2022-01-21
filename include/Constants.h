#pragma once

namespace uhh2 { namespace btw {

enum class LepChannel {
  muo,
  ele,
};



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
