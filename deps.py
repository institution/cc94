#!/usr/bin/env python2.7
import os, sys
s = sys.stdin.read()

xs = s.split(' ')

rs = []
for x in xs:
	fp,ex = os.path.splitext(x)
	if os.path.exists(fp+'.cpp'):
		rs.append(fp+'.RRR')
	
print ' '.join(rs)
