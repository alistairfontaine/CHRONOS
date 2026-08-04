#ifndef CHRONOS_CORE_HPP
#define CHRONOS_CORE_HPP

#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>

namespace Chronos {

    // Representation configuration mappings:
    // A -> 00 (0)
    // C -> 01 (1)
    // G -> 10 (2)
    // T -> 11 (3)
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

    inline char decode_nucleotide(uint8_t bits) {
        switch (bits & 0x03) {
            case 0x00: return 'A';
            case 0x01: return 'C';
            case 0x02: return 'G';
            case 0x03: return 'T';
            default: return 'N';
        }
    }

    // High-density data payload packet block
    // Aligned to 64-bit hardware boundaries to match L1/L2 cache blocks
    struct alignas(8) GenomicWord {
        uint64_t data = 0;
        uint8_t length = 0; // Number of bases packed (Max 32)
    };

    class SearchMatrix {
    private:
        std::vector<GenomicWord> sequenceWords;
        size_t totalBasesParsed = 0;

    public:
        SearchMatrix() = default;

        // Compress raw ASCII target sequences down to 2-bit storage arrays
        void import_raw_sequence(const std::string& sequence) {
            GenomicWord currentWord;
            int bitShift = 0;

            for (char base : sequence) {
                // Skip newline strings or whitespace formatting from FASTA records
                if (base == ' ' || base == '\n' || base == '\r') continue;

                uint64_t encodedBits = encode_nucleotide(base);

                // Shift encoded 2-bits into current 64-bit integer tracking space
                currentWord.data |= (encodedBits << bitShift);
                bitShift += 2;
                currentWord.length++;
                totalBasesParsed++;

                // Once 32 pairs fill the 64-bit limit, commit block to vector heap
                if (currentWord.length == 32) {
                    sequenceWords.push_back(currentWord);
                    currentWord.data = 0;
                    currentWord.length = 0;
                    bitShift = 0;
                }
            }

            // Flush remaining partial data hanging elements
            if (currentWord.length > 0) {
                sequenceWords.push_back(currentWord);
            }
        }

        // Bitwise mutation scan tracking loop execution
        size_t find_exact_matches(const std::string& query) const {
            if (query.empty() || sequenceWords.empty()) return 0;

            // For prototyping tomorrow, we'll implement a fast sliding bit mask tracker.
            // This stub prepares our CLI test infrastructure.
            size_t matchesFound = 0;
            return matchesFound;
        }

        size_t get_word_count() const { return sequenceWords.size(); }
        size_t get_total_bases() const { return totalBasesParsed; }
    };
}

#endif // CHRONOS_CORE_HPP
