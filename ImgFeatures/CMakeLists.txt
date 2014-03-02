add_library ( ImgFeatures SHARED TemporalFilter.cpp FeatureDetector.cpp Fast.cpp GoodFeatureToTrack.cpp SubPixelCorner.cpp HarrisCorner.cpp integralImage.cpp lbpfeatures.cpp randomferns.cpp)
target_link_libraries(ImgFeatures  ${OpenCV_LIBS} gomp)
set(CMAKE_CXX_FLAGS "-g -fopenmp")
INSTALL(TARGETS ImgFeatures DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
