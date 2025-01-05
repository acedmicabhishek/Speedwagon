#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;

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

void compressFile(const std::string& inputFile, const std::string& outputFile) {
    std::string command = "gzip -c " + inputFile + " > " + outputFile;
    if (std::system(command.c_str()) != 0) {
        throw std::runtime_error("Failed to compress file.");
    }
}

void decompressFile(const std::string& inputFile, const std::string& outputFile) {
    std::string command = "gzip -cd " + inputFile + " > " + outputFile;
    if (std::system(command.c_str()) != 0) {
        throw std::runtime_error("Failed to decompress file.");
    }
}

int main() {
    const std::string arrayFileName = "arraydata.txt";
    const std::string bigDataFileName = "bigdata.txt";
    const std::string compressedFileName = "compressed.gz";
    const std::string decompressedFileName = "decompressed.txt";
    const std::string benchmarkFileName = "benchmark.txt";
    const std::string bin = "bin.txt";

    try {
        // Generate data for quicksort
        std::ofstream arrayFile(arrayFileName);
        if (!arrayFile.is_open()) {
            throw std::runtime_error("Error: Could not open file for writing.");
        }

        arrayFile << "[";
        for (int i = 100'000; i >= 1; --i) {
            arrayFile << i;
            if (i != 1) {
                arrayFile << ",";
            }
        }
        arrayFile << "]";
        arrayFile.close();
        std::cout << "Generated descending numbers and saved to " << arrayFileName << std::endl;

        std::ifstream inFile(arrayFileName);
        if (!inFile.is_open()) {
            throw std::runtime_error("Error: Could not open file for reading.");
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

        std::ofstream bigDataFile(bigDataFileName);
        if (!bigDataFile.is_open()) {
            throw std::runtime_error("Error: Could not open file for writing.");
        }

        for (int i = 1; i <= 10000'0000; ++i) {
            bigDataFile << i << "\n";
        }
        bigDataFile.close();
        std::cout << "Generated numbers from 1 to 1,000,000 and saved to " << bigDataFileName << std::endl;

        // Compression benchmark
        std::cout << "Compressing file...\n";
        auto startCompress = std::chrono::high_resolution_clock::now();
        compressFile(bigDataFileName, compressedFileName);
        auto endCompress = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> compressTime = endCompress - startCompress;
        logBenchmarkData("Compression", compressTime.count(), 1'000'000, benchmarkFileName);

        // Decompression benchmark
        std::cout << "Decompressing file...\n";
        auto startDecompress = std::chrono::high_resolution_clock::now();
        decompressFile(compressedFileName, decompressedFileName);
        auto endDecompress = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> decompressTime = endDecompress - startDecompress;
        logBenchmarkData("Decompression", decompressTime.count(), 1'000'000, benchmarkFileName);

        // Clean up
        fs::remove(arrayFileName);
        fs::remove(bigDataFileName);
        fs::remove(compressedFileName);
        fs::remove(decompressedFileName);
        fs::remove(bin);

        std::cout << "Benchmark completed. Results saved in " << benchmarkFileName << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
