add_library ( ImgProc Histogram.cpp meanshift.cpp)
target_link_libraries(ImgProc ${OpenCV_LIBS} )
