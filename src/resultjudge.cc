#include "resultjudge.h"

bool compare_acc_predict(const ResultNode& n1, const ResultNode& n2){
	if(n1.val_predict>n2.val_predict)
		return true;
	else if(n1.val_predict<n2.val_predict)
		return false;
	else
		return n1.id<n2.id;
}

bool compare_acc_true(const ResultNode& n1, const ResultNode& n2){
	if(n1.val_true>n2.val_true)
		return true;
	else if(n1.val_true<n2.val_true)
		return false;
	else
		return n1.id<n2.id;
}

double ResultJudge::dcg(int n){
	__list_node.sort(compare_acc_predict);
	size_t i=1;
	double result=0;
	double fac = log(2.);
	for(Results::const_iterator ni=__list_node.begin(); ni!=__list_node.end(); ++ni){
		result+=(pow(2.0,ni->val_true)-1)*fac/log(1.+i);
		++i;
		if(i>n)
			break;
	}
	return result;
}

double ResultJudge::mdcg(int n){
	__list_node.sort(compare_acc_true);
	size_t i=1;
	double result=0;
	double fac = log(2.);
	for(Results::const_iterator ni=__list_node.begin(); ni!=__list_node.end(); ++ni){
		result+=(pow(2.0,ni->val_true)-1)*fac/log(1.+i);
		++i;
		if(i>n)
			break;
	}
	std::cout<<result<<std::endl;
	return result;
}

double ResultJudge::computeRMSE(){
	// rmse = sqrt(E(theta-theta_*))
	double result=0;
	double ymin = 1e8;
	double ymax = -1e8;
	for(Results::const_iterator iter=__list_node.begin(); iter!=__list_node.end(); ++iter){
		result+=(iter->val_true-iter->val_predict)*(iter->val_true-iter->val_predict);
		if(ymin>iter->val_true)
			ymin = iter->val_true;
		if(ymax<iter->val_true)
			ymax = iter->val_true;
	}
	return sqrt(result/__list_node.size())/(ymax-ymin);
}

double ResultJudge::computeNDCG(int n){
	if(n==-1){
		n = __list_node.size();
	}
	return dcg(n)/mdcg(n);
}

double ResultJudge::computeMAP(int n){
	if(n<=0){
		if(n==-1)
			n=__list_node.size();
		else{
			std::cerr<<"Error: wrong input MAP number"<<std::endl;
			return 0.;
		}
	}
	std::cout<<"here"<<std::endl;
	Results copy_list(__list_node);
	copy_list.sort(compare_acc_predict);
	__list_node.sort(compare_acc_true);
	size_t i=1;
	double result=0.;
	Results::iterator position;
	std::set<size_t> inside_n;
	for(Results::const_iterator iter=__list_node.begin();iter!=__list_node.end();++iter){
		if((position = std::lower_bound(copy_list.begin(),copy_list.end(),*iter,compare_acc_predict))!=copy_list.end()){
			size_t dis = std::distance(copy_list.begin(),position)+1;
			std::cout<<dis<<' ';
			if(dis<=n)
				inside_n.insert(dis);
		}
		++i;
		if(i>n)
			break;
	}
	std::cout<<std::endl;
	std::cout<<"here"<<std::endl;
	i=1;
	std::cout<<inside_n.size()<<std::endl;
	for(std::set<size_t>::iterator ii = inside_n.begin();ii!=inside_n.end();++ii){
		std::cout<<(*ii)<<' ';
		result+=(double)i/(*ii);
		++i;
	}
	std::cout<<std::endl;
	return result/n;
}

double ResultJudge::computeAverageRecall(int n){
	if(n<=0){
		if(n==-1)
			n=__list_node.size();
		else{
			std::cerr<<"Error: wrong input MAP number"<<std::endl;
			return 0.;
		}
	}
	Results copy_list(__list_node);
	copy_list.sort(compare_acc_true);
	__list_node.sort(compare_acc_predict);
	size_t i=1;
	double result=0.;
	Results::iterator position;
	std::set<size_t> inside_n;
	for(Results::const_iterator iter=__list_node.begin(); iter!=__list_node.end(); ++iter){
		if((position = std::lower_bound(copy_list.begin(),copy_list.end(),*iter,compare_acc_true))!=copy_list.end()){
			size_t dis = std::distance(copy_list.begin(),position)+1;
			if(dis<=n)
				inside_n.insert(dis);
		}
		++i;
		if(i>n)
			break;
	}
	i=1;
	std::cout<<inside_n.size()<<std::endl;
	for(std::set<size_t>::iterator ii = inside_n.begin();ii!=inside_n.end();++ii){
		result+=(double)i/(*ii);
		++i;
	}
	return result/n;
}
