cmake_minimum_required(VERSION 3.23)
project(XScript2PackageManager)

set(CMAKE_CXX_STANDARD 20)

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

add_executable(XScript2PackageManager ${XS2SDK_FILES} ${SRC_FILES} main.cpp)
