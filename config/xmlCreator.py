#!/usr/bin/env python3

import os
import sys
import csv
from collections import OrderedDict
import argparse
from itertools import permutations


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

      # Set these values such that there are no more than 2,500 jobs per preselection. This way, you can submit two preselections in parallel to avoid going over 5,000 jobs (current user limit for NAF)
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
            # '2016': self.uhh2Dir+'HighPtSingleTop/data/KerasDNNModels/ttag_dnn_20-12-02-05-00-43/neural_net.json', #binary
            # '2017': self.uhh2Dir+'HighPtSingleTop/data/KerasDNNModels/ttag_dnn_20-12-02-05-00-43/neural_net.json',
            # '2018': self.uhh2Dir+'HighPtSingleTop/data/KerasDNNModels/ttag_dnn_20-12-02-05-00-43/neural_net.json',
            # '2016': self.uhh2Dir+'HighPtSingleTop/data/KerasDNNModels/ttag_dnn_21-01-20-12-59-32/neural_net.json', #multiclass 6 nodes
            # '2017': self.uhh2Dir+'HighPtSingleTop/data/KerasDNNModels/ttag_dnn_21-01-20-12-59-32/neural_net.json',
            # '2018': self.uhh2Dir+'HighPtSingleTop/data/KerasDNNModels/ttag_dnn_21-01-20-12-59-32/neural_net.json',
            '2016': self.uhh2Dir+'HighPtSingleTop/data/KerasDNNModels/ttag_dnn_21-02-01-19-51-16/neural_net.json', #multiclass 3 nodes
            '2017': self.uhh2Dir+'HighPtSingleTop/data/KerasDNNModels/ttag_dnn_21-02-01-19-51-16/neural_net.json',
            '2018': self.uhh2Dir+'HighPtSingleTop/data/KerasDNNModels/ttag_dnn_21-02-01-19-51-16/neural_net.json',
         },
         'WTag': {
            # '2016': self.uhh2Dir+'HighPtSingleTop/data/KerasDNNModels/wtag_dnn_20-12-02-05-01-44/neural_net.json', #binary
            # '2017': self.uhh2Dir+'HighPtSingleTop/data/KerasDNNModels/wtag_dnn_20-12-02-05-01-44/neural_net.json',
            # '2018': self.uhh2Dir+'HighPtSingleTop/data/KerasDNNModels/wtag_dnn_20-12-02-05-01-44/neural_net.json',
            # '2016': self.uhh2Dir+'HighPtSingleTop/data/KerasDNNModels/wtag_dnn_21-01-20-12-58-02/neural_net.json', #multiclass 6 nodes
            # '2017': self.uhh2Dir+'HighPtSingleTop/data/KerasDNNModels/wtag_dnn_21-01-20-12-58-02/neural_net.json',
            # '2018': self.uhh2Dir+'HighPtSingleTop/data/KerasDNNModels/wtag_dnn_21-01-20-12-58-02/neural_net.json',
            '2016': self.uhh2Dir+'HighPtSingleTop/data/KerasDNNModels/wtag_dnn_21-02-01-19-52-16/neural_net.json', #multiclass 3 nodes
            '2017': self.uhh2Dir+'HighPtSingleTop/data/KerasDNNModels/wtag_dnn_21-02-01-19-52-16/neural_net.json',
            '2018': self.uhh2Dir+'HighPtSingleTop/data/KerasDNNModels/wtag_dnn_21-02-01-19-52-16/neural_net.json',
         },
      }

      self.systematics = list()


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


   def setup_systematics(self, selection: str, year: str, channel: str):

      self.systematics.append(systEntity('jec', 'jecsmear_direction')) # During presel, ak4 jets (and thus, hotvr subjets) are affected. During mainsel, ak8 jets are affected.
      self.systematics.append(systEntity('jer', 'jersmear_direction')) # During presel, ak4 jets (and thus, hotvr subjets) are affected. During mainsel, ak8 jets are affected.
      if selection=='mainsel':
         self.systematics.append(systEntity('mur', 'ScaleVariationMuR'))
         self.systematics.append(systEntity('muf', 'ScaleVariationMuF'))
         self.systematics.append(systEntity('murmuf', 'N/A')) # Need to access ScaleVariationMuR and ScaleVariationMuF in another way
         self.systematics.append(systEntity('pileup', 'SystDirection_Pileup'))
         if year in ['2016', '2017']:
            self.systematics.append(systEntity('prefiring', 'SystDirection_Prefiring'))
         if channel=='muo':
            self.systematics.append(systEntity('muontrigger', 'SystDirection_MuonTrigger'))
            self.systematics.append(systEntity('muonid', 'SystDirection_MuonId'))
            self.systematics.append(systEntity('muoniso', 'SystDirection_MuonIso'))
         elif channel=='ele':
            self.systematics.append(systEntity('electrontrigger', 'SystDirection_ElectronTrigger'))
            self.systematics.append(systEntity('electronid', 'SystDirection_ElectronId'))
            self.systematics.append(systEntity('electronreco', 'SystDirection_ElectronReco'))
         self.systematics.append(systEntity('hotvr', 'SystDirection_HOTVRTopTagSF', directions=['merged_up', 'merged_down', 'semimerged_up', 'semimerged_down', 'notmerged_up', 'notmerged_down']))
         self.systematics.append(systEntity('deepak8', 'SystDirection_DeepAK8WTagSF'))
         # self.systematics.append(systEntity('deepjet', 'SystDirection_DeepJetBTagSF')) # TODO: Check how to properly vary deepjet shapes


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
      if self.nickName.startswith('ST_tW'):
          self.mainsel_versions.append(self.nickName.replace('_T', '_Sig_T'))
          self.mainsel_versions.append(self.nickName.replace('_T', '_Bkg_T'))
      # if self.nickName.startswith('WJets'):
      #    self.mainsel_versions.append(self.nickName.replace('WJets', 'WJetsHeavy'))
      #    self.mainsel_versions.append(self.nickName.replace('WJets', 'WJetsLight'))
      # elif self.nickName.startswith('ST_tW'):
      #    decays = ['Had', 'Ele', 'Muo', 'Tau']
      #    for tDecay in decays:
      #       for wDecay in decays:
      #          self.mainsel_versions.append(self.nickName.replace('_T', '_TopTo'+tDecay+'_WTo'+wDecay+'_T'))
      else:
          self.mainsel_versions.append(self.nickName)


