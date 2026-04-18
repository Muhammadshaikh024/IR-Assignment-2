#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace std;

// term -> termId
extern unordered_map<string, int> termToId;
extern vector<string> idToTerm;

// doc metadata
extern vector<string> docNames;

// docTF[docId][termId] = raw tf
extern vector<unordered_map<int, int>> docTF;

// df[termId] = number of docs containing term
extern vector<int> df;

// postings[termId] = list of (docId, tf)
extern unordered_map<int, vector<pair<int,int>>> postings;

// API
int getOrCreateTermId(const string& term);
void addDocumentToIndex(const string& docName, const vector<string>& tokens);
void buildPostingsFromDocTF();
void printIndexStats();