/*
  ==============================================================================

    MarkovChain.cpp
    Created: 9 Apr 2018 1:13:07pm
    Author:  Yongliang He

  ==============================================================================
*/
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <iterator>
#include "MarkovChain.h"
using std::cout;
using std::endl;

MarkovChain::MarkovChain(): isInitialized(false),
order(0),
numState(0)
{
    
}

MarkovChain::~MarkovChain(){
};

void MarkovChain::init(vector<float> data, int order) {
    isInitialized = true;
    reset();
    this->order = order;
    stateList = getStateList(data);
    numState = static_cast<int>(stateList.size());
    initTransitionMatrix(data);
}

void MarkovChain::reset() {
    isInitialized = false;
    this->order = 0;
    stateList.clear();
    numState = 0;
    transitionMatrix.clear();
}

vector<float> MarkovChain::getSequences(int numIter) {
    vector<float> res = getRandomStates();
    vector<float> currentStates = res;
    float nextState = 0.0f;
    int key = 0;
    for (int i = 0; i < numIter; i++) {
        key = statesToKey(currentStates);
        if (transitionMatrix.find(key) == transitionMatrix.end()) {
            currentStates = getRandomStates();
            continue;
        }
        nextState = getNextState(currentStates);
        res.push_back(nextState);
        currentStates.erase(currentStates.begin());
        currentStates.push_back(nextState);
    }
    
    return res;
}


//==========================================================

int MarkovChain::getIndexForState(float state) {
    auto iter = std::find(stateList.begin(), stateList.end(), state);
    return static_cast<int>(iter - stateList.begin());
}

vector<float> MarkovChain::getStateList(vector<float> data) {
    vector<float> res;
    for(auto state: data) {
        auto iter = std::find(res.begin(), res.end(), state);
        if (iter == res.end()) {
            res.push_back(state);
        }
    }
    
    //! print for debug
    /*
    cout << "State list: " << endl;
    for(auto& el: res) {
        cout << el << " ";
    }
    cout << endl;
     */
    return res;
}

void MarkovChain::initTransitionMatrix(vector<float> data) {
    vector<float> sub (order, 0);
    for (vector<float>::iterator it = data.begin(); it != data.end() - order; it++) {
        std::copy(it, it + order, sub.begin());

        int key = statesToKey(sub);
        if (transitionMatrix.find(key) == transitionMatrix.end()) {
            transitionMatrix.insert(std::make_pair(key, vector<int> (numState, 0)));
        }
        int nextIndex = getIndexForState(*(it + order));
        transitionMatrix[key][nextIndex] += 1;
    }
    //! print for debug
    /* cout << "Transition matrix: " << endl;
    for(auto& el: transitionMatrix) {
        int key = el.first;
        vector<int> list = el.second;
        cout << "Key: " << key << "   ";
        for (int i = 0; i < stateList.size(); i++) {
            cout << "to " << stateList[i] << " " <<list[i] << " times. ";
        }
        cout << endl << endl;
    }
    cout << endl;
    */
}

int MarkovChain::statesToKey(vector<float> states) {
    int index = 0;
    int key = 0;
    for (int i = 0; i < order; i++) {
        index = getIndexForState(states[i]);
        key += index * std::pow(numState, (order - i -1));
    }
    return key;
}

vector<float> MarkovChain::keyToStates(int key) {
    vector<float> res (order, 0);
    for (int i = 0; i < order; i++) {
        int remainder = key % numState;
        res[order - i - 1] = stateList[remainder];
        key = key / numState;
    }
    return res;
}

vector<float> MarkovChain::getRandomStates() {
    int key = getRandomKey();
    return keyToStates(key);
}

int MarkovChain::getRandomKey() {
    srand(static_cast<int>(time(NULL)));
    int randNum = rand() % transitionMatrix.size();
    return std::next(transitionMatrix.begin(), randNum)->first;
}

float MarkovChain::getNextState(vector<float> states) {
    int key = statesToKey(states);
    vector<int> counts = transitionMatrix[key];
    int sum = 0;
    for (auto& e: counts) {
        sum += e;
    }

    int r = rand();
    int randNum = r % sum;
    int accumCounts = 0;
    int nextIndex = -1;
    for (int i = 0; i < numState; i++) {
        accumCounts += counts[i];
        if (accumCounts > randNum) {
            nextIndex = i;
            break;
        }
    }
    //! print for debug
    /*
    std::cout << "This state: " << states[0] << " key: " << key << " next state: " << stateList[nextIndex] << " random num: " << r << " accumulate counts: " << accumCounts << " count sum: " << sum << endl;
    */
    assert(nextIndex != -1);
    return stateList[nextIndex];
}












