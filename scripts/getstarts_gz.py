import gzip
import os

in_filename = 'pointcloud.txt.gz'

count = 0;
with gzip.open(in_filename, 'r') as fin:

        for line in fin:
                count = count + 1;
                
print('number of points: ' + str(count))
print('Done!')