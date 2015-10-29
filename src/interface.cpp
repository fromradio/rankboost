#include "RankBoost.h"
#include <unistd.h>


void printUsage(){
	std::cout<<"rankboost usage:\n";
	std::cout<<"	-f training file name\n";
	std::cout<<"	-t testing file name\n";
	std::cout<<"	-o output file name storing the model\n";
	std::cout<<"	-s total steps (300 as default)\n";
	std::cout<<"	-n number of thresholds (10 as default)\n";
}
int main(int argc, char *argv[]){
	if(argc==1){
		printUsage();
		return 0;
	}
	int oc;
	char *b_opt_arg;
	char *filename = NULL;
	char *testfile = NULL;
	char *outfile = NULL;
	char *loadfile = NULL;
	int steps = 300; // number of steps
	int num = 10; // number of thresholds
	while((oc=getopt(argc,argv,"f:t:o:s:n:l:"))!=-1){
		switch(oc){
			case 'f':
				// train from a file
				filename = optarg;
				break;
			case 't':
				// test from a file
				testfile = optarg;
				break;
			case 'o':
				// output the model
				outfile = optarg;
				break;
			case 's':
				// steps
				steps = atoi(optarg);
				break;
			case 'n':
				// number of threshold
				num = atoi(optarg);
				break;
			case 'l':
				// load model
				loadfile = optarg;
				break;
			case '?':
				// output usage
				printUsage();
				break;
		}
	}
	if(filename==NULL&&loadfile==NULL){
		// no model, just skip all following steps
		return 0;
	}
	FileReader fr;
	RankBoostRanker rbr;
	if(filename){
		fr.read(filename);
		rbr.learn(fr.features(),fr.samples(),steps,num);
	}
	else{
		rbr.loadRanker(loadfile);
	}
	if(testfile!=NULL)
		rbr.testFromFile(testfile);
	if(outfile!=NULL)
		rbr.output(outfile);
}