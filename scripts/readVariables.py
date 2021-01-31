import json
import numpy as np

with open('parameters.json') as f:
    param = json.load(f)

variables = param.get('inputVars')

inputs = list()
for v in variables:
    inputs.append({'name': str(v[0]), 'offset': -float(v[1]), 'scale': float(v[2])})

class_labels = list()
if param.get('binary') == True:
    class_labels.append('binary')
else:
    enc = np.load('encoder_classes.npy')
    for i in range(len(enc)):
        class_labels.append('node'+str(i))#+'_'+enc[i])

result = {
    'inputs': inputs,
    'class_labels': class_labels
}

with open('variables.json', 'w') as outfile:
    json.dump(result, outfile)
