import sys

infilename = sys.argv[1]
outfile_postfix = sys.argv[2]

with open(infilename, 'r') as infile:
    with open(infilename.strip('.csv')+outfile_postfix+'.csv', 'w') as outfile:
        firstline = True
        for inline in infile:
            if firstline:
                outfile.write(inline)
                firstline = False
            elif not 'iterativefit' in inline:
                continue
            elif 'jes' in inline and not 'jes,' in inline:
                continue
            else:
                outfile.write(inline)
