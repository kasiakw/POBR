#include "LogoRecognizer.h"

const Scalar LogoRecognizer::MIN_YELLOW = Scalar(20, 100, 100);
const Scalar LogoRecognizer::MAX_YELLOW = Scalar(40, 255, 255);
const Scalar LogoRecognizer::MIN_WHITE = Scalar(0, 170, 0);
const Scalar LogoRecognizer::MAX_WHITE = Scalar(180, 255, 255);

LogoRecognizer::LogoRecognizer(Mat image){
	oldImage = image.clone();
	newImage = Mat(image.size(), CV_8UC3);
}

void LogoRecognizer::recognize(){
	Mat hlsImage;
	cvtColor(oldImage, hlsImage, CV_BGR2HLS);
	newImage = thresholding(hlsImage);

	performSegmentation();
	performAnalysis();
	performRecognition();

	namedWindow("framed");
	imshow("framed", oldImage);
	namedWindow("colored");
	imshow("colored", newImage);
}

Mat LogoRecognizer::thresholding(const Mat& I){
	Mat_<Vec3b> _I = I;
	Mat grayscaleMat (I.size(), CV_8UC3);

	for (int i = 0; i < I.rows; ++i){
		for (int j = 0; j < I.cols; ++j){
			if(isYellow(_I(i, j)[0], _I(i, j)[1], _I(i, j)[2]) || isWhite(_I(i, j)[0], _I(i, j)[1], _I(i, j)[2])){
				grayscaleMat.at<Vec3b>(i, j)[0] = 255;
				grayscaleMat.at<Vec3b>(i, j)[1] = 255;
				grayscaleMat.at<Vec3b>(i, j)[2] = 255;
			}
			else{
				grayscaleMat.at<Vec3b>(i, j)[0] = 0;
				grayscaleMat.at<Vec3b>(i, j)[1] = 0;
				grayscaleMat.at<Vec3b>(i, j)[2] = 0;
			}
        }
	}
	return grayscaleMat;
}

bool LogoRecognizer::isYellow(int h, int l, int s){
	if( (h<=MAX_YELLOW[0] && h>=MIN_YELLOW[0]) && (l<=MAX_YELLOW[1] && l>=MIN_YELLOW[1]) && (s<=MAX_YELLOW[2] && s>=MIN_YELLOW[2]) )
		return true;
	else
		return false;
}

bool LogoRecognizer::isWhite(int h, int l, int s){
	if( (h<=MAX_WHITE[0] && h>=MIN_WHITE[0]) && (l<=MAX_WHITE[1] && l>=MIN_WHITE[1]) && (s<=MAX_WHITE[2] && s>=MIN_WHITE[2]) )
		return true;
	else
		return false;
}

void LogoRecognizer::performSegmentation(){
	int n = 1;
	Scalar color;
	for (int i = 0; i < newImage.rows; ++i){
		for (int j = 0; j < newImage.cols; ++j){
			if(newImage.at<Vec3b>(i, j)[0] == 255 && newImage.at<Vec3b>(i, j)[1] == 255 && newImage.at<Vec3b>(i, j)[2] == 255 ){
				RNG rng(n);
				color = randomColor(rng);
				segments.push_back(Segment(color));
				floodFill(i, j, color);
				++n;
			}
        }
	}
	deleteSmall();
}

Scalar LogoRecognizer::randomColor(RNG& rng ){
    int icolor = (unsigned) rng;
    return Scalar( icolor&255, (icolor>>8)&255, (icolor>>16)&255 );
}

