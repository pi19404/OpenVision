file(GLOB SRC
    "*.cpp"
)
add_library ( ImgApp SHARED ${SRC})
target_link_libraries(ImgApp ${OpenCV_LIBS})
set(CMAKE_CXX_FLAGS "-g ")
