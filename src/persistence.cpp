#include "persistence.h"
#include "indexer.h"
#include <filesystem>
#include <fstream>
#include <stdexcept>

using namespace std;
namespace fs = std::filesystem;

static void writeString(ofstream& out, const string& s) {
    size_t n = s.size();
    out.write(reinterpret_cast<const char*>(&n), sizeof(n));
    out.write(s.data(), static_cast<streamsize>(n));
}

static string readString(ifstream& in) {
    size_t n = 0;
    in.read(reinterpret_cast<char*>(&n), sizeof(n));
    string s(n, '\0');
    in.read(&s[0], static_cast<streamsize>(n));
    return s;
}

void saveIndex(const string& folderPath) {
    fs::create_directories(folderPath);
    string filePath = folderPath + "/index.bin";

    ofstream out(filePath, ios::binary);
    if (!out.is_open()) throw runtime_error("Could not open for write: " + filePath);

    // 1) docNames
    size_t docCount = docNames.size();
    out.write(reinterpret_cast<const char*>(&docCount), sizeof(docCount));
    for (const string& d : docNames) writeString(out, d);

    // 2) idToTerm
    size_t vocabSize = idToTerm.size();
    out.write(reinterpret_cast<const char*>(&vocabSize), sizeof(vocabSize));
    for (const string& t : idToTerm) writeString(out, t);

    // 3) df
    size_t dfSize = df.size();
    out.write(reinterpret_cast<const char*>(&dfSize), sizeof(dfSize));
    for (int x : df) out.write(reinterpret_cast<const char*>(&x), sizeof(x));

    // 4) docTF
    size_t tfDocs = docTF.size();
    out.write(reinterpret_cast<const char*>(&tfDocs), sizeof(tfDocs));
    for (const auto& mp : docTF) {
        size_t m = mp.size();
        out.write(reinterpret_cast<const char*>(&m), sizeof(m));
        for (const auto& kv : mp) {
            int termId = kv.first;
            int tfVal  = kv.second;
            out.write(reinterpret_cast<const char*>(&termId), sizeof(termId));
            out.write(reinterpret_cast<const char*>(&tfVal), sizeof(tfVal));
        }
    }

    // 5) postings
    size_t postTerms = postings.size();
    out.write(reinterpret_cast<const char*>(&postTerms), sizeof(postTerms));
    for (const auto& kv : postings) {
        int termId = kv.first;
        out.write(reinterpret_cast<const char*>(&termId), sizeof(termId));

        const auto& vec = kv.second;
        size_t m = vec.size();
        out.write(reinterpret_cast<const char*>(&m), sizeof(m));

        for (const auto& pr : vec) {
            int docId = pr.first;
            int tfVal = pr.second;
            out.write(reinterpret_cast<const char*>(&docId), sizeof(docId));
            out.write(reinterpret_cast<const char*>(&tfVal), sizeof(tfVal));
        }
    }

    out.close();
}

void loadIndex(const string& folderPath) {
    string filePath = folderPath + "/index.bin";
    ifstream in(filePath, ios::binary);
    if (!in.is_open()) throw runtime_error("Could not open for read: " + filePath);

    // clear existing globals
    termToId.clear();
    idToTerm.clear();
    docNames.clear();
    docTF.clear();
    df.clear();
    postings.clear();

    // 1) docNames
    size_t docCount = 0;
    in.read(reinterpret_cast<char*>(&docCount), sizeof(docCount));
    docNames.resize(docCount);
    for (size_t i = 0; i < docCount; i++) docNames[i] = readString(in);

    // 2) idToTerm
    size_t vocabSize = 0;
    in.read(reinterpret_cast<char*>(&vocabSize), sizeof(vocabSize));
    idToTerm.resize(vocabSize);
    for (size_t i = 0; i < vocabSize; i++) idToTerm[i] = readString(in);

    // rebuild termToId
    for (int i = 0; i < (int)idToTerm.size(); i++) {
        termToId[idToTerm[i]] = i;
    }

    // 3) df
    size_t dfSize = 0;
    in.read(reinterpret_cast<char*>(&dfSize), sizeof(dfSize));
    df.resize(dfSize);
    for (size_t i = 0; i < dfSize; i++) {
        in.read(reinterpret_cast<char*>(&df[i]), sizeof(df[i]));
    }

    // 4) docTF
    size_t tfDocs = 0;
    in.read(reinterpret_cast<char*>(&tfDocs), sizeof(tfDocs));
    docTF.resize(tfDocs);
    for (size_t i = 0; i < tfDocs; i++) {
        size_t m = 0;
        in.read(reinterpret_cast<char*>(&m), sizeof(m));
        for (size_t j = 0; j < m; j++) {
            int termId, tfVal;
            in.read(reinterpret_cast<char*>(&termId), sizeof(termId));
            in.read(reinterpret_cast<char*>(&tfVal), sizeof(tfVal));
            docTF[i][termId] = tfVal;
        }
    }

    // 5) postings
    size_t postTerms = 0;
    in.read(reinterpret_cast<char*>(&postTerms), sizeof(postTerms));
    for (size_t i = 0; i < postTerms; i++) {
        int termId;
        in.read(reinterpret_cast<char*>(&termId), sizeof(termId));

        size_t m = 0;
        in.read(reinterpret_cast<char*>(&m), sizeof(m));
        vector<pair<int,int>> vec;
        vec.reserve(m);

        for (size_t j = 0; j < m; j++) {
            int docId, tfVal;
            in.read(reinterpret_cast<char*>(&docId), sizeof(docId));
            in.read(reinterpret_cast<char*>(&tfVal), sizeof(tfVal));
            vec.push_back({docId, tfVal});
        }
        postings[termId] = move(vec);
    }

    in.close();
}