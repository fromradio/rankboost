#include "RankBoost.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
	int oc;
	char *b_opt_arg;
	char *filename = NULL;
	char *testfile = NULL;
	char *outfile = NULL;
	int steps = 300;
	int num = 10;// number of thresholds
	while((oc=getopt(argc,argv,"f:t:o:s:n:"))!=-1)
	{
		switch(oc)
		{
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
			case '?':
				// output usage
				std::cout<<"rankboost usage:\n";
				std::cout<<"	-f training file name\n";
				std::cout<<"	-t testing file name\n";
				std::cout<<"	-o output file name storing the model\n";
				std::cout<<"	-s total steps (300 as default)\n";
				std::cout<<"	-n number of thresholds (10 as default)\n";
				break;
		}
	}
	if(filename==NULL)
		return 0;
	FileReader fr;
	fr.read(filename);
	RankBoostRanker rbr(fr.features(),fr.samples(),steps,num);
	rbr.learn();
	if(outfile!=NULL)
		rbr.output(outfile);
}