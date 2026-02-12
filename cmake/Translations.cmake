include(FetchContent)

if (DEFINED ENV{ECLIPSE_TRANSLATIONS_REPO_PATH})
    set(TRANSLATIONS_PATH $ENV{ECLIPSE_TRANSLATIONS_REPO_PATH})
else()
    FetchContent_Declare(
        eclipse_translations
        GIT_REPOSITORY https://github.com/EclipseMenu/translations.git
        GIT_TAG main
        GIT_SHALLOW TRUE
        UPDATE_DISCONNECTED FALSE
    )

    FetchContent_MakeAvailable(eclipse_translations)

    set(TRANSLATIONS_PATH ${eclipse_translations_SOURCE_DIR})
endif()

setup_translations(${PROJECT_NAME})