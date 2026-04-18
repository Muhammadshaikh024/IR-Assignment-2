#pragma once
#include <string>
#include <vector>
#include <unordered_set>
using namespace std;

string toLowerCopy(const string& s);
vector<string> tokenize(const string& s);
bool isAllDigits(const string& s);
string normalizeToken(const string& t);

// Full pipeline
vector<string> preprocessText(const string& text, const unordered_set<string>& stopwords);