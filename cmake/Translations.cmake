cmake_minimum_required(VERSION 3.21)

if (DEFINED ENV{ECLIPSE_TRANSLATIONS_REPO_PATH})
    add_subdirectory(
        $ENV{ECLIPSE_TRANSLATIONS_REPO_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}/eclipse-translations
    )
    set(TRANSLATIONS_PATH $ENV{ECLIPSE_TRANSLATIONS_REPO_PATH})
else()
    if (DEFINED ENV{GITHUB_ACTIONS})
        # Delete CPM cache for the translations package to force a re-download
        # This is necessary because github actions keeps the cache between runs (and we don't want that)
        file(REMOVE_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/cpm-cache/eclipse-translations)
    endif()

    CPMAddPackage(
        NAME eclipse-translations
        GITHUB_REPOSITORY EclipseMenu/translations
        GIT_TAG main
    )
    set(TRANSLATIONS_PATH ${eclipse-translations_SOURCE_DIR})
endif()

setup_translations(${PROJECT_NAME})