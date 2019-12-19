#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>

#include <stdio.h>
#include <string>
#include <iostream>

#include "MyBGSubtractorColor.hpp"
#include "HandGesture.hpp"

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{

	Mat frame, bgmask, out_frame;

	//Abrimos la webcam
	VideoCapture cap;

	cap.open("/dev/video2");
	// cap.open(0);
	if (!cap.isOpened())
	{
		printf("\nNo se puede abrir la c�mara\n");
		return -1;
	}

	int cont = 0;
	while (frame.empty() && cont < 2000)
	{

		cap >> frame;
		++cont;
	}
	if (cont >= 2000)
	{
		printf("No se ha podido leer un frame v�lido\n");
		exit(-1);
	}

	// Creamos las ventanas que vamos a usar en la aplicaci�n
	namedWindow("Reconocimiento");
	namedWindow("Fondo");
	namedWindow("Lienzo");

	// creamos el objeto para la substracci�n de fondo
	MyBGSubtractorColor mascara(cap);

	// creamos el objeto para el reconocimiento de gestos
	HandGesture deteccion;
	Mat lienzo(frame.rows,frame.cols, CV_8UC3, Scalar(255, 255, 255));

	// iniciamos el proceso de obtenci�n del modelo del fondo
	mascara.LearnModel();

	for (;;)
	{
		cap >> frame;
		
		flip(frame, frame, 1);

		if (frame.empty())
		{
			printf("Leido frame vacio\n");
			continue;
		}

		int c = cvWaitKey(40);
		if ((char)c == 'q')
			break;
		// obtenemos la mascara del fondo con el frame actual
		mascara.ObtainBGMask(frame, bgmask);

		// CODIGO 2.1
		// limpiar la mascara del fondo de ruido
		medianBlur(bgmask, bgmask, 5);

		int dilation_size = 3;

		Mat element = getStructuringElement(MORPH_RECT, Size(2 * dilation_size + 1, 2 * dilation_size + 1), Point(dilation_size, dilation_size));
		for (int i = 0; i < 6; ++i)
		{
			dilate(bgmask, bgmask, element);
			erode(bgmask, bgmask, element);
		}

		// deteccion de las caracteristicas de la mano
		Point lapiz;
		lapiz = deteccion.FeaturesDetection(bgmask, frame);
		if (lapiz != Point(0, 0))
		{
			circle(lienzo, lapiz, 2, Scalar(0, 0, 0), 2);
		}

		// mostramos el resultado de la sobstracciin de fondo
		imshow("Fondo", bgmask);

		// mostramos el resultado del reconocimento de gestos
		imshow("Reconocimiento", frame);
		imshow("Lienzo", lienzo);
	}

	destroyWindow("Reconocimiento");
	destroyWindow("Fondo");
	destroyWindow("Lienzo");
	cap.release();
	return 0;
}
