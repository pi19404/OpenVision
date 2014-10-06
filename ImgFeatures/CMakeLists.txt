file(GLOB SRC
    "*.cpp"
)
add_library ( ImgFeatures SHARED ${SRC})
target_link_libraries(ImgFeatures  ${OpenCV_LIBS} ImgProc)
set(CMAKE_CXX_FLAGS "-g ")

