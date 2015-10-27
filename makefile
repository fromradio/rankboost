CXX = g++

all: rankboost 
rankboost: rankboost_algo.o rankboost.o src/RankBoost.cpp src/interface.cpp src/RankBoost.h
	$(CXX) rankboost.o rankboost_algo.o -o rankboost

rankboost_algo.o: src/RankBoost.cpp src/RankBoost.h
	$(CXX) -c src/RankBoost.cpp -o $@

rankboost.o: src/interface.cpp src/RankBoost.h
	$(CXX) -c src/interface.cpp -o $@