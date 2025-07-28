#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <cctype> // for isspace

using namespace std;

typedef long long ll;
typedef pair<ll, ll> Point;

// ---------- Trim spaces ----------
string trim(const string &s) {
    size_t start = 0;
    while (start < s.length() && isspace(s[start])) start++;
    size_t end = s.length();
    while (end > start && isspace(s[end - 1])) end--;
    return s.substr(start, end - start);
}

// ---------- Remove surrounding quotes if present ----------
string removeQuotes(const string& s) {
    if (s.length() >= 2 && s.front() == '"' && s.back() == '"') {
        return s.substr(1, s.size() - 2);
    }
    return s;
}

// ---------- Convert base string to decimal ----------
ll convertToDecimal(const string& value, int base) {
    ll result = 0;
    for (char digit : value) {
        ll val;
        if (isdigit(digit)) val = digit - '0';
        else if (isalpha(digit)) val = tolower(digit) - 'a' + 10;
        else continue;
        result = result * base + val;
    }
    return result;
}

// ---------- Lagrange Interpolation at x = 0 ----------
long double lagrangeInterpolationAtZero(const vector<Point>& points) {
    int k = points.size();
    long double result = 0.0;

    for (int i = 0; i < k; ++i) {
        long double xi = points[i].first;
        long double yi = points[i].second;
        long double term = yi;

        for (int j = 0; j < k; ++j) {
            if (i != j) {
                long double xj = points[j].first;
                term *= (-xj) / (xi - xj);
            }
        }

        result += term;
    }

    return round(result);
}

// ---------- Parse JSON manually ----------
vector<Point> parseJSON(const string& filename, int& k) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "❌ Failed to open file: " << filename << endl;
        return {};
    }

    string line;
    vector<Point> points;

    while (getline(file, line)) {
        // Remove all spaces for easier parsing
        line.erase(remove(line.begin(), line.end(), ' '), line.end());

        // Parse "k" value line
        if (line.find("\"k\"") != string::npos) {
            size_t colon = line.find(":");
            if (colon == string::npos) continue;
            string valStr = line.substr(colon + 1);
            valStr.erase(remove(valStr.begin(), valStr.end(), ','), valStr.end());
            valStr = trim(valStr);
            valStr = removeQuotes(valStr);
            try {
                k = stoi(valStr);
            } catch (const exception& e) {
                cerr << "❌ Failed to parse k value from: " << valStr << " (" << e.what() << ")" << endl;
            }
        }
        // Parse points, expecting format like:
        // "1":{
        // "base": "16",
        // "value": "1a"
        // }
        else if (!line.empty() && line[0] == '"' && isdigit(line[1])) {
            // Extract the key (x-coordinate)
            size_t quote_end = line.find("\"", 1);
            if (quote_end == string::npos) continue;
            string key = line.substr(1, quote_end - 1);

            // Read next two lines for base and value
            string baseLine, valueLine;
            if (!getline(file, baseLine) || !getline(file, valueLine)) {
                cerr << "❌ Unexpected end of file while reading base/value for x = " << key << endl;
                break;
            }

            // Parse base line
            size_t colonBase = baseLine.find(":");
            if (colonBase == string::npos) {
                cerr << "❌ Invalid base line format: " << baseLine << endl;
                continue;
            }
            string baseStr = baseLine.substr(colonBase + 1);
            baseStr.erase(remove(baseStr.begin(), baseStr.end(), ','), baseStr.end());
            baseStr = trim(baseStr);
            baseStr = removeQuotes(baseStr);

            int base = 10;
            try {
                base = stoi(baseStr);
            } catch (const exception& e) {
                cerr << "❌ Failed to parse base from: " << baseStr << " (" << e.what() << ")" << endl;
                continue;
            }

            // Parse value line
            size_t colonValue = valueLine.find(":");
            if (colonValue == string::npos) {
                cerr << "❌ Invalid value line format: " << valueLine << endl;
                continue;
            }
            string value = valueLine.substr(colonValue + 1);
            value.erase(remove(value.begin(), value.end(), ','), value.end());
            value = trim(value);
            value = removeQuotes(value);

            // Convert key and value
            ll x = 0, y = 0;
            try {
                x = stoll(key);
            } catch (const exception& e) {
                cerr << "❌ Failed to parse x key from: " << key << " (" << e.what() << ")" << endl;
                continue;
            }
            y = convertToDecimal(value, base);

            cout << "Parsed: x = " << x << ", y = " << y << " (base " << base << ")" << endl;

            points.push_back({x, y});
        }
    }

    return points;
}

// ---------- Try all k-combinations and get majority constant ----------
ll findSecretUsingCombinations(const vector<Point>& points, int k) {
    int n = points.size();
    map<ll, int> countMap;

    vector<int> indices(n, 0);
    for (int i = 0; i < k; ++i) indices[i] = 1;
    sort(indices.begin(), indices.end());

    do {
        vector<Point> subset;
        for (int i = 0; i < n; ++i) {
            if (indices[i]) {
                subset.push_back(points[i]);
            }
        }

        ll secret = (ll)lagrangeInterpolationAtZero(subset);
        countMap[secret]++;

    } while (next_permutation(indices.begin(), indices.end()));

    // Get most frequent value
    ll maxVal = 0;
    int maxCount = 0;
    for (const auto& pr : countMap) {
        if (pr.second > maxCount) {
            maxCount = pr.second;
            maxVal = pr.first;
        }
    }

    return maxVal;
}

// ---------- Main Driver ----------
int main() {
    int k1 = 0, k2 = 0;

    cout << "\n----- Testcase 1 -----\n";
    vector<Point> t1 = parseJSON("testcase1.json", k1);
    if (!t1.empty() && k1 > 0) {
        ll secret1 = findSecretUsingCombinations(t1, k1);
        cout << "✅ Secret for Testcase 1: " << secret1 << "\n";
    } else {
        cout << "❌ Could not find valid data for Testcase 1\n";
    }

    cout << "\n----- Testcase 2 -----\n";
    vector<Point> t2 = parseJSON("testcase2.json", k2);
    if (!t2.empty() && k2 > 0) {
        ll secret2 = findSecretUsingCombinations(t2, k2);
        cout << "✅ Secret for Testcase 2: " << secret2 << "\n";
    } else {
        cout << "❌ Could not find valid data for Testcase 2\n";
    }

    return 0;
}
