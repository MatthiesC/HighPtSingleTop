#include "../../../LegacyTopTagging/Analysis/constants.h"
#include "../../../HighPtSingleTop/Analysis/constants.h"

using namespace macros;

void redrawBorder() { // https://root-forum.cern.ch/t/how-to-redraw-axis-and-plot-borders/28252
   gPad->Update();
   gPad->RedrawAxis();
   // TLine l;
   // l.DrawLine(gPad->GetUxmin(), gPad->GetUymax(), gPad->GetUxmax(), gPad->GetUymax());
   // l.DrawLine(gPad->GetUxmax(), gPad->GetUymin(), gPad->GetUxmax(), gPad->GetUymax());
}

void plot(
  const string & voi,
  const macros::Year & year,
  const Channel & channel,
  const ERegion & region,
  const string & gen_level,
  const string & drds = "DR",
  const bool percent = true,
  const bool log_z = false
)
{
  const string infileDir = (string)getenv("CMSSW_BASE")+"/src/UHH2/HighPtSingleTop/output/Analysis/mainsel/"+macros::kYears.at(year).name+"/"+kChannels.at(channel).name+"/unfolding/migration_matrices/";
  const string infileName = "migration_matrix__"+drds+"__"+voi+"__"+gen_level+"__region_"+kRegions.at(region).name+"__"+macros::kYears.at(year).name+"_"+kChannels.at(channel).name+".root";
  const string infilePath = infileDir + infileName;
  TFile *infile = TFile::Open(infilePath.c_str(), "READ");
  TH2F *hist_raw = (TH2F*)infile->Get("migration_matrix");

  const int n_bins_x = hist_raw->GetNbinsX();
  const int n_bins_y = hist_raw->GetNbinsY();
  // Here we move everything from the overflow bins into the respective last bins:
  for(int i_bin_x = 0; i_bin_x < n_bins_x; i_bin_x++) {
    hist_raw->SetBinContent(n_bins_y, hist_raw->GetBinContent(n_bins_y) + hist_raw->GetBinContent(n_bins_y + 1));
    hist_raw->SetBinContent(n_bins_y + 1, 0.);
    hist_raw->SetBinError(n_bins_y, sqrt(pow(hist_raw->GetBinError(n_bins_y), 2) + pow(hist_raw->GetBinError(n_bins_y + 1), 2)));
    hist_raw->SetBinError(n_bins_y + 1, 0.);
  }
  for(int i_bin_y = 0; i_bin_y < n_bins_y; i_bin_y++) {
    hist_raw->SetBinContent(n_bins_x, hist_raw->GetBinContent(n_bins_x) + hist_raw->GetBinContent(n_bins_x + 1));
    hist_raw->SetBinContent(n_bins_x + 1, 0.);
    hist_raw->SetBinError(n_bins_x, sqrt(pow(hist_raw->GetBinError(n_bins_x), 2) + pow(hist_raw->GetBinError(n_bins_x + 1), 2)));
    hist_raw->SetBinError(n_bins_x + 1, 0.);
  }

  // Here we copy the raw histogram. The copy will be transformed into a proper migration matrix
  TH2F *hist = new TH2F(*hist_raw);
  hist->SetTitle("");
  // hist->SetTitle(voi.c_str());

  const TArrayD *binning = hist->GetYaxis()->GetXbins(); // here, we need to assume that rec and gen binning is the same
  for(int i = 0; i < binning->GetSize(); i++) {
    // cout << binning->GetArray()[i] << endl;
  }

  const int error_multiplicator = 10; // used for the MC stat uncertainty in these plots:
  TH1F *stability = new TH1F("stability", "", binning->GetSize()-1, binning->GetArray());
  TH1F *purity = new TH1F("purity", "", binning->GetSize()-1, binning->GetArray());
  // TH1F *acceptance = new TH1F("acceptance", "", binning->GetSize()-1, binning->GetArray()); // See Paolo's thesis, page 110
  // TH1F *background = new TH1F("background", "", binning->GetSize()-1, binning->GetArray());

  map<string, map<int, double>> columnar_sums;
  // assume that gen = x, rec = y
  const int first_bin_x = 0; // gen
  const int first_bin_y = 1; // rec

  for(int i_bin_x = first_bin_x; i_bin_x <= n_bins_x; i_bin_x++) {
    double sum = 0.;
    double sum_err2 = 0.;
    for(int i_bin_y = first_bin_y; i_bin_y <= n_bins_y; i_bin_y++) {
      const double bin_content = hist_raw->GetBinContent(i_bin_x, i_bin_y);
      const double bin_error = hist_raw->GetBinError(i_bin_x, i_bin_y);
      sum += bin_content;
      if(i_bin_x == i_bin_y) {
        stability->SetBinContent(i_bin_x, bin_content);
        stability->SetBinError(i_bin_x, bin_error);
      }
      sum_err2 += pow(hist_raw->GetBinError(i_bin_x, i_bin_y), 2);
    }
    sum = max(sum, 0.0001);
    columnar_sums["gen_sum"][i_bin_x] = sum; // prevent division by zero
    columnar_sums["gen_sum_err"][i_bin_x] = sqrt(sum_err2); // prevent division by zero
  }
  for(int i_bin_y = first_bin_y; i_bin_y <= n_bins_y; i_bin_y++) {
    double sum = 0.;
    double sum_err2 = 0.;
    for(int i_bin_x = first_bin_x; i_bin_x <= n_bins_x; i_bin_x++) {
      const double bin_content = hist_raw->GetBinContent(i_bin_x, i_bin_y);
      const double bin_error = hist_raw->GetBinError(i_bin_x, i_bin_y);
      sum += bin_content;
      if(i_bin_x == i_bin_y) {
        purity->SetBinContent(i_bin_y, bin_content);
        purity->SetBinError(i_bin_y, bin_error);
      }
      sum_err2 += pow(hist_raw->GetBinError(i_bin_x, i_bin_y), 2);
    }
    sum = max(sum, 0.0001);
    columnar_sums["rec_sum"][i_bin_y] = sum; // prevent division by zero
    columnar_sums["rec_sum_err"][i_bin_y] = sqrt(sum_err2); // prevent division by zero
  }

  // normalize gen columns:
  for(int i_bin_x = first_bin_x; i_bin_x <= n_bins_x; i_bin_x++) {
    // const double norm = 1./columnar_sums["gen_sum"][i_bin_x];
    for(int i_bin_y = first_bin_y; i_bin_y <= n_bins_y; i_bin_y++) {
      // hist->SetBinContent(i_bin_x, i_bin_y, hist->GetBinContent(i_bin_x, i_bin_y) * norm * (percent ? 100. : 1.));
      // hist->SetBinError(i_bin_x, i_bin_y, hist->GetBinError(i_bin_x, i_bin_y) * norm * (percent ? 100. : 1.));
      const auto hist_bin = fraction_uncertainty(hist->GetBinContent(i_bin_x, i_bin_y), columnar_sums["gen_sum"][i_bin_x], hist->GetBinError(i_bin_x, i_bin_y), columnar_sums["gen_sum_err"][i_bin_x]);
      hist->SetBinContent(i_bin_x, i_bin_y, (percent ? 100. : 1.) * hist_bin.first);
      hist->SetBinError(i_bin_x, i_bin_y, (percent ? 100. : 1.) * hist_bin.second);
      if(i_bin_x == i_bin_y) {
        const auto stability_bin = fraction_uncertainty(stability->GetBinContent(i_bin_x), columnar_sums["gen_sum"][i_bin_x], stability->GetBinError(i_bin_x), columnar_sums["gen_sum_err"][i_bin_x]);
        stability->SetBinContent(i_bin_x, stability_bin.first);
        stability->SetBinError(i_bin_x, error_multiplicator * stability_bin.second);
        const auto purity_bin = fraction_uncertainty(purity->GetBinContent(i_bin_x), columnar_sums["rec_sum"][i_bin_x], purity->GetBinError(i_bin_x), columnar_sums["rec_sum_err"][i_bin_x]);
        purity->SetBinContent(i_bin_y, purity_bin.first);
        purity->SetBinError(i_bin_y, error_multiplicator * purity_bin.second);
      }
    }
  }

  // hist = object for drawing (might be modified, having not the original migration matrix values)
  // migration_matrix = object that holds the true migration matrix values, used for further calculations
  TH2F *migration_matrix = new TH2F(*hist);

  const float canvas_height = 700;
  const float canvas_width = 600;

  const float canvas_margin_l = 0.16;
  const float canvas_margin_r = 0.20;
  const float canvas_margin_b = 0.16 * canvas_width / canvas_height;
  const float canvas_margin_t = 0.08 * canvas_width / canvas_height;

  auto coord = new CoordinateConverter();
  coord->init(canvas_margin_l, canvas_margin_r, canvas_margin_b, canvas_margin_t);

  const float tick_length = 0.015; // fraction of canvas width/height
  const float border_y = 1. - kGoldenRatio; // access here, where the canvas is split between main plot and ratio plot
  const float border_margin = 0.015 * canvas_width / canvas_height;

  gStyle->SetLineWidth(1);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(kViridis); // matplotlib's default
  // gStyle->SetPalette(kBird); // ROOT's default

  // const Int_t Number = 3; // red, white, green
  // Double_t Stops[Number]  = { 0.00, 0.50, 1.00};
  // Double_t Red[Number]    = { 1.00, 1.00, 0.00};
  // Double_t Green[Number]  = { 0.00, 1.00, 1.00};
  // Double_t Blue[Number]   = { 0.00, 1.00, 0.00};
  // Int_t nb=50;
  // Float_t opacity=1.;
  // TColor::CreateGradientColorTable(Number,Stops,Red,Green,Blue,nb, opacity);

  TCanvas *canvas = new TCanvas("canvas", "canvas title", canvas_width, canvas_height);

  canvas->cd();
  TPad *p_main = new TPad("pad_main", "pad title", 0, border_y, 1, 1);
  const float p_main_relative_height = 1. - border_y;
  p_main->SetTopMargin(canvas_margin_t / p_main_relative_height);
  p_main->SetBottomMargin(border_margin / p_main_relative_height);
  p_main->SetLeftMargin(canvas_margin_l);
  p_main->SetRightMargin(canvas_margin_r);
  // p_main->SetMargin();
  // p_main->SetFrameFillColor(kGreen);
  p_main->SetTickx(1);
  p_main->SetTicky(1);
  if(log_z) p_main->SetLogz();
  p_main->Draw();

  canvas->cd();
  TPad *p_ratio = new TPad("pad_ratio", "pad title", 0, 0, 1, border_y);
  const float p_ratio_relative_height = border_y;
  p_ratio->SetTopMargin(border_margin / p_ratio_relative_height);
  p_ratio->SetBottomMargin(canvas_margin_b / p_ratio_relative_height);
  p_ratio->SetLeftMargin(canvas_margin_l);
  p_ratio->SetRightMargin(canvas_margin_r);
  // p_ratio->SetMargin();
  // p_ratio->SetFrameFillColor(kOrange);
  p_ratio->SetTickx(1);
  p_ratio->SetTicky(1);
  p_ratio->SetGrid();
  p_ratio->Draw();

  p_main->cd();
  // hist->Draw("colz text");

  const float text_size = 0.03;
  const float small_text_size = 0.018;

  hist->GetXaxis()->SetTitle("");
  hist->GetXaxis()->SetLabelOffset(100.); // make it vanish under other pad

  // hist->GetYaxis()->SetTitle("Reconstructed top quark #it{p}_{T} [GeV]");
  hist->GetYaxis()->SetTitle(((TObjString*)infile->Get("axis_label_rec"))->GetString().Data());
  hist->GetYaxis()->SetTitleSize(text_size / p_main_relative_height); // restore original font size
  hist->GetYaxis()->SetTitleOffset(1.4);
  hist->GetYaxis()->SetLabelSize(text_size / p_main_relative_height); // restore original font size
  hist->GetYaxis()->SetLabelOffset(0.01 / p_main_relative_height);

  const double z_maximum = percent ? 80. : .8;
  const double z_minimum = log_z ? (percent ? 1. : 0.01) : (0.);
  // for log plots, make sure that rare bins which have content less then z_minimum are forced to have z_minimum as value so they are shown with a color (and not just "out-of-range" white)
  // need to do this here at the last step since other previous calculations depend on the correct matrix entries!
  for(int i_bin_x = 1; i_bin_x <= n_bins_x; i_bin_x++) {
    for(int i_bin_y = 1; i_bin_y <= n_bins_y; i_bin_y++) {
      if(hist->GetBinContent(i_bin_x, i_bin_y) < z_minimum) {
        hist->SetBinContent(i_bin_x, i_bin_y, z_minimum);
      }
      else if(hist->GetBinContent(i_bin_x, i_bin_y) >= z_maximum) {
        hist->SetBinContent(i_bin_x, i_bin_y, z_maximum - 0.0001);
      }
    }
  }
  if(log_z) hist->GetZaxis()->ChangeLabel(1,-1,-1,-1,-1,-1, percent ? "#leq 1" : "#leq 10^{#minus2}");

  // hist->GetZaxis()->SetLimits(z_minimum, z_maximum);
  hist->SetMinimum(z_minimum);
  hist->SetMaximum(z_maximum);
  if(percent) hist->GetZaxis()->SetTitle("Migration probability [%]");
  else hist->GetZaxis()->SetTitle("Migration probability");
  hist->GetZaxis()->SetTitleSize(text_size / p_main_relative_height); // restore original font size
  hist->GetZaxis()->SetLabelSize(text_size / p_main_relative_height); // restore original font size
  hist->GetZaxis()->SetTitleOffset(1.2);
  hist->GetZaxis()->SetLabelOffset(0.005 / p_main_relative_height);

  p_main->Update();
  double tickScaleX = (p_main->GetUxmax() - p_main->GetUxmin()) / (p_main->GetX2() - p_main->GetX1()) * (p_main->GetWh() * p_main->GetAbsHNDC());
  double tickScaleY = (p_main->GetUymax() - p_main->GetUymin()) / (p_main->GetY2() - p_main->GetY1()) * (p_main->GetWw() * p_main->GetAbsWNDC());
  hist->GetXaxis()->SetTickLength(canvas->GetWh() * tick_length / tickScaleX);
  hist->GetYaxis()->SetTickLength(canvas->GetWw() * tick_length / tickScaleY);
  redrawBorder();

  hist->Draw("colz");

  // draw text into bins (alternative to "text" draw option)
  const double text_y_offset_from_bin_center = 0.01 * (binning->At(binning->GetSize() - 1) - binning->At(0));
  for(int i_bin_x = 1; i_bin_x <= n_bins_x; i_bin_x++) {
    for(int i_bin_y = 1; i_bin_y <= n_bins_y; i_bin_y++) {
      const double bin_center_x = migration_matrix->GetXaxis()->GetBinCenter(i_bin_x);
      const double bin_center_y = migration_matrix->GetYaxis()->GetBinCenter(i_bin_y);

      string prob;
      stringstream ss;
      ss << std::fixed << setprecision(percent ? 2 : 4) << migration_matrix->GetBinContent(i_bin_x, i_bin_y);
      prob += ss.str() + " #pm ";
      ss.str(""); ss.clear(); // reset stringstream
      ss << std::fixed << setprecision(percent ? 2 : 4) << migration_matrix->GetBinError(i_bin_x, i_bin_y);
      prob += ss.str();
      ss.str(""); ss.clear(); // reset stringstream

      Int_t text_color = kWhite;
      if(i_bin_x == i_bin_y) text_color = kBlack;

      TLatex *text_prob = new TLatex(bin_center_x, bin_center_y + text_y_offset_from_bin_center, prob.c_str());
      text_prob->SetTextAlign(21); // center bottom
      text_prob->SetTextFont(42);
      text_prob->SetTextSize(small_text_size / p_main_relative_height);
      text_prob->SetTextColor(text_color);
      text_prob->Draw();

      string events = "(#it{N} = ";
      ss << int(hist_raw->GetBinContent(i_bin_x, i_bin_y));
      events += ss.str() + ")";
      ss.str(""); ss.clear(); // reset stringstream

      TLatex *text_events = new TLatex(bin_center_x, bin_center_y - text_y_offset_from_bin_center, events.c_str());
      text_events->SetTextAlign(23); // center top
      text_events->SetTextFont(42);
      text_events->SetTextSize(small_text_size / p_main_relative_height);
      text_events->SetTextColor(text_color);
      text_events->Draw();
    }
  }

  // stringstream ss;
  // ss << std::fixed << setprecision(0) << binning->At(binning->GetSize() - 1);
  // const string last_label = "#geq " + ss.str();
  // hist->GetYaxis()->ChangeLabel(-1,-1,-1,-1,-1,-1,last_label);
  // stability->GetXaxis()->ChangeLabel(-1,-1,-1,-1,-1,-1,last_label);



  p_ratio->cd();
  // const string ratio_draw_option = "e0 e1";
  // stability->Draw(ratio_draw_option.c_str());
  // purity->Draw((ratio_draw_option+" same").c_str());

  TGraphAsymmErrors *stability_graph = new TGraphAsymmErrors(stability);
  TGraphAsymmErrors *purity_graph = new TGraphAsymmErrors(purity);

  const double x_offset = 0.02 * (binning->At(binning->GetSize() - 1) - binning->At(0));

  for(int i_point = 0; i_point < stability_graph->GetN(); i_point++) {
    double x, y;
    stability_graph->GetPoint(i_point, x, y);
    stability_graph->SetPoint(i_point, x - x_offset, y);
    stability_graph->SetPointEXlow(i_point, stability_graph->GetErrorXlow(i_point) - x_offset);
    stability_graph->SetPointEXhigh(i_point, stability_graph->GetErrorXhigh(i_point) + x_offset);
  }
  for(int i_point = 0; i_point < purity_graph->GetN(); i_point++) {
    double x, y;
    purity_graph->GetPoint(i_point, x, y);
    purity_graph->SetPoint(i_point, x + x_offset, y);
    purity_graph->SetPointEXlow(i_point, purity_graph->GetErrorXlow(i_point) + x_offset);
    purity_graph->SetPointEXhigh(i_point, purity_graph->GetErrorXhigh(i_point) - x_offset);
  }

  TMultiGraph *mg = new TMultiGraph("mg", "");;
  mg->Add(stability_graph);
  mg->Add(purity_graph);
  mg->Draw("a p");

  TH1F *mg_hist = mg->GetHistogram();

  // Colors close to matplotlib's default color cycle:
  Int_t color_stability = kColors.at("pyplot_orange");
  Int_t color_purity = kColors.at("pyplot_blue");

  Double_t marker_size = 0.8;

  stability_graph->SetMarkerStyle(24);
  stability_graph->SetMarkerSize(marker_size);
  stability_graph->SetMarkerColor(color_stability);
  stability_graph->SetLineColor(color_stability);
  stability_graph->SetLineWidth(1);
  // stability_graph->SetLineStyle(5);

  purity_graph->SetMarkerStyle(25);
  purity_graph->SetMarkerSize(marker_size);
  purity_graph->SetMarkerColor(color_purity);
  purity_graph->SetLineColor(color_purity);
  purity_graph->SetLineWidth(1);
  // purity_graph->SetLineStyle(2);

  double minimum = min(stability->GetMinimum(), purity->GetMinimum());
  double maximum = max(stability->GetMaximum(), purity->GetMaximum());
  const double distance_to_axis = 0.1;
  mg_hist->SetMinimum(minimum - distance_to_axis);
  mg_hist->SetMaximum(maximum + distance_to_axis);
  mg_hist->SetMinimum(0.);
  mg_hist->SetMaximum(1.);

  mg_hist->GetXaxis()->SetLimits(binning->At(0), binning->At(binning->GetSize() - 1));
  // mg_hist->GetXaxis()->SetTitle("Parton-level top quark #it{p}_{T} [GeV]");
  // mg_hist->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
  mg_hist->GetXaxis()->SetTitle(((TObjString*)infile->Get("axis_label_gen"))->GetString().Data());
  mg_hist->GetXaxis()->SetTitleSize(text_size / p_ratio_relative_height); // restore original font size
  mg_hist->GetXaxis()->SetTitleOffset(1.5);
  mg_hist->GetXaxis()->SetLabelSize(text_size / p_ratio_relative_height); // restore original font size
  mg_hist->GetXaxis()->SetLabelOffset(0.01 / p_ratio_relative_height);

  mg_hist->GetYaxis()->SetTitle("");
  mg_hist->GetYaxis()->SetLabelSize(text_size / p_ratio_relative_height); // restore original font size
  mg_hist->GetYaxis()->SetLabelOffset(0.006 / p_ratio_relative_height);
  mg_hist->GetYaxis()->SetNdivisions(505);

  p_ratio->Update();
  tickScaleX = (p_ratio->GetUxmax() - p_ratio->GetUxmin()) / (p_ratio->GetX2() - p_ratio->GetX1()) * (p_ratio->GetWh() * p_ratio->GetAbsHNDC());
  tickScaleY = (p_ratio->GetUymax() - p_ratio->GetUymin()) / (p_ratio->GetY2() - p_ratio->GetY1()) * (p_ratio->GetWw() * p_ratio->GetAbsWNDC());
  mg_hist->GetXaxis()->SetTickLength(canvas->GetWh() * tick_length / tickScaleX);
  mg_hist->GetYaxis()->SetTickLength(canvas->GetWw() * tick_length / tickScaleY);
  redrawBorder();

  const double leg_ratio_x1 = 1. - canvas_margin_r + 0.02;
  const double leg_ratio_y1 = 0.52;
  const double leg_ratio_x2 = 1.;
  const double leg_ratio_y2 = 0.8;
  TLegend *leg_ratio = new TLegend(leg_ratio_x1, leg_ratio_y1, leg_ratio_x2, leg_ratio_y2);
  leg_ratio->SetTextSize(text_size / p_ratio_relative_height);
  leg_ratio->SetBorderSize(0);
  leg_ratio->SetFillStyle(0);
  leg_ratio->AddEntry(stability_graph, "Stability");
  leg_ratio->AddEntry(purity_graph, "Purity");
  leg_ratio->Draw();

  if(error_multiplicator != 1) {
    const string mc_unc_note_text = "MC stat. uncertainty #times "+to_string(error_multiplicator);
    TLatex *mc_unc_note = new TLatex(1. - canvas_margin_r - 0.025, 0.47, mc_unc_note_text.c_str());
    mc_unc_note->SetTextAlign(33); // right top aligned
    mc_unc_note->SetTextFont(52);
    mc_unc_note->SetTextSize(small_text_size / p_ratio_relative_height);
    mc_unc_note->SetNDC();
    mc_unc_note->Draw();
  }


  p_main->cd();

  TText *cms = new TText(canvas_margin_l, 1. - canvas_margin_t / p_main_relative_height + 0.01, "CMS");
  cms->SetTextAlign(11); // left bottom
  cms->SetTextFont(62);
  cms->SetTextSize(0.04 / p_main_relative_height);
  cms->SetNDC();
  // cms->SetTextColor(kWhite);
  cms->Draw();

  TText *prelim = new TText(canvas_margin_l + 0.1, 1. - canvas_margin_t / p_main_relative_height + 0.01, "Simulation, Private Work");
  prelim->SetTextAlign(11); // left bottom
  prelim->SetTextFont(52);
  prelim->SetTextSize(0.025 / p_main_relative_height);
  prelim->SetNDC();
  // prelim->SetTextColor(kWhite);
  prelim->Draw();

  const string text_top_right_string = macros::kYears.at(year).lumi_fb_display+" fb^{#minus1} (13 TeV)";
  TLatex *text_top_right = new TLatex(1. - canvas_margin_r + 0.05, 1. - canvas_margin_t / p_main_relative_height + 0.01, text_top_right_string.c_str());
  text_top_right->SetTextAlign(31); // right bottom aligned
  text_top_right->SetTextFont(42);
  text_top_right->SetTextSize(text_size / p_main_relative_height);
  text_top_right->SetNDC();
  text_top_right->Draw();

  string region_info_text;
  region_info_text += kChannels.at(channel).tlatex;
  // if(region != ERegion::_unfold) region_info_text += ", " + kRegions.at(region).name;
  region_info_text += ", " + kRegions.at(region).tlatex;
  if(year != Year::isRun2) region_info_text += ", " + macros::kYears.at(year).nice_name;
  TLatex *region_info = new TLatex(canvas_margin_l + 0.037, 1. - canvas_margin_t / p_main_relative_height - 0.05, region_info_text.c_str());
  region_info->SetTextAlign(13); // left top aligned
  region_info->SetTextFont(42);
  region_info->SetTextSize(0.025 / p_main_relative_height);
  region_info->SetNDC();
  region_info->SetTextColor(kWhite);
  region_info->Draw();

  // Get condition number from SVD (Singular Value Decomposition)
  // https://twiki.cern.ch/twiki/bin/viewauth/CMS/ScrecUnfolding
  // https://arxiv.org/pdf/hep-ph/9509307.pdf
  TMatrixD matrix = TMatrixD(n_bins_x, n_bins_y);
  TArrayD matrix_entries = TArrayD(n_bins_x * n_bins_y);
  for(int i_bin_x = 1; i_bin_x <= n_bins_x; i_bin_x++) {
    for(int i_bin_y = 1; i_bin_y <= n_bins_y; i_bin_y++) {
      int entry = (i_bin_x - 1) * n_bins_y + (i_bin_y - 1);
      matrix_entries[entry] = migration_matrix->GetBinContent(i_bin_x, i_bin_y);
      // matrix_entries[entry] = i_bin_x == i_bin_y ? 1 : 0;
    }
  }
  matrix.SetMatrixArray(matrix_entries.GetArray());
  TDecompSVD decomp = TDecompSVD(matrix);
  decomp.Decompose();
  const TVectorD & singular_values = decomp.GetSig();
  const double condition_number = singular_values[0] / max(0.0001, singular_values[singular_values.GetNrows() - 1]); // according to TDecompSVD documentation, first element is the largest and last element the smallest
  // Actually, despite the caveat in the TWiki, decomp.Condition() seems to produce the same condition numbers ...
  // cout << condition_number << endl;

  stringstream ss_cond;
  ss_cond << std::fixed << setprecision(2) << condition_number;
  const string cond_text_string = "Cond. = " + ss_cond.str();
  TLatex *cond_text = new TLatex(1. - canvas_margin_r - 0.037, 1. - canvas_margin_t / p_main_relative_height - 0.05, cond_text_string.c_str());
  cond_text->SetTextAlign(33); // left top aligned
  cond_text->SetTextFont(42);
  cond_text->SetTextSize(0.025 / p_main_relative_height);
  cond_text->SetNDC();
  // cond_text->SetTextColor(kWhite);
  cond_text->Draw();

  // Save to disk
  TString plotName = infileName;
  plotName.ReplaceAll(".root", "");
  plotName += (log_z ? (string)"_log" : (string)"_lin")+(string)".pdf";
  const TString plotDir = infileDir;
  gSystem->Exec(((string)"mkdir -p "+(string)plotDir.Data()).c_str());
  const TString plotPath = plotDir + plotName;
  canvas->SaveAs(plotPath.Data());
  delete canvas;
}

