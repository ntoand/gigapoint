import gzip
import os
import sys

print sys.argv
if len(sys.argv) != 3:
	sys.exit('Error: not enough inputs. usage: pts2ply in_pts_file out_ply_file')

in_filename = sys.argv[1]
out_filename = sys.argv[2]
print 'in_filename: ' + in_filename
print 'out_filename: ' + out_filename

# run getstats.py first to get information of the pointcloud

count = 0;
with open(in_filename, 'rt') as fin, open(out_filename, 'wt') as fout:
	
	fout.write('ply\n')
	fout.write('format ascii 1.0\n')
	fout.write('comment Author: Toan Nguyen\n')
	fout.write('element vertex 44508165\n')
	fout.write('property float x\n')
	fout.write('property float y\n')
	fout.write('property float z\n')
	fout.write('property uchar red\n')
	fout.write('property uchar green\n')
	fout.write('property uchar blue\n')
	fout.write('end_header\n')

	for line in fin:
		#print(line)
		line = line.strip()
		values = line.split(' ')
		if len(values) < 3:
			continue
		x = values[0] # - 
		y = values[1] # -
		z = values[2] # -
		fout.write(str(x) + ' ' + str(y) + ' ' + str(z) + ' ' + str(values[4]) + ' ' + str(values[5]) + ' ' + str(values[6]) + '\n')
		count = count + 1;
		if(count % 100000 == 0): print('.'),

print('number of points: ' + str(count))
print('Done!')
