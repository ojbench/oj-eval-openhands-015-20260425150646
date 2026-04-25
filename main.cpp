
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <set>

using namespace std;

const string DATABASE_FILE = "database.dat";
const string INDEX_FILE = "index.dat";

struct Entry {
    string key;
    int value;
    long long file_pos;
};

class FileDatabase {
private:
    unordered_map<string, vector<pair<int, long long>>> index;
    bool index_loaded;
    
    void loadIndex() {
        if (index_loaded) return;
        
        index.clear();
        ifstream idx_file(INDEX_FILE, ios::binary);
        if (!idx_file.is_open()) {
            index_loaded = true;
            return;
        }
        
        string key;
        size_t key_len;
        int value;
        long long file_pos;
        
        while (idx_file.read(reinterpret_cast<char*>(&key_len), sizeof(key_len))) {
            key.resize(key_len);
            idx_file.read(&key[0], key_len);
            idx_file.read(reinterpret_cast<char*>(&value), sizeof(value));
            idx_file.read(reinterpret_cast<char*>(&file_pos), sizeof(file_pos));
            index[key].push_back({value, file_pos});
        }
        
        idx_file.close();
        index_loaded = true;
    }
    
    void saveIndex() {
        ofstream idx_file(INDEX_FILE, ios::binary);
        if (!idx_file.is_open()) return;
        
        for (const auto& [key, entries] : index) {
            for (const auto& [value, file_pos] : entries) {
                size_t key_len = key.length();
                idx_file.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
                idx_file.write(key.c_str(), key_len);
                idx_file.write(reinterpret_cast<const char*>(&value), sizeof(value));
                idx_file.write(reinterpret_cast<const char*>(&file_pos), sizeof(file_pos));
            }
        }
        
        idx_file.close();
    }
    
    bool entryExistsInIndex(const string& key, int value) {
        loadIndex();
        auto it = index.find(key);
        if (it == index.end()) return false;
        
        for (const auto& [v, pos] : it->second) {
            if (v == value) return true;
        }
        return false;
    }
    
public:
    FileDatabase() : index_loaded(false) {}
    
    void insert(const string& key, int value) {
        if (entryExistsInIndex(key, value)) return;
        
        loadIndex();
        
        ofstream data_file(DATABASE_FILE, ios::binary | ios::app);
        if (!data_file.is_open()) return;
        
        long long file_pos = data_file.tellp();
        
        size_t key_len = key.length();
        data_file.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
        data_file.write(key.c_str(), key_len);
        data_file.write(reinterpret_cast<const char*>(&value), sizeof(value));
        
        data_file.close();
        
        index[key].push_back({value, file_pos});
        saveIndex();
    }
    
    void remove(const string& key, int value) {
        loadIndex();
        
        auto it = index.find(key);
        if (it == index.end()) return;
        
        bool found = false;
        long long target_pos = -1;
        
        for (auto& [v, pos] : it->second) {
            if (v == value) {
                target_pos = pos;
                found = true;
                break;
            }
        }
        
        if (!found) return;
        
        it->second.erase(remove_if(it->second.begin(), it->second.end(),
                                  [value](const pair<int, long long>& p) { return p.first == value; }),
                        it->second.end());
        
        if (it->second.empty()) {
            index.erase(it);
        }
        
        ofstream data_file(DATABASE_FILE, ios::binary);
        if (!data_file.is_open()) return;
        
        for (const auto& [k, entries] : index) {
            for (const auto& [v, pos] : entries) {
                size_t key_len = k.length();
                data_file.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
                data_file.write(k.c_str(), key_len);
                data_file.write(reinterpret_cast<const char*>(&v), sizeof(v));
            }
        }
        
        data_file.close();
        
        saveIndex();
    }
    
    void find(const string& key) {
        loadIndex();
        
        auto it = index.find(key);
        if (it == index.end() || it->second.empty()) {
            cout << "null" << endl;
            return;
        }
        
        set<int> values;
        for (const auto& [value, pos] : it->second) {
            values.insert(value);
        }
        
        bool first = true;
        for (int value : values) {
            if (!first) cout << " ";
            cout << value;
            first = false;
        }
        cout << endl;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    FileDatabase db;
    
    int n;
    cin >> n;
    
    for (int i = 0; i < n; ++i) {
        string command;
        cin >> command;
        
        if (command == "insert") {
            string key;
            int value;
            cin >> key >> value;
            db.insert(key, value);
        } else if (command == "delete") {
            string key;
            int value;
            cin >> key >> value;
            db.remove(key, value);
        } else if (command == "find") {
            string key;
            cin >> key;
            db.find(key);
        }
    }
    
    return 0;
}
