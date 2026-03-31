#include "Stemmer.hpp"

#include <algorithm>
#include <array>

namespace eclipse::ai {
    std::string PorterStemmer::stem(std::string word) {
        if (word.size() < 3) return word;

        Stemmer s{std::move(word)};
        if (s.exception1()) return std::move(s.word);

        s.prelude();
        s.markRegions();

        s.step1a();

        if (!s.exception2()) {
            s.step1b();
            s.step1c();
            s.step2();
            s.step3();
            s.step4();
            s.step5();
        }

        s.postlude();

        return std::move(s.word);
    }

    bool PorterStemmer::Stemmer::exception1() {
        static constexpr auto exceptions = std::to_array<std::pair<std::string_view, std::string_view>>({
            {"skies","sky"},
            {"dying","die"},
            {"lying","lie"},
            {"tying","tie"},
            {"idly","idl"},
            {"gently","gentl"},
            {"ugly","ugli"},
            {"early","earli"},
            {"only","onli"},
            {"singly","singl"},
        });

        static constexpr auto exceptions2 = std::to_array<std::string_view>({
            "sky", "news", "howe", "atlas", "cosmos", "bias", "andes"
        });

        for (auto const& [exc, rep] : exceptions) {
            if (word == exc) {
                this->word = rep;
                return true;
            }
        }

        for (auto const& exc : exceptions2) {
            if (word == exc) {
                return true;
            }
        }

        return false;
    }

    bool PorterStemmer::Stemmer::exception2() const {
        static constexpr auto exceptions = std::to_array<std::string_view>({
            "inning", "outing", "canning", "herring", "earring", "proceed", "exceed", "succeed"
        });

        for (auto const& exc : exceptions) {
            if (word == exc) {
                return true;
            }
        }

        return false;
    }

    constexpr static bool isVowel(char c) {
        switch (c) {
            case 'a': case 'e': case 'i': case 'o': case 'u': case 'y': return true;
            default: return false;
        }
    }

    constexpr static bool endsDouble(std::string_view word) {
        auto n = word.size();
        return n >= 2 && word[n - 1] == word[n - 2] &&
            std::string_view("bdfgmnprt").find(word[n - 1]) != std::string_view::npos;
    }

    constexpr static bool isShortSyllable(std::string_view word) {
        auto n = word.size();
        if (n == 2) {
            return isVowel(word[0]) && !isVowel(word[1]);
        }
        if (n >= 3) {
            char a = word[n - 3];
            char b = word[n - 2];
            char c = word[n - 1];
            return !isVowel(a) && isVowel(b) && !isVowel(c) && c != 'w' && c != 'x' && c != 'Y';
        }
        return false;
    }

    void PorterStemmer::Stemmer::prelude() {
        if (!word.empty() && word[0] == 'y') {
            word[0] = 'Y';
            foundY = true;
        }

        for (size_t i = 1; i < word.size(); ++i) {
            if (word[i] == 'y' && isVowel(word[i - 1])) {
                word[i] = 'Y';
                foundY = true;
            }
        }
    }

    void PorterStemmer::Stemmer::postlude() {
        if (!foundY) return;
        for (char& c : word) {
            if (c == 'Y') c = 'y';
        }
    }

    void PorterStemmer::Stemmer::markRegions() {
        P1 = P2 = word.size();

        if (word.starts_with("gener")) P1 = 5;
        else if (word.starts_with("commun")) P1 = 6;
        else if (word.starts_with("arsen")) P1 = 5;
        else {
            for (size_t i = 1; i < word.size(); ++i) {
                if (!isVowel(word[i]) && isVowel(word[i - 1])) {
                    P1 = i + 1;
                    break;
                }
            }
        }

        for (size_t i = P1 + 1; i < word.size(); ++i) {
            if (!isVowel(word[i]) && isVowel(word[i - 1])) {
                P2 = i + 1;
                break;
            }
        }
    }

