#include "HandGesture.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

HandGesture::HandGesture()
{
}

double HandGesture::getAngle(Point s, Point e, Point f)
{

	double v1[2], v2[2];
	v1[0] = s.x - f.x;
	v1[1] = s.y - f.y;

	v2[0] = e.x - f.x;
	v2[1] = e.y - f.y;

	double ang1 = atan2(v1[1], v1[0]);
	double ang2 = atan2(v2[1], v2[0]);

	double angle = ang1 - ang2;
	if (angle > CV_PI)
		angle -= 2 * CV_PI;
	if (angle < -CV_PI)
		angle += 2 * CV_PI;
	return (angle * 180.0 / CV_PI);
}
cv::Point HandGesture::FeaturesDetection(Mat mask, Mat output_img)
{

	vector<vector<Point>> contours;
	Mat temp_mask;
	mask.copyTo(temp_mask);
	int index = -1;

	// CODIGO 3.1
	// detección del contorno de la mano y selección del contorno más largo
	circle(temp_mask, Point(4, 5), 5, cv::Scalar(255));
	findContours(temp_mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	unsigned int aux = 0;

	for (unsigned int i = 0; i < contours.size(); i++)
	{
		if (aux < contours.at(i).size())
		{
			index = i;
			aux = contours.at(i).size();
		}
	}
	//std::cout<<index<<std::endl;
	// pintar el contorno
	drawContours(output_img, contours, index, cv::Scalar(255, 0, 0), 2, 8, vector<Vec4i>(), 0, Point());

	//obtener el convex hull
	vector<int>
		hull;
	convexHull(contours[index], hull);

	// pintar el convex hull
	Point pt0 = contours[index][hull[hull.size() - 1]];
	for (int i = 0; i < hull.size(); i++)
	{
		Point pt = contours[index][hull[i]];
		line(output_img, pt0, pt, Scalar(0, 0, 255), 2, CV_AA);
		pt0 = pt;
	}

	//obtener los defectos de convexidad
	vector<Vec4i> defects;
	convexityDefects(contours[index], hull, defects);

	Rect rectangulo = boundingRect(contours[index]);

	Point centro = Point(rectangulo.tl().x + (rectangulo.br().x - rectangulo.tl().x) / 2,
						 rectangulo.br().y + (rectangulo.tl().y - rectangulo.br().y) / 2);

	if ((centro.x - lastCenter.x) < -20)
		putText(output_img, "derecha", Point(10, 420), 1, FONT_HERSHEY_COMPLEX, Scalar(0, 255, 5));

	else if ((centro.x - lastCenter.x) < 20)
		putText(output_img, "izquierda", Point(10, 420), 1, FONT_HERSHEY_COMPLEX, Scalar(0, 255, 5));
	else
		putText(output_img, "o", Point(10, 420), 1, FONT_HERSHEY_COMPLEX, Scalar(0, 255, 5));

	if ((centro.y - lastCenter.y) < 20)
		putText(output_img, "abajo", Point(10, 400), 1, FONT_HERSHEY_COMPLEX, Scalar(0, 255, 5));

	else if ((centro.y - lastCenter.y) < -20)
		putText(output_img, "arriba", Point(10, 400), 1, FONT_HERSHEY_COMPLEX, Scalar(0, 255, 5));
	else
		putText(output_img, "o", Point(10, 400), 1, FONT_HERSHEY_COMPLEX, Scalar(0, 255, 5));

	lastCenter = centro;

	int cont = 0;
	vector<double> angles;
	//vector que almacena las puntas de los dedos
	vector<Point> puntas;

	for (int i = 0; i < defects.size(); i++)
	{
		Point s = contours[index][defects[i][0]];
		Point e = contours[index][defects[i][1]];
		Point f = contours[index][defects[i][2]];
		float depth = (float)defects[i][3] / 256.0;
		double angle = getAngle(s, e, f);

		puntas.push_back(s);

		// CODIGO 3.2
		// filtrar y mostrar los defectos de convexidad
		if (angle < 120 && depth > 100)
		{
			++cont;
			circle(output_img, f, 5, Scalar(0, 255, 0), 3);
			//cout << depth << endl;
			angles.push_back(angle);
		}
	}

	// std::cout << cont << endl;
	string cadena;
	Point elPunto(0, 900);

	switch (cont)
	{
	case 0:
		//Diferenciar entre puño o 1 deo boundingrect
		float prop1, prop2;
		prop1 = ((float)rectangulo.height / (float)rectangulo.width);
		prop2 = ((float)rectangulo.width / (float)rectangulo.height);

		if ((prop1 < 1.3) && (prop2 < 1.3))
			putText(output_img, "Puño", Point(0, 50), 1, FONT_HERSHEY_COMPLEX, Scalar(0, 255, 5));
		else
		{
			putText(output_img, "Dedo", Point(0, 50), 1, FONT_HERSHEY_COMPLEX, Scalar(0, 255, 5));

			//punta del dedo mas "alto"
			for (unsigned int i = 0; i < puntas.size(); ++i)
			{
				if (elPunto.y > puntas[i].y)
				{
					elPunto.x = puntas[i].x;
					elPunto.y = puntas[i].y;
				}
			}
		}
		break;

	case 1:

		if (angles[0] < 55)
			cadena = "Peace";
		else if (angles[0] < 85)
			cadena = "Rock";
		else
			cadena = "Ronaldinho";

		putText(output_img, cadena, Point(0, 50), 1, FONT_HERSHEY_COMPLEX, Scalar(0, 255, 5));
		break;

	case 2:
		putText(output_img, "3 dedos", Point(0, 50), 1, FONT_HERSHEY_COMPLEX, Scalar(0, 255, 5));
		break;

	case 3:
		putText(output_img, "4 dedos", Point(0, 50), 1, FONT_HERSHEY_COMPLEX, Scalar(0, 255, 5));
		break;
	case 4:
		putText(output_img, "Mano", Point(0, 50), 1, FONT_HERSHEY_COMPLEX, Scalar(0, 255, 5));
		break;
	default:
		putText(output_img, "Franccesco", Point(0, 50), 1, FONT_HERSHEY_COMPLEX, Scalar(0, 255, 5));
		break;
	}
	return elPunto;
}
