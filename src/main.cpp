#include <iostream>
#include <vector>
#include <unordered_set>
#include "preprocess.h"
#include "io.h"
#include "indexer.h"

using namespace std;

int main() {
    try {
        string stopPath = "/home/muhammad/Desktop/Fast/Semester 6/IR/assignment-2/Stopword-List.txt";
        string docsDir  = "/home/muhammad/Desktop/Fast/Semester 6/IR/assignment-2/Trump-Speeches";

        unordered_set<string> stopwords = loadStopwords(stopPath);
        vector<string> files = listFilesInDirectory(docsDir);

        cout << "Loaded stopwords: " << stopwords.size() << "\n";
        cout << "Found documents: " << files.size() << "\n\n";

        for (const string& path : files) {
            string text = readFileText(path);
            vector<string> tokens = preprocessText(text, stopwords);
            addDocumentToIndex(path, tokens);
        }

        buildPostingsFromDocTF();
        printIndexStats();

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}