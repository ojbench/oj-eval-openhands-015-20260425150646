
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

struct IndexEntry {
    int value;
    long long file_pos;
    bool deleted;
};

class FileDatabase {
private:
    unordered_map<string, vector<IndexEntry>> index;
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
        bool deleted;
        
        while (idx_file.read(reinterpret_cast<char*>(&key_len), sizeof(key_len))) {
            if (key_len > 1000) break;
            key.resize(key_len);
            idx_file.read(&key[0], key_len);
            idx_file.read(reinterpret_cast<char*>(&value), sizeof(value));
            idx_file.read(reinterpret_cast<char*>(&file_pos), sizeof(file_pos));
            idx_file.read(reinterpret_cast<char*>(&deleted), sizeof(deleted));
            index[key].push_back({value, file_pos, deleted});
        }
        
        idx_file.close();
        index_loaded = true;
    }
    
    void saveIndex() {
        ofstream idx_file(INDEX_FILE, ios::binary);
        if (!idx_file.is_open()) return;
        
        for (const auto& [key, entries] : index) {
            for (const auto& entry : entries) {
                size_t key_len = key.length();
                idx_file.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
                idx_file.write(key.c_str(), key_len);
                idx_file.write(reinterpret_cast<const char*>(&entry.value), sizeof(entry.value));
                idx_file.write(reinterpret_cast<const char*>(&entry.file_pos), sizeof(entry.file_pos));
                idx_file.write(reinterpret_cast<const char*>(&entry.deleted), sizeof(entry.deleted));
            }
        }
        
        idx_file.close();
    }
    
    bool entryExistsInIndex(const string& key, int value) {
        loadIndex();
        auto it = index.find(key);
        if (it == index.end()) return false;
        
        for (const auto& entry : it->second) {
            if (entry.value == value && !entry.deleted) return true;
        }
        return false;
    }
    
    void compactIfNeeded() {
        int total_entries = 0;
        int deleted_entries = 0;
        
        for (const auto& [key, entries] : index) {
            total_entries += entries.size();
            for (const auto& entry : entries) {
                if (entry.deleted) deleted_entries++;
            }
        }
        
        if (total_entries > 1000 && deleted_entries > total_entries / 2) {
            compact();
        }
    }
    
    void compact() {
        ofstream data_file(DATABASE_FILE, ios::binary);
        if (!data_file.is_open()) return;
        
        unordered_map<string, vector<IndexEntry>> new_index;
        
        for (auto& [key, entries] : index) {
            for (auto& entry : entries) {
                if (!entry.deleted) {
                    long long new_pos = data_file.tellp();
                    
                    size_t key_len = key.length();
                    data_file.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
                    data_file.write(key.c_str(), key_len);
                    data_file.write(reinterpret_cast<const char*>(&entry.value), sizeof(entry.value));
                    
                    new_index[key].push_back({entry.value, new_pos, false});
                }
            }
        }
        
        data_file.close();
        index = move(new_index);
        saveIndex();
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
        
        index[key].push_back({value, file_pos, false});
        saveIndex();
    }
    
    void remove(const string& key, int value) {
        loadIndex();
        
        auto it = index.find(key);
        if (it == index.end()) return;
        
        bool found = false;
        for (auto& entry : it->second) {
            if (entry.value == value && !entry.deleted) {
                entry.deleted = true;
                found = true;
                break;
            }
        }
        
        if (found) {
            saveIndex();
            compactIfNeeded();
        }
    }
    
    void find(const string& key) {
        loadIndex();
        
        auto it = index.find(key);
        if (it == index.end()) {
            cout << "null" << endl;
            return;
        }
        
        set<int> values;
        for (const auto& entry : it->second) {
            if (!entry.deleted) {
                values.insert(entry.value);
            }
        }
        
        if (values.empty()) {
            cout << "null" << endl;
        } else {
            bool first = true;
            for (int value : values) {
                if (!first) cout << " ";
                cout << value;
                first = false;
            }
            cout << endl;
        }
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
