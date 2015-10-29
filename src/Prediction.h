#ifndef PREDICTION_H__
#define PREDICTION_H__
#include "RankBoost.h"

struct ResultNode
{
	double val_true;
	double val_predict;
	ResultNode(double t,double p):val_true(t),val_predict(p){}
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
	const std::list<ResultNode>& results() const;
};

#endif