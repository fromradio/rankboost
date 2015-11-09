#ifndef PREDICTION_H__
#define PREDICTION_H__
#include "rankboost.h"


// A node storing prediction result
//  val_true is the true score of the node
//  val_predict is the prediction score
//  id is the id of the node
struct ResultNode {
  double val_true;
  double val_predict;
  size_t id;
  ResultNode(double t, double p, size_t i): val_true(t), val_predict(p), id(i) {
  }
};


// Prediction of the ranker
class Prediction {
public:

  //
  Prediction();

  // load the ranker
  void loadRanker(const char* filename);

  // load the data
  void loadData(const char* datafile);

  // using the ranker to predict the data
  //   the results consists of the id of data, true label
  //  and predicted value
  void predict();

  // output the result with order by prediction
  void outputOrdered(const char* filename);

  // output the whole results
  void outputResult(const char* filename);

  // getter for the prediction result
  const std::list<ResultNode>& results() const;
private:

  // a rankboost ranker used for prediction
  RankBoostRanker __ranker;

  // a list of samples
  std::list<Sample> __samples;

  // the results
  std::list<ResultNode> __results;

  // whether the ranker is intialzed or obtained
  bool __exist_ranker;

  // whethere the data is loaded
  bool __exist_samples;

};

#endif
