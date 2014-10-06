file(GLOB SRC
    "*.cpp"
)
add_library ( ImgUtils SHARED ${SRC})
target_link_libraries(ImgUtils ${OpenCV_LIBS})
set(CMAKE_CXX_FLAGS "-g ")

