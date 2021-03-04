import sys
import os
from copy import deepcopy
import ROOT as root

year = ''
if len(sys.argv) > 1:
    year = sys.argv[1]
else:
    year = '2016'

outputDir = os.environ.get('CMSSW_BASE')+'/src/UHH2/HighPtSingleTop/output/'
filePath_ttbar_inclusive = outputDir+'presel/'+year+'/muo/nominal/uhh2.AnalysisModuleRunner.MC.TTbar_Inclusive_2016v3.root'
filePath_ttbar_mtt700to1000 = outputDir+'presel/'+year+'/muo/nominal/uhh2.AnalysisModuleRunner.MC.TTbar_Mtt700to1000_2016v3.root'
filePath_ttbar_mtt1000toinf = outputDir+'presel/'+year+'/muo/nominal/uhh2.AnalysisModuleRunner.MC.TTbar_Mtt1000toInf_2016v3.root'

file_ttbar_inclusive = root.TFile.Open(filePath_ttbar_inclusive, 'READ')
hist_ttbar_inclusive = deepcopy(file_ttbar_inclusive.Get('0_MttBefore/mtt_100'))
file_ttbar_inclusive.Close()
file_ttbar_mtt700to1000 = root.TFile.Open(filePath_ttbar_mtt700to1000, 'READ')
hist_ttbar_mtt700to1000 = deepcopy(file_ttbar_mtt700to1000.Get('0_MttBefore/mtt_100'))
file_ttbar_mtt700to1000.Close()
file_ttbar_mtt1000toinf = root.TFile.Open(filePath_ttbar_mtt1000toinf, 'READ')
hist_ttbar_mtt1000toinf = deepcopy(file_ttbar_mtt1000toinf.Get('0_MttBefore/mtt_100'))
file_ttbar_mtt1000toinf.Close()

bin_start_mtt700to1000 = hist_ttbar_inclusive.FindBin(700.1)
bin_end_mtt700to1000 = hist_ttbar_inclusive.FindBin(999.9)
bin_start_mtt1000toinf = hist_ttbar_inclusive.FindBin(1000.1)
n_bins = hist_ttbar_inclusive.GetNbinsX()

integral_mtt700to1000 = 0
integral_mtt1000toinf = 0
integral_inclusive_mtt700to1000 = 0
integral_inclusive_mtt1000toinf = 0

for i in range(bin_start_mtt700to1000, bin_end_mtt700to1000+1):
    integral_mtt700to1000 += hist_ttbar_mtt700to1000.GetBinContent(i)
    integral_inclusive_mtt700to1000 += hist_ttbar_inclusive.GetBinContent(i)
for i in range(bin_start_mtt1000toinf, n_bins+1):
    integral_mtt1000toinf += hist_ttbar_mtt1000toinf.GetBinContent(i)
    integral_inclusive_mtt1000toinf += hist_ttbar_inclusive.GetBinContent(i)

sf_mtt700to1000 = integral_inclusive_mtt700to1000/integral_mtt700to1000
sf_mtt1000toinf = integral_inclusive_mtt1000toinf/integral_mtt1000toinf

print 'sf_mtt700to1000', sf_mtt700to1000
print 'sf_mtt1000toinf', sf_mtt1000toinf
print 'Please multiplicate those scale factors to the respective cross section in your config/database.csv'
