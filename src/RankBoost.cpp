#include "RankBoost.h"

// void RankBoostRanker::init(const std::vector<long>& features)
// {
// 	__features = features;
// }

bool pair_compare(const std::pair<int,double>& p1, const std::pair<int,double>& p2)
{
	return p1.second>p2.second;
}
void RankBoostRanker::init(){
	std::cout<<"Initializing... "<<std::endl;
	size_t total_pairs=0;
	for(size_t i=0;i<__samples.size();++i){
		const RankList& rl = __samples[i];
		for(size_t j=0;j<rl.size()-1;++j)
			for(int k=rl.size()-1;k>=j+1&&rl.at(j).label()>rl.at(k).label();--k)
				++ total_pairs;
	}
	std::cout<<"there are totoally "<<total_pairs<<" pairs"<<std::endl;
	__dis.resize(__samples.size());
	for(size_t i=0;i<__samples.size();++i)
	{
		const RankList& rl = __samples[i];
		__dis[i].resize(rl.size());
		for(size_t j=0;j<rl.size()-1;++j){
			__dis[i][j].resize(rl.size());
			for(int k=j+1;k<rl.size();++k)
				if(rl.at(j).label()>rl.at(k).label())
					__dis[i][j][k]=1.0/total_pairs;
				else
					__dis[i][j][k]=0.0;
		}
	}
	std::cout<<"Distribution done"<<std::endl;
	__potential.resize(__samples.size());
	for(int i=0;i<__samples.size();++i)
		__potential[i].resize(__samples.at(i).size());
	if(__num_threshold<0)
	{
		std::cerr<<"negative threshold number is not supported yet"<<std::endl;
	}
	else
	{
		std::vector<double> fmax(__features.size());
		std::vector<double> fmin(__features.size());
		for(size_t i=0;i<__features.size();++i){
			fmax[i]=-1e6;
			fmin[i]=1e6;
		}
		std::vector<std::vector<std::list<std::pair<int,double> > > > reorgan(__features.size());
		for(int i=0;i<__features.size();++i)
			reorgan[i].resize(__samples.size());
		for(int i=0;i<__samples.size();++i)
		{
			const RankList& rl = __samples[i];
			for(size_t j=0;j<rl.size();++j){
				const Sample& s = rl[j];
				for(std::map<size_t,double>::const_iterator iter = s.features().begin(); iter!=s.features().end();++iter)
				{
					size_t feature_id = __feature_map[iter->first];
					reorgan[feature_id][i].push_back(std::pair<int,double>(j,iter->second));
					// std::cout<<j<<' '<<iter->
					if(iter->second>fmax[feature_id])
						fmax[feature_id]=iter->second;
					if(iter->second<fmin[feature_id])
						fmin[feature_id]=iter->second;
				}
			}
		}
		std::cout<<"Reorganization done"<<std::endl;
		__thresholds.resize(__features.size());
		for(int i=0;i<__features.size();++i){
			double step = (fmax[i]-fmin[i])/__num_threshold;
			__thresholds[i].resize(__num_threshold+1);
			__thresholds[i][0]=fmax[i];
			for(int j=1;j<__num_threshold;++j)
				__thresholds[i][j] = __thresholds[i][j-1]-step;
			__thresholds[i][__num_threshold] = fmin[i]-1e8;
		}
		// std::cout<<fmin[22]<<' '<<fmax[22]<<std::endl;
		// std::cout<<fmin[38]<<' '<<fmax[38]<<std::endl;
		std::cout<<"Threshold setting done"<<std::endl;
		for(size_t i=0;i<__features.size();++i){
			for(size_t j=0;j<__samples.size();++j){
				reorgan[i][j].sort(pair_compare);
			}
		}
		std::cout<<"Sorting done"<<std::endl;
		__sorted_samples.resize(__features.size());
		for(size_t i=0;i<__features.size();++i){
			__sorted_samples[i].resize(__samples.size());
			for(size_t j=0;j<__samples.size();++j){
				__sorted_samples[i][j].reserve(reorgan[i][j].size());
				for(std::list<std::pair<int,double> >::iterator iter = reorgan[i][j].begin();iter != reorgan[i][j].end();++iter){
					__sorted_samples[i][j].push_back(iter->first);
				}
			}
		}
	}
	std::cout<<"Done"<<std::endl;
}
RBWeakRanker RankBoostRanker::trainWeakRanker()
{
	size_t bestfid = -1;
	double maxr = -10;
	double bestthresh = -1.0;
	// std::cout<<__features.size()<<std::endl;
	for(size_t i=0;i<__features.size();++i)
	{
		const std::vector<std::vector<size_t> >& sorted_index = __sorted_samples.at(i);
		std::vector<int> last(__samples.size(),-1);
		double r = 0.;
		size_t count =0;
		for(int j=0;j<__thresholds[i].size();++j){
			double t = __thresholds[i][j];
			for(int k=0;k<__samples.size();++k){
				const RankList& rl = __samples.at(k);
				const std::vector<size_t>& sk = sorted_index.at(k);
				for(int l=last[k]+1;l<sk.size();++l){
					const Sample& p = rl.at(sk[l]);
					if(p.getFeature(__features[i]) > t)
					{
						r += __potential[k][sk[l]];
						++ count;
						last[k]=l;
					}
					else{
						// last[k]=l-1;
						break;
					}
				}
			}
			// if(i==0||i==1||i==22)
			// {
			// 	// if(j==1)
			// 	// {
			// 	// 	for(int k=0;k<__samples.size();++k)
			// 	// 		std::cout<<last[k]<<std::endl;
			// 	// }
			// 	std::cout<<j<<' '<<r<<' '<<__thresholds[i][j]<<' '<<count<<' '<<__features[i]<<std::endl;
			// }
			if(r>maxr)
			{
				maxr=r;
				bestthresh=t;
				// std::cout<<i<<' '<<r<<' '<<bestthresh<<' '<<0.5*log(__z_t+__z_t*r)/(__z_t-__z_t*r)<<std::endl;
				bestfid=__features[i];
			}
		}
	}
	if(bestfid==-1)
		std::cerr<<"Error in weak trainer"<<std::endl;
	__r_t = __z_t*maxr;
	return RBWeakRanker(bestfid,bestthresh);
}