    void PorterStemmer::Stemmer::step1a() {
        /*
        define Step_1a as (
            try (
                [substring] among (
                    '{'}' '{'}s' '{'}s{'}'
                           (delete)
                )
            )
            [substring] among (
                'sses' (<-'ss')
                'ied' 'ies'
                       ((hop 2 <-'i') or <-'ie')
                's'    (next gopast v delete)
                'us' 'ss'
            )
        )
        */

        if (word.ends_with("sses")) {
            word.erase(word.size() - 2); // replace with "ss"
        } else if (word.ends_with("ies") || word.ends_with("ied")) {
            if (word.size() > 4) {
                word.replace(word.size() - 3, 3, "i");
            } else {
                word.replace(word.size() - 3, 3, "ie");
            }
        } else if (word.ends_with("s") && !word.ends_with("ss") && !word.ends_with("us")) {
            auto stem = std::string_view(word).substr(0, word.size() - 1);
            if (std::ranges::any_of(stem, isVowel)) {
                word.pop_back(); // remove last 's' if not preceded by 's' or 'u' and contains a vowel
            }
        }
    }

    void PorterStemmer::Stemmer::step1b() {
        /*
        define Step_1b as (
            [substring] among (
                'eed' 'eedly'
                    (R1 <-'ee')
                'ed' 'edly' 'ing' 'ingly'
                    (
                    test gopast v  delete
                    test substring among(
                        'at' 'bl' 'iz'
                             (<+ 'e')
                        'bb' 'dd' 'ff' 'gg' 'mm' 'nn' 'pp' 'rr' 'tt'
                        // ignoring double c, h, j, k, q, v, w, and x
                             ([next]  delete)
                        ''   (atmark p1  test shortv  <+ 'e')
                    )
                )
            )
        )
        */

        if (word.ends_with("eedly") && word.size() - 5 >= P1) {
            word.erase(word.size() - 3); // replace with "ee"
            return;
        }

        if (word.ends_with("eed") && word.size() - 3 >= P1) {
            word.erase(word.size() - 1); // replace with "ee"
            return;
        }

        static constexpr auto suffixes = std::to_array<std::string_view>({
            "ingly", "edly", "ing", "ed"
        });

        for (auto suf : suffixes) {
            if (!word.ends_with(suf)) continue;

            auto stem = std::string_view(word).substr(0, word.size() - suf.size());
            if (!std::ranges::any_of(stem, isVowel)) return;

            word.erase(word.size() - suf.size());

            if (word.ends_with("at") || word.ends_with("bl") || word.ends_with("iz")) {
                word.push_back('e');
            } else if (endsDouble(word)) {
                word.pop_back();
            } else if (word.size() >= P1 && isShortSyllable(word)) {
                word.push_back('e');
            }

            return;
        }
    }

    void PorterStemmer::Stemmer::step1c() {
        /*
        define Step_1c as (
            ['y' or 'Y']
            non-v not atlimit
            <-'i'
        )
        */

        if (word.size() >= 2 && (word.back() == 'y' || word.back() == 'Y') && !isVowel(word[word.size() - 2])) {
            word.back() = 'i';
        }
    }

    bool PorterStemmer::Stemmer::applyRules(std::span<Rule const> rules) {
        for (auto const& rule : rules) {
            if (!word.ends_with(rule.suffix)) continue;

            size_t pos = word.size() - rule.suffix.size();
            if (rule.region == Rule::Region::P1 && pos < P1) continue;
            if (rule.region == Rule::Region::P2 && pos < P2) continue;

            word.replace(pos, rule.suffix.size(), rule.replacement);
            return true;
        }
        return false;
    }

