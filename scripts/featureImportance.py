import json
from operator import itemgetter

with open('neural_net.json') as f:
    nn = json.load(f)

inputs = nn.get('inputs') # list with entries of format: {'name':string, 'offset':float, 'scale':float}
layer1 = nn.get('layers')[0] # list with entries of format: {'activation':string, 'architecture':string, 'bias':list(float), 'weights':list(float)}

input_names = list()
for i in inputs:
    input_names.append(i.get('name'))

layer1_width = len(layer1.get('bias'))

importance_raw = list() # entries: {'name':string, 'value':float}
for i in range(len(input_names)):
    weightNumber = i
    weightsSum = 0.
    for j in range(layer1_width):
        weightsSum += layer1.get('weights')[weightNumber] # TAKING THE ABSOLUTE VALUE OF THE WEIGHT!!!
        weightNumber += len(input_names)
    importance_raw.append({'name': input_names[i], 'value': weightsSum})

importance_raw_sorted = sorted(importance_raw, key=itemgetter('value'), reverse=True)
importance_norm_sorted = list()
totalSumOfWeights = sum(layer1.get('weights'))
for i in importance_raw_sorted:
    #importance_norm_sorted.append({'name': i.get('name'), 'value': i.get('value')/totalSumOfWeights})
    importance_norm_sorted.append({'name': i.get('name'), 'value': i.get('value')})

chars_column1 = 40
def hline(): print('-'*75)
hline()
print('Variable'+' '*(chars_column1-len('Variable'))+'Importance score')
hline()
for i in range(len(input_names)):
    var_name = importance_norm_sorted[i].get('name')
    cout = ''
    cout += var_name
    cout += ' '*(chars_column1-len(var_name))
    cout += str(importance_norm_sorted[i].get('value'))
    print(cout)
hline()
