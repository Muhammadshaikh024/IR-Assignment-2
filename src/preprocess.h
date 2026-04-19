#pragma once
#include <string>
#include <vector>
#include <unordered_set>

using namespace std;

string toLowerCopy(const string& s);
vector<string> tokenize(const string& s);
bool isAllDigits(const string& s);

// Lemmatize a list of tokens using external helper
vector<string> lemmatizeTokens(const vector<string>& tokens);

// Allowed pipeline only:
// case folding -> tokenization -> stopword removal -> lemmatization
vector<string> preprocessText(const string& text, const unordered_set<string>& stopwords);