class systEntity:

   '''Container to hold information about a systematic uncertainty'''

   def __init__(self, shortName: str, ctxName: str, defaultValue='nominal', directions=['up', 'down']):

      self.shortName = shortName
      self.ctxName = ctxName
      self.defaultValue = defaultValue
      self.directions = directions


class xmlCreator:

   '''Creates XML files for SFrame'''

   def __init__(self, selection: str, year: str, channel: str):

      confCon = configContainer()
      self.uhh2Dir = confCon.uhh2Dir
      self.userName = confCon.userName
      self.userMail = confCon.userMail
      self.yearVars = confCon.yearVars
      self.sample_list = confCon.used_samples[year][channel]
      confCon.setup_systematics(selection, year, channel)
      self.systematics = confCon.systematics

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

      self.write_xml_successful = False
      self.systXmlFilePaths = list()
      self.qcdXmlFilePath = None

      self.force_empty_output_tree = False


   def force_eot(self, eot: bool):

      self.force_empty_output_tree = eot


   def write_xml(self):

      with open(self.xmlFilePath, 'w') as file:
         file.write('''<?xml version="1.0" encoding="UTF-8"?>\n''')
         file.write('''\n''')
         file.write('''<!DOCTYPE JobConfiguration PUBLIC "" "JobConfig.dtd"[\n''')
         file.write('''\n''')
         file.write('''<!ENTITY TargetLumi "'''+str(self.yearVars['targetLumis'][self.year])+'''">\n''')
         if self.is_mainsel:
            file.write('''<!ENTITY PRESELdir "'''+(self.outputDirBase+'presel/'+self.year+'/'+self.channel+'/nominal/')+'''">\n''')
            file.write('''<!ENTITY PRESELfilename "uhh2.AnalysisModuleRunner">\n''')
         file.write('''<!ENTITY OUTPUTdir "'''+(self.outputDirBase+self.selection+'/'+self.year+'/'+self.channel+'/nominal/')+'''">\n''')
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
         file.write('''<Item Name="additionalBranches" Value="jetsAk8PuppiSubstructure_SoftDropPuppi genjetsAk8SubstructureSoftDrop"/>\n''')
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
            file.write('''<Item Name="DeepAK8WTagSFs" Value="'''+self.uhh2Dir+'''HighPtSingleTop/data/ScaleFactors/201X/DeepAK8V2_Top_W_SFs.csv.root"/>\n''')
            file.write('''<Item Name="BTagCalibration" Value="'''+self.yearVars['deepjetSFFiles'][self.year]+'''"/>\n''')
            file.write('''\n''')
            file.write('''<Item Name="NeuralNetFile_tTag" Value="'''+self.yearVars['NNFiles']['tTag'][self.year]+'''"/>\n''')
            file.write('''<Item Name="NeuralNetFile_WTag" Value="'''+self.yearVars['NNFiles']['WTag'][self.year]+'''"/>\n''')
         file.write('''\n''')
         file.write('''<!-- Keys for systematic uncertainties -->\n''')
         for syst in self.systematics:
            if syst.shortName == 'murmuf': continue
            file.write('''<Item Name="'''+syst.ctxName+'''" Value="'''+syst.defaultValue+'''"/>\n''')
         file.write('''\n''')
         file.write('''<!-- Tell AnalysisModuleRunner NOT to use the MC event weight from SFrame; rather let MCLumiWeight (called via CommonModules) calculate the MC event weight. The MC event weight assigned by MCLumiWeight is InputData.Lumi / Cycle.TargetLumi. -->\n''')
         file.write('''<Item Name="use_sframe_weight" Value="false"/>\n''')
         file.write('''<Item Name="AnalysisModule" Value="'''+('HighPtSingleTopMainSelectionModule' if self.is_mainsel else 'HighPtSingleTopPreSelectionModule')+'''"/>\n''')
         file.write('''<Item Name="analysis_channel" Value="'''+self.channel+'''"/>\n''')
         file.write('''<Item Name="QCD_sideband" Value="false"/>\n''')
         file.write('''<Item Name="uhh2Dir" Value="'''+self.uhh2Dir+'''"/>\n''')
         file.write('''<Item Name="EmptyOutputTree" Value="'''+('false' if not self.is_mainsel or self.force_empty_output_tree == False else 'true')+'''"/>\n''')
         file.write('''\n''')
         file.write('''<!-- Switch for debugging of the central AnalysisModule -->\n''')
         file.write('''<Item Name="Debug" Value="false"/>\n''')
         file.write('''\n''')
         file.write('''</UserConfig>\n''')
         file.write('''\n''')
         file.write('''</Cycle>\n''')
         file.write('''</JobConfiguration>\n''')

      self.write_xml_successful = True

      print('Created '+self.xmlFilePath)

      return self.xmlFilePath


   def write_systematics_xml(self, syst: systEntity):

      if not self.write_xml_successful:
         sys.exit('xmlCreator::write_xml() not called. Danger of parsing potentially outdated XML file. Exit.')

      for direction in syst.directions:
         systXmlFilePath = self.xmlFilePath.replace('.xml', '_')+'_'.join(['syst', syst.shortName, direction])+'.xml'
         infile = open(self.xmlFilePath, 'r')
         with open(systXmlFilePath, 'w') as outfile:
            for line in infile:
               newline = line
               # if newline.startswith('<!ENTITY PRESELdir') or newline.startswith('<!ENTITY OUTPUTdir'):
                  # newline = newline.replace('/nominal/', '/'+'_'.join(['syst', syst.shortName, direction])+'/')
               if newline.startswith('<!ENTITY PRESELdir') and (syst.shortName == 'jec' or syst.shortName == 'jer'):
                  newline = newline.replace('/nominal/', '/'+'_'.join(['syst', syst.shortName, direction])+'/')
               if newline.startswith('<!ENTITY OUTPUTdir'):
                  newline = newline.replace('/nominal/', '/'+'_'.join(['syst', syst.shortName, direction])+'/')
               if newline.startswith('<ConfigSGE'):
                  newline = newline.replace('"/>', '_'+'_'.join(['syst', syst.shortName, direction])+'"/>')
               if syst.shortName != 'murmuf' and newline.startswith('<Item Name="'+syst.ctxName):
                  newline = newline.replace(syst.defaultValue, direction)
               if syst.shortName == 'murmuf' and newline.startswith('<Item Name="ScaleVariationMu'):
                  newline = newline.replace(syst.defaultValue, direction)
               if self.is_mainsel and newline.startswith('<Item Name="EmptyOutputTree"'):
                  newline = newline.replace('false', 'true') # Don't save AnalysisTree for mainsel systematics
               outfile.write(newline)
         infile.close()

         print('Created '+systXmlFilePath)

         self.systXmlFilePaths.append(systXmlFilePath)


   def write_all_systematics_xmls(self):

      for syst in self.systematics:

         self.write_systematics_xml(syst)

      return self.systXmlFilePaths


   def write_qcd_sideband_xml(self):

      if not self.write_xml_successful:
         sys.exit('xmlCreator::write_xml() not called. Danger of parsing potentially outdated XML file. Exit.')

      qcdXmlFilePath = self.xmlFilePath.replace('.xml', '_')+'QCDsideband.xml'
      infile = open(self.xmlFilePath, 'r')
      with open(qcdXmlFilePath, 'w') as outfile:
         for line in infile:
            newline = line
            if newline.startswith('<!ENTITY PRESELdir') or newline.startswith('<!ENTITY OUTPUTdir'):
               newline = newline.replace('/nominal/', '/QCDsideband/')
            if newline.startswith('<ConfigSGE'):
               newline = newline.replace('"/>', '_QCDsideband"/>')
            if newline.startswith('<Item Name="QCD_sideband"'):
               newline = newline.replace('false', 'true')
            if self.is_mainsel and newline.startswith('<Item Name="EmptyOutputTree"'):
               newline = newline.replace('false', 'true') # Don't save AnalysisTree for mainsel qcd sideband
            outfile.write(newline)
      infile.close()

      print('Created '+qcdXmlFilePath)

      self.qcdXmlFilePath = qcdXmlFilePath


