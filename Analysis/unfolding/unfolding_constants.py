import numpy as np

class UnfoldingLevel():
    def __init__(self, name, axis_prefix):
        self.name = name
        self.axis_prefix = axis_prefix

unfoldingLevels = {
    'Reco': UnfoldingLevel(
        name='Reco',
        axis_prefix='Detector-level'
    ),
    'Matrix': UnfoldingLevel(
        name='Matrix',
        axis_prefix='Parton-level'
    ),
    'Parton': UnfoldingLevel(
        name='Parton',
        axis_prefix='Parton-level'
    ),
    'Particle': UnfoldingLevel(
        name='Particle',
        axis_prefix='Particle-level'
    ),
}

class Region():
    def __init__(self, cut_rule):
        self.cut_rule = cut_rule

class VOI():
    def __init__(self, name, bins, axis_label_base, branch_name_base=None, alias_base=None):
        self.name = name
        self.bins = np.array(bins, dtype=float)
        self.axis_label_base = axis_label_base
        self.branch_name_base = branch_name_base or name
        self.alias_base = alias_base or '{0}'

    def get_axis_label(self, level):
        return unfoldingLevels.get(level).axis_prefix+' '+self.axis_label_base

    def get_branch_name(self, level):
        return 'btw_VOI_'+unfoldingLevels.get(level).name+'_'+self.branch_name_base

    def get_alias(self, level):
        return self.alias_base.format(self.get_branch_name(unfoldingLevels.get(level).name))

class UnfoldingConstants():
    def __init__(self):
        self.Regions = {
            'unfold': Region(
                cut_rule='((btw_Region_heavyTags_int == 1) | (btw_Region_heavyTags_int == 2)) & ((btw_Region_bTags_int == 2) | (btw_Region_bTags_int == 3))'
            ),
            '1t': Region(
                cut_rule='(btw_Region_heavyTags_int == 1)'
            ),
            '0t1W': Region(
                cut_rule='(btw_Region_heavyTags_int == 2)'
            ),
            '1b': Region(
                cut_rule='(btw_Region_bTags_int == 2)'
            ),
            '2b': Region(
                cut_rule='(btw_Region_bTags_int == 3)'
            ),
            '1b1t': Region(
                cut_rule='(btw_Region_int == 21)'
            ),
            '1b0t1W': Region(
                cut_rule='(btw_Region_int == 22)'
            ),
            '2b1t': Region(
                cut_rule='(btw_Region_int == 31)'
            ),
            '2b0t1W': Region(
                cut_rule='(btw_Region_int == 32)'
            ),
        }

        self.VOIs = {
            'TopQuarkPt': VOI(
                name='TopQuarkPt',
                bins=[200, 270, 350, 450, 600],
                axis_label_base='top quark #it{p}_{T} [GeV]',
            ),
            'TopQuarkAbsY': VOI(
                name='TopQuarkAbsY',
                bins=[0, 0.4, 0.9, 1.5, 2.5],
                axis_label_base='top quark |#it{y}|',
                branch_name_base='TopQuarkY',
                alias_base='abs({0})',
            ),
            'WBosonPt': VOI(
                name='WBosonPt',
                bins=[200, 270, 350, 450, 600],
                axis_label_base='associated W boson #it{p}_{T} [GeV]',
            ),
            'WBosonAbsY': VOI(
                name='TopQuarkAbsY',
                bins=[0, 0.4, 0.9, 1.5, 2.5],
                axis_label_base='associated W boson |#it{y}|',
                branch_name_base='WBosonY',
                alias_base='abs({0})',
            ),

            # 'TopQuarkPt': {
            #     'Reco':   np.array([200, 270, 350, 450, 600], dtype=float),
            #     'Matrix': np.array([200, 270, 350, 450, 600], dtype=float),
            #     'axis_label': 'top quark #it{p}_{T} [GeV]',
            # },
            # 'TopQuarkAbsY': {
            #     'Reco':   np.array([0, 0.5, 1.3, 2.5], dtype=float),
            #     'Matrix': np.array([0, 0.5, 1.3, 2.5], dtype=float),
            #     'axis_label': 'top quark |#it{y}|',
            #     'btw_voi': 'TopQuarkY',
            #     'rule': 'abs({0})',
            # },
            # 'WBosonPt': {
            #     'Reco':   np.array([200, 270, 350, 450, 600], dtype=float),
            #     'Matrix': np.array([200, 270, 350, 450, 600], dtype=float),
            #     'axis_label': 'W boson #it{p}_{T} [GeV]',
            # },
            # 'WBosonAbsY': {
            #     'Reco':   np.array([0, 0.5, 1.3, 2.5], dtype=float),
            #     'Matrix': np.array([0, 0.5, 1.3, 2.5], dtype=float),
            #     'axis_label': 'W boson |#it{y}|',
            #     'btw_voi': 'WBosonY',
            #     'rule': 'abs({0})',
            # },
            # 'tWSystemMass': {
            #     'Reco':   np.array([600, 700, 800, 900], dtype=float),
            #     'Matrix': np.array([600, 700, 800, 900], dtype=float),
            #     'axis_label': '#it{m}_{tW} [GeV]',
            # },
            # 'tWSystemCustomDPhi': {
            #     # 'Reco':   np.array([0, 0.6, 0.85, 1], dtype=float)*np.pi,
            #     # 'Matrix': np.array([0, 0.6, 0.85, 1], dtype=float)*np.pi,
            #     'Reco':   np.array([1, 2, 3, 5], dtype=float),
            #     'Matrix': np.array([1, 2, 3, 5], dtype=float),
            #     'axis_label': '(#Delta#phi_{tW})',
            #     'btw_voi': 'tWSystemDPhi',
            #     'rule': '10**(1 - {0} / 3.14159)',
            # },
        }
    #     self.bins = {}
    #     for k, v in self.bin_edges:
    #         self.bins[k] = {
    #             'reco':
    #         }
    #
    # def setup_bins(self, variable='TopQuarkPt', level='reco'):
    #     return self.bins.get(variable).get(level)
    # def get_axis_label(self, voi, level):
    #     label = ''
    #     if label == 'Reco':
    #         label += 'Reconstructed'
    #     elif label == 'Matrix':
    #         label += 'Matrix-level'
    #     elif label == 'Parton':
    #         label += 'Parton-level'
    #     elif label == 'Particle':
    #         label += 'Particle-level'
    #     label += ' '+self.VOIs[voi].get('axis_label', 'LABEL NOT DEFINED')

