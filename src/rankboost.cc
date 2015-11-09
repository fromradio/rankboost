// Copyright 2015 Ruimin Wang
// Author: Ruimin Wang, wangruimin_sx@qiyi.com

#include "rankboost.h"
#include "filereader.h"

bool pair_compare(const std::pair<int,double>& p1, const std::pair<int,double>& p2) {
  return p1.second > p2.second;
}

RankBoostRanker::RankBoostRanker()
  : __steps(300), __num_threshold(10), __z_t(1.), __r_t(0.) {
}

RankBoostRanker::RankBoostRanker(const std::vector<RankList>& samples, const std::vector<size_t>& features, int steps, int num_threshold)
  : __samples(samples),
    __features(features),
    __steps(steps),
    __num_threshold(num_threshold),
    __z_t(1.),
    __r_t(0.) {
}

void RankBoostRanker::init() {
  std::cout << "Initializing... " << std::endl;
  size_t total_pairs = 0;
  for(size_t i = 0; i < __samples.size(); ++i) {
    const RankList& rl = __samples[i];
    for(size_t j = 0; j < rl.size()-1; ++j)
      for(int k = rl.size()-1; k >= j + 1 && rl.at(j).label() > rl.at(k).label(); -- k)
        ++ total_pairs;
  }
  std::cout << "there are totoally " << total_pairs << " pairs" << std::endl;
  __distribution.resize(__samples.size());
  for(size_t i = 0; i < __samples.size(); ++ i) {
    const RankList& rl = __samples[i];
    __distribution[i].resize(rl.size());
    for(size_t j = 0; j < rl.size() - 1; ++j) {
      __distribution[i][j].resize(rl.size());
      for(int k = j + 1; k < rl.size(); ++ k)
        if(rl.at(j).label() > rl.at(k).label())
          __distribution[i][j][k] = 1.0/total_pairs;
        else
          __distribution[i][j][k] = 0.0;
    }
  }
  std::cout << "Distribution done" << std::endl;
  __potential.resize(__samples.size());
  for(int i = 0; i < __samples.size(); ++ i)
    __potential[i].resize(__samples.at(i).size());
  if(__num_threshold < 0) {
    std::cerr << "negative threshold number is not supported yet" << std::endl;
  }
  else {
    std::vector<double> fmax(__features.size());
    std::vector<double> fmin(__features.size());
    for(size_t i = 0; i < __features.size(); ++i) {
      fmax[i] = -1e6;
      fmin[i] = 0;// sparse
    }
    __penalties.resize(__features.size(),0);
    std::vector<std::vector<std::list<std::pair<int,double> > > > reorgan(__features.size());
    for(int i = 0; i < __features.size(); ++ i)
      reorgan[i].resize(__samples.size());
    for(int i = 0; i < __samples.size(); ++ i) {
      const RankList& rl = __samples[i];
      for(size_t j = 0; j<rl.size(); ++ j) {
        const Sample& s = rl[j];
        for(std::map<size_t,double>::const_iterator iter = s.features().begin(); iter != s.features().end(); ++ iter) {
          size_t feature_id = __feature_map[iter->first];
          reorgan[feature_id][i].push_back(std::pair<int,double>(j,iter->second));
          if(iter->second > fmax[feature_id])
            fmax[feature_id] = iter->second;
          if(iter->second < fmin[feature_id])
            fmin[feature_id] = iter->second;
        }
      }
    }
    std::cout << "Reorganization done" << std::endl;
    __thresholds.resize(__features.size());
    std::cout << __features.size() << std::endl;
    for(int i = 0; i < __features.size(); ++ i) {
      double step = (fmax[i] - fmin[i]) / __num_threshold;
      __thresholds[i].resize(__num_threshold + 1);
      __thresholds[i][0] = fmax[i];
      for(int j = 1; j < __num_threshold; ++ j)
        __thresholds[i][j] = __thresholds[i][j - 1] - step;
      __thresholds[i][__num_threshold] = fmin[i] - 1e8;
    }
    std::cout << "Threshold setting done" << std::endl;
    for(size_t i = 0; i < __features.size(); ++ i) {
      for(size_t j = 0; j < __samples.size(); ++ j) {
        reorgan[i][j].sort(pair_compare);
        __penalties[i] += reorgan[i][j].size();
      }
    }
    std::cout << "Sorting done" << std::endl;
    __sorted_samples.resize(__features.size());
    for(size_t i = 0; i < __features.size(); ++ i) {
      __sorted_samples[i].resize(__samples.size());
      for(size_t j = 0; j < __samples.size(); ++ j) {
        __sorted_samples[i][j].reserve(reorgan[i][j].size());
        for(std::list<std::pair<int,double> >::iterator iter = reorgan[i][j].begin(); iter != reorgan[i][j].end(); ++ iter) {
          __sorted_samples[i][j].push_back(iter->first);
        }
      }
    }
  }
  __whole_number = 0;
  for(size_t i = 0; i < __samples.size(); ++ i) {
    __whole_number += __samples[i].size();
  }
  __ratio_feature = 0.033333333;
  __feature_cutoff = static_cast<size_t>(__ratio_feature*__whole_number);
  std::cout << "Done" << std::endl;
}
RBWeakRanker RankBoostRanker::trainWeakRanker() {
  size_t bestfid = -1;
  double maxr = -10;
  double mmr = -10;
  double bestthresh = -1.0;
  for(size_t i = 0; i < __features.size(); ++ i) {
    if(__penalties[i] < 5 || __penalties[i] > __feature_cutoff)
      continue;
    if(__chosen_features.find(__features[i]) != __chosen_features.end())
      continue;
    const std::vector<std::vector<size_t> >& sorted_index = __sorted_samples.at(i);
    std::vector<int> last(__samples.size(), -1);
    double r = 0.;
    size_t count = 0;
    for(int j = 0; j < __thresholds[i].size(); ++ j) {
      double t = __thresholds[i][j];
      size_t penalty = 0;
      for(int k = 0; k < __samples.size(); ++ k) {
        const RankList& rl = __samples.at(k);
        const std::vector<size_t>& sk = sorted_index.at(k);
        penalty += sk.size();
        for(int l = last[k] + 1; l < sk.size(); ++ l) {
          const Sample& p = rl.at(sk[l]);
          if(p.getFeature(__features[i]) > t) {
            r += __potential[k][sk[l]];
            ++ count;
            last[k] = l;
          }
          else{
            break;
          }
        }
      }
      if(r > maxr) {
        maxr = r;
        mmr = r;
        bestthresh = t;
        bestfid = __features[i];
      }
    }
  }
  if(bestfid == -1)
    std::cerr << "Error in weak trainer" << std::endl;
  __r_t = __z_t * maxr;
  return RBWeakRanker(bestfid,bestthresh);
}

