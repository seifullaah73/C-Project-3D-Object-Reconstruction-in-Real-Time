#include<vector>
#include<opencv\cv.h>
#include<opencv\highgui.h>
#include<opencv2/imgproc/imgproc.hpp>
#include<cstdlib>
#include<iostream>
#include<cmath>
#include<omp.h>

using namespace cv;
using namespace std;

Scalar meanpixel(Mat matrx)
{
	Scalar meanpix;

	#pragma omp parallel for
	for(int i=0;i<matrx.rows;i++)
	{
		for(int j=0;j<matrx.cols;j++)
		{
			meanpix[0]=meanpix[0]+double(matrx.at<Vec3b>(i,j)[0]);
			meanpix[1]=meanpix[1]+double(matrx.at<Vec3b>(i,j)[1]);
			meanpix[2]=meanpix[2]+double(matrx.at<Vec3b>(i,j)[2]);
		}
	}
	meanpix[0]=floor((meanpix[0]/(matrx.rows*matrx.cols))+0.5);
	meanpix[1]=floor((meanpix[1]/(matrx.rows*matrx.cols))+0.5);
	meanpix[2]=floor((meanpix[2]/(matrx.rows*matrx.cols))+0.5);
	return meanpix;
}

Mat covarmatrixcal(Mat matrx1, Scalar meanscal)
{

	double rr=0,rg=0,rb=0,gr=0,gg=0,gb=0,br=0,bg=0,bb=0;
	int k=0;
	k=(matrx1.rows*matrx1.cols);
	Vec3b temp1;
	#pragma omp parallel for
	for(int i = 0; i < matrx1.rows;i++)
	{
		for(int j=0;j<matrx1.cols;j++)
		{
			temp1=matrx1.at<Vec3b>(i,j);
			rr=rr+((double(temp1[0])-meanscal[0])*(double(temp1[0])-meanscal[0]));
			rg=rg+((double(temp1[0])-meanscal[0])*(double(temp1[1])-meanscal[1]));
			rb=rb+((double(temp1[0])-meanscal[0])*(double(temp1[2])-meanscal[2]));
			gg=gg+((double(temp1[1])-meanscal[1])*(double(temp1[1])-meanscal[1]));
			gb=gb+((double(temp1[1])-meanscal[1])*(double(temp1[2])-meanscal[2]));
			bb=bb+((double(temp1[2])-meanscal[2])*(double(temp1[2])-meanscal[2]));
		}
	}
	rr=rr/(k-1);
	rg=rg/(k-1);
	rb=rb/(k-1);
	gr=rg;
	gg=gg/(k-1);
	gb=gb/(k-1);
	br=rb;
	bg=gb;
	bb=bb/(k-1);

	double m[3][3]={{rr,rg,rb},{gr,gg,gb},{br,bg,bb}};

	Mat covmatrix(3,3,CV_64F,m);
	return covmatrix.clone();

}

double mahadistance(Mat invcov, Scalar meanvec, Vec3b patternvec)
{
	double distmeasure=0;
	Scalar x1,x2;
	
	//mahalanobis distance is equal to (x-mean)^T*inv(cov)*(x-mean)
	x1[0]=(patternvec[0]-meanvec[0]);
	x1[1]=(patternvec[1]-meanvec[1]);
	x1[2]=(patternvec[2]-meanvec[2]);
	
	#pragma omp parallel for
	for(int k = 0; k < 3;k++)
	{
		x2[0]=x2[0]+(x1[k]*invcov.at<double>(k,0));
		x2[1]=x2[1]+(x1[k]*invcov.at<double>(k,1));  
		x2[2]=x2[2]+(x1[k]*invcov.at<double>(k,2));
	}

	distmeasure=((x2[0]*x1[0])+(x2[1]*x1[1])+(x2[2]*x1[2]));
	return distmeasure;
}

Mat inversemat(Mat matrx)
{
	double determ;
	determ = (matrx.at<double>(0,0)*((matrx.at<double>(1,1)*matrx.at<double>(2,2))-(matrx.at<double>(1,2)*matrx.at<double>(2,1))))-(matrx.at<double>(0,1)*((matrx.at<double>(1,0)*matrx.at<double>(2,2))-(matrx.at<double>(2,0)*matrx.at<double>(1,2))))+(matrx.at<double>(0,2)*((matrx.at<double>(1,0)*matrx.at<double>(2,1))-(matrx.at<double>(1,1)*matrx.at<double>(2,0))));
	Mat mt=matrx.t();
	double m11,m12,m13,m21,m22,m23,m31,m32,m33;
	m11=(mt.at<double>(1,1)*mt.at<double>(2,2))-(mt.at<double>(2,1)*mt.at<double>(1,2));
	m12=(mt.at<double>(1,0)*mt.at<double>(2,2))-(mt.at<double>(2,0)*mt.at<double>(1,2));
	m13=(mt.at<double>(1,0)*mt.at<double>(2,1))-(mt.at<double>(2,0)*mt.at<double>(1,1));
	m21=(mt.at<double>(0,1)*mt.at<double>(2,2))-(mt.at<double>(2,1)*mt.at<double>(0,2));
	m22=(mt.at<double>(0,0)*mt.at<double>(2,2))-(mt.at<double>(2,0)*mt.at<double>(0,2));
	m23=(mt.at<double>(0,0)*mt.at<double>(2,1))-(mt.at<double>(2,0)*mt.at<double>(0,1));
	m31=(mt.at<double>(0,1)*mt.at<double>(1,2))-(mt.at<double>(1,1)*mt.at<double>(0,2));
	m32=(mt.at<double>(0,0)*mt.at<double>(1,2))-(mt.at<double>(1,0)*mt.at<double>(0,2));
	m33=(mt.at<double>(0,0)*mt.at<double>(1,1))-(mt.at<double>(1,0)*mt.at<double>(0,1));

	double imat[3][3]={{(m11/determ),(-m12/determ),(m13/determ)},{(-m21/determ),(m22/determ),(-m23/determ)},{(m31/determ),(-m32/determ),(m33/determ)}};
	Mat invmat(3,3,CV_64F,imat);
	return invmat.clone();	//as only the address to where the data is stored is passed to the function the object is destroyed so an empty matrix is returned with initial values -9.23842e+61
}

Mat imagesegmentation(Mat image, Mat icovar, Scalar meanmat,double distance, Mat mask, Mat element)
{	
   #pragma omp parallel for
    for(int i = 0; i < image.rows;i++)
    {
        for(int j=0; j<image.cols;j++)	//in case it fails changed it from i=1 to i=0
		{
			Vec3b pixel= image.at<Vec3b>(i,j);	//prints wierd characters
			double mdist=mahadistance(icovar,meanmat,pixel);
			if(mdist<distance)
				mask.at<uchar>(i,j)=255;
			else
				mask.at<uchar>(i,j)=0;
		}
    }
	Mat mask1=mask;
	morphologyEx(mask,mask1,2,element);
    return mask;
}

Mat Cornerdetect(Mat image, Mat corners,int numcorners)
{
	Mat grayscale;
	#pragma omp parallel sections
	{	
		#pragma omp section 
		{ cvtColor(image,grayscale,CV_BGR2GRAY); }
		//goodfeatures(grayimage, output to store corners, quality factor, distance factor)
		#pragma omp section 
		{ goodFeaturesToTrack(grayscale,corners,numcorners,0.01,70); }   
		// Mark these corners on the original image
	}
	
	if(corners.rows!=0)
	{
		cornerSubPix(grayscale, corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1)); 
	}
	return corners; 
}
