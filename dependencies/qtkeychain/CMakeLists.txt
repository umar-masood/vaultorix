cmake_minimum_required(VERSION 3.16)
project(QtKeychain VERSION 1.0 LANGUAGES CXX)

# -------------------------
#           Qt
# -------------------------
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# -------------------------
#      Library sources
# -------------------------
set(qtkeychain_SOURCES
    keychain.cpp
    keychain.h
    keychain_p.h
    gnomekeyring_p.h
)

if(WIN32)
    list(APPEND qtkeychain_SOURCES keychain_win.cpp plaintextstore.cpp)
elseif(APPLE)
    list(APPEND qtkeychain_SOURCES keychain_apple.mm)
elseif(UNIX AND NOT APPLE)
    list(APPEND qtkeychain_SOURCES keychain_unix.cpp gnomekeyring.cpp libsecret.cpp plaintextstore.cpp)
endif()

# -------------------------
#      Create library
# -------------------------
add_library(QtKeychain STATIC ${qtkeychain_SOURCES})

# -------------------------
#      Include directories
# -------------------------
target_include_directories(QtKeychain
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}          # headers available as <qtkeychain/...>
        ${CMAKE_CURRENT_BINARY_DIR}          # for moc-generated headers
)

# -------------------------
#      Link Qt libraries
# -------------------------
target_link_libraries(QtKeychain
    PUBLIC Qt6::Core
)

# -------------------------
#      Windows specific
# -------------------------
if(WIN32)
    target_link_libraries(QtKeychain PRIVATE crypt32)
    target_compile_definitions(QtKeychain PRIVATE UNICODE)
    set_target_properties(QtKeychain PROPERTIES CXX_STANDARD 17)
endif()

# -------------------------
#      MOC handling
# -------------------------
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# -------------------------
#      Export header
# -------------------------
include(GenerateExportHeader)
generate_export_header(QtKeychain
    EXPORT_FILE_NAME qkeychain_export.h
    EXPORT_MACRO_NAME QKEYCHAIN_EXPORT
)