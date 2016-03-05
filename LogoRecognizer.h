#pragma once
#include "opencv2/imgproc/imgproc.hpp"
#include "Segment.h"

class LogoRecognizer
{
public:
	list<Segment> segments;

	LogoRecognizer(Mat image);
	void recognize();

private:
	Mat oldImage;
	Mat newImage;
	static const Scalar MIN_YELLOW;
	static const Scalar MAX_YELLOW;
	static const Scalar MIN_WHITE;
	static const Scalar MAX_WHITE;

	void performSegmentation();
	void performAnalysis();
	void performRecognition();
	Mat thresholding(const Mat&);
	bool isYellow(int h, int l, int s);
	bool isWhite(int h, int l, int s);
	void floodFill(int row, int col, Scalar color);
	Scalar randomColor(RNG& rng );
	void deleteSmall();
	bool isSLetter(const Segment&);
	bool isULetter(const Segment&);
	bool isBLetter(const Segment&);
	bool isWLetter(const Segment&);
	bool isALetter(const Segment&);
	bool isYLetter(const Segment&);
	bool isWord(const Segment&);
};

