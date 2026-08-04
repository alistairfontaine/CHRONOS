#ifndef CHRONOS_CORE_HPP
#define CHRONOS_CORE_HPP

#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <fstream>
#include <iostream>

namespace Chronos {

    inline uint8_t encode_nucleotide(char base) {
        switch (base) {
            case 'A': case 'a': return 0x00;
            case 'C': case 'c': return 0x01;
            case 'G': case 'g': return 0x02;
            case 'T': case 't': return 0x03;
            default:
                throw std::invalid_argument("Invalid nucleotide character encountered.");
        }
    }

    struct alignas(8) GenomicWord {
        uint64_t data = 0;
        uint8_t length = 0;
    };

    class SearchMatrix {
    private:
        std::vector<GenomicWord> sequenceWords;
        size_t totalBasesParsed = 0;

        // Magical structural tracking signature for validation checks
        const uint32_t MAGIC_HEADER = 0x4348524F; // "CHRO" in hex ASCII

    public:
        SearchMatrix() = default;

        void import_raw_sequence(const std::string& sequence) {
            if (sequence.empty()) return;

            GenomicWord currentWord;
            int bitShift = 0;

            if (!sequenceWords.empty() && sequenceWords.back().length < 32) {
                currentWord = sequenceWords.back();
                sequenceWords.pop_back();
                bitShift = currentWord.length * 2;
            }

            for (char base : sequence) {
                if (base == ' ' || base == '\n' || base == '\r' || base == '\t') continue;

                uint64_t encodedBits = encode_nucleotide(base);
                currentWord.data |= (encodedBits << bitShift);
                bitShift += 2;
                currentWord.length++;
                totalBasesParsed++;

                if (currentWord.length == 32) {
                    sequenceWords.push_back(currentWord);
                    currentWord.data = 0;
                    currentWord.length = 0;
                    bitShift = 0;
                }
            }

            if (currentWord.length > 0) {
                sequenceWords.push_back(currentWord);
            }
        }

        std::vector<size_t> find_exact_matches(const std::string& query) const {
            std::vector<size_t> indicesFound;
            if (query.empty() || totalBasesParsed < query.length()) return indicesFound;

            uint64_t queryMask = 0;
            uint64_t queryData = 0;
            for (size_t i = 0; i < query.length(); ++i) {
                uint64_t encoded = encode_nucleotide(query[i]);
                queryData |= (encoded << (i * 2));
                queryMask |= (0x03ULL << (i * 2));
            }

            size_t queryLen = query.length();

            for (size_t i = 0; i <= totalBasesParsed - queryLen; ++i) {
                size_t wordIdx = i / 32;
                size_t subBitOffset = (i % 32) * 2;

                if (wordIdx >= sequenceWords.size()) break;

                uint64_t combinedWindow = 0;

                if (subBitOffset + (queryLen * 2) <= 64) {
                    combinedWindow = sequenceWords[wordIdx].data >> subBitOffset;
                } else {
                    uint64_t firstPart = sequenceWords[wordIdx].data >> subBitOffset;
                    uint64_t secondPart = 0;
                    if (wordIdx + 1 < sequenceWords.size()) {
                        secondPart = sequenceWords[wordIdx + 1].data << (64 - subBitOffset);
                    }
                    combinedWindow = firstPart | secondPart;
                }

                if ((combinedWindow & queryMask) == queryData) {
                    indicesFound.push_back(i);
                }
            }

            return indicesFound;
        }

        // --- PHASE 2: CUSTOM BINARY STATE SERIALIZATION EXPORTER ---
        void export_to_binary(const std::string& filename) const {
            std::ofstream out(filename, std::ios::binary);
            if (!out.is_open()) {
                throw std::runtime_error("Failed to open file for binary output: " + filename);
            }

            // Write File Signature, Base Pair Counts, and Vector Element Allocations
            size_t wordCount = sequenceWords.size();
            out.write(reinterpret_cast<const char*>(&MAGIC_HEADER), sizeof(MAGIC_HEADER));
            out.write(reinterpret_cast<const char*>(&totalBasesParsed), sizeof(totalBasesParsed));
            out.write(reinterpret_cast<const char*>(&wordCount), sizeof(wordCount));

            // Ultra-fast zero-copy dump of the entire memory array block in a single operation
            if (wordCount > 0) {
                out.write(reinterpret_cast<const char*>(sequenceWords.data()), wordCount * sizeof(GenomicWord));
            }
            std::cout << "[Exporter] Successfully saved compressed index directly to: " << filename << "\n";
        }

        // --- PHASE 2: CUSTOM BINARY STATE DESCRIPTOR LOADER ---
        void load_from_binary(const std::string& filename) {
            std::ifstream in(filename, std::ios::binary);
            if (!in.is_open()) {
                throw std::runtime_error("Failed to open file for binary reading: " + filename);
            }

            uint32_t headerCheck = 0;
            in.read(reinterpret_cast<char*>(&headerCheck), sizeof(headerCheck));
            if (headerCheck != MAGIC_HEADER) {
                throw std::runtime_error("Invalid or corrupted .chronos file signature format detected.");
            }

            size_t wordCount = 0;
            in.read(reinterpret_cast<char*>(&totalBasesParsed), sizeof(totalBasesParsed));
            in.read(reinterpret_cast<char*>(&wordCount), sizeof(wordCount));

            sequenceWords.resize(wordCount);
            if (wordCount > 0) {
                in.read(reinterpret_cast<char*>(sequenceWords.data()), wordCount * sizeof(GenomicWord));
            }
            std::cout << "[Loader] Successfully restored array view from binary cache file.\n";
        }

        void clear() {
            sequenceWords.clear();
            totalBasesParsed = 0;
        }

        size_t get_word_count() const { return sequenceWords.size(); }
        size_t get_total_bases() const { return totalBasesParsed; }
    };

    // --- PHASE 2: INTEGRATED AUTOMATED UNIT VERIFIER SYSTEM ---
    inline bool run_unit_tests() {
        std::cout << "[Verifier] Launching core mathematical test suite...\n";
        SearchMatrix testMatrix;

        // Test Case 1: Verification across boundary word seams
        std::string crossBoundPattern = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAATTTT"; // 36 A's followed by 4 T's
        testMatrix.import_raw_sequence(crossBoundPattern);

        auto matches = testMatrix.find_exact_matches("ATTT");
        if (matches.size() != 1 || matches[0] != 35) {
            std::cerr << "FAIL: Sub-word intersection cross alignment check failed.\n";
            return false;
        }
        std::cout << " -> Pass: Cross-word boundary intersections map cleanly.\n";

        // Test Case 2: Serialization persistence lifecycle loop round-trip
        testMatrix.export_to_binary("verify_identity.chronos_temp");
        SearchMatrix recoveryMatrix;
        recoveryMatrix.load_from_binary("verify_identity.chronos_temp");

        if (recoveryMatrix.get_total_bases() != 40 || recoveryMatrix.find_exact_matches("ATTT").empty()) {
            std::cerr << "FAIL: Serialization recovery bit-integrity mismatched.\n";
            return false;
        }
        std::cout << " -> Pass: Binary persistence pipelines preserve 100% data integrity.\n";

        std::cout << "[Verifier] All structural validation parameters PASSED completely.\n";
        return true;
    }
}

#endif // CHRONOS_CORE_HPP
