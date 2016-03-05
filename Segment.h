#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <list>

using namespace std;
using namespace cv;

class Segment
{
public:
    list<Point2i> points;
    Scalar color;             

    double area;               
    double m01;
    double m10;
    double m02;
    double m20;
    double m03;
    double m30;
    double m11;
    double m21;
    double m12;

    double center_i;
    double center_j;

    double M01;
    double M10;
    double M02;
    double M20;
    double M03;
    double M30;
    double M11;
    double M21;
    double M12;

    double NM1;
    double NM2;
    double NM3;
	double NM7;

    Segment();
    Segment(Scalar _color);
    bool operator==(const Segment& q );
	void analyseSegment();

private:
	double calc_mpq(int p, int q);
};

