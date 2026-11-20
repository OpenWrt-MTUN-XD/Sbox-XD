set(PLATFORM_SOURCES 3rdparty/WinCommander.cpp sys/windows/guihelper.cpp sys/windows/MiniDump.cpp)
set(PLATFORM_LIBRARIES wininet wsock32 ws2_32 user32 rasapi32 iphlpapi)

include(cmake/windows/generate_product_version.cmake)

# 文件版本号与 sboxxd_version.txt / CMake 项目版本同源，避免资源信息里的版本与发版号脱节
string(REPLACE "." ";" SBOX_VERSION_PARTS "${SBOX_PROJECT_VERSION}")
list(GET SBOX_VERSION_PARTS 0 SBOX_VERSION_MAJOR)
list(GET SBOX_VERSION_PARTS 1 SBOX_VERSION_MINOR)
list(GET SBOX_VERSION_PARTS 2 SBOX_VERSION_PATCH)

generate_product_version(
        QV2RAY_RC
        ICON "${CMAKE_SOURCE_DIR}/res/sboxxd.ico"
        NAME "Sbox-XD"
        BUNDLE "Sbox-XD"
        VERSION_MAJOR ${SBOX_VERSION_MAJOR}
        VERSION_MINOR ${SBOX_VERSION_MINOR}
        VERSION_PATCH ${SBOX_VERSION_PATCH}
        COMPANY_NAME "Sbox-XD"
        COMPANY_COPYRIGHT "Sbox-XD"
        FILE_DESCRIPTION "Sbox-XD"
)
add_definitions(-DUNICODE -D_UNICODE -DNOMINMAX)
set(GUI_TYPE WIN32)
if (MINGW)
    if (NOT DEFINED MinGW_ROOT)
        set(MinGW_ROOT "C:/msys64/mingw64")
    endif ()
else ()
    add_compile_options("/utf-8")
    add_compile_options("/std:c++17")
    add_definitions(-D_WIN32_WINNT=0x600 -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)
endif ()
