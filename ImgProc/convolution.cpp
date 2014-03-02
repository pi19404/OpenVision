#include "convolution.hpp"

SeperableSConvolution::SeperableSConvolution()
{
}

void SeperableSConvolution::setKernel(Mat row,Mat col,bool rsymm,bool csymm)
{
    N=std::max(row.cols,row.rows);
    rowk.push_back(col.clone());
    colk.push_back(row.clone());

    cerr <<row <<endl;
    cerr << col << endl;
    csymmetric.push_back(rsymm);
    rsymmetric.push_back(csymm);
}

void SeperableSConvolution::clearKernel()
{
    rowk.resize(0);
    colk.resize(0);
    rsymmetric.resize(0);
    csymmetric.resize(0);
}


void SeperableSConvolution::apply(Mat &src,Mat &dst)
{


    int ch=rowk.size();
    Size s=Size(src.cols,src.rows);
    int n=floor((N+0.5)/2);


    dst.create(s.height,s.width,CV_32FC(ch));
    //ch=1;
    //temporary buffer allocated on the stack
    AutoBuffer<float> _row((s.width + n*2)*ch);
    AutoBuffer<float> _res(ch);

    float *res=(float*)_res;
    float *row = (float*)_row + n*ch;
    //N=3
    for(int y=0;y<s.height;y++)
    {
        //pointer to the source and destination
        float *srow0 = (float*)(src.data + src.step*y),*srow1=0;
        float *drow = (float*)(dst.data + dst.step*y);


         //performing vertical convolution
         //using the row kernel
         for(int x = 0; x < s.width; x++ )
         {
             #pragma unroll_factor=20
             for(int l=0;l<ch;l++)
             {
             row[x*ch+l] = srow0[x]*(rowk[l].at<float>(n));
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
            #pragma unroll
            for(int l=0;l<ch;l++)
            {
            float p=srow0[x]+(1*(rsymmetric[l]?1:-1))*srow1[x];
            row[x*ch+l]+=(rowk[l].at<float>(n-k))*(p);
            }
        }
       }



       //preforming horizontal convoltuion

       for(int x=0;x<s.width;x++)
       {
           #pragma unroll
           for(int l=0;l<ch;l++)
           {
               res[l]=row[x*ch+l]*(colk[l].at<float>(n));
           }

           for(int k=1;k<=n;k++)
           {

               #pragma unroll
               for(int l=0;l<ch;l++)
               {
                   int index1=std::max(x-k,0);
                   int index2=std::min(x+k,s.width-1);
                   float p=(row[index1*ch+l]+(csymmetric[l]?1:-1)*row[index2*ch+l])*(colk[l].at<float>(n-k));
                    res[l]=res[l]+p;
               }
           }
           //storing the result of convolution
           #pragma unroll
           for(int l=0;l<ch;l++)
           {
               drow[x*ch+l]=res[l];
           }
       }

    }

        //row -= n*ch;
}
