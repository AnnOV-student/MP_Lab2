#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <map>
#include <cstdlib>
#include "passenger.h"
#include "search_structures.h"

using namespace std;
using namespace std::chrono;

vector<Passenger> readCSV(const string& filename) {
    vector<Passenger> data;
    ifstream file(filename);
    string line;
    
    while (getline(file, line)) {
        stringstream ss(line);
        Passenger p;
        string fio, cabinNumStr, cabinType, destinationPort;
        
        try {
            if (!getline(ss, fio, ',') || 
                !getline(ss, cabinNumStr, ',') || 
                !getline(ss, cabinType, ',') || 
                !getline(ss, destinationPort, ',')) {
                continue;
            }
            
            p.fio = fio;
            p.cabinNumber = stoi(cabinNumStr);
            p.cabinType = cabinType;
            p.destinationPort = destinationPort;
            
            data.push_back(p);
        } catch (const exception& e) {
            cerr << "Ошибка парсинга строки: " << line << " | Причина: " << e.what() << endl;
            continue; 
        }
    }
    file.close();
	return data;
}

int main() {
    srand(time(0));
    vector<int> sizes = {100, 500, 1000, 2500, 5000, 10000, 25000, 50000, 100000, 250000, 500000, 750000, 1000000};
    
    ofstream resultsFile("search_benchmark.csv");
    resultsFile << "Size,Linear_ns,BST_ns,RBT_ns,Hash_ns,MultiMap_ns,HashCollisions\n";

    cout << "Начинаем тестирование алгоритмов ПОИСКА...\n";

    for (int size : sizes) {
        string filename = "data_" + to_string(size) + ".csv";
        vector<Passenger> data = readCSV(filename);
        if (data.empty()) continue;

        // 1. Инициализация структур (Время заполнения НЕ учитываем)
        BST bst;
        RedBlackTree rbt;
        HashTable ht(size * 1.5); // Емкость хэш-таблицы с запасом
        multimap<string, Passenger> mmap;

        for (const auto& p : data) {
            bst.insert(p.fio, p);
            rbt.insert(p.fio, p);
            ht.insert(p.fio, p);
            mmap.insert({p.fio, p});
        }

        //Выбираем случайный ключ для поиска
        string targetKey = data[rand() % data.size()].fio;

        //Линейный поиск
        auto start = high_resolution_clock::now();
        vector<Passenger> resLinear = linearSearch(data, targetKey);
        auto stop = high_resolution_clock::now();
        long long timeLinear = duration_cast<nanoseconds>(stop - start).count();

        //BST
        start = high_resolution_clock::now();
        vector<Passenger> resBST = bst.search(targetKey);
        stop = high_resolution_clock::now();
        long long timeBST = duration_cast<nanoseconds>(stop - start).count();

        //RBT
        start = high_resolution_clock::now();
        vector<Passenger> resRBT = rbt.search(targetKey);
        stop = high_resolution_clock::now();
        long long timeRBT = duration_cast<nanoseconds>(stop - start).count();

        //Hash Table
        start = high_resolution_clock::now();
        vector<Passenger> resHash = ht.search(targetKey);
        stop = high_resolution_clock::now();
        long long timeHash = duration_cast<nanoseconds>(stop - start).count();

        //multimap
        start = high_resolution_clock::now();
        auto range = mmap.equal_range(targetKey);
        vector<Passenger> resMMap;
        for (auto it = range.first; it != range.second; ++it) {
            resMMap.push_back(it->second);
        }
        stop = high_resolution_clock::now();
        long long timeMMap = duration_cast<nanoseconds>(stop - start).count();

        resultsFile << size << "," << timeLinear << "," << timeBST << "," 
                    << timeRBT << "," << timeHash << "," << timeMMap << "," 
                    << ht.getCollisions() << "\n";
                    
        cout << "Размер " << size << " обработан.\n";
    }

    cout << "Тестирование завершено! Результаты в search_benchmark.csv\n";
    return 0;
}