void LogoRecognizer::floodFill(int row, int col, Scalar color){
	list<Point2i> pointQueue;
	pointQueue.push_back(Point2i(col, row));
	Point2i temp, left, right, top, bottom;

	while(!pointQueue.empty()){
		//pobranie pierwszego elementu z kolejki
		temp = pointQueue.front();
		pointQueue.pop_front();
		//zamalowanie piksela w obrazie na wybrany kolor
		newImage.at<Vec3b>(temp)[0] = (uchar)color[0];
		newImage.at<Vec3b>(temp)[1] = (uchar)color[1];
		newImage.at<Vec3b>(temp)[2] = (uchar)color[2];
		//dodanie punktu do segmentu
		segments.back().points.push_back(temp);

		left = Point2i(temp.x-1, temp.y);
		right = Point2i(temp.x+1, temp.y);
		top = Point2i(temp.x, temp.y-1);
		bottom = Point2i(temp.x, temp.y+1);

		/* jesli wsp. sasiadow mieszcza sie w zakresie, w binarnym obrazie jest bialy piksel 
		i nie ma juz w kolejce punktu o takich wspolrzednych,
		to wstaw punkt do kolejki*/
		if(left.x >= 0 && newImage.at<Vec3b>(left)[0] == 255 && newImage.at<Vec3b>(left)[1] == 255 && newImage.at<Vec3b>(left)[2] == 255 && 
			find(pointQueue.begin(), pointQueue.end(), left) == pointQueue.end()){
			pointQueue.push_back(left);
		}
		if(right.x < newImage.cols && newImage.at<Vec3b>(right)[0] == 255 && newImage.at<Vec3b>(right)[1] == 255 && newImage.at<Vec3b>(right)[2] == 255 && 
			find(pointQueue.begin(), pointQueue.end(), right) == pointQueue.end()){
			pointQueue.push_back(right);
		}
		if(top.y >= 0 && newImage.at<Vec3b>(top)[0] == 255 && newImage.at<Vec3b>(top)[1] == 255 && newImage.at<Vec3b>(top)[2] == 255 && 
			find(pointQueue.begin(), pointQueue.end(), top) == pointQueue.end()){
			pointQueue.push_back(top);
		}
		if(bottom.y < newImage.rows && newImage.at<Vec3b>(bottom)[0] == 255 && newImage.at<Vec3b>(bottom)[1] == 255 && newImage.at<Vec3b>(bottom)[2] == 255 &&
			find(pointQueue.begin(), pointQueue.end(), bottom) == pointQueue.end()){
			pointQueue.push_back(bottom);
		}
	}

}

void LogoRecognizer::deleteSmall(){
	Scalar black = Scalar(0, 0, 0);
	for (list<Segment>::iterator it=segments.begin(); it!=segments.end(); ++it){
		it->area = it->points.size();
		if(it->area < 300){
			it->color = black;
			for (list<Point2i>::iterator i=it->points.begin(); i!=it->points.end(); ++i){
				newImage.at<Vec3b>(*i)[0] = 0;
				newImage.at<Vec3b>(*i)[1] = 0;
				newImage.at<Vec3b>(*i)[2] = 0;
			}
		}
	}
	segments.remove(Segment(black));
}

void LogoRecognizer::performAnalysis(){
	cout.setf( ios::fixed );
	cout<<endl;
	for (list<Segment>::iterator it=segments.begin(); it!=segments.end(); ++it)
		it->analyseSegment();
}

void LogoRecognizer::performRecognition(){
	list<Segment> S, U, B, W, A, Y;
	list<Segment> word;

	for (list<Segment>::iterator it=segments.begin(); it!=segments.end(); ++it){
		if(isSLetter(*it))
			S.push_back(*it);
		else if(isULetter(*it))
			U.push_back(*it);
		else if(isBLetter(*it))
			B.push_back(*it);
		else if(isWLetter(*it))
			W.push_back(*it);
		else if(isALetter(*it))
			A.push_back(*it);
		else if(isYLetter(*it))
			Y.push_back(*it);
		else if(isWord(*it))
			word.push_back(*it);
		else{
			for (list<Point2i>::iterator i=it->points.begin(); i!=it->points.end(); ++i){
				newImage.at<Vec3b>(*i)[0] = 0;
				newImage.at<Vec3b>(*i)[1] = 0;
				newImage.at<Vec3b>(*i)[2] = 0;
			}
		}
	}

	vector<Point2i> minS, minY, minWord;
	vector<Point2i> maxS, maxY, maxWord;
	int xMin, yMin, xMax, yMax;
	if(!S.empty() && !Y.empty())
	{
		for (list<Segment>::iterator it=S.begin(); it!=S.end(); ++it){
			yMax = xMax = 0;
			xMin = oldImage.cols;
			yMin = oldImage.rows;
			for (list<Point2i>::iterator i=it->points.begin(); i!=it->points.end(); ++i){
				if(xMin > i->x)
					xMin = i->x;
				if(yMin > i->y)
					yMin = i->y;
				if(xMax < i->x)
					xMax = i->x;
				if(yMax < i->y)
					yMax = i->y;
			}
			minS.push_back(Point2i(xMin, yMin));
			maxS.push_back(Point2i(xMax, yMax));
		}
		for (list<Segment>::iterator it=Y.begin(); it!=Y.end(); ++it){
			yMax = xMax = 0;
			xMin = oldImage.cols;
			yMin = oldImage.rows;
			for (list<Point2i>::iterator i=it->points.begin(); i!=it->points.end(); ++i){
				if(xMin > i->x)
					xMin = i->x;
				if(yMin > i->y)
					yMin = i->y;
				if(xMax < i->x)
					xMax = i->x;
				if(yMax < i->y)
					yMax = i->y;
			}
			minY.push_back(Point2i(xMin, yMin));
			maxY.push_back(Point2i(xMax, yMax));
		}
		for(int i=0; i<S.size(); ++i){
			yMin = (minS[i].y > minY[i].y) ? minY[i].y : minS[i].y;
			yMax = (maxS[i].y > maxY[i].y) ? maxS[i].y : maxY[i].y;
			xMin = minS[i].x;
			xMax = maxY[i].x;
			rectangle(oldImage, Point2i(xMin-2, yMin-2), Point2i(xMax+2, yMax+2), Scalar(0, 0, 255), +2, 4);
		}
	}
	if(!word.empty()){
		for (list<Segment>::iterator it=word.begin(); it!=word.end(); ++it){
			yMax = xMax = 0;
			xMin = oldImage.cols;
			yMin = oldImage.rows;
			for (list<Point2i>::iterator i=it->points.begin(); i!=it->points.end(); ++i){
				if(xMin > i->x)
					xMin = i->x;
				if(yMin > i->y)
					yMin = i->y;
				if(xMax < i->x)
					xMax = i->x;
				if(yMax < i->y)
					yMax = i->y;
			}
			minWord.push_back(Point2i(xMin, yMin));
			maxWord.push_back(Point2i(xMax, yMax));
		}
		for(int i=0; i<word.size(); ++i)
			rectangle(oldImage, Point2i(minWord[i].x-2, minWord[i].y-2), Point2i(maxWord[i].x+2, maxWord[i].y+2), Scalar(0, 0, 255), +2, 4);
	}
}

