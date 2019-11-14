#include "UHH2/HighPtSingleTop/include/METXYCorrections.h"

using namespace std;
using namespace uhh2;


METXYCorrections::METXYCorrections(Context & ctx) {

  year = extract_year(ctx);
  init_done = false;
  usemetv2 = false;
  if(year == Year::is2017v2) usemetv2 = true; // recommended for 2017
}


// based on: https://lathomas.web.cern.ch/lathomas/METStuff/XYCorrections/XYMETCorrection.h

bool METXYCorrections::process(Event & event) {

  if(!init_done) {
    isMC = !(event.isRealData);
    init_done = true;
  }

  int npv = event.pvs->size();
  if(npv > 100) npv = 100;
  int runnb = event.run;

  double uncorr_MET = event.met->pt();
  double uncorr_MET_phi = event.met->phi();

  //std::pair<double, double> XYcorr_MET_METphi(uncorr_MET, uncorr_METphi);

  double METxcorr(0.), METycorr(0.);

  int runera = -1;

  if(isMC) {
    if(year == Year::is2016v3) {
      runera = y2016MC;
    }
    else if(year == Year::is2017v2) {
      runera = y2017MC;
    }
    else if(year == Year::is2018) {
      runera = y2018MC;
    }
  }
  else {
    // 2016
    if     (runnb >= 272007 && runnb <= 275376) runera = y2016B;
    else if(runnb >= 275657 && runnb <= 276283) runera = y2016C;
    else if(runnb >= 276315 && runnb <= 276811) runera = y2016D;
    else if(runnb >= 276831 && runnb <= 277420) runera = y2016E;
    else if(runnb >= 277772 && runnb <= 278808) runera = y2016F;
    else if(runnb >= 278820 && runnb <= 280385) runera = y2016G;
    else if(runnb >= 280919 && runnb <= 284044) runera = y2016H;
    
    // 2017
    else if(runnb >= 297020 && runnb <= 299329) runera = y2017B;
    else if(runnb >= 299337 && runnb <= 302029) runera = y2017C;
    else if(runnb >= 302030 && runnb <= 303434) runera = y2017D;
    else if(runnb >= 303435 && runnb <= 304826) runera = y2017E;
    else if(runnb >= 304911 && runnb <= 306462) runera = y2017F;
    
    // 2018
    else if(runnb >= 315252 && runnb <= 316995) runera = y2018A;
    else if(runnb >= 316998 && runnb <= 319312) runera = y2018B;
    else if(runnb >= 319313 && runnb <= 320393) runera = y2018C;
    else if(runnb >= 320394 && runnb <= 325273) runera = y2018D;
  }
  
  if(!usemetv2){ //Current recommendation for 2016 and 2018
    if(runera==y2016B) METxcorr = -(-0.0478335*npv -0.108032);
    else if(runera==y2016B) METycorr = -(0.125148*npv +0.355672);
    else if(runera==y2016C) METxcorr = -(-0.0916985*npv +0.393247);
    else if(runera==y2016C) METycorr = -(0.151445*npv +0.114491);
    else if(runera==y2016D) METxcorr = -(-0.0581169*npv +0.567316);
    else if(runera==y2016D) METycorr = -(0.147549*npv +0.403088);
    else if(runera==y2016E) METxcorr = -(-0.065622*npv +0.536856);
    else if(runera==y2016E) METycorr = -(0.188532*npv +0.495346);
    else if(runera==y2016F) METxcorr = -(-0.0313322*npv +0.39866);
    else if(runera==y2016F) METycorr = -(0.16081*npv +0.960177);
    else if(runera==y2016G) METxcorr = -(0.040803*npv -0.290384);
    else if(runera==y2016G) METycorr = -(0.0961935*npv +0.666096);
    else if(runera==y2016H) METxcorr = -(0.0330868*npv -0.209534);
    else if(runera==y2016H) METycorr = -(0.141513*npv +0.816732);
    else if(runera==y2017B) METxcorr = -(-0.259456*npv +1.95372);
    else if(runera==y2017B) METycorr = -(0.353928*npv -2.46685);
    else if(runera==y2017C) METxcorr = -(-0.232763*npv +1.08318);
    else if(runera==y2017C) METycorr = -(0.257719*npv -1.1745);
    else if(runera==y2017D) METxcorr = -(-0.238067*npv +1.80541);
    else if(runera==y2017D) METycorr = -(0.235989*npv -1.44354);
    else if(runera==y2017E) METxcorr = -(-0.212352*npv +1.851);
    else if(runera==y2017E) METycorr = -(0.157759*npv -0.478139);
    else if(runera==y2017F) METxcorr = -(-0.232733*npv +2.24134);
    else if(runera==y2017F) METycorr = -(0.213341*npv +0.684588);
    else if(runera==y2018A) METxcorr = -(0.362865*npv -1.94505);
    else if(runera==y2018A) METycorr = -(0.0709085*npv -0.307365);
    else if(runera==y2018B) METxcorr = -(0.492083*npv -2.93552);
    else if(runera==y2018B) METycorr = -(0.17874*npv -0.786844);
    else if(runera==y2018C) METxcorr = -(0.521349*npv -1.44544);
    else if(runera==y2018C) METycorr = -(0.118956*npv -1.96434);
    else if(runera==y2018D) METxcorr = -(0.531151*npv -1.37568);
    else if(runera==y2018D) METycorr = -(0.0884639*npv -1.57089);
    else if(runera==y2016MC) METxcorr = -(-0.195191*npv -0.170948);
    else if(runera==y2016MC) METycorr = -(-0.0311891*npv +0.787627);
    else if(runera==y2017MC) METxcorr = -(-0.217714*npv +0.493361);
    else if(runera==y2017MC) METycorr = -(0.177058*npv -0.336648);
    else if(runera==y2018MC) METxcorr = -(0.296713*npv -0.141506);
    else if(runera==y2018MC) METycorr = -(0.115685*npv +0.0128193);
  }
  else { //these are the corrections for v2 MET recipe (currently recommended for 2017)
    if(runera==y2016B) METxcorr = -(-0.0374977*npv +0.00488262);
    else if(runera==y2016B) METycorr = -(0.107373*npv -0.00732239);
    else if(runera==y2016C) METxcorr = -(-0.0832562*npv +0.550742);
    else if(runera==y2016C) METycorr = -(0.142469*npv -0.153718);
    else if(runera==y2016D) METxcorr = -(-0.0400931*npv +0.753734);
    else if(runera==y2016D) METycorr = -(0.127154*npv +0.0175228);
    else if(runera==y2016E) METxcorr = -(-0.0409231*npv +0.755128);
    else if(runera==y2016E) METycorr = -(0.168407*npv +0.126755);
    else if(runera==y2016F) METxcorr = -(-0.0161259*npv +0.516919);
    else if(runera==y2016F) METycorr = -(0.141176*npv +0.544062);
    else if(runera==y2016G) METxcorr = -(0.0583851*npv -0.0987447);
    else if(runera==y2016G) METycorr = -(0.0641427*npv +0.319112);
    else if(runera==y2016H) METxcorr = -(0.0706267*npv -0.13118);
    else if(runera==y2016H) METycorr = -(0.127481*npv +0.370786);
    else if(runera==y2017B) METxcorr = -(-0.19563*npv +1.51859);
    else if(runera==y2017B) METycorr = -(0.306987*npv -1.84713);
    else if(runera==y2017C) METxcorr = -(-0.161661*npv +0.589933);
    else if(runera==y2017C) METycorr = -(0.233569*npv -0.995546);
    else if(runera==y2017D) METxcorr = -(-0.180911*npv +1.23553);
    else if(runera==y2017D) METycorr = -(0.240155*npv -1.27449);
    else if(runera==y2017E) METxcorr = -(-0.149494*npv +0.901305);
    else if(runera==y2017E) METycorr = -(0.178212*npv -0.535537);
    else if(runera==y2017F) METxcorr = -(-0.165154*npv +1.02018);
    else if(runera==y2017F) METycorr = -(0.253794*npv +0.75776);
    else if(runera==y2018A) METxcorr = -(0.362642*npv -1.55094);
    else if(runera==y2018A) METycorr = -(0.0737842*npv -0.677209);
    else if(runera==y2018B) METxcorr = -(0.485614*npv -2.45706);
    else if(runera==y2018B) METycorr = -(0.181619*npv -1.00636);
    else if(runera==y2018C) METxcorr = -(0.503638*npv -1.01281);
    else if(runera==y2018C) METycorr = -(0.147811*npv -1.48941);
    else if(runera==y2018D) METxcorr = -(0.520265*npv -1.20322);
    else if(runera==y2018D) METycorr = -(0.143919*npv -0.979328);
    else if(runera==y2016MC) METxcorr = -(-0.159469*npv -0.407022);
    else if(runera==y2016MC) METycorr = -(-0.0405812*npv +0.570415);
    else if(runera==y2017MC) METxcorr = -(-0.182569*npv +0.276542);
    else if(runera==y2017MC) METycorr = -(0.155652*npv -0.417633);
    else if(runera==y2018MC) METxcorr = -(0.299448*npv -0.13866);
    else if(runera==y2018MC) METycorr = -(0.118785*npv +0.0889588);
  }
  
  double corr_MET_x = uncorr_MET * TMath::Cos(uncorr_MET_phi) + METxcorr;
  double corr_MET_y = uncorr_MET * TMath::Sin(uncorr_MET_phi) + METycorr;
  
  double corr_MET = TMath::Sqrt(corr_MET_x * corr_MET_x + corr_MET_y * corr_MET_y);

  double corr_MET_phi(0.);
  /*if(corr_MET_x == 0 && corr_MET_y > 0) corr_MET_phi = TMath::Pi();
  else if(corr_MET_x == 0 && corr_MET_y < 0) corr_MET_phi = -TMath::Pi();
  else if(corr_MET_x > 0) corr_MET_phi = TMath::ATan(corr_MET_y / corr_MET_x);
  else if(corr_MET_x < 0 && corr_MET_y > 0) corr_MET_phi = TMath::ATan(corr_MET_y / corr_MET_x) + TMath::Pi();
  else if(corr_MET_x > 0 && corr_MET_y < 0) corr_MET_phi = TMath::ATan(corr_MET_y / corr_MET_x) - TMath::Pi();
  else corr_MET_phi = 0;*/

  if(corr_MET_x > 0) corr_MET_phi = TMath::ATan(corr_MET_y / corr_MET_x);
  else if(corr_MET_x == 0) {
    if(corr_MET_y > 0) corr_MET_phi = 0.5*TMath::Pi();
    else if(corr_MET_y < 0) corr_MET_phi = -0.5*TMath::Pi();
    else if(corr_MET_y == 0) {
      std::cout << "MET XY corrector WARNING: MET px and py are zero. Set MET phi = 0." << std::endl;
    }
  }
  else if(corr_MET_x < 0) {
    if(corr_MET_y > 0) corr_MET_phi = 0.5*TMath::Pi() + TMath::ATan(-corr_MET_x / corr_MET_y);
    else if(corr_MET_y < 0) corr_MET_phi = -0.5*TMath::Pi() - TMath::ATan(corr_MET_x / corr_MET_y);
    else if(corr_MET_y == 0) corr_MET_phi = -TMath::Pi();
  }

  //XYcorr_MET_METphi.first = corr_MET;
  //XYcorr_MET_METphi.second = corr_METphi;

  event.met->set_pt(corr_MET);
  event.met->set_phi(corr_MET_phi);

  return true;
}


METXYCorrections::~METXYCorrections() {}
