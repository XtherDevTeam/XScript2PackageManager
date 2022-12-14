cmake_minimum_required(VERSION 3.23)
project(XScript2PackageManager)

set(CMAKE_CXX_STANDARD 20)

include("${CMAKE_CURRENT_SOURCE_DIR}/XScript2/Share/Git.cmake")

set(WHERE_IS_REPO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/XScript2")
set(XS_GIT_COMMIT_HASH "")
get_git_hash(XS_GIT_COMMIT_HASH)

set(WHERE_IS_REPO_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(PM_GIT_COMMIT_HASH "")
get_git_hash(PM_GIT_COMMIT_HASH)

file(GLOB SRC_FILES
        Manage/*.cpp
        Network/*.cpp
        Utils/*)

file(GLOB XS2SDK_FILES
        "XScript2/Frontend/*.cpp"
        "XScript2/Frontend/Parsers/*.cpp"
        "XScript2/Backend/*.cpp"
        "XScript2/Backend/Compiler/*.cpp"
        "XScript2/Executor/*.cpp"
        "XScript2/Share/*.cpp"
        "XScript2/Share/Exceptions/*.cpp"
        "XScript2/Core/*.cpp"
        "XScript2/Core/Serializatior/*.cpp"
        "XScript2/Core/Reader/*.cpp"
        "XScript2/NativeLibrary/*.cpp"
        )

file(GLOB XARCHIVE_SRC_FILES
        Utils/XArchive/Exceptions/*.cpp
        Utils/XArchive/Sources/*.cpp)

file (GLOB WHEREAMI_SRC_FILES
        Utils/whereami/*.cpp)

add_executable(xpm ${WHEREAMI_SRC_FILES} ${XS2SDK_FILES} ${XARCHIVE_SRC_FILES} ${SRC_FILES} main.cpp)

if (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    TARGET_LINK_LIBRARIES(xpm dl)
    TARGET_LINK_LIBRARIES(xpm pthread)
elseif (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    TARGET_LINK_LIBRARIES(xpm wsock32 ws2_32)
    set(CMAKE_EXE_LINKER_FLAGS "--static -lpthread")
endif ()

add_definitions(-DXSCRIPT2_COMMIT_VERSION=L\"${XS_GIT_COMMIT_HASH}\" -DPM_COMMIT_VERSION=L\"${PM_GIT_COMMIT_HASH}\")