void RankBoostRanker::updatePotential(){
	for(int i=0;i<__samples.size();++i){
		const RankList& rl=__samples.at(i);
		for(int j=0;j<rl.size();++j){
			double p =0.0;
			for(int k=j+1;k<rl.size();++k)
				p+=__dis[i][j][k];
			for(int k=0;k<j;++k)
				p-=__dis[i][k][j];
			__potential[i][j] = p;
		}
	}
}

void RankBoostRanker::learn()
{
	// initialize ranker
	init();
	for(int i=0; i<__steps; ++i){
		updatePotential();
		RBWeakRanker wr(trainWeakRanker());
		__rankers.push_back(wr);
		double alpha_t = 0.5*log((__z_t+__r_t)/(__z_t-__r_t));
		// std::cout<<log(2.0)<<std::endl;
		// std::cout<<__z_t<<' '<<__r_t<<std::endl;
		__alpha.push_back(alpha_t);
		__z_t = 0.0;
		for(int i=0;i<__samples.size();++i){
			const RankList& rl= __samples.at(i);
			for(int j=0;j<rl.size()-1;++j){
				for(int k=j+1;k<rl.size();++k){
					__dis[i][j][k] = __dis[i][j][k]*exp(alpha_t*(wr.score(rl.at(k))-wr.score(rl.at(j))));
					__z_t += __dis[i][j][k];
				}
			}
		}
		for(int i=0;i<__samples.size();++i){
			for(int j=0;j<__samples[i].size()-1;++j)
				for(int k=j+1;k<__samples[i].size();++k)
					__dis[i][j][k] /= __z_t;
		}
		std::cout<<i<<'\t'<<wr.fid()<<'\t'<<wr.threshold()<<'\t'<<__r_t<<std::endl;
	}
	// for(int i=0;i<__rankers.size();++i)
	// 	std::cout<<__rankers[i].fid()<<':'<<__rankers[i].threshold()<<':'<<__alpha[i]<<std::endl;
}