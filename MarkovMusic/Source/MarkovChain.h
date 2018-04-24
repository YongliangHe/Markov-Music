/*
  ==============================================================================

    MarkovChain.h
    Created: 9 Apr 2018 1:13:07pm
    Author:  Yongliang He

  ==============================================================================
*/

#pragma once

#include <stdio.h>
#include <map>
#include <vector>
using std::vector;
using std::map;

class MarkovChain {
public:
    MarkovChain();
    virtual ~MarkovChain();
    void init(vector<float> data, int order);
    void reset();
    vector<float> getSequences(int numIter);
private:
#ifdef JUCE_DEBUG
    friend class Test;
#endif
    int getIndexForState(float state);
    vector<float> getStateList(vector<float> data);
    void initTransitionMatrix(vector<float> data);
    int statesToKey(vector<float> states);
    vector<float> keyToStates(int key);
    vector<float> getRandomStates();
    int getRandomKey();
    float getNextState(vector<float> states);
    
    bool isInitialized;
    int order;
    int numState;
    vector<float> stateList;
    map<int, vector<int>> transitionMatrix;    
};
