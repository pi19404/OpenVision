#ifndef CONVOLUTION_HPP
#define CONVOLUTION_HPP

#include "Common/OpenCVCommon.hpp"

template<typename  T>
class Convolution
{

private :
int _image_width;
int _image_height;
int _kernel_width;
int _kernel_height;
int _image_channels;
int _kernel_channels;
int _kernel_norm;
public:
    Convolution();

    //border type
    enum{BORDER_ZERO,BORDER_CONSTANT,BORDER_REPLICATE};

    /**
     * @brief checkPixel checks if the pixel lies within the image borders
     * @param _input is the input image
     * @param p is 2D location of the pixel
     * @return  boolen value
     */
    bool checkPixel(Point p)
    {
        bool flag_top_left=(p.y>=0 && p.x=0);
        bool flag_bottom_right=(p.y< _image_height) && (p.x <_image_width);
        return flag_top_left && flag_bottom_right;
    }


    /**
     * @brief getPixelValue returns the pixel value at location of point in matrix M
     * @param M input matrix
     * @param p indicates the location
     * @return
     */
    T  getPixelValue(Mat &M,Point p)
    {
            T val=M.data + M.step*p.y + p.x*M.elemSize();
            return val;
    }

    void setPixelValue(Mat &M,Point p,T value)
    {

        M.data+ M.step*p.y + p.x*M.elemSize()=value;
    }
    /**
     * @brief conv2d methods defines a convolution operations
     * @param _input
     * @param _kernel
     * @param dst
     */
    void conv2d(Mat _input,Mat _kernel,Mat &_out)
    {
            _image_height=_input.rows;
            _image_width =_input.cols;
            _kernel_height=_kernel.rows;
            _kernel_width=_kernel.cols;
            _image_channels=_input.channels ();
            _kernel_channels=_kernel.channels ();

            //dst.create (_image_height,_image_width,_input.type ());

            for(int y=0;y<_image_height;y++)
            {

            for(int x=0;x<_image_width;x++)
            {


                //performs computation for pixels in the valid range
                Point p=Point(x,y);

                //local variable used  to store convolution sum
                float value=0;
                //loop over the rows of the pixel neighborhood
                //loop over the columns of the pixels neighborhood
                for(int i=0;i<_kernel_height;i++)
                for(int j=0;j<_kernel_width;j++)
                {
                    Point pixel=Point(y+i-(_kernel_height/2),(x+j-(_kernel_width/2)));
                    //condition defines pixels lying within the image borders
                    bool flag=checkPixel (pixel);
                    if(flag==true)
                    {
                    T val=getPixelValue(_input,pixel);
                    value=value+val;
                    }
                   }

                //setting the result in the destination matrix
                setPixelValue (_out,Point(x,y),value);
            }


            }

            }




protected:
    Mat _input;
    Mat _kernel;
    Mat _out;
    bool _separable;
    bool _normalize;
    int _boundaryMode;

public:
//function to compute convolution
void run(Mat src,Mat &dst);

//function to compute convolution with input kernel.
void run(Mat src,Mat kernel,Mat &dst,int boundaryMode);

void setKernel(Mat kernel,bool normalize=true)
{
    kernel.copyTo (_kernel);
}

};

#endif // CONVOLUTION_HPP
