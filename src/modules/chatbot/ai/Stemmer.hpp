#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace eclipse::ai {
    class PorterStemmer {
    public:
        static std::string stem(std::string word);

    private:
        struct Stemmer {
            std::string word;
            size_t P1 = 0;
            size_t P2 = 0;
            bool foundY = false;

            Stemmer(std::string word) : word(std::move(word)) {}

            bool exception1();
            bool exception2() const;

            void prelude();
            void postlude();
            void markRegions();

            void step1a();
            void step1b();
            void step1c();

            struct Rule {
                std::string_view suffix;
                std::string_view replacement;
                enum class Region { None, P1, P2 } region;
            };

            bool applyRules(std::span<Rule const> rules);

            void step2();
            void step3();
            void step4();
            void step5();
        };
    };
}