#include "../include/ChronosCore.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "CHRONOS Genomic Search Matrix v1.0.0 Alpha Initiated\n";
    std::cout << "-----------------------------------------------------\n";

    try {
        Chronos::SearchMatrix matrix;

        // Sample baseline gene target (Simulating a compressed sequence segment)
        std::string sampleSequence = "ATCGATCGATCGATCGATCGATCGATCGATCG";
        matrix.import_raw_sequence(sampleSequence);

        std::cout << "Successfully parsed sample genetic block.\n";
        std::cout << "Total Base Pairs Processed: " << matrix.get_total_bases() << " bases.\n";
        std::cout << "Packed Hardware Memory Words: " << matrix.get_word_count() << " (64-bit words).\n";

    } catch (const std::exception& e) {
        std::cerr << "Fatal initialization failure: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
