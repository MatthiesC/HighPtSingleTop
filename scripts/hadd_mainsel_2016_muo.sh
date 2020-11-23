#!/bin/bash

shopt -s extglob
shopt extglob

workdir=${CMSSW_BASE}/src/UHH2/HighPtSingleTop/output/mainsel/2016/muo/nominal/
hadddir=hadded/
preFix=uhh2.AnalysisModuleRunner.

mkdir -p ${workdir}${hadddir}
hadd -f ${workdir}${hadddir}${preFix}DATA.DATA.root ${workdir}/${preFix}DATA.DATA*.root
hadd -f ${workdir}${hadddir}${preFix}MC.QCD.root ${workdir}/${preFix}MC.QCD*.root
hadd -f ${workdir}${hadddir}${preFix}MC.Diboson.root ${workdir}/${preFix}MC.Diboson*.root
hadd -f ${workdir}${hadddir}${preFix}MC.DYJets.root ${workdir}/${preFix}MC.DYJets*.root
hadd -f ${workdir}${hadddir}${preFix}MC.WJetsLight.root ${workdir}/${preFix}MC.WJetsLight*.root
hadd -f ${workdir}${hadddir}${preFix}MC.WJetsHeavy.root ${workdir}/${preFix}MC.WJetsHeavy*.root
hadd -f ${workdir}${hadddir}${preFix}MC.TTbar.root ${workdir}/${preFix}MC.TTbar*.root
hadd -f ${workdir}${hadddir}${preFix}MC.ST_non-tW.root ${workdir}/${preFix}MC.ST_tChannel*.root ${workdir}/${preFix}MC.ST_sChannel*.root
hadd -f ${workdir}${hadddir}${preFix}MC.ST_tW_DR_NoFullyHadronic.root ${workdir}/${preFix}MC.ST_tW_DR_NoFullyHadronic*.root
hadd -f ${workdir}${hadddir}${preFix}MC.ST_tW_DR_NoFullyHadronic_Sig.root ${workdir}/${preFix}MC.ST_tW_DR_NoFullyHadronic_@(TopToHad_WToMuo|TopToMuo_WToHad)_T@(|bar)_2016v3.root
hadd -f ${workdir}${hadddir}${preFix}MC.ST_tW_DR_NoFullyHadronic_Bkg.root ${workdir}/${preFix}MC.ST_tW_DR_NoFullyHadronic_!(TopToHad_WToMuo|TopToMuo_WToHad)_T@(|bar)_2016v3.root
hadd -f ${workdir}${hadddir}${preFix}MC.ST_tW_DR_inclusiveDecays.root ${workdir}/${preFix}MC.ST_tW_DR_inclusiveDecays*.root
hadd -f ${workdir}${hadddir}${preFix}MC.ST_tW_DR_inclusiveDecays_Sig.root ${workdir}/${preFix}MC.ST_tW_DR_inclusiveDecays_@(TopToHad_WToMuo|TopToMuo_WToHad)_T@(|bar)_2016v3.root
hadd -f ${workdir}${hadddir}${preFix}MC.ST_tW_DR_inclusiveDecays_Bkg.root ${workdir}/${preFix}MC.ST_tW_DR_inclusiveDecays_!(TopToHad_WToMuo|TopToMuo_WToHad)_T@(|bar)_2016v3.root
hadd -f ${workdir}${hadddir}${preFix}MC.ST_tW_DS_NoFullyHadronic.root ${workdir}/${preFix}MC.ST_tW_DS_NoFullyHadronic*.root
hadd -f ${workdir}${hadddir}${preFix}MC.ST_tW_DS_NoFullyHadronic_Sig.root ${workdir}/${preFix}MC.ST_tW_DS_NoFullyHadronic_@(TopToHad_WToMuo|TopToMuo_WToHad)_T@(|bar)_2016v3.root
hadd -f ${workdir}${hadddir}${preFix}MC.ST_tW_DS_NoFullyHadronic_Bkg.root ${workdir}/${preFix}MC.ST_tW_DS_NoFullyHadronic_!(TopToHad_WToMuo|TopToMuo_WToHad)_T@(|bar)_2016v3.root

shopt -u extglob
shopt extglob
