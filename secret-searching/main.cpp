#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

using namespace std;

typedef long long ll;
typedef pair<ll, ll> Point;

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
        line.erase(remove(line.begin(), line.end(), ' '), line.end());

        if (line.find("\"k\"") != string::npos) {
            size_t colon = line.find(":");
            string valStr = line.substr(colon + 1);
            valStr.erase(remove(valStr.begin(), valStr.end(), ','), valStr.end());
            try {
                k = stoi(valStr);
            } catch (...) {
                cerr << "❌ Failed to parse k value from: " << valStr << endl;
            }
        }
        else if (isdigit(line[1])) {
            string key = line.substr(1, line.find("\"", 1) - 1);

            string baseLine, valueLine;
            getline(file, baseLine);
            getline(file, valueLine);

            // Parse base
            size_t colon = baseLine.find(":");
            string baseStr = baseLine.substr(colon + 2);
            baseStr.erase(remove(baseStr.begin(), baseStr.end(), ','), baseStr.end());
            int base = stoi(baseStr);

            // Parse value
            colon = valueLine.find(":");
            string value = valueLine.substr(colon + 2);
            value.erase(remove(value.begin(), value.end(), '"'), value.end());
            value.erase(remove(value.begin(), value.end(), ','), value.end());

            ll x = stoll(key);
            ll y = convertToDecimal(value, base);

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

    // Get most frequent result
    ll maxVal = 0, maxCount = 0;
    for (map<ll, int>::iterator it = countMap.begin(); it != countMap.end(); ++it) {
        if (it->second > maxCount) {
            maxCount = it->second;
            maxVal = it->first;
        }
    }

    return maxVal;
}

// ---------- Main Driver ----------
int main() {
    int k1 = 0, k2 = 0;

    cout << "\n----- Testcase 1 -----\n";
    vector<Point> t1 = parseJSON("testcase1.json", k1);
    if (!t1.empty()) {
        ll secret1 = findSecretUsingCombinations(t1, k1);
        cout << "✅ Secret for Testcase 1: " << secret1 << "\n";
    }

    cout << "\n----- Testcase 2 -----\n";
    vector<Point> t2 = parseJSON("testcase2.json", k2);
    if (!t2.empty()) {
        ll secret2 = findSecretUsingCombinations(t2, k2);
        cout << "✅ Secret for Testcase 2: " << secret2 << "\n";
    }

    return 0;
}
