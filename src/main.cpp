#include "../include/ChronosCore.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>

void parse_fasta_file(const std::string& filepath, Chronos::SearchMatrix& matrix) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open target file path: " + filepath);
    }

    std::string line;
    std::string currentSequence = "";
    size_t headersEncountered = 0;

    std::cout << "[Parser] Reading FASTA file records...\n";

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        // Skip metadata comment line nodes commonly found in FASTA dumps
        if (line[0] == '>') {
            headersEncountered++;
            if (!currentSequence.empty()) {
                matrix.import_raw_sequence(currentSequence);
                currentSequence.clear();
            }
            std::cout << "[Record] Target Domain: " << line.substr(1, 40) << "...\n";
        } else {
            currentSequence += line;
        }
    }

    // Flush any remaining sequence tail segments
    if (!currentSequence.empty()) {
        matrix.import_raw_sequence(currentSequence);
    }
}

int main(int argc, char* argv[]) {
    std::cout << "CHRONOS Genomic Search Matrix v1.0.0 Alpha\n";
    std::cout << "-------------------------------------------\n";

    if (argc < 3) {
        std::cerr << "Usage Error: " << argv[0] << " <fasta_file_path> <search_query_sequence>\n";
        std::cerr << "Example:     " << argv[0] << " ebola.fasta ATCG\n";
        return 1;
    }

    std::string filepath = argv[1];
    std::string query = argv[2];

    try {
        Chronos::SearchMatrix matrix;

        auto startTime = std::chrono::high_resolution_clock::now();
        parse_fasta_file(filepath, matrix);
        auto parsedTime = std::chrono::high_resolution_clock::now();

        std::cout << "\n[Metrics] Sequence Importer Processing Stats:\n";
        std::cout << " -> Total Base Pairs Loaded: " << matrix.get_total_bases() << " bases.\n";
        std::cout << " -> Allocated Memory Words:  " << matrix.get_word_count() << " (64-bit words).\n";

        auto d1 = std::chrono::duration_cast<std::chrono::milliseconds>(parsedTime - startTime).count();
        std::cout << " -> Ingestion Performance:   " << d1 << " ms\n\n";

        std::cout << "[Matrix] Executing sliding bitwise search loop for: " << query << "\n";

        auto scanStart = std::chrono::high_resolution_clock::now();
        std::vector<size_t> results = matrix.find_exact_matches(query);
        auto scanEnd = std::chrono::high_resolution_clock::now();

        auto d2 = std::chrono::duration_cast<std::chrono::microseconds>(scanEnd - scanStart).count();

        std::cout << "[Results] Matrix Search Calculations Finished:\n";
        std::cout << " -> Total Exact Matches Found: " << results.size() << "\n";
        std::cout << " -> Core Execution Latency:    " << d2 << " microseconds (µs)\n";

        if (!results.empty()) {
            std::cout << " -> Initial Match Indices:     ";
            size_t displayCount = std::min(results.size(), size_t(5));
            for (size_t i = 0; i < displayCount; ++i) {
                std::cout << results[i] << " ";
            }
            if (results.size() > 5) std::cout << "...";
            std::cout << "\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Fatal runtime calculation failure: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
