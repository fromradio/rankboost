#ifndef RANK_BOOST_H__
#define RANK_BOOST_H__

#include <iostream>
#include <algorithm>
#include <cmath>
#include <map>
#include <vector>
#include <fstream>
#include <list>
#include <string>
#include <set>
#include <sstream>


typedef std::map<size_t,double> FeatureMap;
class Sample
{
	size_t __id;
	double __label;
	FeatureMap __feas;
public:
	Sample(size_t id,double label,const FeatureMap& feas)
		: __id(id),__label(label),__feas(feas){}
	bool has(size_t i) const{
		if(__feas.find(i)==__feas.end())
			return false;
		else
			return true;
	}
	double getFeature(size_t i) const{
		if(__feas.find(i)==__feas.end())
			return 0.0;
		else
			return __feas.at(i);
	}
	const FeatureMap& features() const{
		return __feas;
	}
	double label() const{
		return __label;
	}
	size_t id() const{
		return __id;
	}
};

typedef std::vector<Sample> RankList;

class RBWeakRanker
{
	size_t __fid;
	double __threshold;
	double __q;//not used in this version
public:
	RBWeakRanker()
		:__fid(-1),__threshold(-1.){}
	RBWeakRanker(size_t fid, double threshold)
		: __fid(fid),__threshold(threshold){}
	RBWeakRanker(const RBWeakRanker& ranker)
		: __fid(ranker.fid()),__threshold(ranker.threshold())
		{}
	double score(const Sample& s){
		if(s.getFeature(__fid)>__threshold)
			return 1;
		else
			return 0.0;
	}
	size_t fid() const {return __fid;}
	void setFid(size_t f){__fid = f;}
	double threshold() const {return __threshold;}
	void setThreshold(double t){__threshold=t;}
};

class FileReader
{
	std::vector<RankList> __samples;
	std::vector<size_t> __features;
public:
	FileReader(){}
	FileReader(const char* filename)
	{
		this->read(filename);
	}
	const std::vector<RankList> samples() const{
		return __samples;
	}
	const std::vector<size_t> features() const{
		return __features;
	}
	void read(const char* filename){
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
			// std::cout<<fea.size()<<' '<<qid<<std::endl;
		}
		__samples.push_back(tempsample);
		__features.reserve(features.size());
		for(std::set<size_t>::iterator fi = features.begin();fi!=features.end();++fi){
			__features.push_back(*fi);
		}
	}
};

class RankBoostRanker
{
private:
	std::vector<size_t> 		__features;
	std::map<size_t,size_t> 	__feature_map;
	std::vector<std::vector<double> > __potential;
	std::vector<std::vector<std::vector<double> > > __dis; // distribution D(x_i,x_j)
	std::vector<double> __alpha;
	std::vector<std::vector<std::vector<size_t> > >__sorted_samples;
	std::vector<RankList> __samples;
	int __steps;
	int __num_threshold;
	std::vector<std::vector<double> > 	__thresholds;
	double __z_t;
	double __r_t;
	std::vector<RBWeakRanker> __rankers;

	/** private functions */
	void init();
	RBWeakRanker trainWeakRanker();
	void updatePotential();
public:
	RankBoostRanker()
		:__steps(300),__num_threshold(10),__z_t(1.),__r_t(0.)
	{}
	RankBoostRanker(const std::vector<size_t>& features, const std::vector<RankList>& samples, int steps = 300, int num_threshold = 10)
		:__samples(samples),__features(features),__steps(steps),__num_threshold(num_threshold),__z_t(1.),__r_t(0.)
	{
		for(size_t i=0;i<__features.size();++i)
			__feature_map[__features[i]]=i;
	}
	void learn();
	void learn(const std::vector<size_t>& features, const std::vector<RankList>& samples, int steps = 300, int num_threshold = 10){
		__samples = samples;
		__features = features;
		for(size_t i=0;i<__features.size();++i)
			__feature_map[__features[i]]=i;
		__steps = steps;
		__num_threshold = num_threshold;
		__z_t = 1.0;
		__r_t = 0.0;
		this->learn();
	}
	double eval(const Sample& s)
	{
		double score = 0.;
		for(int i=0;i<__rankers.size();++i)
		{
			score += __rankers[i].score(s)*__alpha[i];
		}
		return score;
	}
	void output(const char* filename);
	void loadRanker(const char* filename);
	void testFromFile(const char* filename);
};

#endif