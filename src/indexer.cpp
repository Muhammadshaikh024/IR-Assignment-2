#include "indexer.h"
#include <iostream>

using namespace std;

// definitions
unordered_map<string, int> termToId;
vector<string> idToTerm;
vector<string> docNames;
vector<unordered_map<int, int>> docTF;
vector<int> df;
unordered_map<int, vector<pair<int,int>>> postings;

int getOrCreateTermId(const string& term) {
    auto it = termToId.find(term);
    if (it != termToId.end()) return it->second;

    int newId = (int)idToTerm.size();
    termToId[term] = newId;
    idToTerm.push_back(term);
    df.push_back(0);
    return newId;
}

void addDocumentToIndex(const string& docName, const vector<string>& tokens) {
    int docId = (int)docNames.size();
    docNames.push_back(docName);
    docTF.push_back({});

    unordered_set<int> seenInDoc;

    for (const string& tok : tokens) {
        int termId = getOrCreateTermId(tok);
        docTF[docId][termId]++;

        if (seenInDoc.find(termId) == seenInDoc.end()) {
            df[termId]++;
            seenInDoc.insert(termId);
        }
    }
}

void buildPostingsFromDocTF() {
    postings.clear();
    for (int docId = 0; docId < (int)docTF.size(); docId++) {
        for (const auto& kv : docTF[docId]) {
            int termId = kv.first;
            int tfVal  = kv.second;
            postings[termId].push_back({docId, tfVal});
        }
    }
}

void printIndexStats() {
    cout << "==== Index Stats ====\n";
    cout << "Documents: " << docNames.size() << "\n";
    cout << "Vocabulary size: " << idToTerm.size() << "\n";
    cout << "Terms with postings: " << postings.size() << "\n";

    // show a few sample terms
    int show = min((int)idToTerm.size(), 10);
    cout << "Sample terms:\n";
    for (int i = 0; i < show; i++) {
        cout << "  termId=" << i
             << " term=\"" << idToTerm[i] << "\""
             << " df=" << df[i]
             << " postings=" << postings[i].size() << "\n";
    }
}