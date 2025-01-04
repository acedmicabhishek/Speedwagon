#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <cstdio>

int partition(std::vector<int>& arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        if (arr[j] < pivot) {
            ++i;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quicksort(std::vector<int>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}
void logBenchmarkData(const std::string& task, double elapsedTime, size_t dataSize, const std::string& logFileName) {
    std::ofstream logFile(logFileName, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Error: Could not open log file " << logFileName << " for writing.\n";
        return;
    }

    logFile << task << " Benchmark\n";
    logFile << "Data Size: " << dataSize << "\n";
    logFile << "Time Taken: " << elapsedTime << " seconds\n";
    logFile << "-----------------------------\n";
    logFile.close();
}

int main() {
    const std::string inputFileName = "arraydata.txt";
    const std::string outputFileName = "bin.txt";
    const std::string benchmarkFileName = "benchmark.txt";

    std::ofstream outFile(inputFileName);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file " << inputFileName << " for writing.\n";
        return 1;
    }

    outFile << "[";
    for (int i = 100'000; i >= 1; --i) {
        outFile << i;
        if (i != 1) {
            outFile << ",";
        }
    }
    outFile << "]";
    outFile.close();

    std::cout << "Generated numbers in descending order and saved to " << inputFileName << std::endl;

    std::ifstream inFile(inputFileName);
    if (!inFile.is_open()) {
        std::cerr << "Error: Could not open file " << inputFileName << " for reading.\n";
        return 1;
    }

    std::vector<int> arrayData;
    char ch;
    int number;

    inFile >> ch;
    while (inFile >> number) {
        arrayData.push_back(number);
        inFile >> ch;
        if (ch == ']') break;
    }
    inFile.close();

    auto startSort = std::chrono::high_resolution_clock::now();
    quicksort(arrayData, 0, arrayData.size() - 1);
    auto endSort = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> sortElapsedTime = endSort - startSort;
    std::cout << "Time taken to sort the array: " << sortElapsedTime.count() << " seconds." << std::endl;

    logBenchmarkData("Sorting", sortElapsedTime.count(), arrayData.size(), benchmarkFileName);

    std::ofstream binaryOutFile(outputFileName, std::ios::binary);
    if (!binaryOutFile.is_open()) {
        std::cerr << "Error: Could not open file " << outputFileName << " for writing.\n";
        return 1;
    }

    for (const int& num : arrayData) {
        binaryOutFile.write(reinterpret_cast<const char*>(&num), sizeof(int));
    }
    binaryOutFile.close();

    std::cout << "Sorted numbers written to " << outputFileName << " in binary format." << std::endl;

    if (remove(outputFileName.c_str()) != 0) {
        std::cerr << "Error: Could not delete the file " << outputFileName << "\n";
    } else {
        std::cout << "The file " << outputFileName << " has been deleted successfully." << std::endl;
    }
    if (remove(inputFileName.c_str()) != 0) {
        std::cerr << "Error: Could not delete the file " << inputFileName << "\n";
    } else {
        std::cout << "The file " << inputFileName << " has been deleted successfully." << std::endl;
    }

    return 0;
}
