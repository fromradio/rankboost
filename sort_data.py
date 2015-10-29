import sys
if __name__ == '__main__':
	input_file = sys.argv[1]
	out_file = sys.argv[2]
	outfile = open(out_file,'w')
	dic = {}
	with open(input_file) as fp:
		for line in fp:
			data = line.rstrip().split(' ')
			label = float(data[0])
			qid = int(data[2])
			dic.setdefault(qid,[])
			dic[qid].append((label,data[3:]))
	for qid in dic:
		l = dic[qid]
		ll = sorted(l,key=lambda x:x[0],reverse = True)
		for i in ll:
			outfile.write(str(i[0])+' qid '+str(qid)+' '+' '.join(i[1])+'\n')