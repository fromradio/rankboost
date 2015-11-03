#ifndef PREDICTION_H__
#define PREDICTION_H__
#include "RankBoost.h"

struct ResultNode
{
	double val_true;
	double val_predict;
	size_t id;
	ResultNode(double t,double p,size_t i):val_true(t),val_predict(p),id(i){}
};

class Prediction
{
private:
	RankBoostRanker __ranker;
	std::list<Sample> __samples;
	std::list<ResultNode> __results;
	bool __exist_ranker;
	bool __exist_samples;
public:
	Prediction();
	void loadRanker(const char* filename);
	void loadData(const char* datafile);
	void predict();
	void outputOrdered(const char* filename);
	void outputResult(const char* filename);
	const std::list<ResultNode>& results() const;
};

#endif