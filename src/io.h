#pragma once
#include <string>
#include <vector>
#include <unordered_set>

using namespace std;

// Read full file into a string
string readFileText(const string& filePath);

// Load stopwords (one word per line, or whitespace separated)
unordered_set<string> loadStopwords(const string& stopwordsPath);

// List all regular files in a directory (non-recursive), sorted by filename
vector<string> listFilesInDirectory(const string& dirPath);