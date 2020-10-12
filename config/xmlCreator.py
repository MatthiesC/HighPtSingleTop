#!/usr/bin/env python3

import os
import sys
import csv
from collections import OrderedDict
import argparse
from itertools import permutations
# import xml.etree.ElementTree as ET


class configContainer:

   '''Container class for configurating XML files'''

   userName = str()
   uhh2Dir = str()
   userMail = str()
   yearVars = dict()
   used_samples = OrderedDict()

   def __init__(self):

      self.userName = os.environ.get('USER')
      self.uhh2Dir = os.environ.get('CMSSW_BASE')+'/src/UHH2/'
      self.userMail = '@'.join([self.userName, 'mail.desy.de']) # Avoid spam due to public code on GitHub

      self.yearVars['yearVersions'] = {
         '2016': 'v3',
         '2017': 'v2',
         '2018': '',
      }

      self.yearVars['preselFileSplit'] = {
         '2016': '30',
         '2017': '42',
         '2018': '50',
      }

      self.yearVars['targetLumis'] = {
         '2016': 35921.87,
         '2017': 41528.99,
         '2018': 59740.56,
      }

      self.yearVars['lumiFiles'] = {
         # '2016': self.uhh2Dir+'common/data/2016/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.root',
         # '2017': self.uhh2Dir+'common/data/2017/Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON_v1.root',
         # '2018': self.uhh2Dir+'common/data/2018/Cert_314472-325175_13TeV_PromptReco_Collisions18_JSON.root',
         '2016': self.uhh2Dir+'HighPtSingleTop/data/GoldenJSONs/Cert_271036-284044_13TeV_ReReco_07Aug2017_Collisions16_JSON.root',
         '2017': self.uhh2Dir+'HighPtSingleTop/data/GoldenJSONs/Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON_v1.root',
         '2018': self.uhh2Dir+'HighPtSingleTop/data/GoldenJSONs/Cert_314472-325175_13TeV_17SeptEarlyReReco2018ABC_PromptEraD_Collisions18_JSON.root',
      }

      self.yearVars['hotvrSFFiles'] = {
         '2016': self.uhh2Dir+'HOTVR/data/2016TopTaggingScaleFactors.root',
         '2017': self.uhh2Dir+'HOTVR/data/2017TopTaggingScaleFactors.root',
         '2018': self.uhh2Dir+'HOTVR/data/2018TopTaggingScaleFactors.root',
      }

      self.yearVars['deepjetSFFiles'] = {
         '2016': self.uhh2Dir+'HighPtSingleTop/data/ScaleFactors/2016/DeepJet_2016LegacySF_V1__reduced.csv', # if using CP5 samples, need to use other sf file
         '2017': self.uhh2Dir+'HighPtSingleTop/data/ScaleFactors/2017/DeepFlavour_94XSF_V4_B_F__reduced.csv',
         '2018': self.uhh2Dir+'HighPtSingleTop/data/ScaleFactors/2018/DeepJet_102XSF_V2__reduced.csv',
      }

      self.yearVars['NNFiles'] = {
         'tTag': {
            '2016': self.uhh2Dir+'HighPtSingleTop/data/KerasDNNModels/2016/dnn_20-02-11-14-50-22/neural_net.json',
            '2017': self.uhh2Dir+'',
            '2018': self.uhh2Dir+'',
         },
         'WTag': {
            '2016': self.uhh2Dir+'',
            '2017': self.uhh2Dir+'',
            '2018': self.uhh2Dir+'',
         },
      }

   @staticmethod
   def read_database(years: list, channels: list):

      used_samples = OrderedDict()
      for year in years:
         used_samples[year] = OrderedDict()
         for channel in channels:
            used_samples[year][channel] = list()
            with open('database.csv', 'r') as file:
               reader = csv.DictReader(file)
               for row in reader:
                  use_me = row['use_me']=='True' and row['year']==year and (row['channel']==channel or row['channel']=='both')
                  if use_me:
                     used_samples[year][channel].append(sampleEntity(row))
      configContainer.used_samples = used_samples


