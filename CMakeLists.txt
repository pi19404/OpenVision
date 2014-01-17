cmake_minimum_required(VERSION 2.8)
PROJECT(OpenVisionLibrary)
find_package( OpenCV 2.4.5 EXACT)
find_library(FFTW_LIBRARY NAMES fftw3f)
set(FFTW_LIBS "${FFTW_LIBRARY}")
set(CMAKE_CXX_FLAGS "-g -Wall")

SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
SET(CMAKE_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/lib")
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


if(OPENCV_FOUND)
  include_directories(${OpenCV_INCLUDE_DIRS})
  target_link_libraries (OpenVisionLibrary ${OpenCV_LIBRARIES} )
endif(OPENCV_FOUND)

set( NAME_SRC
    ImgProc/Histogram.cpp
    ImgProc/meanshift.cpp
)

set( NAME_HEADERS       
ImgProc/meanshift.h
ImgProc/lineDetector.hpp
ImgProc/Histogram.hpp
Common/Common.hpp
)

add_library ( OpenVisionLibrary SHARED ${NAME_SRC})
target_link_libraries(OpenVisionLibrary ${OpenCV_LIBS} )


include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

include_directories("${PROJECT_SOURCE_DIR}/ImgProc")
include_directories("${PROJECT_SOURCE_DIR}/Common")
include_directories("${PROJECT_SOURCE_DIR}/ImgFeatures")
include_directories("${PROJECT_SOURCE_DIR}/ImgVO")
include_directories("${PROJECT_SOURCE_DIR}/others/ACE")
include_directories("${PROJECT_SOURCE_DIR}/others/fastSegment")
add_subdirectory(others)
add_subdirectory(ImgProc)
add_subdirectory(ImgFeatures)
add_subdirectory(ImgVO)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_VERBOSE_MAKEFILE ON)

add_executable( test_vo demo/test_vo.cpp )
target_link_libraries(test_vo ImgVO ImgFeatures ${OpenCV_LIBS} )

add_executable( test_line demo/test_line.cpp )
target_link_libraries(test_line ${FFTW_LIBS} ACE ImgProc ${OpenCV_LIBS} )

add_executable( meanShiftTest demo/test_meanshift.cpp )
target_link_libraries(meanShiftTest OpenVisionLibrary ${OpenCV_LIBS} )


add_executable( testHistogram demo/test_histogram.cpp )
target_link_libraries(testHistogram OpenVisionLibrary ${OpenCV_LIBS} )

add_executable( testfindShape demo/test_mshape.cpp )
target_link_libraries(testfindShape OpenVisionLibrary ${OpenCV_LIBS} )


#INSTALL(TARGETS testHistogram meanShiftTest test_vo DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
#INSTALL(TARGETS test_vo DESTINATION ${CMAKE_BINARY_DIR}/bin)
