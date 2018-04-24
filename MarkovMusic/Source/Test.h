/*
  ==============================================================================

    Test.h
    Created: 9 Apr 2018 1:05:30pm
    Author:  Yongliang He

  ==============================================================================
*/
#pragma once

#include <iostream>
#include "MarkovChain.h"
class Test: public UnitTest{
public:
    Test(): UnitTest("Markov chain test"){

    }
    void runTest() override {
        beginTest("Part 1: getStateList");
        vector<float> data {11, 33, 33, 22, 1, 1, 3, 45, 2};
        markovChain.init(data, 1);
        vector<float> output {11, 33, 22, 1, 3, 45, 2};
        for (int i = 0; i < output.size(); i++) {
            expectEquals(markovChain.stateList[i], output[i]);
        }
        data = {1};
        markovChain.init(data, 1);
        output.clear();
        output.push_back(1);
        expectEquals(markovChain.stateList[0], output[0]);


        beginTest("Part 2: getIndexForState");
        data = {11, 22, 33};
        markovChain.init(data, 1);
        for (int i = 0; i < static_cast<int>(data.size()); i++) {
            expectEquals(markovChain.getIndexForState(data[i]), i);
        }

        beginTest("Part 3: statesToKey and keyToStates");
        //! Test decimalism
        data = {11, 22, 33, 44, 55, 66, 77, 88, 99, 1010};
        markovChain.init(data, 3);
        vector<float> inn {22, 11, 99};
        expectEquals(markovChain.statesToKey(inn), 108);
        vector<float> outt = markovChain.keyToStates(108);
        for (int i = 0; i < inn.size(); i++) {
            expectEquals(outt[i], inn[i]);
        }
        inn = {11, 11, 11};
        expectEquals(markovChain.statesToKey(inn), 0);
        outt = markovChain.keyToStates(0);
        for (int i = 0; i < inn.size(); i++) {
            expectEquals(outt[i], inn[i]);
        }

        //! Test binary
        data = {1, 0, 0, 0, 0, 1, 1, 0};
        markovChain.init(data, 3);
        inn = {1, 1, 0};
        expectEquals(markovChain.statesToKey(inn), 1);
        outt = markovChain.keyToStates(1);
        for (int i = 0; i < inn.size(); i++) {
            expectEquals(outt[i], inn[i]);
        }
        inn = {0, 0, 1};
        expectEquals(markovChain.statesToKey(inn), 6);
        outt = markovChain.keyToStates(6);
        for (int i = 0; i < inn.size(); i++) {
            expectEquals(outt[i], inn[i]);
        }

        
        beginTest("Part 4: initTransitionMatrix");
        data = {0, 11, 22, 33, 44, 55, 66, 77, 88, 99};
        markovChain.init(data, 1);
        map<int, vector<int>>::iterator it;
        int t;
        for (it = markovChain.transitionMatrix.begin(), t = 0; it != markovChain.transitionMatrix.end(); it++, t++) {
            expectEquals(it->first, t);
            expectEquals(it->second[t + 1], 1);
        }
        markovChain.init(data, 3);
        int key = 0;
        for (it = markovChain.transitionMatrix.begin(), t = 0; it != markovChain.transitionMatrix.end(); it++, t++) {
            key = 0;
            for (int i = 0; i < markovChain.order; i++) {
                key += (i + t) * pow(markovChain.numState, markovChain.order - i - 1);
            }
            expectEquals(it->first, key);
            expectEquals(it->second[t + markovChain.order], 1);
        }
        
        beginTest("Part 5: getNextState");
        data = {1, 30, -2, 50, 9, 6, 100, 22, 31, 73};
        markovChain.init(data, 3);
        float result = markovChain.getNextState({1, 30, -2});
        expectEquals(result, 50.0f);
        result = markovChain.getNextState({30, -2, 50});
        expectEquals(result, 9.0f);
    }
    
private:
    MarkovChain markovChain;
};
