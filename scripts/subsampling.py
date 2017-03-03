import gzip
import os
import sys

print sys.argv
if len(sys.argv) != 4:
	sys.exit('Error: not enough inputs. usage: subsampling in_file out_file factor')

in_filename = sys.argv[1]
out_filename = sys.argv[2]
factor = int(sys.argv[3])
print 'in_filename: ' + in_filename
print 'out_filename: ' + out_filename
print 'factor: ' + str(factor)

# run getstats.py first to get information of the pointcloud

count = 0;
remain = 0;
with open(in_filename, 'rt') as fin, open(out_filename, 'wt') as fout:
	
	for line in fin:
		if count % factor == 0:
			fout.write(line)
			remain += 1

		count += 1
		if(count % 100000 == 0): 
			print('.'),

print('number of points: ' + str(count))
print('number of points after subsampling: ' + str(remain))
print('Done!')