class sampleEntity:

   '''Container to hold information about a data or MC sample, as read from CSV database'''

   def __init__(self, csvRow: OrderedDict):

      self.year = csvRow['year']
      self.channel = csvRow['channel']
      self.is_data = True if csvRow['is_data']=='True' else False
      self.nickName = csvRow['nickName']
      self.xmlPath = csvRow['xmlPath']
      self.xsection = float(csvRow['xsection']) # to be given in pb
      self.weightedEvents = float(csvRow['weightedEvents'])
      self.lumi = 1. if self.is_data else self.weightedEvents/self.xsection
      self.mainsel_versions = list()

      # init mainsel_versions
      if self.nickName.startswith('WJets'):
         self.mainsel_versions.append(self.nickName.replace('WJets', 'WJetsHeavy'))
         self.mainsel_versions.append(self.nickName.replace('WJets', 'WJetsLight'))
      elif self.nickName.startswith('ST_tW'):
         decays = ['Had', 'Ele', 'Muo', 'Tau']
         for tDecay in decays:
            for wDecay in decays:
               self.mainsel_versions.append(self.nickName.replace('_T', '_TopTo'+tDecay+'_WTo'+wDecay+'_T'))
      else:
         self.mainsel_versions.append(self.nickName)


class xmlCreator:

   '''Creates XML files for SFrame'''

   def __init__(self, selection: str, year: str, channel: str):

      confCon = configContainer()
      self.uhh2Dir = confCon.uhh2Dir
      self.userName = confCon.userName
      self.userMail = confCon.userMail
      self.yearVars = confCon.yearVars
      self.sample_list = confCon.used_samples[year][channel]

      if selection not in ['presel', 'mainsel']:
         sys.exit('Given value of argument "selection" not valid. Abort.')
      self.selection = selection
      self.is_mainsel = True if selection=='mainsel' else False

      if year not in ['2016', '2017', '2018']:
         sys.exit('Given value of argument "year" not valid. Abort.')
      self.year = year
      self.yearVersion = self.yearVars['yearVersions'][year]

      if channel not in ['ele', 'muo']:
         sys.exit('Given value of argument "channel" not valid. Abort.')
      self.channel = channel
      self.is_muo = True if channel=='muo' else False

      self.outputDirBase = self.uhh2Dir+'HighPtSingleTop/output/'
      if not os.path.isdir(self.outputDirBase):
         sys.exit('Warning: Make sure to create output directory via "ln -s". Abort.')

      self.xmlFileName = '_'.join(['parsedConfigFile', self.selection, self.year, self.channel])+'.xml'
      self.xmlFilePathBase = self.uhh2Dir+'HighPtSingleTop/config/'+'_'.join(['config', self.selection, self.year, self.channel])+'/'
      os.makedirs(self.xmlFilePathBase, exist_ok=True)
      self.xmlFilePath = self.xmlFilePathBase+self.xmlFileName

   def write_xml(self):

      with open(self.xmlFilePath, 'w') as file:
         file.write('''<?xml version="1.0" encoding="UTF-8"?>\n''')
         file.write('''\n''')
         file.write('''<!DOCTYPE JobConfiguration PUBLIC "" "JobConfig.dtd"[\n''')
         file.write('''\n''')
         file.write('''<!ENTITY TargetLumi "'''+str(self.yearVars['targetLumis'][self.year])+'''">\n''')
         if self.is_mainsel:
            file.write('''<!ENTITY PRESELdir "'''+(self.outputDirBase+'presel/'+self.year+'/'+self.channel+'/NOMINAL/')+'''">\n''')
            file.write('''<!ENTITY PRESELfilename "uhh2.AnalysisModuleRunner">\n''')
         file.write('''<!ENTITY OUTPUTdir "'''+(self.outputDirBase+self.selection+'/'+self.year+'/'+self.channel+'/NOMINAL/')+'''">\n''')
         file.write('''<!ENTITY b_Cacheable "False">\n''')
         file.write('''<!ENTITY NEVT "-1">\n''')
         file.write('''<!ENTITY YEARsuffix "_'''+self.year+self.yearVersion+'''">\n''')
         file.write('''<!ENTITY PROOFdir "/nfs/dust/cms/user/'''+self.userName+'''/.proof2">\n''')
         file.write('''\n''')
         for s in self.sample_list:
            if self.is_mainsel:
               file.write('''<!ENTITY '''+s.nickName+''' "&PRESELdir;/&PRESELfilename;'''+('.DATA.' if s.is_data else '.MC.')+s.nickName+'''&YEARsuffix;.root">\n''')
            else:
               file.write('''<!ENTITY '''+s.nickName+''' SYSTEM "'''+self.uhh2Dir+'common/UHH2-datasets/'+s.xmlPath+'''">\n''')
         file.write('''\n''')
         file.write(''']>\n''')
         file.write('''\n''')
         file.write('''<!--\n''')
         file.write('''<ConfigParse NEventsBreak="'''+('200000' if self.is_mainsel else '0')+'''" FileSplit="'''+('0' if self.is_mainsel else self.yearVars['preselFileSplit'][self.year])+'''" AutoResubmit="5"/>\n''')
         file.write('''<ConfigSGE RAM="4" DISK="3" Mail="'''+self.userMail+'''" Notification="as" Workdir="'''+'''_'''.join(['workdir', self.selection, self.year, self.channel])+'''"/>\n''')
         file.write('''-->\n''')
         file.write('''\n''')
         file.write('''<!-- OutputLevel controls which messages are printed; set to VERBOSE or DEBUG for more verbosity, to WARNING or ERROR for less -->\n''')
         file.write('''<JobConfiguration JobName="ExampleCycleJob" OutputLevel="INFO">\n''')
         file.write('''<Library Name="libSUHH2HighPtSingleTop"/>\n''')
         file.write('''<Package Name="SUHH2HighPtSingleTop.par"/>\n''')
         file.write('''<Cycle Name="uhh2::AnalysisModuleRunner" OutputDirectory="&OUTPUTdir;/" PostFix="" TargetLumi="&TargetLumi;">\n''')
         file.write('''\n''')
         for s in self.sample_list:
            if self.is_mainsel:
               for v in s.mainsel_versions:
                  file.write('''<InputData Lumi="'''+str(s.lumi)+'''" NEventsMax="&NEVT;" Type="'''+('DATA' if s.is_data else 'MC')+'''" Version="'''+v+'''&YEARsuffix;" Cacheable="&b_Cacheable;"> <In FileName="&'''+s.nickName+''';" Lumi="0.0"/> <InputTree Name="AnalysisTree"/> <OutputTree Name="AnalysisTree"/> </InputData>\n''')
            else:
               file.write('''<InputData Lumi="'''+str(s.lumi)+'''" NEventsMax="&NEVT;" Type="'''+('DATA' if s.is_data else 'MC')+'''" Version="'''+s.nickName+'''&YEARsuffix;" Cacheable="&b_Cacheable;"> &'''+s.nickName+'''; <InputTree Name="AnalysisTree"/> <OutputTree Name="AnalysisTree"/> </InputData>\n''')
         file.write('''\n''')
         file.write('''<UserConfig>\n''')
         file.write('''\n''')
         file.write('''<Item Name="PrimaryVertexCollection" Value="offlineSlimmedPrimaryVertices"/>\n''')
         file.write('''<Item Name="METName" Value="slimmedMETs"/>\n''')
         file.write('''<Item Name="ElectronCollection" Value="slimmedElectronsUSER"/>\n''')
         file.write('''<Item Name="MuonCollection" Value="slimmedMuonsUSER"/>\n''')
         file.write('''<Item Name="JetCollection" Value="jetsAk4CHS"/>\n''')
         file.write('''<Item Name="GenJetCollection" Value="slimmedGenJets"/>\n''')
         file.write('''<Item Name="TopJetCollection" Value="hotvrPuppi"/>\n''')
         file.write('''<Item Name="GenTopJetCollection" Value="hotvrGen"/>\n''')
         file.write('''<Item Name="GenParticleCollection" Value="GenParticles"/>\n''')
         file.write('''<Item Name="GenInfoName" Value="genInfo"/>\n''')
         file.write('''<Item Name="additionalBranches" Value="jetsAk8Puppi jetsAk8CHS jetsAk8PuppiSubstructure_SoftDropPuppi jetsAk8CHSSubstructure_SoftDropCHS genjetsAk8SubstructureSoftDrop genjetsAk8Substructure"/>\n''')
         if self.is_mainsel:
            file.write('''<Item Name="Ak8recCollection" Value="jetsAk8PuppiSubstructure_SoftDropPuppi"/>\n''')
            file.write('''<Item Name="Ak8genCollection" Value="genjetsAk8SubstructureSoftDrop"/>\n''')
         file.write('''\n''')
         file.write('''<Item Name="lumi_file" Value="'''+self.yearVars['lumiFiles'][self.year]+'''"/>\n''')
         file.write('''<Item Name="lumihists_lumi_per_bin" Value="500."/>\n''')
         file.write('''\n''')
         file.write('''<Item Name="pileup_directory" Value="'''+self.uhh2Dir+'common/data/'+self.year+'''/MyMCPileupHistogram.root"/>\n''')
         file.write('''<Item Name="pileup_directory_data" Value="'''+self.uhh2Dir+'common/data/'+self.year+'''/MyDataPileupHistogram'''+self.year+'''.root"/>\n''')
         if self.is_mainsel:
            file.write('''<Item Name="pileup_directory_data_up" Value="'''+self.uhh2Dir+'common/data/'+self.year+'''/MyDataPileupHistogram'''+self.year+'''_72383.root"/>\n''')
            file.write('''<Item Name="pileup_directory_data_down" Value="'''+self.uhh2Dir+'common/data/'+self.year+'''/MyDataPileupHistogram'''+self.year+'''_66017.root"/>\n''')
            file.write('''\n''')
            file.write('''<Item Name="HOTVRTopTagSFs" Value="'''+self.yearVars['hotvrSFFiles'][self.year]+'''"/>\n''')
            file.write('''<Item Name="DeepAK8WTagSFs" Value="'''+self.uhh2Dir+'''HighPtSingleTop/data/ScaleFactors/201X/DeepAK8V2_Top_W_SFs.csv"/>\n''')
            file.write('''<Item Name="BTagCalibration" Value="'''+self.yearVars['deepjetSFFiles'][self.year]+'''"/>\n''')
            file.write('''\n''')
            file.write('''<Item Name="NeuralNetFile_tTag" Value="'''+self.yearVars['NNFiles']['tTag'][self.year]+'''"/>\n''')
            file.write('''<Item Name="NeuralNetFile_WTag" Value="'''+self.yearVars['NNFiles']['WTag'][self.year]+'''"/>\n''')
            file.write('''\n''')
            file.write('''<!-- Keys for systematic uncertainties -->\n''')
            file.write('''<Item Name="ScaleVariationMuR" Value="none"/>\n''')
            file.write('''<Item Name="ScaleVariationMuF" Value="none"/>\n''')
            file.write('''<Item Name="SystDirection_Pileup" Value="nominal"/>\n''')
            file.write('''<Item Name="SystDirection_Prefiring" Value="nominal"/>\n''')
            if self.is_muo:
               file.write('''<Item Name="SystDirection_MuonTrigger" Value="nominal"/>\n''')
               file.write('''<Item Name="SystDirection_MuonId" Value="nominal"/>\n''')
               file.write('''<Item Name="SystDirection_MuonIso" Value="nominal"/>\n''')
            else:
               file.write('''<Item Name="SystDirection_ElectronTrigger" Value="nominal"/>\n''')
               file.write('''<Item Name="SystDirection_ElectronId" Value="nominal"/>\n''')
               file.write('''<Item Name="SystDirection_ElectronReco" Value="nominal"/>\n''')
            file.write('''<Item Name="SystDirection_HOTVRTopTagSF" Value="nominal"/>\n''')
            file.write('''<Item Name="SystDirection_DeepAK8WTagSF" Value="nominal"/>\n''')
            file.write('''<Item Name="SystDirection_DeepJetBTagSF" Value="nominal"/>\n''')
         file.write('''\n''')
         file.write('''<!-- Keys for JES/JER smearing. During preselection, AK4 (and HOTVR) jets are affected. During main selection, AK8 jets are affected. -->\n''')
         file.write('''<Item Name="jecsmear_direction" Value="nominal"/>\n''')
         file.write('''<Item Name="jersmear_direction" Value="nominal"/>\n''')
         file.write('''\n''')
         file.write('''<!-- Tell AnalysisModuleRunner NOT to use the MC event weight from SFrame; rather let MCLumiWeight (called via CommonModules) calculate the MC event weight. The MC event weight assigned by MCLumiWeight is InputData.Lumi / Cycle.TargetLumi. -->\n''')
         file.write('''<Item Name="use_sframe_weight" Value="false"/>\n''')
         file.write('''<Item Name="AnalysisModule" Value="'''+('HighPtSingleTopMainSelectionModule' if self.is_mainsel else 'HighPtSingleTopPreSelectionModule')+'''"/>\n''')
         file.write('''<Item Name="analysis_channel" Value="'''+self.channel+'''"/>\n''')
         file.write('''<Item Name="uhh2Dir" Value="'''+self.uhh2Dir+'''"/>\n''')
         file.write('''<Item Name="EmptyOutputTree" Value="false"/>\n''')
         file.write('''\n''')
         file.write('''<!-- Switch for debugging of the central AnalysisModule -->\n''')
         file.write('''<Item Name="Debug" Value="false"/>\n''')
         file.write('''\n''')
         file.write('''</UserConfig>\n''')
         file.write('''\n''')
         file.write('''</Cycle>\n''')
         file.write('''</JobConfiguration>\n''')

      return self.xmlFilePath

   def delete_xml(self):

      os.remove(self.xmlFilePath)


