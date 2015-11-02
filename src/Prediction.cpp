#include "Prediction.h"

Prediction::Prediction()
	:__exist_ranker(false),__exist_samples(false){}

void Prediction::loadRanker(const char* filename){
	__ranker.loadRanker(filename);
	__exist_ranker=true;
}

void Prediction::loadData(const char* datafile){
	std::ifstream datastream(datafile);
	if(!datastream){
		std::cerr<<"Warning: empty data file"<<std::endl;
		return;
	}
	std::string str,tempstr;
	double label;
	size_t id;
	int qid;
	size_t feature;
	double val;
	while(std::getline(datastream,str)){
		std::istringstream is(str);
		is>>id;
		is>>label;
		is>>tempstr;
		is>>qid;
		FeatureMap fea;
		while(is>>feature>>val)
			fea[feature]=val;
		__samples.push_back(Sample(id,label,fea));
	}
	__exist_samples=true;
}

void Prediction::predict(){
	if(!__exist_samples||!__exist_ranker){
		std::cerr<<"Ranker or Test data is missing, prediction ends"<<std::endl;
		return;
	}
	size_t i=0;
	for(std::list<Sample>::iterator sit=__samples.begin(); sit!=__samples.end(); ++sit){
		__results.push_back(ResultNode(sit->label(),__ranker.eval(*sit),sit->id()));
	}
}

const std::list<ResultNode>& Prediction::results() const{
	return __results;
}

bool compare_result(const ResultNode& n1, const ResultNode& n2){
	return n1.val_predict>n2.val_predict;
}
void Prediction::outputOrdered(const char* filename){
	std::ofstream fout(filename);
	__results.sort(compare_result);
	for(std::list<ResultNode>::iterator iter=__results.begin();iter!=__results.end();++iter){
		fout<<iter->id<<' '<<iter->val_predict<<std::endl;
	}
}

void Prediction::outputResult(const char* filename){
	std::ofstream fout(filename);
	fout<<"label"<<' '<<"score"<<std::endl;
	for(std::list<ResultNode>::iterator iter=__results.begin();iter!=__results.end();++iter){
		fout<<iter->id<<' '<<iter->val_predict<<std::endl;
	}
}