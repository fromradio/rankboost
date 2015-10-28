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
// #include <ext/hash_map>

// #if __cplusplus <= 199711L
//   #error This library needs at least a C++11 compliant compiler
// #endif

class Ranker
{
};

typedef std::map<size_t,double> FeatureMap;
class Sample
{
	// size_t __ind;
	int __label;
	FeatureMap __feas;
public:
	Sample(int label,const FeatureMap& feas)
		: __label(label),__feas(feas){}
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
	int label() const{
		return __label;
	}
};

typedef std::vector<Sample> RankList;
// class RankList
// {
// 	typedef std::map<int,double> Feature;
// 	// sparse storage of rank
// 	int __ind;
// 	std::vector<Sample> __samples;
// public:
// 	const Sample& operator[](size_t i)const{
// 		return __samples[i];
// 	}
// 	Sample& operator[](size_t i){
// 		return __samples[i];
// 	}
// };

class RBWeakRanker
{
	size_t __fid;
	double __threshold;
	double __q;//not used in this version
public:
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
	int fid() const {return __fid;}
	double threshold() const {return __threshold;}
};

class FileReader
{
	std::vector<RankList> __samples;
	std::vector<size_t> __features;
public:
	const std::vector<RankList> samples() const{
		return __samples;
	}
	const std::vector<size_t> features() const{
		return __features;
	}
	void read(const char* filename){
		std::ifstream fin(filename);
		std::string str;
		std::set<size_t> features;
		// std::map<int,RankList> samples;
		int label;
		int qid;
		int f;
		double val;
		size_t curr_qid = -1;
		bool begin = false;
		RankList tempsample;
		while(std::getline(fin,str)) {
			std::istringstream is(str);
			is>>label;
			is>>str;
			is>>qid;
			// if(samples.find(qid)==samples.end())
			// 	samples[qid].reserve(100);
			FeatureMap fea;
			while(is>>f>>val)
			{
				features.insert(f);
				fea[f] = val;
			}
			if(qid==curr_qid)
				tempsample.push_back(Sample(label,fea));
			else{
				if(begin)
					__samples.push_back(tempsample);
				else
					begin = true;
				curr_qid = qid;
				tempsample.clear();
				tempsample.push_back(Sample(label,fea));
			}
		}
		__samples.push_back(tempsample);
		std::cout<<"sample size "<<__samples.size()<<std::endl;
		std::cout<<"feature size"<<features.size()<<std::endl;
		// for(size_t i = 0 ; i < __samples.size(); ++i)
		// 	std::cout<<__samples[i].size()<<std::endl;
		__features.reserve(features.size());
		for(std::set<size_t>::iterator fi = features.begin();fi!=features.end();++fi){
			__features.push_back(*fi);
		}
		// for(std::map<int,RankList>::iterator ri=samples.begin();ri!=samples.end();++ri){
		// 	__samples.push_back(ri->second);
		// }
	}
};

class RankBoostRanker
	: public Ranker
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
	RankBoostRanker(std::vector<size_t> features, std::vector<RankList> samples, int steps = 300, int num_threshold = 10)
		:__samples(samples),__features(features),__steps(steps),__num_threshold(num_threshold),__z_t(1.),__r_t(0.)
	{
		for(size_t i=0;i<__features.size();++i)
			__feature_map[__features[i]]=i;
	}
	void learn();
	double eval(const Sample& s)
	{
		double score = 0.;
		for(FeatureMap::const_iterator iter = s.features().begin(); iter != s.features().end(); ++iter){
			score += __alpha[iter->first]*__rankers[iter->first].score(s);
		}
		return score;
	}
	
};

#endif