#include "RankBoost.h"
#include "ResultJudge.h"
#include "Prediction.h"
#include <unistd.h>

void printUsage(){

}

int main(int argc, char* argv[]){
	/** must load a model file */
	if(argc==1){
		printUsage();
		return 0;
	}
	int oc;
	char *modelfilename;
	char *testfilename;
	int pretype = 0;
	while((oc=getopt(argc,argv,"m:t:n:"))!=-1){
		switch(oc){
			case 'm':
				modelfilename=optarg;
				break;
			case 't':
				testfilename=optarg;
				break;
			case 'n':
				pretype=atoi(optarg);
				break;
			case '?':
				printUsage();
				break;
		}
	}
	if(modelfilename==NULL||testfilename==NULL){
		return 0;
	}
	Prediction pre;
	pre.loadRanker(modelfilename);
	pre.loadData(testfilename);
	pre.predict();
	ResultJudge rj(pre.results());
	switch(pretype){
		case 0:
			// rmse
			std::cout<<"RMSE error "<<rj.computeRMSE()<<std::endl;
			break;
		case 1:
			std::cout<<"NDCG@20 error"<<rj.computeNDCG(20)<<std::endl;
			break;
		case 2:
			std::cout<<"NDCG@100 error"<<rj.computeNDCG(100)<<std::endl;
			break;
		case 3:
			std::cout<<"NDCG of whole list"<<rj.computeNDCG(-1)<<std::endl;
		case 4:
			std::cout<<"NDCG@10 error "<<rj.computeNDCG(10)<<std::endl;
	}
}