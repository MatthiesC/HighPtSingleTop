void draw(bool _normalized) {

  TString input_path = "/nfs/dust/cms/user/matthies/HighPtSingleTop/102X_v1/GeneratorStudies/GenSingleTopQuarkStudy/";

  TFile* tf_tW_DR = TFile::Open(input_path+"uhh2.AnalysisModuleRunner.MC.ST_tW_DR.root", "READ");
  TFile* tf_tW_DS = TFile::Open(input_path+"uhh2.AnalysisModuleRunner.MC.ST_tW_DS.root", "READ");
  TFile* tf_tChannel = TFile::Open(input_path+"uhh2.AnalysisModuleRunner.MC.ST_tChannel.root", "READ");
  TFile* tf_sChannel = TFile::Open(input_path+"uhh2.AnalysisModuleRunner.MC.ST_sChannel.root", "READ");

  TH1F* h1_tW_DR = (TH1F*)tf_tW_DR->Get("GenSingleTopQuarkHists/topQuarkPt");
  TH1F* h1_tW_DS = (TH1F*)tf_tW_DS->Get("GenSingleTopQuarkHists/topQuarkPt");
  TH1F* h1_tChannel = (TH1F*)tf_tChannel->Get("GenSingleTopQuarkHists/topQuarkPt");
  TH1F* h1_sChannel = (TH1F*)tf_sChannel->Get("GenSingleTopQuarkHists/topQuarkPt");

  vector<float> v_binning;
  for(uint i = 0; i < 500/10; i++) v_binning.push_back(i*10);
  for(uint i = 0; i < 300/15; i++) v_binning.push_back(i*15+500);
  for(uint i = 0; i < 300/20; i++) v_binning.push_back(i*20+800);
  for(uint i = 0; i <= 400/50; i++) v_binning.push_back(i*50+1100);
  //for(float f : v_binning) cout << f << endl;
  Double_t a_binning[v_binning.size()];
  copy(v_binning.begin(), v_binning.end(), a_binning);

  TH1F *h1_tW_DR_rebinned = dynamic_cast<TH1F*>(h1_tW_DR->Rebin(v_binning.size()-1, "h1_tW_DR_rebinned", a_binning));
  TH1F *h1_tW_DS_rebinned = dynamic_cast<TH1F*>(h1_tW_DS->Rebin(v_binning.size()-1, "h1_tW_DS_rebinned", a_binning));
  TH1F *h1_tChannel_rebinned = dynamic_cast<TH1F*>(h1_tChannel->Rebin(v_binning.size()-1, "h1_tChannel_rebinned", a_binning));
  TH1F *h1_sChannel_rebinned = dynamic_cast<TH1F*>(h1_sChannel->Rebin(v_binning.size()-1, "h1_sChannel_rebinned", a_binning));

  TH1F *h1_tW_DS_over_DR_rebinned = dynamic_cast<TH1F*>(h1_tW_DS->Rebin(v_binning.size()-1, "h1_tW_DS_over_DR_rebinned", a_binning));
  for(uint i = 0; i <= h1_tW_DS_rebinned->GetNbinsX(); i++) {
    h1_tW_DS_over_DR_rebinned->SetBinContent(i, h1_tW_DS_rebinned->GetBinContent(i) / h1_tW_DR_rebinned->GetBinContent(i));
    h1_tW_DS_over_DR_rebinned->SetBinError(i, h1_tW_DS_over_DR_rebinned->GetBinContent(i)*sqrt( h1_tW_DR_rebinned->GetBinError(i)*h1_tW_DR_rebinned->GetBinError(i) / (h1_tW_DR_rebinned->GetBinContent(i)*h1_tW_DR_rebinned->GetBinContent(i)) + h1_tW_DS_rebinned->GetBinError(i)*h1_tW_DS_rebinned->GetBinError(i) / (h1_tW_DS_rebinned->GetBinContent(i)*h1_tW_DS_rebinned->GetBinContent(i)) ));
  }

  double integral_tW_DR = h1_tW_DR_rebinned->Integral();
  double integral_tW_DS = h1_tW_DS_rebinned->Integral();
  double integral_tChannel = h1_tChannel_rebinned->Integral();
  double integral_sChannel = h1_sChannel_rebinned->Integral();

  // cross sections in units of pb
  double xsection_tW = 71.7;
  double xsection_tChannel = 216.99;
  double xsection_sChannel = 10.32;

  h1_tW_DR_rebinned->Scale(1/integral_tW_DR);
  h1_tW_DS_rebinned->Scale(1/integral_tW_DS);
  h1_tChannel_rebinned->Scale(1/integral_tChannel);
  h1_sChannel_rebinned->Scale(1/integral_sChannel);

  if(!_normalized) {
    h1_tW_DR_rebinned->Scale(xsection_tW);
    h1_tW_DS_rebinned->Scale(xsection_tW);
    h1_tChannel_rebinned->Scale(xsection_tChannel);
    h1_sChannel_rebinned->Scale(xsection_sChannel);    
  }

  vector<TH1F*> h1_rebinned;
  h1_rebinned.push_back(h1_tW_DR_rebinned);
  h1_rebinned.push_back(h1_tW_DS_rebinned);
  h1_rebinned.push_back(h1_tChannel_rebinned);
  h1_rebinned.push_back(h1_sChannel_rebinned);

  for(uint i = 0; i <= h1_tW_DR_rebinned->GetNbinsX(); i++) {
    float bin_width = h1_tW_DR_rebinned->GetBinWidth(i);
    for(uint j = 0; j < h1_rebinned.size(); j++) {
      h1_rebinned.at(j)->SetBinContent(i, h1_rebinned.at(j)->GetBinContent(i)/bin_width);
      h1_rebinned.at(j)->SetBinError(i, h1_rebinned.at(j)->GetBinError(i)/bin_width);
    }
  }

  TCanvas* c = new TCanvas("c", "c", 600, 800);

  //// Main plot

  c->cd();

  TPad* p1 = new TPad("p1", "p1", 0, 0.3, 1, 1);

  // margin arguments: l, r, b, t
  p1->SetMargin(0.15,0.05,0.015,0.05);
  p1->Draw();
  p1->cd();
  p1->SetTicks(1,1);
  p1->SetLogy();
  p1->SetFrameLineWidth(1);

  gStyle->SetOptStat(false);

  h1_tW_DR_rebinned->Draw("h");
  h1_tW_DS_rebinned->Draw("h same");
  h1_tChannel_rebinned->Draw("h same");
  h1_sChannel_rebinned->Draw("h same");

  h1_tW_DR_rebinned->SetLineColor(kBlue);
  h1_tW_DS_rebinned->SetLineColor(kRed);
  h1_tChannel_rebinned->SetLineColor(kOrange+1);
  h1_sChannel_rebinned->SetLineColor(kGreen+2);
  /*
  h1_tW_DR_rebinned->SetLineWidth(2);
  h1_tW_DS_rebinned->SetLineWidth(2);
  h1_tW_DS_rebinned->SetLineStyle(9);
  h1_sChannel_rebinned->SetLineStyle(9);
  */
  if(_normalized) h1_tW_DR_rebinned->GetYaxis()->SetTitle("1/#sigma #times d#sigma/dp_{T}^{t} (1/GeV)");
  else h1_tW_DR_rebinned->GetYaxis()->SetTitle("d#sigma/dp_{T}^{t} (pb/GeV)");
  h1_tW_DR_rebinned->GetYaxis()->SetTitleSize(0.05);
  h1_tW_DR_rebinned->GetXaxis()->SetTitle("generated p_{T}^{t} (GeV)");
  h1_tW_DR_rebinned->GetXaxis()->SetLabelSize(0);
  h1_tW_DR_rebinned->SetTitle("");

  float maximum = 0;
  for(uint i = 0; i < h1_rebinned.size(); i++) {
    maximum = max(h1_rebinned.at(i)->GetMaximum(), (double)maximum);
    h1_rebinned.at(i)->SetMarkerSize(0);
  }
  h1_tW_DR_rebinned->SetMaximum(20*maximum);

  TText *title = new TText(0.2, 0.9, "Single top-quark production");
  title->SetNDC();
  title->SetTextAlign(13);
  title->SetTextFont(62); // default
  title->SetTextSize(0.04);
  title->Draw();

  TLatex *pp = new TLatex(0.2, 0.825, "pp collisions, #sqrt{s} = 13 TeV");
  pp->SetNDC();
  pp->SetTextAlign(13);
  pp->SetTextFont(42);
  pp->SetTextSize(0.04);
  pp->Draw();

  TLegend *leg = new TLegend(0.46, 0.4, 0.9, 0.77);
  leg->AddEntry(h1_tW_DR_rebinned, "tW POWHEG [DR]", "l");
  leg->AddEntry(h1_tW_DS_rebinned, "tW POWHEG [DS]", "l");
  leg->AddEntry(h1_tChannel_rebinned, "#it{t}-channel POWHEG", "l");
  leg->AddEntry(h1_sChannel_rebinned, "#it{s}-channel aMC@NLO", "l");
  leg->SetBorderSize(0);
  leg->SetTextSize(0.04);
  leg->Draw();

  //// Ratio plot

  c->cd();

  TPad* p2 = new TPad("p2", "p2", 0, 0, 1, 0.3);

  double scaling = 1.6;

  p2->SetMargin(0.15,0.05,0.35,0.03);
  p2->Draw();
  p2->cd();
  p2->SetTicks(1,1);
  p2->SetFrameLineWidth(1);

  h1_tW_DS_over_DR_rebinned->Draw("h");
  h1_tW_DS_over_DR_rebinned->GetYaxis()->SetTitle("tW DS / DR");
  h1_tW_DS_over_DR_rebinned->GetXaxis()->SetTitle("Generated parton-level p_{T}^{t} (GeV)");
  h1_tW_DS_over_DR_rebinned->GetXaxis()->SetTickLength(0.07);
  h1_tW_DS_over_DR_rebinned->GetXaxis()->SetTitleSize(0.07*scaling);
  h1_tW_DS_over_DR_rebinned->GetYaxis()->SetTitleSize(0.07*scaling);
  h1_tW_DS_over_DR_rebinned->GetXaxis()->SetTitleOffset(1.2);
  h1_tW_DS_over_DR_rebinned->GetYaxis()->SetTitleOffset(0.62);
  h1_tW_DS_over_DR_rebinned->GetYaxis()->CenterTitle();
  h1_tW_DS_over_DR_rebinned->GetXaxis()->SetLabelSize(0.085);
  h1_tW_DS_over_DR_rebinned->GetYaxis()->SetLabelSize(0.085);
  h1_tW_DS_over_DR_rebinned->GetXaxis()->SetLabelOffset(0.01);
  h1_tW_DS_over_DR_rebinned->GetYaxis()->SetLabelOffset(0.01);
  h1_tW_DS_over_DR_rebinned->SetTitle("");
  h1_tW_DS_over_DR_rebinned->SetMarkerSize(0);
  h1_tW_DS_over_DR_rebinned->SetLineColor(kBlack);
  h1_tW_DS_over_DR_rebinned->SetMaximum(1.19);

  if(!_normalized) c->SaveAs("plots/toppt.eps");
  else c->SaveAs("plots/toppt_norm.eps");

  tf_tW_DR->Close();
  tf_tW_DS->Close();
  tf_tChannel->Close();
  tf_sChannel->Close();

  return;
}


void toppt() {

  draw(false);
  draw(true);
}
