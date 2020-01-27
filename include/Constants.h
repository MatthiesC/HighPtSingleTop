#pragma once


namespace MyConstants {

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

  binning pt_binning = {
    {200, 250},
    {250, 300},
    {300, 350},
    {350, 400},
    {400, 450},
    {450, 500},
    {500, 550},
    {550, 600},
    {600, 680},
    {680, 800},
    {800, 1000}
  };

  const double pt_binning_edges[12] = {200, 250, 300, 350, 400, 450, 500, 550, 600, 680, 800, 1000};
}
