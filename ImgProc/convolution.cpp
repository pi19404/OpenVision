#include "convolution.hpp"

SeperableSConvolution::SeperableSConvolution()
{
}

void SeperableSConvolution::setKernel(Mat row,Mat col,bool symm)
{
    if(rowk.size()==0)
    assert(row.cols==col.cols);
    else
    {
        assert(row.cols==N && col.cols==N);
    }
    N=row.cols;
    rowk.push_back(row);
    colk.push_back(col);
    symmetric.push_back(symm);
}

void SeperableSConvolution::clearKernel()
{
    rowk.resize(0);
    colk.resize(0);
    symmetric.resize(0);
}


void SeperableSConvolution::apply(Mat &src,Mat &dst)
{

    int ch=rowk.size();
    Size s=Size(src.cols,src.rows);
    int n=floor((N+0.5)/2);
    int x,y;

    dst.create(s.height,s.width,CV_32FC(ch));
    //temporary buffer allocated on the stack
    AutoBuffer<float> _row((s.width + n*2)*ch);

    AutoBuffer<float> _res(ch);

    float *res=(float*)_res;

    float *row = (float*)_row + n*ch;

    for(int y=0;y<s.height;y++)
    {
        //pointer to the source and destination
        float *srow0 = (float*)(src.data + src.step*y),*srow1=0;
        float *drow = (float*)(dst.data + dst.step*y);


         //performing vertical convolution
         //using the row kernel
         for( x = 0; x < s.width; x++ )
         {
             for(int l=0;l<ch;l++)
             {
             row[x*ch+l] = srow0[x]*rowk[l].at<float>(n);
             }

         }


       //performing vertical convolution accessing n rows about current row
       //accessing a 2 rows at a time,performing computation
       for(int k=1;k<=n;k++)
       {

        //accessing the vertial abouts about the current row
        srow0 = (float*)(src.data + src.step*std::max(y-k,0));
        srow1 = (float*)(src.data + src.step*std::min(y+k,s.height-1));
        for(int x=0;x<s.width;x++)
        {
            for(int l=0;l<ch;l++)
            {
            float p=srow0[x]+(1*(symmetric[l]?1:-1))*srow1[x];
            row[x*ch+l]+=rowk[l].at<float>(k)*(p);
            }
        }
       }

       //initialize the border pixels of the buffer
       //replicate pixels on the other size
       for(x=0;x<n;x++)
       {
           int c=1;
           for(int l=0;l<ch;l++)
           {
           row[(x+n-c)*ch+l]=row[(x+n)*ch+l];
           row[(s.width+x)*ch+l]=row[(s.width+x-c)*ch+l];
           }
       }

       //preforming horizontal convoltuion

       for(x=0;x<s.width;x++)
       {
           for(int l=0;l<ch;l++)
           {
               res[l]=row[x*ch+l]*colk[l].at<float>(n);
           }

           for(int k=1;k<=n;k++)
           {

               for(int l=0;l<ch;l++)
               {
                   float p=(row[(x+k)]+(1*symmetric[l]?1:-1)*row[(x-k)])*colk[l].at<float>(k);
                    res[l]=res[l]+p;
               }
           }
           //storing the result of convolution
           for(int l=0;l<ch;l++)
           {
               drow[x*ch+l]=res[l];
           }
       }

    }

        row -= n*ch;
}
