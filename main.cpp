// image process toolbox.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "iostream"
#include "string"
#include "vector"
#include "opencv2\opencv.hpp"

typedef unsigned char uchar;
using namespace cv;
using namespace std;

cv::Mat otsu(cv::Mat temp, int method);
cv::Mat SPC(cv::Mat temp, float para1);
int _tmain(int argc, char** argv)
{
	Mat input,output,colorimg;
	if(argc < 3){
		cout<<"command invalid";
		return -1;
	}
	string filename=argv[1];
	filename=filename+".png";
	int operation=stoi(string(argv[2]));
	float para1=0;
	if(argc>3)
		para1=stof(string(argv[3]));
	input = imread(filename, 0); 
	colorimg = imread(filename, 1); 
	if(input.empty()){
		cout<<("Can not load image: "+filename);
		return -1;
	}
	switch( operation) {
	case 1 :cout<<("show gray image:"+filename);
			input.copyTo(output);
			cv::imshow("gray image",output);
            break;  
	case 2 :cout<<("show color image:"+filename);
			colorimg.copyTo(output);
			cv::imshow("color image",output);
            break;  
	case 3 :output=otsu(input,0);
			cv::imshow("After Otsu's threshold",output);
		    break;
	case 4 :output=otsu(input,1);
			cv::imshow("After VE Otsu's threshold",output);
		    break;
	case 5 :output=otsu(input,2);
			cv::imshow("After WOV Otsu's threshold",output);
		    break;
	case 6 :output=otsu(input,3);
			cv::imshow("After RC Otsu's threshold",output);
		    break;
	case 7 :cv::threshold(input,output,para1,255,CV_THRESH_BINARY);
			cv::imshow("After Binary threshold",output);
		    break;
	case 8 :output=SPC(input,para1);
			cv::imshow("After Statistical Process Control threshold",output);
		    break;
	default:cout<<"command invalid";
			return -1;
			break;
	}
	cv::imwrite("output.png",output);
	waitKey();
	return 0;
}



cv::Mat otsu(cv::Mat temp, int method){
    cv::Mat otsu,RC;
    temp.convertTo(temp,CV_8U);
	temp.copyTo(otsu);
	temp.copyTo(RC);
    cv::threshold(temp,RC,140,255,CV_THRESH_OTSU);
    float pi=0,ui=0,u=0;
    float n=0;
    vector <float> p0;
    vector <float> p1;
    vector <float> u0;
    vector <float> u1;
    vector <float> delta;
    float h[256];
    for(int k=0;k<256;k++)
        h[k]=0;
    for(int i=0;i<otsu.rows;i++)
        for(int j=0;j<otsu.cols;j++){
            if(method==3){
				if(RC.ptr<uchar>(i)[j]==0){
					h[otsu.ptr<uchar>(i)[j]]++;
					n++;
				}
			}
			else{
				h[otsu.ptr<uchar>(i)[j]]++;
				n++;
			}
        }
    for(int k=0;k<256;k++){
        pi+=h[k]/n;
        p0.push_back(pi);
        p1.push_back(1-pi);

    }
    for(int k=0;k<256;k++){
        if(p0[k]!=0)
            for(int l=0;l<k+1;l++)
                ui+=float(l)*h[l]/n/p0[k];
        else
            ui=0;
        u0.push_back(ui);
        if(p1[k]!=0)
            for(int l=k+1;l<256;l++)
                u+=float(l)*h[l]/n/p1[k];
        else
            u=0;
        u1.push_back(u);
        u=0;
        ui=0;
    }
	if(method==0 || method==3) //Otsu's threshold
		for(int k=0;k<256;k++)
		 delta.push_back( p0[k]*u0[k]*u0[k]+p1[k]*u1[k]*u1[k] );
	else if(method==1) //VE method
		for(int k=0;k<256;k++)
		 delta.push_back(  (1-h[k]/n)*(p0[k]*u0[k]*u0[k]+p1[k]*u1[k]*u1[k])  );
	else if(method==2) //WOV method
		for(int k=0;k<256;k++) 
		 delta.push_back(  p0[k]*p0[k]*u0[k]*u0[k]+p1[k]*u1[k]*u1[k] );
    float max=0;
    int TH=0;
    for(int i=0;i<256;i++)
        if(delta[i]>max){
            max = delta[i];
            TH=i;
        }
	cv::threshold(temp,otsu,TH,255,CV_THRESH_BINARY);
    return otsu;
}
cv::Mat SPC(cv::Mat temp, float para1){
	cv::Mat SPC;
	temp.copyTo(SPC);
    Scalar mean,std;
    cv::meanStdDev(SPC,mean,std);
    float c=para1;
    for(int i=0;i<SPC.rows;i++)
        for(int j=0;j<SPC.cols;j++)
            if(SPC.ptr<uchar>(i)[j]>mean.val[0]-c*std.val[0] && SPC.ptr<uchar>(i)[j]<mean.val[0]+c*std.val[0])
                SPC.ptr<uchar>(i)[j]=0;
            else
                SPC.ptr<uchar>(i)[j]=255;
    return SPC;
}
