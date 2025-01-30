cmake_minimum_required(VERSION 3.21)

project(sbosubmit CXX)

set(SBOSUBMIT_VERSION 0.1rc1)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(
    $<$<CONFIG:Debug>:-O0>
    -ggdb3
    -Wshadow
    -Wpointer-arith
    -Wcast-qual
    -Wcast-align
    -Wconversion
    -Wzero-as-null-pointer-constant
    -Wuseless-cast
    -Wsign-conversion
    -Wfloat-conversion
    -Wlogical-op
    -Wmissing-declarations
    -Wredundant-decls
)

add_executable(sbosubmit)
add_subdirectory(src)

find_package(LibArchive REQUIRED)
find_package(CURL REQUIRED)

target_link_libraries(sbosubmit
    PRIVATE
    ${CURL_LIBRARIES}
    ${LibArchive_LIBRARIES}
)

set(CMAKE_INSTALL_MANDIR man CACHE STRING "MANDIR")
set(CMAKE_INSTALL_DOCDIR doc/sbosubmit-${SBOSUBMIT_VERSION} CACHE STRING "DOCDIR")

install(TARGETS sbosubmit RUNTIME)
install(FILES man/sbosubmit.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
install(FILES
    doc/BUGS
    doc/ChangeLog
    doc/CONDITIONS
    doc/INSTALL
    doc/LICENSE
    doc/README
    doc/TODO
    TYPE DOC
)
