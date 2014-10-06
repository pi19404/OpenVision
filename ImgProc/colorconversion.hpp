//#ifndef COLORCONVERSION_HPP
//#define COLORCONVERSION_HPP


//#include "Common/OpenCVCommon.hpp"
//#ifdef CV_NEON
//#include <arm_neon.h>
//#endif

///**
// * @brief The Interleave class : This class encapsulates methods that perform the interleaving
// * and de-interleaving operations for a 3 channel BGR image .
// */
//class Interleave
//{
//    Interleave()
//    {

//    }

//    Interleave()
//    {

//    }

//    /**
//     * @brief process : this is the main function that performs interveaving or deinterleaving operation
//     *                  based on the suppplied input operations code
//     * @param src     : this is output/input multichannel image depending on operation code
//     * @param dst     : this output/input array of channels that are required to be interleaved/de-interleaved
//     * @param code    : 0-deinterleaving 1-interleaving
//     */
//    void process(Mat src,Mat &dst,int code=0)
//    {

//        bool status=neon_process(src,dst,code);
//    }

//    bool neon_process(Mat src,Mat dst,int code)
//    {
//      #ifdef CV_NEON

//      return true;
//      #endif
//      return false;
//    }

//};


//class ColorConversion
//{
//public:
//    ColorConversion();

//    void proces(Mat src,Mat &dst,int code)
//    {
//            #ifdef CV_NEON



//            #endif

//    }
//};

//#endif // COLORCONVERSION_HPP
