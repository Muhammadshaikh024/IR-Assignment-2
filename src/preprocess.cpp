#include "preprocess.h"
#include <cctype>
#include <algorithm>
#include <unordered_map>

using namespace std;

string toLowerCopy(const string& s) {
    string out = s;
    transform(out.begin(), out.end(), out.begin(),
              [](unsigned char c){ return (char)tolower(c); });
    return out;
}

vector<string> tokenize(const string& s) {
    vector<string> tokens;
    string cur;
    for (unsigned char ch : s) {
        if (isalnum(ch)) cur.push_back((char)ch);
        else if (!cur.empty()) { tokens.push_back(cur); cur.clear(); }
    }
    if (!cur.empty()) tokens.push_back(cur);
    return tokens;
}

bool isAllDigits(const string& s) {
    if (s.empty()) return false;
    for (unsigned char c : s) if (!isdigit(c)) return false;
    return true;
}

// lightweight lemmatizer (fast, no external deps)
static string lemmaWord(const string& w) {
    static const unordered_map<string,string> irregular = {
        {"men","man"},{"women","woman"},{"children","child"},
        {"mice","mouse"},{"geese","goose"},{"feet","foot"},
        {"went","go"},{"gone","go"},{"did","do"},{"done","do"},
        {"was","be"},{"were","be"},{"is","be"},{"are","be"},
        {"has","have"},{"had","have"}
    };

    auto it = irregular.find(w);
    if (it != irregular.end()) return it->second;

    string x = w;
    int n = (int)x.size();

    // plural nouns
    if (n > 4 && x.substr(n-3) == "ies") return x.substr(0, n-3) + "y"; // parties->party
    if (n > 3 && x.substr(n-2) == "es")  return x.substr(0, n-2);       // boxes->box
    if (n > 2 && x.back() == 's' && x[n-2] != 's') return x.substr(0, n-1); // cars->car

    // past tense
    if (n > 4 && x.substr(n-3) == "ied") return x.substr(0, n-3) + "y"; // studied->study
    if (n > 3 && x.substr(n-2) == "ed")  return x.substr(0, n-2);       // worked->work

    // gerund
    if (n > 5 && x.substr(n-3) == "ing") {
        string stem = x.substr(0, n-3); // running->runn (rough)
        if (stem.size() >= 2 && stem.back() == stem[stem.size()-2]) stem.pop_back(); // runn->run
        return stem;
    }

    return x;
}

vector<string> lemmatizeTokens(const vector<string>& tokens) {
    vector<string> out;
    out.reserve(tokens.size());
    for (const string& t : tokens) out.push_back(lemmaWord(t));
    return out;
}

vector<string> preprocessText(const string& text, const unordered_set<string>& stopwords) {
    string lower = toLowerCopy(text);
    vector<string> raw = tokenize(lower);

    vector<string> filtered;
    filtered.reserve(raw.size());
    for (const string& tok : raw) {
        if (tok.empty()) continue;
        if (isAllDigits(tok)) continue;
        if (stopwords.find(tok) != stopwords.end()) continue;
        filtered.push_back(tok);
    }

    return lemmatizeTokens(filtered);
}