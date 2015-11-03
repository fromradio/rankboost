// Copyright 2015 Ruimin Wang
// Author: Ruimin Wang, wangruimin_sx@qiyi.com, ruimin.wang13@gmail.com

#include "filereader.h"
#include <fstream>

FileReader::FileReader()
{
}

FileReader::FileReader(const char *filename)
{
	read(filename);
}

const std::vector<RankList>& FileReader::samples() const
{
	return __samples;
}

const std::vector<size_t>& FileReader::features() const
{
	return __features;
}

void FileReader::read(const char *filename)
{
	std::ifstream fin(filename);
	std::string str,tempstr;
	std::set<size_t> features;
	double label;
	int qid;
	size_t id;
	size_t f;
	double val;
	size_t curr_qid = -1;
	bool begin = false;
	RankList tempsample;
	while(std::getline(fin,str)) {
		std::istringstream is(str);
		is>>id;
		is>>label;
		is>>tempstr;
		is>>qid;
		FeatureMap fea;
		while(is>>f>>val)
		{
			features.insert(f);
			fea[f] = val;
		}
		if(qid==curr_qid)
			tempsample.push_back(Sample(id,label,fea));
		else{
			if(begin)
				__samples.push_back(tempsample);
			else
				begin = true;
			curr_qid = qid;
			tempsample.clear();
			tempsample.push_back(Sample(id,label,fea));
		}
	}
	__samples.push_back(tempsample);
	__features.reserve(features.size());
	for(std::set<size_t>::iterator fi = features.begin();fi!=features.end();++fi){
		__features.push_back(*fi);
	}
}