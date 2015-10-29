CXX = g++

all: rankboost rbtest
rankboost: ./obj/rankboost_algo.o ./obj/rankboost.o src/RankBoost.cpp src/interface.cpp src/RankBoost.h
	$(CXX) ./obj/rankboost.o ./obj/rankboost_algo.o -o rankboost

rbtest: ./obj/rankboost_test.o ./obj/prediction.o ./obj/resultjudge.o ./obj/rankboost_algo.o
	$(CXX) ./obj/rankboost_test.o ./obj/prediction.o ./obj/resultjudge.o ./obj/rankboost_algo.o -o $@

./obj/rankboost_test.o: src/rankboost_test.cpp src/Prediction.h
	$(CXX) -c src/rankboost_test.cpp -o $@
./obj/prediction.o: src/Prediction.cpp src/Prediction.h
	$(CXX) -c src/Prediction.cpp -o $@
./obj/resultjudge.o: src/ResultJudge.cpp src/ResultJudge.h
	$(CXX) -c src/ResultJudge.cpp -o $@

./obj/rankboost_algo.o: src/RankBoost.cpp src/RankBoost.h
	$(CXX) -c src/RankBoost.cpp -o $@

./obj/rankboost.o: src/interface.cpp src/RankBoost.h
	$(CXX) -c src/interface.cpp -o $@