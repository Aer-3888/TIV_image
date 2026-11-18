cmake_minimum_required(VERSION 3.14)
project(Projet_OpenCV_CMake)


set(CMAKE_CXX_STANDARD 14)
find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})


add_executable(Projet_OpenCV_CMake
        include/histogram.hpp
        src/histogram.cpp
        src/main_test_opencv.cpp)

target_link_libraries(Projet_OpenCV_CMake ${OpenCV_LIBS})


add_executable(test_detect_plus src/test_detect_plus.cpp)
target_link_libraries(test_detect_plus ${OpenCV_LIBS})

add_executable(crop_icons src/crop_icons.cpp)
target_link_libraries(crop_icons ${OpenCV_LIBS})
target_compile_definitions(crop_icons PRIVATE
        "TIV_IMAGE_SOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")

add_executable(align_pages
        include/alignment.hpp
        src/alignment.cpp
        src/align_pages.cpp)
target_link_libraries(align_pages ${OpenCV_LIBS})


add_executable(binary_strip
        include/histogram.hpp
        src/histogram.cpp
        src/binaryStrip.cpp)
target_link_libraries(binary_strip ${OpenCV_LIBS})
