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
	MyBGSubtractorColor prueba(cap);
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

	// creamos el objeto para la substracci�n de fondo

	// creamos el objeto para el reconocimiento de gestos

	// iniciamos el proceso de obtenci�n del modelo del fondo

	prueba.LearnModel();

	for (;;)
	{
		cap >> frame;
		//flip(frame, frame, 1);
		if (frame.empty())
		{
			printf("Le�do frame vac�o\n");
			continue;
		}
		int c = cvWaitKey(40);
		if ((char)c == 'q')
			break;
		// obtenemos la m�scara del fondo con el frame actual
		prueba.ObtainBGMask(frame, bgmask);

		// CODIGO 2.1
		// limpiar la m�scara del fondo de ruido
		medianBlur(bgmask, bgmask, 5);

		int dilation_size = 2;
		Mat element = getStructuringElement(MORPH_RECT, Size(2 * dilation_size + 1, 2 * dilation_size + 1), Point(dilation_size, dilation_size));
		for (int i = 0; i < 5; ++i)
		{
			dilate(bgmask, bgmask, element);
			erode(bgmask, bgmask, element);
		}

		// deteccion de las caracter�sticas de la mano
		HandGesture deteccion;
		deteccion.FeaturesDetection(bgmask,frame);

		// mostramos el resultado de la sobstracci�n de fondo

		// mostramos el resultado del reconocimento de gestos

		imshow("Reconocimiento", frame);
		imshow("Fondo", bgmask);
	}

	destroyWindow("Reconocimiento");
	destroyWindow("Fondo");
	cap.release();
	return 0;
}
