// The contents of this file are in the public domain. See LICENSE_FOR_EXAMPLE_PROGRAMS.txt
/*

    This example program shows how to find frontal human faces in an image and
    estimate their pose.  The pose takes the form of 68 landmarks.  These are
    points on the face such as the corners of the mouth, along the eyebrows, on
    the eyes, and so forth.  
    

    This example is essentially just a version of the face_landmark_detection_ex.cpp
    example modified to use OpenCV's VideoCapture object to read from a camera instead 
    of files.


    Finally, note that the face detector is fastest when compiled with at least
    SSE2 instructions enabled.  So if you are using a PC with an Intel or AMD
    chip then you should enable at least SSE2 instructions.  If you are using
    cmake to compile this program you can enable them by using one of the
    following commands when you create the build project:
        cmake path_to_dlib_root/examples -DUSE_SSE2_INSTRUCTIONS=ON
        cmake path_to_dlib_root/examples -DUSE_SSE4_INSTRUCTIONS=ON
        cmake path_to_dlib_root/examples -DUSE_AVX_INSTRUCTIONS=ON
    This will set the appropriate compiler options for GCC, clang, Visual
    Studio, or the Intel compiler.  If you are using another compiler then you
    need to consult your compiler's manual to determine how to enable these
    instructions.  Note that AVX is the fastest but requires a CPU from at least
    2011.  SSE4 is the next fastest and is supported by most current machines.  
*/
/*
#include <dlib/opencv.h>
//@zmdsjtu@163.com
//2016-12-4
//http://blog.csdn.net/zmdsjtu/article/details/53454071
#include <dlib/opencv.h>
#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>

using namespace dlib;
using namespace std;

int main()
{
	try
	{
		cv::VideoCapture cap(0);
		if (!cap.isOpened())
		{
			cerr << "Unable to connect to camera" << endl;
			return 1;
		}

		//image_window win;

		// Load face detection and pose estimation models.
		frontal_face_detector detector = get_frontal_face_detector();
		shape_predictor pose_model;
		deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

		// Grab and process frames until the main window is closed by the user.
		while (cv::waitKey(30) != 27)
		{
			// Grab a frame
			cv::Mat temp;
			cap >> temp;

			cv_image<bgr_pixel> cimg(temp);
			// Detect faces 
			std::vector<rectangle> faces = detector(cimg);
			// Find the pose of each face.
			std::vector<full_object_detection> shapes;
			for (unsigned long i = 0; i < faces.size(); ++i)
				shapes.push_back(pose_model(cimg, faces[i]));

			if (!shapes.empty()) {
				for (int i = 0; i < 68; i++) {
					circle(temp, cvPoint(shapes[0].part(i).x(), shapes[0].part(i).y()), 3, cv::Scalar(0, 0, 255), -1);
					//	shapes[0].part(i).x();//68��
					putText(temp, to_string(i), cvPoint(shapes[0].part(i).x(), shapes[0].part(i).y()), CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 0), 1, 4);

				}
			}
			//Display it all on the screen
			imshow("Dlib������", temp);
		

		}
	}
	catch (serialization_error& e)
	{
		cout << "You need dlib's default face landmarking model file to run this example." << endl;
		cout << "You can get it from the following URL: " << endl;
		cout << "   http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
		cout << endl << e.what() << endl;
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
}
*/

//--------------------------------------������˵����-------------------------------------------
//		����˵������OpenCV3������š�OpenCV3���鱾����ʾ������09
//		��������������OpenCV��װĿ¼��Samples�ļ����еĹٷ�ʾ������-���ù����������˶�Ŀ����
//		�������ò���ϵͳ�� Windows 7 64bit
//		��������IDE�汾��Visual Studio 2010
//		��������OpenCV�汾��	3.0 beta
//		2014��11�� Revised by @ǳī_ë����
//------------------------------------------------------------------------------------------------


/************************************************************************
* Copyright(c) 2011  Yang Xian
* All rights reserved.*
* File:	opticalFlow.cpp
* Brief: lk���������˶�Ŀ����
* Version: 1.0
* Author: Yang Xian
* Email: xyang2011@sinano.ac.cn
* Date:	2011/11/18
* History:
************************************************************************/


//---------------------------------��ͷ�ļ��������ռ�������֡�----------------------------
//		����������������ʹ�õ�ͷ�ļ��������ռ�
//-------------------------------------------------------------------------------------------------
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <cstdio>

using namespace std;
using namespace cv;





//-----------------------------------��ȫ�ֺ���������-----------------------------------------
//		����������ȫ�ֺ���
//-------------------------------------------------------------------------------------------------
void tracking(Mat &frame, Mat &output);
bool addNewPoints();
bool acceptTrackedPoint(int i);

