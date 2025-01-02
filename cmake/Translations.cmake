cmake_minimum_required(VERSION 3.21)

if (DEFINED ENV{ECLIPSE_TRANSLATIONS_REPO_PATH})
    add_subdirectory(
        $ENV{ECLIPSE_TRANSLATIONS_REPO_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}/eclipse-translations
    )
else()
    CPMAddPackage(
        NAME eclipse-translations
        GITHUB_REPOSITORY EclipseMenu/translations
        GIT_TAG main
    )
endif()

setup_translations(${PROJECT_NAME})