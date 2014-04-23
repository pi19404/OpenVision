file(GLOB SRC
    "*.cpp"
)
add_library ( ImgProc SHARED ${SRC})
target_link_libraries(ImgProc ${OpenCV_LIBS})