bool LogoRecognizer::isSLetter(const Segment& s){
	bool ret = false;
	if(s.NM1>=0.28 && s.NM1<0.3 && s.NM2>=0.02 && s.NM2<0.04 && 
		s.NM3>=0.003 && s.NM3<0.005 && s.NM7>=0.0124 && s.NM7<0.0136)
		ret = true;
	return ret;
}

bool LogoRecognizer::isULetter(const Segment& s){
	bool ret = false;
	if(s.NM1>=0.23 && s.NM1<0.24 && s.NM2>=0.01 && s.NM2<0.013 && 
		s.NM3>=0.00009 && s.NM3<0.00045 && s.NM7>=0.0104 && s.NM7<0.012)
		ret = true;
	return ret;
}

bool LogoRecognizer::isBLetter(const Segment& s){
	bool ret = false;
	if(s.NM1>=0.19 && s.NM1<0.23 && s.NM2>=0.004 && s.NM2<0.011 && 
		s.NM3>=0.0001 && s.NM3<0.0003 && s.NM7>=0.0084 && s.NM7<0.0103)
		ret = true;
	return ret;
}

bool LogoRecognizer::isWLetter(const Segment& s){
	bool ret = false;
	if(s.NM1>=0.21 && s.NM1<0.25 && s.NM2>=0.002 && s.NM2<0.015 && 
		s.NM3>=0.0007 && s.NM3<0.0015 && s.NM7>=0.0104 && s.NM7<0.0128)
		ret = true;
	return ret;
}

bool LogoRecognizer::isALetter(const Segment& s){
	bool ret = false;
	if(s.NM1>=0.22 && s.NM1<0.24 && s.NM2>=0.008 && s.NM2<0.016 && 
		s.NM3>=0.0018 && s.NM3<0.0028 && s.NM7>=0.0095 && s.NM7<0.0109)
		ret = true;
	return ret;
}

bool LogoRecognizer::isYLetter(const Segment& s){
	bool ret = false;
	if(s.NM1>=0.32 && s.NM1<0.38 && s.NM2>=0.03 && s.NM2<0.07 && 
		s.NM3>=0.022 && s.NM3<0.033 && s.NM7>=0.0183 && s.NM7<0.0218)
		ret = true;
	return ret;
}

bool LogoRecognizer::isWord(const Segment& s){
	bool ret = false;
	if(s.NM1>=0.44 && s.NM1<0.48 && s.NM2>=0.16 && s.NM2<0.19 && 
		s.NM3>=0.00009 && s.NM3<0.0002 && s.NM7>=0.0091 && s.NM7<0.0096)
		ret = true;
	return ret;
}

