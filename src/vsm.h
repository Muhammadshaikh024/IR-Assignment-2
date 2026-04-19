#pragma once
#include <vector>
#include <unordered_map>
#include <string>

using namespace std;

extern vector<double> idfValues;
extern vector<unordered_map<int, double>> docWeights;
extern vector<double> docNorms;

void buildVSMWeights();

// topK removed
vector<pair<int,double>> searchQueryVSM(const vector<string>& queryTokens,double alpha = 0.005);