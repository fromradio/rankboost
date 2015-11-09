#!/usr/bin/python
# encoding=utf-8

# from simple rankboost to RankLib format
import sys

if __name__ =='__main__':
	file_name = sys.argv[1]
	out_name = sys.argv[2]
	outfile = open(out_name,'w')
	outfile.write("## RankBoost\n")
	# outfile.write()
	first = True
	with open(file_name) as fp:
		for line in fp:
			if first:
				outfile.write("## Iteration = "+line)
				outfile.write("## No. of threshold candidates = 10\n")
				first = False
			else:
				outfile.write(':'.join(line.rstrip().split(' '))+' ')