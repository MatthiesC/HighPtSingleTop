#!/usr/bin/env python

import sys
from collections import OrderedDict
import ROOT as root
import numpy as np

class ScaleFactor:

    '''Container class'''

    list_object = list()
    list_year = list()
    list_version = list()
    list_mistagging_rate = list()
    list_pt_intervals = list()

    def __init__(self, csvRow=None):

        self.object = csvRow[0]
        self.year = csvRow[1]
        self.version = csvRow[2]
        self.mistagging_rate = csvRow[3]
        self.pt_low = int(csvRow[4])
        self.pt_high = int(csvRow[5])
        self.sf = float(csvRow[6])
        self.sf_err_down = float(csvRow[7])
        self.sf_err_up = float(csvRow[8])

        pairs = list()
        pairs.append([self.list_object, self.object])
        pairs.append([self.list_year, self.year])
        pairs.append([self.list_version, self.version])
        pairs.append([self.list_mistagging_rate, self.mistagging_rate])
        pairs.append([self.list_pt_intervals, (self.pt_low, self.pt_high)])
        for x,y in pairs:
            if(y not in x): x.append(y)


### READ CSV FILE:

infilename = sys.argv[1]
print 'Input file:', infilename

dummy = None
sf_list = list()
with open(infilename, 'r') as infile:
    firstline = True;
    for line in infile:
        if(firstline):
            firstline = False
            continue
        row = line.split(',')
        for i in range(len(row)):
            row[i] = row[i].strip()
        # print(row)
        sf = ScaleFactor(row)
        sf_list.append(sf)
        dummy = sf

print "Found these possible values:"
print "Objects:", dummy.list_object
print "Years:", dummy.list_year
print "Versions:", dummy.list_version
print "Mistagging rates:", dummy.list_mistagging_rate
print "Pt intervals:" , dummy.list_pt_intervals

sf_dict = OrderedDict()
for o in dummy.list_object:
    sf_dict[o] = OrderedDict()
    for y in dummy.list_year:
        sf_dict[o][y] = OrderedDict()
        for v in dummy.list_version:
            sf_dict[o][y][v] = OrderedDict()
            for m in dummy.list_mistagging_rate:
                sf_dict[o][y][v][m] = OrderedDict()

for sf in sf_list:
    pt_bin_key = str(sf.pt_low)+'to'+str(sf.pt_high)
    values = OrderedDict()
    values["sf"] = sf.sf
    values["sf_err_up"] = sf.sf_err_up
    values["sf_err_down"] = sf.sf_err_down
    sf_dict[sf.object][sf.year][sf.version][sf.mistagging_rate][pt_bin_key] = values


### CONVERT CSV TO ROOT:

outfilename = infilename+'.root'
print 'Creating ROOT file:', outfilename
outfile = root.TFile.Open(outfilename, "RECREATE")
outfile.cd()

for o in sf_dict.keys():
    for y in sf_dict[o].keys():
        for v in sf_dict[o][y].keys():
            for m in sf_dict[o][y][v].keys():
                list_pt_center = list()
                list_pt_err_low = list()
                list_pt_err_high = list()
                list_sf = list()
                list_sf_err_up = list()
                list_sf_err_down = list()
                for pt_bin in sf_dict[o][y][v][m].keys():
                    pt_low = float(pt_bin.split('to')[0])
                    pt_high = float(pt_bin.split('to')[1])
                    pt_center = pt_low + (pt_high - pt_low) / 2.
                    list_pt_center.append(pt_center)
                    list_pt_err_low.append(pt_center - pt_low)
                    list_pt_err_high.append(pt_high - pt_center)
                    list_sf.append(sf_dict[o][y][v][m][pt_bin]['sf'])
                    list_sf_err_up.append(sf_dict[o][y][v][m][pt_bin]['sf_err_up'])
                    list_sf_err_down.append(sf_dict[o][y][v][m][pt_bin]['sf_err_down'])
                if len(list_pt_err_low)==0: continue # check if combination of o/y/v/m actually has tagging scale factors
                graph = root.TGraphAsymmErrors(len(list_pt_center), np.array(list_pt_center, dtype='double'), np.array(list_sf, dtype='double'), np.array(list_pt_err_low, dtype='double'), np.array(list_pt_err_high, dtype='double'), np.array(list_sf_err_down, dtype='double'), np.array(list_sf_err_up, dtype='double'))
                graph_name = '_'.join([o, y, v, m])
                graph.SetName(graph_name)
                graph.SetTitle(infilename.split('/')[-1].split('_')[0]+'_'+graph_name)
                graph.Write()

outfile.Close()
print 'Done.'
