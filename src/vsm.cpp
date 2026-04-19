#include "vsm.h"
#include "indexer.h"
#include <cmath>
#include <algorithm>
using namespace std;

vector<double> idfValues;
vector<unordered_map<int, double>> docWeights;
vector<double> docNorms;

void buildVSMWeights() {
    int N = (int)docNames.size();
    int V = (int)idToTerm.size();

    idfValues.assign(V, 0.0);
    docWeights.assign(N, {});
    docNorms.assign(N, 0.0);

    for (int termId = 0; termId < V; termId++) {
        if (df[termId] > 0) {
            idfValues[termId] = log((double)N / (double)df[termId]);
        }
    }

    for (int docId = 0; docId < N; docId++) {
        double sqSum = 0.0;
        for (const auto& kv : docTF[docId]) {
            int termId = kv.first;
            int tfRaw  = kv.second;
            if (tfRaw <= 0) continue;

            double tfWeight = 1.0 + log((double)tfRaw);
            double w = tfWeight * idfValues[termId];

            if (w != 0.0) {
                docWeights[docId][termId] = w;
                sqSum += w * w;
            }
        }
        docNorms[docId] = sqrt(sqSum);
    }
}

vector<pair<int,double>> searchQueryVSM(const vector<string>& queryTokens, double alpha) {
    vector<pair<int,double>> empty;
    if (docNames.empty()) return empty;

    unordered_map<int,int> qTF;
    for (const string& tok : queryTokens) {
        auto it = termToId.find(tok);
        if (it == termToId.end()) continue;
        qTF[it->second]++;
    }
    if (qTF.empty()) return empty;

    unordered_map<int,double> qWeights;
    double qSq = 0.0;

    for (const auto& kv : qTF) {
        int termId = kv.first;
        int tfRaw  = kv.second;

        double tfWeight = 1.0 + log((double)tfRaw);
        double wq = tfWeight * idfValues[termId];
        if (wq == 0.0) continue;

        qWeights[termId] = wq;
        qSq += wq * wq;
    }

    double qNorm = sqrt(qSq);
    if (qNorm == 0.0) return empty;

    unordered_map<int,double> dot;
    for (const auto& kv : qWeights) {
        int termId = kv.first;
        double wq  = kv.second;

        auto pit = postings.find(termId);
        if (pit == postings.end()) continue;

        for (const auto& p : pit->second) {
            int docId = p.first;
            auto dit = docWeights[docId].find(termId);
            if (dit == docWeights[docId].end()) continue;
            dot[docId] += wq * dit->second;
        }
    }

    vector<pair<int,double>> results;
    for (const auto& kv : dot) {
        int docId = kv.first;
        double denom = qNorm * docNorms[docId];
        if (denom == 0.0) continue;

        double score = kv.second / denom;
        if (score >= alpha) {
            results.push_back({docId, score});
        }
    }

    sort(results.begin(), results.end(),
         [](const pair<int,double>& a, const pair<int,double>& b) {
             if (a.second != b.second) return a.second > b.second;
             return a.first < b.first;
         });

    return results; 
}