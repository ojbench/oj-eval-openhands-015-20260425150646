
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <filesystem>

using namespace std;

const string DATABASE_FILE = "database.txt";

struct Entry {
    string key;
    int value;
};

vector<Entry> readAllEntries() {
    vector<Entry> entries;
    ifstream file(DATABASE_FILE);
    
    if (!file.is_open()) {
        return entries;
    }
    
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string key;
        int value;
        
        if (iss >> key >> value) {
            entries.push_back({key, value});
        }
    }
    
    file.close();
    return entries;
}

bool entryExists(const vector<Entry>& entries, const string& key, int value) {
    for (const auto& entry : entries) {
        if (entry.key == key && entry.value == value) {
            return true;
        }
    }
    return false;
}

void insertEntry(const string& key, int value) {
    vector<Entry> entries = readAllEntries();
    
    if (entryExists(entries, key, value)) {
        return;
    }
    
    ofstream file(DATABASE_FILE, ios::app);
    if (file.is_open()) {
        file << key << " " << value << endl;
        file.close();
    }
}

void deleteEntry(const string& key, int value) {
    vector<Entry> entries = readAllEntries();
    
    ofstream file(DATABASE_FILE);
    if (!file.is_open()) {
        return;
    }
    
    bool found = false;
    for (const auto& entry : entries) {
        if (entry.key == key && entry.value == value) {
            found = true;
        } else {
            file << entry.key << " " << entry.value << endl;
        }
    }
    
    file.close();
}

void findEntries(const string& key) {
    vector<Entry> entries = readAllEntries();
    vector<int> values;
    
    for (const auto& entry : entries) {
        if (entry.key == key) {
            values.push_back(entry.value);
        }
    }
    
    if (values.empty()) {
        cout << "null" << endl;
    } else {
        sort(values.begin(), values.end());
        for (size_t i = 0; i < values.size(); ++i) {
            if (i > 0) cout << " ";
            cout << values[i];
        }
        cout << endl;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n;
    cin >> n;
    
    for (int i = 0; i < n; ++i) {
        string command;
        cin >> command;
        
        if (command == "insert") {
            string key;
            int value;
            cin >> key >> value;
            insertEntry(key, value);
        } else if (command == "delete") {
            string key;
            int value;
            cin >> key >> value;
            deleteEntry(key, value);
        } else if (command == "find") {
            string key;
            cin >> key;
            findEntries(key);
        }
    }
    
    return 0;
}
