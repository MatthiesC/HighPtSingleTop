#!/usr/bin/env python3

import os
import sys
import csv
from collections import OrderedDict
import argparse
from itertools import permutations
from termcolor import colored
import subprocess

# to include CrossSectionHelper:
sys.path.append(os.path.join(os.environ.get('CMSSW_BASE'), 'src/UHH2/common/UHH2-datasets'))
from CrossSectionHelper import MCSampleValuesHelper
helper = MCSampleValuesHelper()

sys.path.append(os.path.join(os.environ.get('CMSSW_BASE'), 'src/UHH2/LegacyTopTagging/Analysis'))
from constants import _YEARS, _JECSMEAR_SOURCES

sys.path.append(os.path.join(os.environ.get('CMSSW_BASE'), 'src/UHH2/LegacyTopTagging/config'))
from database import samplesDict


def check_consistency(selection, year, channel=None):

   if selection == 'presel':
      if channel != None: raise ValueError('Preselection has no channel argument!')
   elif selection == 'mainsel':
      if channel == None: raise ValueError('Mainselection requires channel argument!')
   else:
      raise ValueError('Selection argument must be "mainsel" or "presel"')


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
         'UL16preVFP': '',
         'UL16postVFP': '',
         'UL17': '',
         'UL18': '',
      }

      # Set these values such that there are no more than 2,500 jobs per preselection. This way, you can submit two preselections in parallel to avoid going over 5,000 jobs (current user limit for NAF)
      self.yearVars['preselFileSplit'] = {
         'UL16preVFP': '20',
         'UL16postVFP': '20',
         'UL17': '20',
         'UL18': '20',
      }

      self.yearVars['targetLumis'] = {
         'UL16preVFP': _YEARS['UL16preVFP'].get('lumi_pb'),
         'UL16postVFP': _YEARS['UL16postVFP'].get('lumi_pb'),
         'UL17': _YEARS['UL17'].get('lumi_pb'),
         'UL18': _YEARS['UL18'].get('lumi_pb'),
      }

      self.yearVars['lumiFiles'] = {
         # '2016': self.uhh2Dir+'common/data/2016/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.root',
         # '2017': self.uhh2Dir+'common/data/2017/Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON_v1.root',
         # '2018': self.uhh2Dir+'common/data/2018/Cert_314472-325175_13TeV_PromptReco_Collisions18_JSON.root',
         # '2016': self.uhh2Dir+'HighPtSingleTop/data/GoldenJSONs/Cert_271036-284044_13TeV_ReReco_07Aug2017_Collisions16_JSON.root',
         # '2017': self.uhh2Dir+'HighPtSingleTop/data/GoldenJSONs/Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON_v1.root',
         # '2018': self.uhh2Dir+'HighPtSingleTop/data/GoldenJSONs/Cert_314472-325175_13TeV_17SeptEarlyReReco2018ABC_PromptEraD_Collisions18_JSON.root',

         'UL16preVFP': self.uhh2Dir+'common/UHH2-data/UL16preVFP/Cert_271036-284044_13TeV_Legacy2016_Collisions16_JSON_UL16preVFP_normtag.root',
         'UL16postVFP': self.uhh2Dir+'common/UHH2-data/UL16postVFP/Cert_271036-284044_13TeV_Legacy2016_Collisions16_JSON_UL16postVFP_normtag.root',
         'UL17': self.uhh2Dir+'common/UHH2-data/UL17/Cert_294927-306462_13TeV_UL2017_Collisions17_GoldenJSON_normtag.root',
         'UL18': self.uhh2Dir+'common/UHH2-data/UL18/Cert_314472-325175_13TeV_Legacy2018_Collisions18_JSON_normtag.root',
      }

      self.yearVars['pileupFiles'] = {
         'mc': {
            'UL16preVFP': self.uhh2Dir+'common/UHH2-data/UL16preVFP/MyMCPileupHistogram_UL16preVFP.root',
            'UL16postVFP': self.uhh2Dir+'common/UHH2-data/UL16postVFP/MyMCPileupHistogram_UL16postVFP.root',
            'UL17': self.uhh2Dir+'common/UHH2-data/UL17/MyMCPileupHistogram_UL17.root',
            'UL18': self.uhh2Dir+'common/UHH2-data/UL18/MyMCPileupHistogram_UL18.root',
         },
         'data': {
            'UL16preVFP': self.uhh2Dir+'common/UHH2-data/UL16preVFP/MyDataPileupHistogram_UL16preVFP.root',
            'UL16postVFP': self.uhh2Dir+'common/UHH2-data/UL16postVFP/MyDataPileupHistogram_UL16postVFP.root',
            'UL17': self.uhh2Dir+'common/UHH2-data/UL17/MyDataPileupHistogram_UL17.root',
            'UL18': self.uhh2Dir+'common/UHH2-data/UL18/MyDataPileupHistogram_UL18.root',
         },
         'dataUp': {
            'UL16preVFP': self.uhh2Dir+'common/UHH2-data/UL16preVFP/MyDataPileupHistogram_UL16preVFP_72383.root',
            'UL16postVFP': self.uhh2Dir+'common/UHH2-data/UL16postVFP/MyDataPileupHistogram_UL16postVFP_72383.root',
            'UL17': self.uhh2Dir+'common/UHH2-data/UL17/MyDataPileupHistogram_UL17_72383.root',
            'UL18': self.uhh2Dir+'common/UHH2-data/UL18/MyDataPileupHistogram_UL18_72383.root',
         },
         'dataDown': {
            'UL16preVFP': self.uhh2Dir+'common/UHH2-data/UL16preVFP/MyDataPileupHistogram_UL16preVFP_66017.root',
            'UL16postVFP': self.uhh2Dir+'common/UHH2-data/UL16postVFP/MyDataPileupHistogram_UL16postVFP_66017.root',
            'UL17': self.uhh2Dir+'common/UHH2-data/UL17/MyDataPileupHistogram_UL17_66017.root',
            'UL18': self.uhh2Dir+'common/UHH2-data/UL18/MyDataPileupHistogram_UL18_66017.root',
         },
      }

      self.yearVars['hotvrSFFiles'] = {
         # '2016': self.uhh2Dir+'HOTVR/data/2016TopTaggingScaleFactors.root',
         # '2017': self.uhh2Dir+'HOTVR/data/2017TopTaggingScaleFactors.root',
         # '2018': self.uhh2Dir+'HOTVR/data/2018TopTaggingScaleFactors.root',
      }

      self.yearVars['btagalgo'] = {
         # '2016': 'DeepCSV',
         # '2017': 'DeepCSV',
         # '2018': 'DeepCSV',
      }

      self.yearVars['deepjetSFFiles'] = {
         # '2016': self.uhh2Dir+'HighPtSingleTop/data/ScaleFactors/2016/DeepJet_2016LegacySF_V1__reduced.csv', # if using CP5 samples, need to use other sf file
         # '2017': self.uhh2Dir+'HighPtSingleTop/data/ScaleFactors/2017/DeepFlavour_94XSF_V4_B_F__reduced.csv',
         # '2018': self.uhh2Dir+'HighPtSingleTop/data/ScaleFactors/2018/DeepJet_102XSF_V2__reduced.csv',
      }

      self.yearVars['deepcsvSFFiles'] = {
         # '2016': self.uhh2Dir+'HighPtSingleTop/data/ScaleFactors/2016/DeepCSV_2016LegacySF_V1__reduced.csv', # if using CP5 samples, need to use other sf file
         # '2017': self.uhh2Dir+'HighPtSingleTop/data/ScaleFactors/2017/DeepCSV_94XSF_V5_B_F__reduced.csv',
         # '2018': self.uhh2Dir+'HighPtSingleTop/data/ScaleFactors/2018/DeepCSV_102XSF_V2__reduced.csv',
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

   # @staticmethod
   # def read_database(years: list, channels: list):
   #
   #    used_samples = OrderedDict()
   #    for year in years:
   #       used_samples[year] = OrderedDict()
   #       for channel in channels:
   #          used_samples[year][channel] = list()
   #          with open('database.csv', 'r') as file:
   #             reader = csv.DictReader(file)
   #             for row in reader:
   #                use_me = row['use_me']=='True' and row['year']==year and (row['channel']==channel or row['channel']=='both')
   #                if use_me:
   #                   used_samples[year][channel].append(sampleEntity(row))
   #    configContainer.used_samples = used_samples
   #
   # @staticmethod
   # def read_database_106X_v2(years: list):
   #
   #    sys.path.append(os.path.join(os.environ.get('CMSSW_BASE'), 'src/UHH2/LegacyTopTagging/config'))
   #    from database import samplesDict
   #    used_samples = OrderedDict()
   #    for year in years:
   #       used_samples[year] = list()
   #       for k, v in samplesDict.items():
   #          use_me = v.get('years') == None or year in v.get('years', [])
   #          use_me = use_me and (v.get('analysis') == None or 'tw' in v.get('analysis'))
   #          if use_me:
   #             sample_entity = sampleEntity('106X_v2', (k, v, year,))
   #             if not os.path.isfile(sample_entity.xmlPath):
   #                print(colored('XML for sample  '+sample_entity.nickName+' ('+year+')  does not exist. Skipping this sample', 'red'))
   #                continue
   #             used_samples[year].append(sample_entity)
   #    configContainer.used_samples = used_samples
   #
   # def setup_systematics(self, selection: str, year: str, channel: str):
   #
   #    check_consistency(selection, year, channel)
   #
   #    self.systematics = list()
   #
   #    self.systematics.append(systEntity('jec', 'jecsmear_direction')) # During presel, ak4 jets (and thus, hotvr subjets) are affected. During mainsel, ak8 jets are affected.
   #    self.systematics.append(systEntity('jer', 'jersmear_direction')) # During presel, ak4 jets (and thus, hotvr subjets) are affected. During mainsel, ak8 jets are affected.
   #    if selection=='mainsel':
   #       self.systematics.append(systEntity('mur', 'ScaleVariationMuR'))
   #       self.systematics.append(systEntity('muf', 'ScaleVariationMuF'))
   #       self.systematics.append(systEntity('murmuf', 'N/A')) # Need to access ScaleVariationMuR and ScaleVariationMuF in another way
   #       self.systematics.append(systEntity('pileup', 'SystDirection_PileUp'))
   #       if year != '2018':
   #          self.systematics.append(systEntity('prefiring', 'SystDirection_Prefiring'))
   #       if channel=='muo':
   #          self.systematics.append(systEntity('muontrigger', 'SystDirection_MuonTrigger'))
   #          self.systematics.append(systEntity('muonid', 'SystDirection_MuonId'))
   #          self.systematics.append(systEntity('muoniso', 'SystDirection_MuonIso'))
   #       elif channel=='ele':
   #          self.systematics.append(systEntity('electrontrigger', 'SystDirection_ElectronTrigger'))
   #          self.systematics.append(systEntity('electronid', 'SystDirection_ElectronId'))
   #          self.systematics.append(systEntity('electronreco', 'SystDirection_ElectronReco'))
   #       self.systematics.append(systEntity('hotvr', 'SystDirection_HOTVRTopTagSF', directions=['merged_up', 'merged_down', 'semimerged_up', 'semimerged_down', 'notmerged_up', 'notmerged_down']))
   #       self.systematics.append(systEntity('deepak8', 'SystDirection_DeepAK8WTagSF'))
   #       # self.systematics.append(systEntity('btagging', 'SystDirection_BTagSF')) # TODO: Check how to properly vary b-tagging shapes


class sampleEntity:

   '''Container to hold information about a data or MC sample, as read from CSV database'''

   def __init__(self, ver, csvRow, extra_syst=None):

      if ver == '106X_v2':
         k, v, year = csvRow
         if extra_syst:
            db_name = v['extra_systs'][extra_syst]
         else:
            db_name = v['db_name']
         self.year = year
         self.channel = v.get('channel', ['ele', 'muo'])
         self.is_data = k.startswith('DATA_')
         self.nickName = k
         self.n_das = None
         self.n_pnfs = None
         self.pnfs_sum_of_weights = helper.get_nevt(db_name, '13TeV', self.year)
         self.xs_gen = None
         self.xs_theo = None
         self.lumi = 1. if self.is_data else helper.get_lumi(db_name, '13TeV', self.year, kFactor=v.get('kfac', False), Corrections=v.get('corr', False))
         self.xsection = self.pnfs_sum_of_weights / self.lumi
         self.xmlPath = os.path.join(os.environ.get('CMSSW_BASE'), 'src/UHH2/common/UHH2-datasets', helper.get_xml(db_name, '13TeV', self.year))
      else:
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
         self.mainsel_versions.append(self.nickName.replace('_T', '_dnnSig_T'))
         self.mainsel_versions.append(self.nickName.replace('_T', '_dnnBkg_T'))
         # self.mainsel_versions.append(self.nickName.replace('_T', '_trueSig_T'))
         # self.mainsel_versions.append(self.nickName.replace('_T', '_trueBkg_T'))

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
      self.jecsmear_sources = ['Total']
      if self.shortName == 'jes':
         for k in _JECSMEAR_SOURCES.keys():
            self.jecsmear_sources.append(k)


class xmlCreator:

   '''Creates XML files for SFrame'''

   # def __init__(self, selection: str, year: str, channel: str):
   def __init__(self, ignore_data=False, extra_syst=None):

      self.ignore_data = ignore_data
      if self.ignore_data:
         print(colored('Caveat: Ignoring DATA', 'blue'))

      self.extra_syst = extra_syst

      self.userName = os.environ.get('USER')
      self.uhh2Dir = os.path.join(os.environ.get('CMSSW_BASE'), 'src/UHH2')+'/'
      self.userMail = '@'.join([self.userName, 'mail.desy.de']) # Avoid spam due to public code on GitHub

      confCon = configContainer()
      self.yearVars = confCon.yearVars
      # confCon.setup_systematics(selection, year, channel)
      # self.systematics = confCon.systematics

      # if selection not in ['presel', 'mainsel']:
      #    sys.exit('Given value of argument "selection" not valid. Abort.')
      # self.selection = selection
      # self.is_mainsel = True if selection=='mainsel' else False

      # if year not in ['2016', '2017', '2018']:
      #    sys.exit('Given value of argument "year" not valid. Abort.')
      # self.year = year
      # self.yearVersion = self.yearVars['yearVersions'][year]

      # if channel not in ['ele', 'muo']:
      #    sys.exit('Given value of argument "channel" not valid. Abort.')
      # self.channel = channel
      # self.is_muo = True if channel=='muo' else False

      self.outputDirBase = os.path.join(self.uhh2Dir, 'HighPtSingleTop/output')
      if not os.path.isdir(self.outputDirBase):
         sys.exit('Warning: Make sure to create output directory via "ln -s". Abort.')
      self.outputDirBase = os.path.join(self.outputDirBase, 'Analysis')

      self.xmlFileNameBase = 'parsedConfigFile'

      # self.xmlFileName = '_'.join(['parsedConfigFile', self.selection, self.year, self.channel])+'.xml'
      # self.xmlFilePathBase = self.uhh2Dir+'HighPtSingleTop/config/'+'_'.join(['config', self.selection, self.year, self.channel])+'/'
      # os.makedirs(self.xmlFilePathBase, exist_ok=True)
      # self.xmlFilePath = self.xmlFilePathBase+self.xmlFileName

      # self.write_xml_successful = False
      # self.systXmlFilePaths = list()
      # self.qcdXmlFilePath = None

      self.written_xmls = dict()


   def read_database_106X_v2(self, years: list):

      self.used_samples = OrderedDict()
      for year in years:
         self.used_samples[year] = list()
         for k, v in samplesDict.items():
            use_me = v.get('years') == None or year in v.get('years', [])
            use_me = use_me and (v.get('analysis') == None or 'tw' in v.get('analysis'))
            if self.extra_syst:
               use_me = use_me and (v.get('extra_systs') != None and self.extra_syst in v.get('extra_systs').keys())
            # use_me = use_me and ('DYJetsToLL_HT100to' in k) # HACK
            if use_me:
               sample_entity = sampleEntity('106X_v2', (k, v, year,), self.extra_syst)
               if self.ignore_data and sample_entity.is_data: continue
               if not os.path.isfile(sample_entity.xmlPath):
                  print(colored('XML for sample  '+sample_entity.nickName+' ('+year+')  does not exist. Skipping this sample', 'red'))
                  continue
               self.used_samples[year].append(sample_entity)


   @staticmethod
   def get_systematics(selection, year, channel=None):

      check_consistency(selection, year, channel)

      systematics = OrderedDict()

      def add_syst_entity(dict: OrderedDict, syst: systEntity):
         dict[syst.shortName] = syst

      add_syst_entity(systematics, systEntity('jes', 'jecsmear_direction')) # During presel, ak4 jets (and thus, hotvr subjets) are affected. During mainsel, ak8 jets are affected.
      add_syst_entity(systematics, systEntity('jer', 'jersmear_direction')) # During presel, ak4 jets (and thus, hotvr subjets) are affected. During mainsel, ak8 jets are affected.
      if selection=='mainsel':
         add_syst_entity(systematics, systEntity('mur', 'ScaleVariationMuR'))
         add_syst_entity(systematics, systEntity('muf', 'ScaleVariationMuF'))
         add_syst_entity(systematics, systEntity('murmuf', 'N/A')) # Need to access ScaleVariationMuR and ScaleVariationMuF in another way
         add_syst_entity(systematics, systEntity('ps', 'SystDirection_PS', directions=['FSRup_2', 'FSRdown_2', 'ISRup_2', 'ISRdown_2']))
         add_syst_entity(systematics, systEntity('pileup', 'SystDirection_Pileup'))
         add_syst_entity(systematics, systEntity('prefiring', 'SystDirection_Prefiring'))
         add_syst_entity(systematics, systEntity('unclenergy', 'SystDirection_UnclusteredEnergy'))
         if channel=='muo':
            add_syst_entity(systematics, systEntity('muontrigger', 'SystDirection_MuonTrigger'))
            add_syst_entity(systematics, systEntity('muonid', 'SystDirection_MuonId'))
            add_syst_entity(systematics, systEntity('muoniso', 'SystDirection_MuonIso'))
         elif channel=='ele':
            # add_syst_entity(systematics, systEntity('electrigger', 'SystDirection_ElectronTrigger'))
            add_syst_entity(systematics, systEntity('elecid', 'SystDirection_ElectronId'))
            add_syst_entity(systematics, systEntity('elecreco', 'SystDirection_ElectronReco'))
         # add_syst_entity(systematics, systEntity('hotvr', 'SystDirection_HOTVRTopTagSF', directions=['merged_up', 'merged_down', 'semimerged_up', 'semimerged_down', 'notmerged_up', 'notmerged_down']))
         # add_syst_entity(systematics, systEntity('deepak8', 'SystDirection_DeepAK8WTagSF'))
         add_syst_entity(systematics, systEntity('btagging', 'SystDirection_BTaggingShape', directions=['cferr1_up', 'cferr1_down', 'cferr2_up', 'cferr2_down',
            'lf_up', 'lf_down', 'lfstats1_up', 'lfstats1_down', 'lfstats2_up', 'lfstats2_down',
            'hf_up', 'hf_down', 'hfstats1_up', 'hfstats1_down', 'hfstats2_up', 'hfstats2_down']))

      return systematics


   def write_xml(self, selection, year, channel=None):

      check_consistency(selection, year, channel)

      configDir = os.path.join(self.uhh2Dir, '_'.join(['HighPtSingleTop/config/config', selection, year]))+('_'+channel if channel else '')
      os.makedirs(configDir, exist_ok=True)
      xmlFilePath = os.path.join(configDir, '_'.join([self.xmlFileNameBase, selection, year])+('_'+channel if channel else '')+('_syst_'+self.extra_syst if self.extra_syst else '')+'.xml')
      is_mainsel = selection == 'mainsel'
      workdirName = '_'.join(['workdir', selection, year])+('_'+channel if channel else '')+('_syst_'+self.extra_syst if self.extra_syst else '')
      systematics = xmlCreator.get_systematics(selection, year, channel)

      with open(xmlFilePath, 'w') as file:
         file.write('''<?xml version="1.0" encoding="UTF-8"?>\n''')
         file.write('''\n''')
         file.write('''<!DOCTYPE JobConfiguration PUBLIC "" "JobConfig.dtd"[\n''')
         file.write('''\n''')
         file.write('''<!ENTITY TargetLumi "'''+str(self.yearVars['targetLumis'][year])+'''">\n''')
         if is_mainsel:
            file.write('''<!ENTITY PRESELdir "'''+os.path.join(self.outputDirBase, 'presel', year, 'syst_'+self.extra_syst if self.extra_syst else 'nominal')+'''">\n''')
            file.write('''<!ENTITY PRESELfilename "uhh2.AnalysisModuleRunner">\n''')
         file.write('''<!ENTITY OUTPUTdir "'''+os.path.join(self.outputDirBase, selection, year, channel if channel else '', 'syst_'+self.extra_syst if self.extra_syst else 'nominal')+'''">\n''')
         file.write('''<!ENTITY b_Cacheable "False">\n''')
         file.write('''<!ENTITY NEVT "-1">\n''')
         file.write('''<!ENTITY YEARsuffix "_'''+year+self.yearVars['yearVersions'][year]+'''">\n''')
         file.write('''<!ENTITY PROOFdir "/nfs/dust/cms/user/'''+self.userName+'''/.proof2">\n''')
         file.write('''\n''')
         # for s in self.sample_list:
         for s in self.used_samples[year]:
            if is_mainsel:
               if channel in s.channel:
                  file.write('''<!ENTITY '''+s.nickName+''' "&PRESELdir;/&PRESELfilename;'''+('.DATA.' if s.is_data else '.MC.')+s.nickName+'''&YEARsuffix;.root">\n''')
            else:
               file.write('''<!ENTITY '''+s.nickName+''' SYSTEM "'''+s.xmlPath+'''">\n''')
         file.write('''\n''')
         file.write(''']>\n''')
         file.write('''\n''')
         file.write('''<!--\n''')
         file.write('''<ConfigParse NEventsBreak="'''+('500000' if is_mainsel else '0')+'''" FileSplit="'''+('0' if is_mainsel else self.yearVars['preselFileSplit'][year])+'''" AutoResubmit="5"/>\n''')
         file.write('''<ConfigSGE RAM="'''+('4' if is_mainsel else '8')+'''" DISK="3" Mail="'''+self.userMail+'''" Notification="as" Workdir="'''+workdirName+'''"/>\n''')
         file.write('''-->\n''')
         file.write('''\n''')
         file.write('''<!-- OutputLevel controls which messages are printed; set to VERBOSE or DEBUG for more verbosity, to WARNING or ERROR for less -->\n''')
         file.write('''<JobConfiguration JobName="ExampleCycleJob" OutputLevel="INFO">\n''')
         file.write('''<Library Name="libSUHH2HighPtSingleTop"/>\n''')
         file.write('''<Package Name="SUHH2HighPtSingleTop.par"/>\n''')
         file.write('''<Cycle Name="uhh2::AnalysisModuleRunner" OutputDirectory="&OUTPUTdir;/" PostFix="" TargetLumi="&TargetLumi;">\n''')
         file.write('''\n''')
         # for s in self.sample_list:
         for s in self.used_samples[year]:
            if is_mainsel:
               if channel in s.channel:
                  for v in s.mainsel_versions:
                     file.write('''<InputData Lumi="'''+str(s.lumi)+'''" NEventsMax="&NEVT;" Type="'''+('DATA' if s.is_data else 'MC')+'''" Version="'''+v+'''&YEARsuffix;" Cacheable="&b_Cacheable;"> <In FileName="&'''+s.nickName+''';" Lumi="0.0"/> <InputTree Name="AnalysisTree"/> <OutputTree Name="AnalysisTree"/> </InputData>\n''')
            else:
               file.write('''<InputData Lumi="'''+str(s.lumi)+'''" NEventsMax="&NEVT;" Type="'''+('DATA' if s.is_data else 'MC')+'''" Version="'''+s.nickName+'''&YEARsuffix;" Cacheable="&b_Cacheable;"> &'''+s.nickName+'''; <InputTree Name="AnalysisTree"/> <OutputTree Name="AnalysisTree"/> </InputData>\n''')
         file.write('''\n''')
         file.write('''<UserConfig>\n''')
         file.write('''\n''')
         file.write('''<Item Name="PrimaryVertexCollection" Value="offlineSlimmedPrimaryVertices"/>\n''')
         file.write('''<Item Name="METName" Value="slimmedMETsPuppi"/>\n''') # PF MET: slimmedMETs
         file.write('''<Item Name="genMETName" Value="slimmedMETs_GenMET"/>\n''')
         file.write('''<Item Name="ElectronCollection" Value="slimmedElectronsUSER"/>\n''')
         file.write('''<Item Name="MuonCollection" Value="slimmedMuonsUSER"/>\n''')
         file.write('''<Item Name="JetCollection" Value="jetsAk4Puppi"/>\n''') # jetsAk4CHS
         file.write('''<Item Name="GenJetCollection" Value="slimmedGenJets"/>\n''')
         file.write('''<Item Name="TopJetCollection" Value="hotvrPuppi"/>\n''')
         file.write('''<Item Name="GenTopJetCollection" Value="hotvrGen"/>\n''')
         file.write('''<Item Name="GenParticleCollection" Value="GenParticles"/>\n''')
         file.write('''<Item Name="GenInfoName" Value="genInfo"/>\n''')
         if is_mainsel:
            file.write('''<Item Name="additionalBranches" Value="jetsAk8PuppiSubstructure_SoftDropPuppi genjetsAk8SubstructureSoftDrop jetsAk4CHS slimmedMETs btw_bool_reco_sel btw_bool_matrix_sel btw_bool_parton_sel btw_bool_particle_sel"/>\n''')
         else:
            file.write('''<Item Name="additionalBranches" Value="jetsAk8PuppiSubstructure_SoftDropPuppi genjetsAk8SubstructureSoftDrop jetsAk4CHS slimmedMETs"/>\n''')
         # if self.is_mainsel:
         # file.write('''<Item Name="AK8Collection_rec" Value="jetsAk8PuppiSubstructure_SoftDropPuppi"/>\n''')
         # file.write('''<Item Name="AK8Collection_gen" Value="genjetsAk8SubstructureSoftDrop"/>\n''')
         file.write('''\n''')
         file.write('''<Item Name="lumi_file" Value="'''+self.yearVars['lumiFiles'][year]+'''"/>\n''')
         file.write('''<Item Name="lumihists_lumi_per_bin" Value="1000."/>\n''')
         file.write('''\n''')
         file.write('''<Item Name="pileup_directory" Value="'''+self.yearVars['pileupFiles']['mc'][year]+'''"/>\n''')
         file.write('''<Item Name="pileup_directory_data" Value="'''+self.yearVars['pileupFiles']['data'][year]+'''"/>\n''')
         if is_mainsel:
            file.write('''<Item Name="pileup_directory_data_up" Value="'''+self.yearVars['pileupFiles']['dataUp'][year]+'''"/>\n''')
            file.write('''<Item Name="pileup_directory_data_down" Value="'''+self.yearVars['pileupFiles']['dataDown'][year]+'''"/>\n''')
            # file.write('''\n''')
            # file.write('''<Item Name="HOTVRTopTagSFs" Value="'''+self.yearVars['hotvrSFFiles'][self.year]+'''"/>\n''')
            # file.write('''<Item Name="DeepAK8WTagSFs" Value="'''+self.uhh2Dir+'''HighPtSingleTop/data/ScaleFactors/201X/DeepAK8V2_Top_W_SFs.csv.root"/>\n''')
            # file.write('''<Item Name="BTagAlgorithm" Value="'''+self.yearVars['btagalgo'][self.year]+'''"/>\n''')
            # file.write('''<Item Name="BTagCalibration_DeepJet" Value="'''+self.yearVars['deepjetSFFiles'][self.year]+'''"/>\n''')
            # file.write('''<Item Name="BTagCalibration_DeepCSV" Value="'''+self.yearVars['deepcsvSFFiles'][self.year]+'''"/>\n''')
            file.write('''\n''')
            file.write('''<Item Name="apply_TopPtReweighting" Value="false"/>\n''')
            file.write('''<Item Name="VJetsReweighting_do_EWK" Value="true"/>\n''')
            file.write('''<Item Name="VJetsReweighting_do_QCD_EWK" Value="false"/>\n''')
            file.write('''<Item Name="VJetsReweighting_do_QCD_NLO" Value="true"/>\n''')
            file.write('''<Item Name="VJetsReweighting_do_QCD_NNLO" Value="false"/>\n''')
            file.write('''<Item Name="BTagSFNJetReweightFile" Value="'''+os.path.join(os.environ.get('CMSSW_BASE'), 'src/UHH2/HighPtSingleTop/Analysis/btag_njet_sf/btag_njet_sf.root')+'''"/>\n''')
            # file.write('''\n''')
            file.write('''<Item Name="NeuralNetFile_1t" Value="/nfs/dust/cms/user/matthies/uhh2-106X-v2/CMSSW_10_6_28/src/UHH2/HighPtSingleTop/data/KerasDNNModels/ttag_dnn_22-05-12-02-46-10/neural_net.json"/>\n''')
            file.write('''<Item Name="NeuralNetFile_0t1W" Value="/nfs/dust/cms/user/matthies/uhh2-106X-v2/CMSSW_10_6_28/src/UHH2/HighPtSingleTop/data/KerasDNNModels/wtag_dnn_22-05-12-02-49-32/neural_net.json"/>\n''')
         file.write('''\n''')
         file.write('''<!-- Keys for systematic uncertainties -->\n''')
         file.write('''<Item Name="extra_syst" Value="'''+('true' if self.extra_syst else 'false')+'''"/>\n''')
         for syst in systematics.values():
            if syst.shortName == 'murmuf': continue
            file.write('''<Item Name="'''+syst.ctxName+'''" Value="'''+syst.defaultValue+'''"/>\n''')
         file.write('''\n''')
         # file.write('''<!-- Tell AnalysisModuleRunner NOT to use the MC event weight from SFrame; rather let MCLumiWeight (called via CommonModules) calculate the MC event weight. The MC event weight assigned by MCLumiWeight is InputData.Lumi / Cycle.TargetLumi. -->\n''')
         file.write('''<Item Name="use_sframe_weight" Value="false"/>\n''')
         file.write('''<Item Name="AnalysisModule" Value="'''+('HighPtSingleTopMainSelectionModule' if is_mainsel else 'HighPtSingleTopPreSelectionModule')+'''"/>\n''')
         if is_mainsel:
            file.write('''<Item Name="analysis_channel" Value="'''+channel+'''"/>\n''')
            file.write('''<Item Name="bVetoLike" Value="true"/>\n''')
            file.write('''<Item Name="apply_DNNs" Value="true"/>\n''')
        # file.write('''<Item Name="QCD_sideband" Value="false"/>\n''')
         # file.write('''<Item Name="uhh2Dir" Value="'''+self.uhh2Dir+'''"/>\n''')
         # file.write('''<Item Name="EmptyOutputTree" Value="false"/>\n''')
         file.write('''\n''')
         file.write('''<!-- Switch for debugging of the central AnalysisModule -->\n''')
         file.write('''<Item Name="debug" Value="false"/>\n''')
         file.write('''\n''')
         file.write('''</UserConfig>\n''')
         file.write('''\n''')
         file.write('''</Cycle>\n''')
         file.write('''</JobConfiguration>\n''')

      dict_append = {
         'xmlFilePath': xmlFilePath,
         'workdirName': workdirName,
      }
      self.written_xmls.setdefault(selection, OrderedDict()).setdefault(year, OrderedDict()).setdefault(channel if channel else 'all', OrderedDict())['nominal'] = dict_append
      print('Created '+xmlFilePath)

      return xmlFilePath


   def write_systematics_xml(self, syst: systEntity, selection, year, channel=None):

      if self.extra_syst:
         sys.exit('Not valid to call "write_systematics_xml()" for extra systematic.')

      # if not self.write_xml_successful:
      #    sys.exit('xmlCreator::write_xml() not called. Danger of parsing potentially outdated XML file. Exit.')

      is_mainsel = selection == 'mainsel'
      dict_nominal = self.written_xmls[selection][year][channel if channel else 'all']['nominal']

      for direction in syst.directions:
         systXmlFilePath = dict_nominal['xmlFilePath'].replace('.xml', '_')+'_'.join(['syst', syst.shortName, direction])+'.xml'
         systWorkdirName = dict_nominal['workdirName']+'_'+'_'.join(['syst', syst.shortName, direction])
         infile = open(dict_nominal['xmlFilePath'], 'r')
         with open(systXmlFilePath, 'w') as outfile:
            for line in infile:
               newline = line
               if newline.startswith('<!ENTITY DATA_') or ( newline.startswith('<InputData') and 'Type="DATA"' in newline ):
                  continue
               # if newline.startswith('<!ENTITY PRESELdir') or newline.startswith('<!ENTITY OUTPUTdir'):
                  # newline = newline.replace('/nominal/', '/'+'_'.join(['syst', syst.shortName, direction])+'/')
               # if newline.startswith('<!ENTITY PRESELdir') and (syst.shortName == 'jec' or syst.shortName == 'jer'):
               #    newline = newline.replace('/nominal/', '/'+'_'.join(['syst', syst.shortName, direction])+'/')
               if newline.startswith('<!ENTITY OUTPUTdir'):
                  newline = newline.replace('/nominal', '/'+'_'.join(['syst', syst.shortName, direction]))
               if newline.startswith('<ConfigSGE'): # this changes the workdir name
                  newline = newline.replace(dict_nominal['workdirName'], systWorkdirName)
               if syst.shortName != 'murmuf' and newline.startswith('<Item Name="'+syst.ctxName):
                  newline = newline.replace(syst.defaultValue, direction)
               if syst.shortName == 'murmuf' and newline.startswith('<Item Name="ScaleVariationMu'):
                  newline = newline.replace(syst.defaultValue, direction)
               # if self.is_mainsel and newline.startswith('<Item Name="EmptyOutputTree"'):
               #    newline = newline.replace('false', 'true') # Don't save AnalysisTree for mainsel systematics
               outfile.write(newline)
         infile.close()

         dict_append = {
            'xmlFilePath': systXmlFilePath,
            'workdirName': systWorkdirName,
         }
         self.written_xmls.setdefault(selection, OrderedDict()).setdefault(year, OrderedDict()).setdefault(channel if channel else 'all', OrderedDict()).setdefault(syst.shortName, OrderedDict())[direction] = dict_append
         print('Created '+systXmlFilePath)


   def write_all_systematics_xmls(self, selection, year, channel=None):

      systematics = xmlCreator.get_systematics(selection, year, channel)
      for syst in systematics.values():
         if not (syst.shortName.startswith('jes') or syst.shortName.startswith('jer')): continue # HACK to prevent creating XML files for other basic systematics than JES and JER variations
         self.write_systematics_xml(syst, selection, year, channel)


   # def write_qcd_sideband_xml(self):
   #
   #    if not self.write_xml_successful:
   #       sys.exit('xmlCreator::write_xml() not called. Danger of parsing potentially outdated XML file. Exit.')
   #
   #    qcdXmlFilePath = self.xmlFilePath.replace('.xml', '_')+'QCDsideband.xml'
   #    infile = open(self.xmlFilePath, 'r')
   #    with open(qcdXmlFilePath, 'w') as outfile:
   #       for line in infile:
   #          newline = line
   #          if newline.startswith('<!ENTITY PRESELdir') or newline.startswith('<!ENTITY OUTPUTdir'):
   #             newline = newline.replace('/nominal/', '/QCDsideband/')
   #          if newline.startswith('<ConfigSGE'):
   #             newline = newline.replace('"/>', '_QCDsideband"/>')
   #          if newline.startswith('<Item Name="QCD_sideband"'):
   #             newline = newline.replace('false', 'true')
   #          if self.is_mainsel and newline.startswith('<Item Name="EmptyOutputTree"'):
   #             newline = newline.replace('false', 'true') # Don't save AnalysisTree for mainsel qcd sideband
   #          outfile.write(newline)
   #    infile.close()
   #
   #    print('Created '+qcdXmlFilePath)
   #
   #    self.qcdXmlFilePath = qcdXmlFilePath


   def write_bash_scripts(self, selection, year, channel=None):

      written_xmls = self.written_xmls.setdefault(selection, OrderedDict()).setdefault(year, OrderedDict()).setdefault(channel if channel else 'all', OrderedDict())
      configDir = os.path.dirname(written_xmls['nominal']['xmlFilePath'])
      systematics = xmlCreator.get_systematics(selection, year, channel)

      scriptFilePath_sframe_batch = os.path.join(configDir, 'run_all_sframe_batch.sh')
      with open(scriptFilePath_sframe_batch, 'w') as outfile:
         outfile.write('#!/bin/bash\n')
         newline_base = 'sframe_batch.py $1 '
         for syst, dict in written_xmls.items():
            if syst == 'nominal':
               outfile.write(newline_base+os.path.basename(dict['xmlFilePath'])+'\n')
            else:
               for direction in systematics[syst].directions:
                  outfile.write(newline_base+os.path.basename(dict[direction]['xmlFilePath'])+'\n')
      p = subprocess.Popen('chmod +x '+scriptFilePath_sframe_batch, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
      p.wait()
      print('Created '+scriptFilePath_sframe_batch)

      scriptFilePath_run_local = os.path.join(configDir, 'run_all_local.sh')
      with open(scriptFilePath_run_local, 'w') as outfile:
         outfile.write('#!/bin/bash\n')
         newline_base = 'python run_local.py $1 '
         for syst, dict in written_xmls.items():
            if syst == 'nominal':
               outfile.write(newline_base+dict['workdirName']+'\n')
            else:
               for direction in systematics[syst].directions:
                  outfile.write(newline_base+dict[direction]['workdirName']+'\n')
      p = subprocess.Popen('chmod +x '+scriptFilePath_run_local, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
      p.wait()
      print('Created '+scriptFilePath_run_local)
      link_run_local_command = 'ln -s '+os.path.abspath(os.path.join(configDir, '..', 'run_local.py'))+' '+os.path.join(configDir, 'run_local.py')
      p = subprocess.Popen(link_run_local_command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
      p.wait()


if __name__=='__main__':

   selections = ['presel', 'mainsel']
   # years = ['2016', '2017', '2018']
   years = ['UL16preVFP', 'UL16postVFP', 'UL17', 'UL18']
   channels = ['ele', 'muo']

   if not sys.argv[1:]: sys.exit('No arguments provided. Exit.')
   parser = argparse.ArgumentParser()
   parser.add_argument('--all', action='store_true', help='Create XML files for all selections, years, and channels.')
   parser.add_argument('--syst', action='store_true', help='Create XML files for systematic uncertainties.')
   parser.add_argument('-i', '--ignore-data', action='store_true', help='Ignore data, only do MC.')
   parser.add_argument('-x', '--extra-systs', action='store_true', help='Create XML files for special systematics which require different MC samples (e.g. hdamp variations).')
   parser.add_argument('-s', '--selections', choices=selections, nargs='*', default=selections)
   parser.add_argument('-y', '--years', choices=years, nargs='*', default=years)
   parser.add_argument('-c', '--channels', choices=channels, nargs='*', default=channels)
   # parser.add_argument('-a', '--auto-complete', action='store_true', help='Auto-complete arguments if not all arguments for selections, years, and channels are given. E.g. if only selections and channels are given, assume that you want to create XML files for all years.')
   args = parser.parse_args(sys.argv[1:])

   # if(args.all == True):
   #    if(len(args.selections) + len(args.years) + len(args.channels) != 0):
   #       sys.exit('Not allowed to use "--all" option jointly with manually given selection, year, or channel argument. Exit.')
   #    args.selections = selections
   #    args.years = years
   #    args.channels = channels
   #    if(args.auto_complete):
   #       print('Warning: You already specified "--all". Therefore, "--auto-complete" will not have any effect.')
   # else:
   #    if args.auto_complete:
   #       print('Auto-completing arguments.')
   #       if not args.selections: args.selections = selections
   #       if not args.years: args.years = years
   #       if not args.channels: args.channels = channels
   #    else:
   #       for p in permutations([args.selections, args.years, args.channels]):
   #          if p[0] and (not p[1] or not p[2]):
   #             sys.exit('You specified arguments for at least one of the three options: "--selections", "--years", "--channels", but not for all three of them. Also, you did not specify "--auto-complete" to compensate for this. Exit.')

   print('Going to create XML files for:')
   print('  Selections: '+', '.join(str(x) for x in args.selections))
   print('  Years: '+', '.join(str(x) for x in args.years))
   print('  Channels: '+', '.join(str(x) for x in args.channels))

   # configContainer.read_database(args.years, args.channels)
   # configContainer.read_database_106X_v2(args.years)

   x = xmlCreator(ignore_data=args.ignore_data)
   x.read_database_106X_v2(args.years)

   if 'presel' in args.selections:
      for year in args.years:
         x.write_xml('presel', year)
         if args.syst: x.write_all_systematics_xmls('presel', year)
         x.write_bash_scripts('presel', year)

   if 'mainsel' in args.selections:
      for year in args.years:
         for channel in args.channels:
            x.write_xml('mainsel', year, channel)
            if args.syst: x.write_all_systematics_xmls('mainsel', year, channel)
            x.write_bash_scripts('mainsel', year, channel)

      # for selection in args.selections:
      #    for channel in args.channels:
      #       # x = xmlCreator(selection, year, channel)
      #       x.write_xml()
      #       x.write_qcd_sideband_xml()
      #       if args.syst:
      #          x.write_all_systematics_xmls()


   #_________________________________________________
   # Now do the extra systematics:
   if args.extra_systs:
      all_extra_systs = set()
      for k, v in samplesDict.items():
         if v.get('extra_systs'):
            for syst in v.get('extra_systs'):
               all_extra_systs.add(syst)
      all_extra_systs = list(all_extra_systs)
      all_extra_systs = sorted(all_extra_systs)
      print('Going to create XML files for these extra systematics (auto-extracted from database):')
      print(all_extra_systs)

      for syst in all_extra_systs:
         xx = xmlCreator(ignore_data=args.ignore_data, extra_syst=syst)
         xx.read_database_106X_v2(args.years)

         if 'presel' in args.selections:
            for year in args.years:
               xx.write_xml('presel', year)

         if 'mainsel' in args.selections:
            for year in args.years:
               for channel in args.channels:
                  xx.write_xml('mainsel', year, channel)
