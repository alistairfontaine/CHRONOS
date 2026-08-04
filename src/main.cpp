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

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (line[0] == '>') {
            if (!currentSequence.empty()) {
                matrix.import_raw_sequence(currentSequence);
                currentSequence.clear();
            }
        } else {
            currentSequence += line;
        }
    }
    if (!currentSequence.empty()) {
        matrix.import_raw_sequence(currentSequence);
    }
}

int main(int argc, char* argv[]) {
    std::cout << "CHRONOS Genomic Search Matrix v1.0.0 Phase 2\n";
    std::cout << "--------------------------------------------\n";

    if (argc >= 2 && std::string(argv[1]) == "--test") {
        return Chronos::run_unit_tests() ? 0 : 1;
    }

    if (argc < 4) {
        std::cerr << "Usage Error: " << argv[0] << " <mode> <input_file> <query>\n";
        std::cerr << "Modes available:\n";
        std::cerr << "  --scan  : Ingest FASTA raw file, parse pattern string.\n";
        std::cerr << "  --save  : Ingest FASTA raw file, compress directly into a cached .chronos matrix file.\n";
        std::cerr << "  --load  : Skip FASTA translation completely. Load .chronos file, parse pattern string.\n";
        return 1;
    }

    std::string mode = argv[1];
    std::string filepath = argv[2];
    std::string query = argv[3];

    try {
        Chronos::SearchMatrix matrix;

        if (mode == "--scan" || mode == "--save") {
            auto t0 = std::chrono::high_resolution_clock::now();
            parse_fasta_file(filepath, matrix);
            auto t1 = std::chrono::high_resolution_clock::now();
            std::cout << "[Parser] FASTA file ingested in "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << " ms.\n";

            if (mode == "--save") {
                matrix.export_to_binary(query); // Treat 3rd argument as the out file path
                return 0;
            }
        }
        else if (mode == "--load") {
            auto t0 = std::chrono::high_resolution_clock::now();
            matrix.load_from_binary(filepath);
            auto t1 = std::chrono::high_resolution_clock::now();
            std::cout << "[Loader] Cached binary loaded in "
                      << std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() << " µs.\n";
        }
        else {
            std::cerr << "Unknown mode identifier passed.\n";
            return 1;
        }

        // Execute Search Loop Optimization Verification
        std::cout << "[Engine] Vectoring sliding search bitmask for pattern: " << query << "\n";
        auto s0 = std::chrono::high_resolution_clock::now();
        std::vector<size_t> matches = matrix.find_exact_matches(query);
        auto s1 = std::chrono::high_resolution_clock::now();

        std::cout << "[Results] Processing Metrics:\n";
        std::cout << " -> System Total Size: " << matrix.get_total_bases() << " bases.\n";
        std::cout << " -> Matches Discovered: " << matches.size() << "\n";
        std::cout << " -> Computational Latency: "
                  << std::chrono::duration_cast<std::chrono::microseconds>(s1 - s0).count() << " µs\n";

    } catch (const std::exception& e) {
        std::cerr << "Fatal runtime calculation failure: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
