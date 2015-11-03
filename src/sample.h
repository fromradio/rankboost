// Copyright 2015 Ruimin Wang
// Author: Ruimin Wang, wangruimin_sx@qiyi.com, ruimin.wang13@gmail.com

#ifndef SAMPLE_H__
#define SAMPLE_H__

#include <map>

// sparse representation for feature, a mapping from size_t to double
typedef std::map<size_t,double> FeatureMap;

//  Sparse representation for a given sample 
//		including the id, label(float) and a map of feature
class Sample{
public:
	// Constructing a Sample with id, label and feature map
	Sample(size_t id, double label, const FeatureMap& feas);

	// show whether the sample has a specified feature
	bool has(size_t ind) const;

	// return the value of feature 'ind', 0 is returned if
	// 'ind' is not in the map
	double getFeature(size_t ind) const;

	// getter for the feature map
	const FeatureMap& features() const;

	// getter for the label
	double label() const;

	// getter for the id
	size_t id() const;

private:

	// the id of sample
	size_t __id;

	// the label of the sample, the value is allowed to be float
	double __label;

	// the feature map, a sparse representation of feature
	// the value is set 0. as default for those are not in
	// the map
	FeatureMap __feature_map;
};
#endif