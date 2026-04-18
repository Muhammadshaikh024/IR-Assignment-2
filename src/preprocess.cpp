#include "preprocess.h"
#include <cctype>
#include <algorithm>
using namespace std;

string toLowerCopy(const string& s) {
    string out = s;
    transform(out.begin(), out.end(), out.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return out;
}

vector<string> tokenize(const string& s) {
    vector<string> tokens;
    string cur;
    cur.reserve(32);

    for (unsigned char ch : s) {
        if (isalnum(ch)) {
            cur.push_back(static_cast<char>(ch));
        } else {
            if (!cur.empty()) {
                tokens.push_back(cur);
                cur.clear();
            }
        }
    }
    if (!cur.empty()) tokens.push_back(cur);
    return tokens;
}

bool isAllDigits(const string& s) {
    if (s.empty()) return false;
    for (unsigned char c : s) {
        if (!isdigit(c)) return false;
    }
    return true;
}

string normalizeToken(const string& t) {
    // Lightweight normalization (you can replace with stemming/lemmatization later)
    if (t.size() <= 2) return t;

    string x = t;
    if (x.size() > 4 && x.substr(x.size() - 3) == "ing") {
        x = x.substr(0, x.size() - 3);
    }
    else if (x.size() > 3 && x.substr(x.size() - 2) == "ed") {
        x = x.substr(0, x.size() - 2);
    }
    else if (x.size() > 3 && x.substr(x.size() - 2) == "es") {
        x = x.substr(0, x.size() - 2);
    }
    else if (x.size() > 2 && x.back() == 's') {
        x = x.substr(0, x.size() - 1);
    }

    return x;
}

vector<string> preprocessText(const string& text, const unordered_set<string>& stopwords) {
    vector<string> output;
    output.reserve(text.size() / 6);

    string lower = toLowerCopy(text);
    vector<string> raw = tokenize(lower);

    for (const string& tok : raw) {
        if (tok.empty()) continue;
        if (isAllDigits(tok)) continue;

        string norm = normalizeToken(tok);
        if (norm.empty()) continue;
        if (stopwords.find(norm) != stopwords.end()) continue;

        output.push_back(norm);
    }

    return output;
}