    void PorterStemmer::Stemmer::step2() {
        /*
        define Step_2 as (
            [substring] R1 among (
                'tional'  (<-'tion')
                'enci'    (<-'ence')
                'anci'    (<-'ance')
                'abli'    (<-'able')
                'entli'   (<-'ent')
                'izer' 'ization'
                          (<-'ize')
                'ational' 'ation' 'ator'
                          (<-'ate')
                'alism' 'aliti' 'alli'
                          (<-'al')
                'fulness' (<-'ful')
                'ousli' 'ousness'
                          (<-'ous')
                'iveness' 'iviti'
                          (<-'ive')
                'biliti' 'bli'
                          (<-'ble')
                'ogi'     ('l' <-'og')
                'fulli'   (<-'ful')
                'lessli'  (<-'less')
                'li'      (valid_LI delete)
            )
        )
        */
        static constexpr auto rules = std::to_array<Rule>({
            {"ational", "ate", Rule::Region::P1},
            {"tional", "tion", Rule::Region::P1},
            {"enci", "ence", Rule::Region::P1},
            {"anci", "ance", Rule::Region::P1},
            {"abli", "able", Rule::Region::P1},
            {"entli", "ent", Rule::Region::P1},
            {"ization", "ize", Rule::Region::P1},
            {"izer", "ize", Rule::Region::P1},
            {"ation", "ate", Rule::Region::P1},
            {"ator", "ate", Rule::Region::P1},
            {"alism", "al", Rule::Region::P1},
            {"aliti", "al", Rule::Region::P1},
            {"alli", "al", Rule::Region::P1},
            {"fulness", "ful", Rule::Region::P1},
            {"ousli", "ous", Rule::Region::P1},
            {"ousness", "ous", Rule::Region::P1},
            {"iveness", "ive", Rule::Region::P1},
            {"iviti", "ive", Rule::Region::P1},
            {"biliti", "ble", Rule::Region::P1},
            {"fulli", "ful", Rule::Region::P1},
            {"lessli", "less", Rule::Region::P1},
            {"bli", "ble", Rule::Region::P1},
        });

        if (this->applyRules(rules)) return;

        // handle 'ogi' and 'li' separately since they have special conditions
        if (word.ends_with("ogi") && word.size() - 3 >= P1 && word[word.size() - 4] == 'l') {
            word.replace(word.size() - 3, 3, "og");
        } else if (word.ends_with("li") && word.size() - 2 >= P1) {
            char c = word[word.size() - 3];
            if (std::string_view("cdeghkmnrt").find(c) != std::string_view::npos) {
                word.erase(word.size() - 2);
            }
        }
    }

    void PorterStemmer::Stemmer::step3() {
        /*
        define Step_3 as (
            [substring] R1 among (
                'tional'  (<- 'tion')
                'ational' (<- 'ate')
                'alize'   (<-'al')
                'icate' 'iciti' 'ical'
                          (<-'ic')
                'ful' 'ness'
                          (delete)
                'ative'
                          (R2 delete)  // 'R2' added Dec 2001
            )
        )
        */

        static constexpr auto rules = std::to_array<Rule>({
            {"ational", "ate", Rule::Region::P1},
            {"tional", "tion", Rule::Region::P1},
            {"alize", "al", Rule::Region::P1},
            {"icate", "ic", Rule::Region::P1},
            {"iciti", "ic", Rule::Region::P1},
            {"ical", "ic", Rule::Region::P1},
            {"ful", "", Rule::Region::P1},
            {"ness", "", Rule::Region::P1},
            {"ative", "", Rule::Region::P2},
        });

        this->applyRules(rules);
    }

    void PorterStemmer::Stemmer::step4() {
        /*
        define Step_4 as (
            [substring] R2 among (
                'al' 'ance' 'ence' 'er' 'ic' 'able' 'ible' 'ant' 'ement'
                'ment' 'ent' 'ism' 'ate' 'iti' 'ous' 'ive' 'ize'
                          (delete)
                'ion'     ('s' or 't' delete)
            )
        )
        */

        static constexpr auto suffixes = std::to_array<std::string_view>({
            "al", "ance", "ence", "er", "ic", "able", "ible",
            "ant", "ement", "ment", "ent", "ism", "ate",
            "iti", "ous", "ive", "ize"
        });

        for (auto suf : suffixes) {
            if (word.ends_with(suf) && word.size() - suf.size() >= P2) {
                word.erase(word.size() - suf.size());
                return;
            }
        }

        if (word.size() >= 4 && word.ends_with("ion") && word.size() - 3 >= P2) {
            char c = word[word.size() - 4];
            if (c == 's' || c == 't') {
                word.erase(word.size() - 3);
            }
        }
    }

    void PorterStemmer::Stemmer::step5() {
        /*
        define Step_5 as (
            [substring] among (
                'e' (R2 or (R1 not shortv) delete)
                'l' (R2 'l' delete)
            )
        )
        */

        if (word.ends_with("e")) {
            if (word.size() - 1 >= P2) {
                word.pop_back();
            } else if (word.size() - 1 >= P1 && !isShortSyllable(std::string_view(word).substr(0, word.size() - 1))) {
                word.pop_back();
            }
        } else if (word.ends_with("l") && word.size() - 1 >= P2 && word[word.size() - 2] == 'l') {
            word.pop_back();
        }
    }
}
