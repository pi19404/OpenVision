cmake_minimum_required(VERSION 2.8)
PROJECT(OpenVisionLibrary)
find_package( OpenCV 2.4.5 EXACT)

if(OPENCV_FOUND)
  include_directories(${OpenCV_INCLUDE_DIRS})
  target_link_libraries (OpenVisionLibrary ${OpenCV_LIBRARIES})
endif(OPENCV_FOUND)

set( NAME_SRC
    ImgProc/Histogram.cpp
    ImgProc/meanshift.cpp
)

set( NAME_HEADERS       
ImgProc/meanshift.h
ImgProc/Histogram.hpp
Common/Common.hpp
)

add_library ( OpenVisionLibrary ${NAME_SRC})
#target_link_libraries(OpenVisionLibrary ${OpenCV_LIBS} )


include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

include_directories("${PROJECT_SOURCE_DIR}/ImgProc")
include_directories("${PROJECT_SOURCE_DIR}/Common")
add_subdirectory(ImgProc)
add_subdirectory(ImgFeatures)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
SET(CMAKE_INSTALL_RPATH "/usr/local/lib")
add_executable( meanShiftTest demo/test_meanshift.cpp )
target_link_libraries(meanShiftTest OpenVisionLibrary ${OpenCV_LIBS} )


add_executable( testHistogram demo/test_histogram.cpp )
target_link_libraries(testHistogram OpenVisionLibrary ${OpenCV_LIBS} )


INSTALL(TARGETS testHistogram meanShiftTest RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin)
