// Copyright 2015 Ruimin Wang
// Author: Ruimin Wang, wangruimin_sx@qiyi.com, ruimin.wang13@gmail.com

#include "rbweakranker.h"

RBWeakRanker::RBWeakRanker()
  : __fid(-1), __threshold(-1.0) {
}

RBWeakRanker::RBWeakRanker(size_t fid, double threshold)
  : __fid(fid), __threshold(threshold) {
}

RBWeakRanker::RBWeakRanker(const RBWeakRanker& ranker)
  : __fid(ranker.fid()), __threshold(ranker.threshold()) {
}

double RBWeakRanker::score(const Sample& s) const {
  if(__fid == -1)
    return 0.;
  if(s.getFeature(__fid) > __threshold)
    return 1.;
  else
    return 0.;
}

size_t RBWeakRanker::fid() const {
  return __fid;
}

void RBWeakRanker::setFid(size_t fid) {
  __fid=fid;
}

double RBWeakRanker::threshold() const {
  return __threshold;
}

void RBWeakRanker::setThreshold(double t) {
  __threshold=t;
}
