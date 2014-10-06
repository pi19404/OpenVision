


INCLUDES="-I/usr/local/include -I/usr/local/include/opencv -I/usr/local/include/opencv2 -I. -I../ImgProc -I../Common -I.."



gcc  ${INCLUDES} ../ImgProc/Histogram.cpp test_histogram.cpp -L/usr/local/lib `pkg-config --libs opencv`


#gcc -I/usr/local/include -I/usr/local/include/opencv -I/usr/local/include/opencv2 -I. -I../ImgProc -I../Common #Histogram.cpp test_histogram.cpp -L/usr/local/lib `pkg-config --libs opencv`


