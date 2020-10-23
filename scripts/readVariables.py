import json

with open('parameters.json') as f:
    param = json.load(f)

variables = param.get('inputVars')

inputs = list()
for v in variables:
    inputs.append({'name': str(v[0]), 'offset': -float(v[1]), 'scale': float(v[2])})

result = {
    'inputs': inputs,
    'class_labels': ['binary']
}

with open('variables.json', 'w') as outfile:
    json.dump(result, outfile)