unfoldingConstants = UnfoldingConstants()


# def get_region_rule(region):
#     if region == None:
#         return '(True)'
#     elif region == 'unfold':
#         return '((btw_Region_heavyTags_int == 1) | (btw_Region_heavyTags_int == 2)) & ((btw_Region_bTags_int == 2) | (btw_Region_bTags_int == 3))'
#     elif region == '1t':
#         return '(btw_Region_heavyTags_int == 1)'
#     elif region == '0t1W':
#         return '(btw_Region_heavyTags_int == 2)'
#     elif region == '0t0W':
#         return '(btw_Region_heavyTags_int == 3)'
#     elif region == '0b':
#         return '(btw_Region_bTags_int == 1)'
#     elif region == '1b':
#         return '(btw_Region_bTags_int == 2)'
#     elif region == '2b':
#         return '(btw_Region_bTags_int == 3)'
#     elif region == '0b1t':
#         return '(btw_Region_int == 11)'
#     elif region == '0b0t1W':
#         return '(btw_Region_int == 12)'
#     elif region == '0b0t0W':
#         return '(btw_Region_int == 13)'
#     elif region == '1b1t':
#         return '(btw_Region_int == 21)'
#     elif region == '1b0t1W':
#         return '(btw_Region_int == 22)'
#     elif region == '1b0t0W':
#         return '(btw_Region_int == 23)'
#     elif region == '2b1t':
#         return '(btw_Region_int == 31)'
#     elif region == '2b0t1W':
#         return '(btw_Region_int == 32)'
#     elif region == '2b0t0W':
#         return '(btw_Region_int == 33)'
#     else:
#         sys.exit('Unknown region: '+str(region))




# _VOI_INTERVALS = {
#     'TopQuarkPt' = [
#         VarInterval('TopQuarkPt', 200, 250),
#         VarInterval('TopQuarkPt', 250, 300),
#         VarInterval('TopQuarkPt', 300, 370),
#         VarInterval('TopQuarkPt', 370, 450),
#         VarInterval('TopQuarkPt', 450, 550),
#     ],
# }
