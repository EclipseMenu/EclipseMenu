#pragma once
#include <source_location>
#include <string_view>

// shared utils for debugging
namespace eclipse::debug::util {
    template <size_t N>
    struct StaticString {
        char data[N];
        constexpr operator std::string_view() const { return std::string_view(data, N); }
    };

    consteval size_t getFunctionNameSize(const char* fileName, size_t line, const char* functionName) {
        size_t size = 1;
        constexpr char begin[] = "src/";
        size_t beginIndex = 0;
        for (int i = 0; fileName[i]; i++) {
            for (int j = 0; begin[j]; j++) {
                if (fileName[i + j] != begin[j]) break;
                if (begin[j + 1] == 0) {
                    beginIndex = i;
                }
            }
        }
        for (int i = beginIndex; fileName[i]; i++) {
            size++;
        }
        size += 1;
        while (line > 0) {
            size++;
            line /= 10;
        }
        size += 2;
        for (int i = 0; functionName[i]; i++) {
            size++;
        }
        return size;
    }

    template <size_t N>
    consteval auto getFunctionName(std::source_location loc) {
        StaticString<N> result = {};

        // Strip absolute path to just the project path (src/...)
        constexpr char begin[] = "src/";
        size_t beginIndex = 0;
        for (int i = 0; loc.file_name()[i]; i++) {
            for (int j = 0; begin[j]; j++) {
                if (loc.file_name()[i + j] != begin[j]) break;
                if (begin[j + 1] == 0) {
                    beginIndex = i;
                }
            }
        }

        // Filename
        size_t index = 0;
        result.data[index++] = '(';
        for (int i = beginIndex; loc.file_name()[i]; i++) {
            result.data[index++] = loc.file_name()[i];
        }

        result.data[index++] = ':';

        // Line number
        size_t line = loc.line();
        size_t lineIndex = index;
        do {
            result.data[index++] = '0' + line % 10;
            line /= 10;
        } while (line > 0);

        // Reverse line number (it was written backwards)
        std::reverse(result.data + lineIndex, result.data + index);

        result.data[index++] = ')';
        result.data[index++] = ' ';

        // Function name
        for (int i = 0; loc.function_name()[i]; i++) {
            result.data[index++] = loc.function_name()[i];
        }

        return result;
    }

    template <size_t N, size_t K>
    constexpr auto removeFromString(const char (&expr)[N], const char (&remove)[K]) {
        StaticString<N> result = {};
        size_t srcIndex = 0;
        size_t dstIndex = 0;
        while (srcIndex < N) {
            size_t matchIndex = 0;
            while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[
                       matchIndex])
                matchIndex++;
            if (matchIndex == K - 1)
                srcIndex += matchIndex;
            result.data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
            srcIndex++;
        }
        return result;
    }
}

#define GET_FUNC_NAME(targetName)\
constexpr auto info_##targetName = std::source_location::current();\
constexpr auto size_##targetName = eclipse::debug::util::getFunctionNameSize(info_##targetName.file_name(), info_##targetName.line(), info_##targetName.function_name());\
constexpr auto func_##targetName = eclipse::debug::util::getFunctionName<size_##targetName + 1>(info_##targetName);\
constexpr auto targetName = eclipse::debug::util::removeFromString(func_##targetName.data, "__cdecl ")\
