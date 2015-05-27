#!/usr/bin/python
import sys

if len(sys.argv) < 2:
    print 'Please specify a TSV map file.'
    sys.exit()

rows = 0
cols = 0


flag = False
file = sys.argv[1]
print file
with open(file, 'rU') as f:
    for line in f:
        rows += 1
        out = []
        for c in line:
            if c == 'F':
                out.append('#')
                flag = True
            elif c == '\t' and flag:
                flag = False
            elif c == '\t':
                out.append(' ')
        
        print ''.join(out)
        cols = max(cols, len(out))

print (cols, rows)
                
