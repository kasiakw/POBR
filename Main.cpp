#include <iostream>
#include <algorithm>
#include <cmath>
#include "LogoRecognizer.h"

int main(int, char *[]) {

	//cv::Mat image = cv::imread("obrazy/subway2.jpg");
	//cv::Mat image = cv::imread("obrazy/subway3.jpg");
	//cv::Mat image = cv::imread("obrazy/subway4.jpg");
	cv::Mat image = cv::imread("obrazy/subway5.jpg");
	//cv::Mat image = cv::imread("obrazy/subway.jpg");

	if (image.empty())
	{
		std::cout << "This image is empty!" << std::endl;
		return 1;
	}

	LogoRecognizer lg = LogoRecognizer(image);
	lg.recognize();
	waitKey(-1);
	return 0;
}