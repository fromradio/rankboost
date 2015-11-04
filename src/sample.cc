// Copyright 2015 Ruimin Wang
// Author: Ruimin Wang, wangruimin_sx@qiyi.com, ruimin.wang13@gmail.com

#include "sample.h"

Sample::Sample(size_t id, double label, const FeatureMap& feas)
	: __id(id), __label(label), __feature_map(feas) {
}

bool Sample::has(size_t ind) const {
	if(__feature_map.find(ind) == __feature_map.end())
		return false;
	else
		return true;
}

double Sample::getFeature(size_t ind) const {
	if(__feature_map.find(ind) == __feature_map.end())
		return 0.;
	else
		return __feature_map.at(ind);
}

const FeatureMap& Sample::features() const {
	return __feature_map;
}

double Sample::label() const {
	return __label;
}

size_t Sample::id() const {
	return __id;
}