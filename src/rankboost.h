// Copyright 2015 Ruimin Wang
// Author: Ruimin Wang, wangruimin_sx@qiyi.com

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

#include "sample.h"
#include "rbweakranker.h"

// declaration of RankList
typedef std::vector<Sample> RankList;

// class RankBoostRanker learns a RankBoost model from given data
//	to better predict data from iqiyi, I modify the origin RankBoost
//	algorithm a litter.
//		Difference with origin RankBoost algorithm:
//			In current version, I force that a feature can only 
//		appear once. Meanwhile some frequent features are filtered.
//  Calling approach:
// 		either user defines a ranker with samples, features and 
//  other information like: RankBoostRanker(samples,features,steps,
//  num_threshold) and learn them or directly learn from these
//  information by calling learn function.
class RankBoostRanker {

private:
	/** private functions */
	void init();
	RBWeakRanker trainWeakRanker();
	void updatePotential();

public:

	// steps are set 300 as default. threshold number is set 10
	// z is 1.0, r is 0.
	RankBoostRanker();

	// comprehensive constructor 
	RankBoostRanker(
		const std::vector<RankList>& samples, 
		const std::vector<size_t>& features, 
		int steps, 
		int num_threshold
		);

	// kernel function:
	//	learning the whole ranker
	void learn();

	// learning algorithm given samples, features and other
	// information
	void learn(
		const std::vector<size_t>& features, 
		const std::vector<RankList>& samples, 
		int steps, 
		int num_threshol);

	// evaluation a given sample
	//	a float number is returned like regression
	double eval(const Sample& s);

	// saving rankboost ranker in a file
	void output(const char* filename);

	// loaing rankboost ranker from a file
	void loadRanker(const char* filename);

	// comopute rmse of a file
	void testFromFile(const char* filename);

private:

	// the whole number of samples
	size_t 						__whole_number;

	// the ration of feature
	double 						__ratio_feature;

	// feature cutoff
	size_t 						__feature_cutoff;

	// the feature vector that return the index of a feature
	std::vector<size_t> 		__features;

	// reverse map from index of feature to index in the vector
	std::map<size_t,size_t> 	__feature_map;

	// penalties of features, i.e. how many times feature appears
	// in the samples
	std::vector<size_t> 		__penalties;

	// the potential of samples, making computation faster
	std::vector<std::vector<double> > __potential;

	// set storing indices of chosen features
	std::set<size_t> 			__chosen_features;

	// current distribution of pairs
	std::vector<std::vector<std::vector<double> > > __distribution;

	// the weight of weak rankers
	std::vector<double> __alpha;

	// sorted samples according to different feature values
	std::vector<std::vector<std::vector<size_t> > >__sorted_samples;

	// the samples
	//		samples is a vector of RankList, each list is sorted
	// according to the label of sample. 
	std::vector<RankList> __samples;

	// number of steps 
	int __steps;

	// how many thresholds are using
	int __num_threshold;

	// all thresholds
	std::vector<std::vector<double> > 	__thresholds;

	// the normalization of current list
	double __z_t;

	// the estimated error
	double __r_t;

	// the rankers
	std::vector<RBWeakRanker> __rankers;

};

#endif