// void plot(
//   const string & voi,
//   const macros::Year & year,
//   const Channel & channel,
//   const ERegion & region,
//   const string & gen_level,
//   const string & drds = "DR",
//   const bool percent = true,
//   const bool log_z = false
// );

void migration_matrices() {
  cout << "Hello World from MigrationMatrixPlotter!" << endl;

  const set<string> vois = {
    "TopQuarkPt",
    "TopQuarkAbsY",
    "WBosonPt",
    "WBosonAbsY",
    // "tWSystemMass",
    // "tWSystemCustomDPhi",
  };

  // const set<Year> years = { Year::isUL18, Year::isUL17, Year::isUL16preVFP, Year::isUL16postVFP };
  const set<Year> years = { Year::isRun2 };

  // const set<Channel> channels = { Channel::isMuo, Channel::isEle };
  const set<Channel> channels = { Channel::isMuo, Channel::isEle, Channel::isBoth };
  // const set<Channel> channels = { Channel::isBoth };

  const string gen_level = "Matrix";
  // const string gen_level = "Parton";
  // const string gen_level = "Particle";

  const set<ERegion> regions = {
    ERegion::_unfold,
    ERegion::_1t,
    ERegion::_0t1W,
    ERegion::_1b1t,
    ERegion::_1b0t1W,
    ERegion::_2b1t,
    ERegion::_2b0t1W,
  };

  for(const string & voi : vois) {
    for(const Year & year : years) {
      for(const Channel & channel : channels) {
        for(const ERegion & region : regions) {
          plot(
            voi,
            year,
            channel,
            region,
            gen_level,
            "DR",
            // "DS",
            true,
            false
          );
        }
      }
    }
  }
  // plot("TopQuarkPt", macros::Year::isUL18, Channel::isMuo, ERegion::_unfold, gen_level, "DR", true, true);
}