if __name__=='__main__':

   selections = ['presel', 'mainsel']
   years = ['2016', '2017', '2018']
   channels = ['ele', 'muo']

   parser = argparse.ArgumentParser()
   parser.add_argument('--all', action='store_true', help='Create XML files for all selections, years, and channels.')
   parser.add_argument('--syst', action='store_true', help='Create XML files for systematic uncertainties.')
   parser.add_argument('-s', '--selections', choices=selections, nargs='*', default=[])
   parser.add_argument('-y', '--years', choices=years, nargs='*', default=[])
   parser.add_argument('-c', '--channels', choices=channels, nargs='*', default=[])
   args = parser.parse_args(sys.argv[1:])

   if(args.all == True):
      if(len(args.selections) + len(args.years) + len(args.channels) != 0):
         sys.exit('Not allowed to use "--all" option jointly with manually given selection, year, or channel argument. Exit.')
      args.selections = selections
      args.years = years
      args.channels = channels
   else:
      for p in permutations([args.selections, args.years, args.channels]):
         if p[0] and (not p[1] or not p[2]):
            sys.exit('You specified arguments for at least one of the three options: "--selections", "--years", "--channels", but not for all three of them. Exit.')

   print('Going to create XML files for:')
   print('  Selections: '+', '.join(str(x) for x in args.selections))
   print('  Years: '+', '.join(str(x) for x in args.years))
   print('  Channels: '+', '.join(str(x) for x in args.channels))

   configContainer.read_database(args.years, args.channels)

   for selection in args.selections:
      for year in args.years:
         for channel in args.channels:
            x = xmlCreator(selection, year, channel)
            xmlFileName_nominal = x.write_xml()
            print('Created '+xmlFileName_nominal)
            if args.syst:
               print('Still need to do systematics.')
            # x.write_systematics_files with help of ElementTree
