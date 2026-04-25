
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

const string DATABASE_FILE = "database.txt";

struct Entry {
    string key;
    int value;
};

bool entryExists(const string& key, int value) {
    ifstream file(DATABASE_FILE);
    if (!file.is_open()) return false;
    
    string file_key;
    int file_value;
    
    while (file >> file_key >> file_value) {
        if (file_key == key && file_value == value) {
            file.close();
            return true;
        }
    }
    
    file.close();
    return false;
}

void insertEntry(const string& key, int value) {
    if (entryExists(key, value)) return;
    
    ofstream file(DATABASE_FILE, ios::app);
    if (file.is_open()) {
        file << key << " " << value << "\n";
        file.close();
    }
}

void deleteEntry(const string& key, int value) {
    vector<Entry> entries;
    ifstream file(DATABASE_FILE);
    
    if (file.is_open()) {
        string file_key;
        int file_value;
        
        while (file >> file_key >> file_value) {
            if (!(file_key == key && file_value == value)) {
                entries.push_back({file_key, file_value});
            }
        }
        
        file.close();
    }
    
    ofstream out_file(DATABASE_FILE);
    if (out_file.is_open()) {
        for (const auto& entry : entries) {
            out_file << entry.key << " " << entry.value << "\n";
        }
        out_file.close();
    }
}

void findEntries(const string& key) {
    vector<int> values;
    ifstream file(DATABASE_FILE);
    
    if (file.is_open()) {
        string file_key;
        int file_value;
        
        while (file >> file_key >> file_value) {
            if (file_key == key) {
                values.push_back(file_value);
            }
        }
        
        file.close();
    }
    
    if (values.empty()) {
        cout << "null" << endl;
    } else {
        sort(values.begin(), values.end());
        values.erase(unique(values.begin(), values.end()), values.end());
        
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
