#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <opencv2/opencv.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <iterator>
#include <android/log.h>

#define mod_w 300
#define mod_h 150

using namespace cv;

class cars     //main class
{
	public:	    //variables kept public but precaution taken all over the code

	Mat image_input;          //main input image
	Mat image_main_result;    //the final result
	Mat storage;              //introduced to stop detection of same car more than once

	CascadeClassifier cascade;    //the main cascade classifier
	CascadeClassifier checkcascade;        //a test classifier,car detected by both main and test is stated as car

	int num;

	void getimage(Mat src) //getting the input image
	{
		if(! src.data ){}
		else{
			image_input = src.clone();
			storage = src.clone();              //initialising storage
			image_main_result = src.clone();    //initialising result
		}
	}


	void cascade_load(string cascade_string)            //loading the main cascade
	{
		cascade.load(cascade_string);
		if( !cascade.load(cascade_string) ){}
		else{}
	}


	void checkcascade_load(string checkcascade_string)               //loading the test/check cascade
	{
		checkcascade.load(checkcascade_string);
		if( !checkcascade.load(checkcascade_string) ){
			__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "Could not load classifier cascade");
		}
		else{}
	}

	void setnum(){
		num = 0;
	}

	void findcars()                 //main function
	{
		int i = 0;

		Mat img = storage.clone();
		Mat temp;                    //for region of interest.If a car is detected(after testing) by one classifier,then it will not be available for other one

		if(img.empty() ){}

		int cen_x;
		int cen_y;
		vector<Rect> cars;
		const static Scalar colors[] =  { CV_RGB(0,0,255),CV_RGB(0,255,0),CV_RGB(255,0,0),CV_RGB(255,255,0),CV_RGB(255,0,255),CV_RGB(0,255,255),CV_RGB(255,255,255),CV_RGB(128,0,0),CV_RGB(0,128,0),CV_RGB(0,0,128),CV_RGB(128,128,128),CV_RGB(0,0,0)};

		Mat gray;

		cvtColor( img, gray, CV_BGR2GRAY );

		Mat resize_image(cvRound (img.rows), cvRound(img.cols), CV_8UC1 );

		resize( gray, resize_image, resize_image.size(), 0, 0, INTER_LINEAR );
		equalizeHist( resize_image, resize_image );

		cascade.detectMultiScale( resize_image, cars,1.1,2,0,Size(10,10));                 //detection using main classifier

		for( vector<Rect>::const_iterator main = cars.begin(); main != cars.end(); main++, i++ ){
			Mat resize_image_reg_of_interest;
			vector<Rect> nestedcars;
			Point center;
			Scalar color = colors[i%8];

			//getting points for bouding a rectangle over the car detected by main
			int x0 = cvRound(main->x);
			int y0 = cvRound(main->y);
			int x1 = cvRound((main->x + main->width-1));
			int y1 = cvRound((main->y + main->height-1));

			if( checkcascade.empty() )
				continue;
			resize_image_reg_of_interest = resize_image(*main);
			checkcascade.detectMultiScale( resize_image_reg_of_interest, nestedcars,1.1,2,0,Size(30,30));

			for( vector<Rect>::const_iterator sub = nestedcars.begin(); sub != nestedcars.end(); sub++ )      //testing the detected car by main using checkcascade
			{
				center.x = cvRound((main->x + sub->x + sub->width*0.5));        //getting center points for bouding a circle over the car detected by checkcascade
				cen_x = center.x;
				center.y = cvRound((main->y + sub->y + sub->height*0.5));
				cen_y = center.y;
				if(cen_x>(x0+15) && cen_x<(x1-15) && cen_y>(y0+15) && cen_y<(y1-15))         //if centre of bounding circle is inside the rectangle boundary over a threshold the the car is certified
				{
					rectangle( image_main_result, cvPoint(x0,y0),
								cvPoint(x1,y1),
								color, 3, 8, 0);               //detecting boundary rectangle over the final result

					//masking the detected car to detect second car if present
					Rect region_of_interest = Rect(x0, y0, x1-x0, y1-y0);
					temp = storage(region_of_interest);
					temp = Scalar(255,255,255);

					num = num+1;     //num if number of cars detected
				}
			}
		}
	if(image_main_result.empty()){
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "res image is empty!");
	}

	return;
	}
};