//-----------------------------------��ȫ�ֱ���������-----------------------------------------
//		����������ȫ�ֱ���
//-------------------------------------------------------------------------------------------------
string window_name = "optical flow tracking";
Mat gray;	// ��ǰͼƬ
Mat gray_prev;	// Ԥ��ͼƬ
vector<Point2f> points[2];	// point0Ϊ�������ԭ��λ�ã�point1Ϊ���������λ��
vector<Point2f> initial;	// ��ʼ�����ٵ��λ��
vector<Point2f> features;	// ��������
int maxCount = 500;	// �������������
double qLevel = 0.01;	// �������ĵȼ�
double minDist = 10.0;	// ��������֮�����С����
vector<uchar> status;	// ����������״̬��������������Ϊ1������Ϊ0
vector<float> err;


//--------------------------------��help( )������----------------------------------------------
//		���������������Ϣ
//-------------------------------------------------------------------------------------------------
static void help()
{
	//�����ӭ��Ϣ��OpenCV�汾
	cout << "\n\n\t\t\t�ǳ���л����OpenCV3������š�һ�飡\n"
		<< "\n\n\t\t\t��Ϊ����OpenCV3��ĵ�9������ʾ������\n"
		<< "\n\n\t\t\t   ��ǰʹ�õ�OpenCV�汾Ϊ��" << CV_VERSION
		<< "\n\n  ----------------------------------------------------------------------------";
}


//-----------------------------------��main( )������--------------------------------------------
//		����������̨Ӧ�ó������ں��������ǵĳ�������￪ʼ
//-------------------------------------------------------------------------------------------------
int main()
{

	Mat frame;
	Mat result;

	VideoCapture capture("1.avi");

	help();
	if (capture.isOpened())	// ����ͷ��ȡ�ļ�����
	{
		while (true)
		{
			capture >> frame;

			if (!frame.empty())
			{
				tracking(frame, result);
			}
			else
			{
				printf(" --(!) No captured frame -- Break!");
				break;
			}

			int c = waitKey(50);
			if ((char)c == 27)
			{
				break;
			}
		}
	}
	system("pause");
	return 0;
}

//-------------------------------------------------------------------------------------------------
// function: tracking
// brief: ����
// parameter: frame	�������Ƶ֡
//			  output �и��ٽ������Ƶ֡
// return: void
//-------------------------------------------------------------------------------------------------
void tracking(Mat &frame, Mat &output)
{

	//�˾�����OpenCV3��Ϊ��
	cvtColor(frame, gray, COLOR_BGR2GRAY);
	//�˾�����OpenCV2��Ϊ��
	//cvtColor(frame, gray, CV_BGR2GRAY);

	frame.copyTo(output);

	// ���������
	if (addNewPoints())
	{
		goodFeaturesToTrack(gray, features, maxCount, qLevel, minDist);
		points[0].insert(points[0].end(), features.begin(), features.end());
		initial.insert(initial.end(), features.begin(), features.end());
	}

	if (gray_prev.empty())
	{
		gray.copyTo(gray_prev);
	}
	// l-k�������˶�����
	calcOpticalFlowPyrLK(gray_prev, gray, points[0], points[1], status, err);
	// ȥ��һЩ���õ�������
	int k = 0;
	for (size_t i = 0; i<points[1].size(); i++)
	{
		if (acceptTrackedPoint(i))
		{
			initial[k] = initial[i];
			points[1][k++] = points[1][i];
		}
	}
	points[1].resize(k);
	initial.resize(k);
	// ��ʾ��������˶��켣
	for (size_t i = 0; i<points[1].size(); i++)
	{
		line(output, initial[i], points[1][i], Scalar(0, 0, 255));
		circle(output, points[1][i], 3, Scalar(0, 255, 0), -1);
	}

	// �ѵ�ǰ���ٽ����Ϊ��һ�˲ο�
	swap(points[1], points[0]);
	swap(gray_prev, gray);

	imshow(window_name, output);
}


//-------------------------------------------------------------------------------------------------
// function: addNewPoints
// brief: ����µ��Ƿ�Ӧ�ñ����
// parameter:
// return: �Ƿ���ӱ�־
//-------------------------------------------------------------------------------------------------
bool addNewPoints()
{
	return points[0].size() <= 10;
}

//-------------------------------------------------------------------------------------------------
// function: acceptTrackedPoint
// brief: ������Щ���ٵ㱻����
// parameter:
// return:
//-------------------------------------------------------------------------------------------------
bool acceptTrackedPoint(int i)
{
	return status[i] && ((abs(points[0][i].x - points[1][i].x) + abs(points[0][i].y - points[1][i].y)) > 2);
}


