#ifndef RESULT_JUDGE_H__
#define RESULT_JUDGE_H__

#include "RankBoost.h"
#include "Prediction.h"


class ResultJudge
{
private:
	typedef std::list<ResultNode> Results;
	Results __list_node;

	// private functions
	double dcg(int n);
	double mdcg(int n);
public:
	ResultJudge(const Results & listnode)
		:__list_node(listnode){ }
	double computeRMSE();
	double computeNDCG(int n);
	double computeMAP(int n);
	double computeAverageRecall(int n);
};

#endif