void RankBoostRanker::updatePotential() {
  for(int i = 0; i < __samples.size(); ++ i) {
    const RankList& rl=__samples.at(i);
    for(int j = 0; j < rl.size(); ++ j) {
      double p = 0.0;
      for(int k = j + 1; k < rl.size(); ++ k)
        p += __distribution[i][j][k];
      for(int k = 0; k < j; ++ k)
        p-=__distribution[i][k][j];
      __potential[i][j] = p;
    }
  }
}

void RankBoostRanker::learn()
{
  // initialize ranker
  init();
  // iteration
  for(int s = 0; s < __steps; ++ s){
    updatePotential();
    RBWeakRanker wr(trainWeakRanker());
    __chosen_features.insert(wr.fid());
    __rankers.push_back(wr);
    double alpha_t = 0.5 * log((__z_t + __r_t)/(__z_t - __r_t));
    __alpha.push_back(alpha_t);
    __z_t = 0.0;
    for(int i = 0; i < __samples.size(); ++ i) {
      const RankList& rl =  __samples.at(i);
      for(int j = 0; j < rl.size() - 1; ++ j) {
        for(int k = j + 1; k < rl.size(); ++ k) {
          __distribution[i][j][k] = __distribution[i][j][k] * exp(alpha_t*(wr.score(rl.at(k)) - wr.score(rl.at(j))));
          __z_t += __distribution[i][j][k];
        }
      }
    }
    for(int i = 0; i < __samples.size(); ++ i) {
      for(int j = 0; j < __samples[i].size() - 1; ++ j)
        for(int k = j + 1; k < __samples[i].size(); ++ k)
          __distribution[i][j][k] /= __z_t;
    }
    std::cout << s << '\t' << wr.fid() << '\t' << wr.threshold() << '\t' << alpha_t << '\t' << __r_t << std::endl;
  }
}

void RankBoostRanker::learn(const std::vector<size_t>& features,
  const std::vector<RankList>& samples,
  int steps,
  int num_threshold) {
  __samples = samples;
  __features = features;
  __feature_map.clear();
  for(size_t i = 0; i < __features.size(); ++ i)
    __feature_map[__features[i]] = i;
  __steps = steps;
  __num_threshold = num_threshold;
  __z_t = 1.;
  __r_t = 0.;
  learn();
}

double RankBoostRanker::eval(const Sample& s)
{
  double score = 0.;
  for(size_t i = 0; i < __rankers.size(); ++ i)
    score += __rankers[i].score(s) * __alpha[i];
  return score;
}

void RankBoostRanker::output(const char* filename)
{
  std::ofstream fout(filename);
  fout << __rankers.size() << std::endl;
  for(int i = 0; i < __rankers.size(); ++ i) {
    fout << __rankers[i].fid() << ' ' << __rankers[i].threshold() << ' ' << __alpha[i] << std::endl;
  }
}

void RankBoostRanker::loadRanker(const char* filename)
{
  std::cout << "loading ranker" << std::endl;
  std::ifstream fin(filename);
  if(! fin) {
    std::cerr << "Warning: loading null file, ranker loading skips" << std::endl;
    return;
  }
  int size;
  fin >> size;
  double t,alpha;
  int n;
  __rankers.reserve(size);
  __alpha.resize(size);
  for(int i=0;i<size;++i) {
    fin >> n >> t >> alpha;
    __rankers.push_back(RBWeakRanker(n,t));
    __alpha[i] = alpha;
  }
}

void RankBoostRanker::testFromFile(const char*filename)
{
  std::cout << "loading test file" << std::endl;
  FileReader fr(filename);
  std::cout << "done" << std::endl;
  const std::vector<RankList>& samples = fr.samples();
  const std::vector<size_t>& features = fr.features();
  double rmse = 0;
  size_t count = 0;
  for(int i = 0; i < samples.size(); ++ i) {
    count += samples[i].size();
    for(int j = 0; j < samples[i].size(); ++ j) {
      double score = this->eval(samples[i][j]);
      rmse += (score - samples[i][j].label())*(score - samples[i][j].label());
    }
  }
  std::cout << rmse << ' ' << count << std::endl;
  std::cout << "TEST RMSE IS " << sqrt(rmse / count) << std::endl;
}