extern "C" {
	JNIEXPORT int JNICALL Java_com_testopencv_haveimgfun_LibImgFun_ImgFun(
			JNIEnv* env, jobject obj, jintArray buf, int w, int h);

	JNIEXPORT int JNICALL Java_com_testopencv_haveimgfun_LibImgFun_ImgFun(
			JNIEnv* env, jobject obj, jintArray buf, int w, int h){

		jint *cbuf;
		cbuf = env->GetIntArrayElements(buf, false);		// read in picture
		if (cbuf == NULL) {
			return 0;
		}
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "native started");

		Mat myimg(h, w, CV_8UC4, (unsigned char*) cbuf);
		Mat image1 = myimg;
		Mat image;
		resize(image1,image,Size(mod_w,mod_h),0,0,INTER_LINEAR);        //resizing image to get best experimental results
		cars detectcars;             			//          //creating car object

		detectcars.getimage(image);           //get the image
		detectcars.setnum();                  //set number of cars detected as 0

		// init the path of needed files in the phone storage
		const char * checkcas = "/storage/emulated/0/carDetection/checkcas.xml";
		const char * cas1 = "/storage/emulated/0/carDetection/cas1.xml";
		const char * cas2 = "/storage/emulated/0/carDetection/cas2.xml";
		const char * cas3 = "/storage/emulated/0/carDetection/cas3.xml";
		const char * cas4 = "/storage/emulated/0/carDetection/cas4.xml";

		detectcars.checkcascade_load(checkcas);      //load the test cascade

		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "processing");

		// load cascade files and do matching
		for(int i = 1;i< 5;i++){
			string cas;
			switch (i){
			case 1: cas = cas1;
				break;
			case 2: cas = cas2;
				break;
			case 3: cas = cas3;
				break;
			case 4: cas = cas4;
				break;
			default:
				break;
			}
			detectcars.cascade_load(cas);		// do car recognition
			detectcars.findcars();
		}

		// print number of cars
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "there are %d cars in this photo",detectcars.num);

		// generate output image
		IplImage resImg = IplImage(detectcars.image_main_result);
		cvSaveImage("/storage/emulated/0/carDetection/resImg.jpg",&resImg);
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "res img stored");

		return detectcars.num;
	}
}


/*
    extern "C" {
    JNIEXPORT jintArray JNICALL Java_com_testopencv_haveimgfun_LibImgFun_ImgFun(
            JNIEnv* env, jobject obj, jintArray buf, int w, int h);

    JNIEXPORT jintArray JNICALL Java_com_testopencv_haveimgfun_LibImgFun_ImgFun(
            JNIEnv* env, jobject obj, jintArray buf, int w, int h) {

        jint *cbuf;
        cbuf = env->GetIntArrayElements(buf, false);
        if (cbuf == NULL) {
            return 0;
        }

        Mat myimg(h, w, CV_8UC4, (unsigned char*) cbuf);
        IplImage image=IplImage(myimg);
        IplImage* image3channel = change4channelTo3InIplImage(&image);

        IplImage* pCannyImage=cvCreateImage(cvGetSize(image3channel),IPL_DEPTH_8U,1);

        cvCanny(image3channel,pCannyImage,50,150,3);

        int* outImage=new int[w*h];
        for(int i=0;i<w*h;i++)
        {
            outImage[i]=(int)pCannyImage->imageData[i];
        }

        int size = w * h;
        jintArray result = env->NewIntArray(size);
        env->SetIntArrayRegion(result, 0, size, outImage);
        env->ReleaseIntArrayElements(buf, cbuf, 0);
        return result;
    }
    }

    IplImage * change4channelTo3InIplImage(IplImage * src) {
        if (src->nChannels != 4) {
            return NULL;
        }

        IplImage * destImg = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
        for (int row = 0; row < src->height; row++) {
            for (int col = 0; col < src->width; col++) {
                CvScalar s = cvGet2D(src, row, col);
                cvSet2D(destImg, row, col, s);
            }
        }

        return destImg;
    }
*/
