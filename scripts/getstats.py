import csv
import sys

print sys.argv
in_filename = ''
xyz_pos = []
if len(sys.argv) != 2:
    sys.exit('Error: not enough inputs. usage: getstats in_file')

in_filename = sys.argv[1]
print "Input: " + in_filename

bmin = [100000000, 100000000, 100000000]
bmax = [-100000000, -100000000, -100000000]
i = 0
with open(in_filename, 'rt') as fin:
    for line in fin:
        values = line.split(' ')
        if len(values) < 3:
            continue

        x = float(values[0])
        y = float(values[1])
        z = float(values[2])

        bmin[0] = min(bmin[0], x)
        bmin[1] = min(bmin[1], y)
        bmin[2] = min(bmin[2], z)
        bmax[0] = max(bmin[0], x)
        bmax[1] = max(bmin[1], y)
        bmax[2] = max(bmin[2], z)

        i+=1
        if(i % 100000 == 0): print('.'),

print "== summary =="
print "# points:" + str(i)
print "Min: " + str(bmin)
print "Max: " + str(bmax)