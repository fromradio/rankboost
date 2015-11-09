#ifndef RBWEAK_RANKER_H__
#define RBWEAK_RANKER_H__

//  Weaker ranker for rankboost algorithm
//    the ranker classify given sample according to a threshold
//  along a specified feature. in the ranker we stored fid which
//  is the index of the feature and the corresponding threshold.
//  we can score a given sample given fid and threshold
#include <cstddef>

#include "sample.h"

class RBWeakRanker {
public:
  // default constructor, fid is set as -1, threshold is set as 0.
  RBWeakRanker();

  // constructor setting fid and threshold
  RBWeakRanker(size_t fid, double threshold);

  // copy constructor
  RBWeakRanker(const RBWeakRanker& ranker);

  // score a given sample, 0. is returned if fid equals to -1
  double score(const Sample& s) const;

  // getter and setter of fid
  size_t fid() const;
  void setFid(size_t);

  // getter and setter of threshold
  double threshold() const;
  void setThreshold(double t);

private:

  // the index of feature
  size_t __fid;

  // the threshold, the ranker will give a 1.0 score if the value
  // of the corresponding feature is larger than threshold
  double __threshold;


  double __q;   // TODO
};


#endif
