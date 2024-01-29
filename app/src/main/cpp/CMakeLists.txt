cmake_minimum_required(VERSION 3.4.1)
find_library(log-lib log)
add_library(
        myapplication
        SHARED
        myapplication.cpp
)
target_link_libraries(
        myapplication
        ${log-lib}
        binder_ndk
)