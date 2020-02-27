cmake_minimum_required(VERSION 3.3)
project(xxint_docs)

set(XXINT_GHPAGES_REPO https://github.com/tov/xxint.git
        CACHE STRING "Git repo URL to push HTML docs to")

set(DOXYGEN_PROJECT_NAME           "++int")
set(DOXYGEN_GENERATE_HTML          YES)
set(DOXYGEN_GENERATE_LATEX         NO)
set(DOXYGEN_GENERATE_MAN           NO)
set(DOXYGEN_JAVADOC_AUTOBRIEF      YES)
set(DOXYGEN_SOURCE_BROWSER         YES)
set(DOXYGEN_INLINE_SOURCES         YES)
set(DOXYGEN_CLANG_ASSISTED_PARSING YES)
set(DOXYGEN_EXCLUDE_SYMBOLS        detail)
set(DOXYGEN_GENERATE_TREEVIEW      YES)
set(DOXYGEN_DISABLE_INDEX          YES)
set(DOXYGEN_HIDE_FRIEND_COMPOUNDS  YES)
set(DOXYGEN_HIDE_SCOPE_NAMES       YES)
set(DOXYGEN_HTML_DYNAMIC_SECTIONS  YES)
set(DOXYGEN_SHOW_INCLUDE_FILES     NO)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE FRONTMATTER.md)

doxygen_add_docs(xxint_docs
        FRONTMATTER.md
        ../xxint)

add_custom_target(xxint_upload_docs
        COMMAND ghp-import -n ${CMAKE_CURRENT_BINARY_DIR}/html
        COMMAND git push -f ${XXINT_GHPAGES_REPO} gh-pages
        DEPENDS xxint_docs)
