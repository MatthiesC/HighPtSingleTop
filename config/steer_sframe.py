#!/usr/bin/env python3

import os
import sys
import argparse
import shutil
import time
import datetime
import subprocess


def get_args(sys_argv):

    selections = ['presel', 'mainsel']
    years = ['2016', '2017', '2018']
    channels = ['ele', 'muo']
    options = ['s', 'r', 'a']


    parser = argparse.ArgumentParser()

    parser.add_argument('-s', '--selections', choices=selections, nargs='*', default=[])

    parser.add_argument('--all', action='store_true', help='All years and channels')
    parser.add_argument('-y', '--years', choices=years, nargs='*', default=[])
    parser.add_argument('-c', '--channels', choices=channels, nargs='*', default=[])

    parser.add_argument('-n', '--new', action='store_true', help='Create new workdirs, move old output.')
    parser.add_argument('-o', '--option', choices=options, nargs=1, default=[], help='Call sframe_batch.py option')

    args = parser.parse_args(sys_argv)

    if args.all and len(args.years)+len(args.channels):
        sys.exit('Cannot use "--all" option jointly with "--years" or "--channels" options. Exit.')

    return args


class SFrameBatchProject:

    '''Info text'''

    global_uhh2DirPath = os.environ.get('CMSSW_BASE')+'/src/UHH2/'
    global_configDirPath = global_uhh2DirPath+'HighPtSingleTop/config/'
    global_outputDirPath = global_uhh2DirPath+'HighPtSingleTop/output/'
    global_project_timestamp = str()

    def __init__(self, selection: str, year: str, channel: str, syst_name: str, syst_direction=''):

        self.outputDirPath = self.global_outputDirPath+'/'.join([selection, year, channel])+'/'
        self.workDirName = '_'.join(['workdir', selection, year, channel])
        self.configDirPath = self.global_configDirPath+'_'.join(['config', selection, year, channel])+'/'
        self.configFileName = '_'.join(['parsedConfigFile', selection, year, channel])
        if syst_name == 'nominal':
            self.outputDirPath += 'nominal/'
            self.workDirName += '/'
            self.configFileName += '.xml'
        else:
            self.outputDirPath += '_'.join(['syst', syst_name, syst_direction])+'/'
            self.workDirName += '_'.join(['', 'syst', syst_name, syst_direction])+'/'
            self.configFileName += '_'.join(['', 'syst', syst_name, syst_direction])+'.xml'
        self.outputWorkDirPath = self.outputDirPath+self.workDirName
        self.configWorkDirPath = self.configDirPath+self.workDirName
        self.configFilePath = self.configDirPath+self.configFileName

    def print_paths(self):

        print('All relevant paths and files for this project:')
        print(self.configDirPath)
        print(self.configFilePath)
        print(self.configWorkDirPath)
        print(self.outputDirPath)
        print(self.outputWorkDirPath)

    def retire_old_project(self):

        if os.path.isdir(self.configWorkDirPath):
            shutil.rmtree(self.configWorkDirPath)
        if os.path.isdir(self.outputWorkDirPath):
            shutil.rmtree(self.outputWorkDirPath)
        if os.path.isdir(self.outputDirPath):
            timestamp = time.time()
            timestamp = datetime.datetime.fromtimestamp(timestamp).strftime('%Y-%m-%d-%H-%M-%S')
            shutil.move(self.outputDirPath, self.outputDirPath[:-1]+'__retired-on-'+timestamp)

    def create_new_project(self):

        self.retire_old_project()
        proc = subprocess.Popen(['sframe_batch.py', self.configFileName], cwd=self.configDirPath)

        return proc

    def submit_jobs(self):

        proc = subprocess.Popen(['sframe_batch.py', '-s', self.configFileName], cwd=self.configDirPath)

        return proc

    def resubmit_jobs(self):

        proc = subprocess.Popen(['sframe_batch.py', '-r', self.configFileName], cwd=self.configDirPath)

        return proc

    def hadd_job_outputs(self):

        proc = subprocess.Popen(['sframe_batch.py', '-a', self.configFileName], cwd=self.configDirPath)

        return proc

    def check_status_of_jobs(self):

        if not os.path.isdir(self.configWorkDirPath):
            sys.exit('Cannot check status if config workdir does not exist yet. Did you create new SFrameBatchProject? Exit.')
        proc = subprocess.Popen(['sframe_batch.py', self.configFileName], cwd=self.configDirPath)

        return proc

    def run_local(self, max_jobs: int):

        proc = subprocess.Popen(['python', 'run_local.py', '-m', str(max_jobs), self.configWorkDirPath], cwd=self.global_configDirPath)

        return proc


if __name__ == '__main__':

    args = get_args(sys.argv[1:])

    project = SFrameBatchProject('presel', '2016', 'muo', 'jec', 'up')
    project.check_status_of_jobs()
