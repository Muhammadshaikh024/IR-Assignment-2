#include "io.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <stdexcept>

using namespace std;
namespace fs = std::filesystem;

string readFileText(const string& filePath) {
    ifstream in(filePath);
    if (!in.is_open()) {
        throw runtime_error("Could not open file: " + filePath);
    }

    ostringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

unordered_set<string> loadStopwords(const string& stopwordsPath) {
    ifstream in(stopwordsPath);
    if (!in.is_open()) {
        throw runtime_error("Could not open stopwords file: " + stopwordsPath);
    }

    unordered_set<string> stopwords;
    string w;
    while (in >> w) {
        // lowercase to match preprocessing style
        for (char& c : w) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
        stopwords.insert(w);
    }
    return stopwords;
}

vector<string> listFilesInDirectory(const string& dirPath) {
    vector<string> files;

    if (!fs::exists(dirPath)) {
        throw runtime_error("Directory does not exist: " + dirPath);
    }
    if (!fs::is_directory(dirPath)) {
        throw runtime_error("Path is not a directory: " + dirPath);
    }

    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().string());
        }
    }

    sort(files.begin(), files.end());
    return files;
}