CXX = g++

OBJ_DIR = ./obj

SRC_DIR = ./src

all: clean rankboost rbtest
rankboost: $(OBJ_DIR)/rankboost_algo.o $(OBJ_DIR)/rankboost.o $(OBJ_DIR)/sample.o $(OBJ_DIR)/rbweakranker.o $(OBJ_DIR)/filereader.o
	$(CXX) $(OBJ_DIR)/rankboost.o $(OBJ_DIR)/rankboost_algo.o $(OBJ_DIR)/sample.o $(OBJ_DIR)/rbweakranker.o $(OBJ_DIR)/filereader.o -o rankboost

rbtest: $(OBJ_DIR)/rankboost_test.o $(OBJ_DIR)/prediction.o $(OBJ_DIR)/resultjudge.o $(OBJ_DIR)/rankboost_algo.o $(OBJ_DIR)/rankboost.o $(OBJ_DIR)/sample.o $(OBJ_DIR)/rbweakranker.o $(OBJ_DIR)/filereader.o
	$(CXX) $(OBJ_DIR)/rankboost_test.o $(OBJ_DIR)/prediction.o $(OBJ_DIR)/resultjudge.o $(OBJ_DIR)/rankboost_algo.o $(OBJ_DIR)/sample.o $(OBJ_DIR)/rbweakranker.o $(OBJ_DIR)/filereader.o -o $@

$(OBJ_DIR)/rankboost_test.o: $(SRC_DIR)/rankboost_test.cpp $(SRC_DIR)/Prediction.h
	$(CXX) -c $(SRC_DIR)/rankboost_test.cpp -o $@
$(OBJ_DIR)/prediction.o: $(SRC_DIR)/Prediction.cpp $(SRC_DIR)/Prediction.h
	$(CXX) -c $(SRC_DIR)/Prediction.cpp -o $@
$(OBJ_DIR)/resultjudge.o: $(SRC_DIR)/ResultJudge.cpp $(SRC_DIR)/ResultJudge.h
	$(CXX) -c $(SRC_DIR)/ResultJudge.cpp -o $@

$(OBJ_DIR)/rankboost_algo.o: $(SRC_DIR)/rankboost.cc $(SRC_DIR)/rankboost.h
	$(CXX) -c $(SRC_DIR)/rankboost.cc -o $@

$(OBJ_DIR)/rankboost.o: $(SRC_DIR)/interface.cc $(SRC_DIR)/rankboost.h $(SRC_DIR)/rankboost.cc
	$(CXX) -c $(SRC_DIR)/interface.c -o $@

$(OBJ_DIR)/sample.o: $(SRC_DIR)/sample.h $(SRC_DIR)/sample.cc
	$(CXX) -c $(SRC_DIR)/sample.cc -o $@

$(OBJ_DIR)/rbweakranker.o: $(SRC_DIR)/rbweakranker.h $(SRC_DIR)/rbweakranker.cc
	$(CXX) -c $(SRC_DIR)/rbweakranker.cc -o $@

$(OBJ_DIR)/filereader.o: $(SRC_DIR)/filereader.h $(SRC_DIR)/filereader.cc
	$(CXX) -c $(SRC_DIR)/filereader.cc -o $@

.PHONY: clean

clean:
	rm -f $(OBJ_DIR)/*.o