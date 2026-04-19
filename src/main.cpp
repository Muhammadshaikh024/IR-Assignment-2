#include <iostream>
#include <vector>
#include <unordered_set>
#include <filesystem>
#include <algorithm>
#include "preprocess.h"
#include "io.h"
#include "indexer.h"
#include "vsm.h"
#include "persistence.h"

using namespace std;
namespace fs = std::filesystem;

static void runOneQuery(const string& q, const unordered_set<string>& stopwords) {
    vector<string> qTokens = preprocessText(q, stopwords);
    vector<pair<int,double>> ans = searchQueryVSM(qTokens, 0.005);

    vector<int> ids;
    ids.reserve(ans.size());
    for (const auto& r : ans) ids.push_back(r.first + 1);
    sort(ids.begin(), ids.end());

    cout << "query= '" << q << "'\n\n";
    cout << "Length=" << ids.size() << "\n";
    cout << "{";
    for (int i = 0; i < (int)ids.size(); i++) {
        cout << "'" << ids[i] << "'";
        if (i + 1 < (int)ids.size()) cout << ", ";
    }
    cout << "}\n";
}

int main(int argc, char* argv[]) {
    try {
        string stopPath = "/home/muhammad/Desktop/Fast/Semester 6/IR/assignment-2/Stopword-List.txt";
        string docsDir  = "/home/muhammad/Desktop/Fast/Semester 6/IR/assignment-2/Trump-Speeches";
        string indexDir = "saved_index";
        string indexBin = indexDir + "/index.bin";

        unordered_set<string> stopwords = loadStopwords(stopPath);

        if (fs::exists(indexBin)) {
            loadIndex(indexDir);
        } else {
            vector<string> files = listFilesInDirectory(docsDir);
            for (const string& path : files) {
                string text = readFileText(path);
                vector<string> tokens = preprocessText(text, stopwords);
                addDocumentToIndex(path, tokens);
            }
            buildPostingsFromDocTF();
            saveIndex(indexDir);
        }

        buildVSMWeights();

        // single-query mode for Flask
        if (argc >= 3 && string(argv[1]) == "--query") {
            string q = argv[2];
            runOneQuery(q, stopwords);
            return 0;
        }

        // interactive mode
        while (true) {
            cout << "\nEnter query (or exit): ";
            string q;
            getline(cin, q);
            if (q == "exit") break;
            runOneQuery(q, stopwords);
        }

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}