if __name__=='__main__':

   selections = ['presel', 'mainsel']
   years = ['2016', '2017', '2018']
   channels = ['ele', 'muo']

   if not sys.argv[1:]: sys.exit('No arguments provided. Exit.')
   parser = argparse.ArgumentParser()
   parser.add_argument('--all', action='store_true', help='Create XML files for all selections, years, and channels.')
   parser.add_argument('--syst', action='store_true', help='Create XML files for systematic uncertainties.')
   parser.add_argument('-s', '--selections', choices=selections, nargs='*', default=[])
   parser.add_argument('-y', '--years', choices=years, nargs='*', default=[])
   parser.add_argument('-c', '--channels', choices=channels, nargs='*', default=[])
   parser.add_argument('-a', '--auto-complete', action='store_true', help='Auto-complete arguments if not all arguments for selections, years, and channels are given. E.g. if only selections and channels are given, assume that you want to create XML files for all years.')
   parser.add_argument('--eot', action='store_true', help='Force empty output trees (saves disk space and time during further processing of root files). Caveat: When preparing root files for DNN training, output trees need to be filled.')
   args = parser.parse_args(sys.argv[1:])

   if(args.all == True):
      if(len(args.selections) + len(args.years) + len(args.channels) != 0):
         sys.exit('Not allowed to use "--all" option jointly with manually given selection, year, or channel argument. Exit.')
      args.selections = selections
      args.years = years
      args.channels = channels
      if(args.auto_complete):
         print('Warning: You already specified "--all". Therefore, "--auto-complete" will not have any effect.')
   else:
      if args.auto_complete:
         print('Auto-completing arguments.')
         if not args.selections: args.selections = selections
         if not args.years: args.years = years
         if not args.channels: args.channels = channels
      else:
         for p in permutations([args.selections, args.years, args.channels]):
            if p[0] and (not p[1] or not p[2]):
               sys.exit('You specified arguments for at least one of the three options: "--selections", "--years", "--channels", but not for all three of them. Also, you did not specify "--auto-complete" to compensate for this. Exit.')

   print('Going to create XML files for:')
   print('  Selections: '+', '.join(str(x) for x in args.selections))
   print('  Years: '+', '.join(str(x) for x in args.years))
   print('  Channels: '+', '.join(str(x) for x in args.channels))

   configContainer.read_database(args.years, args.channels)

   for selection in args.selections:
      for year in args.years:
         for channel in args.channels:
            x = xmlCreator(selection, year, channel)
            x.force_eot(args.eot)
            x.write_xml()
            # x.write_qcd_sideband_xml()
            if args.syst:
               x.write_all_systematics_xmls()
