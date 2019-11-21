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



int main(int argc, char** argv)
{

	Mat frame, bgmask, out_frame;



	//Abrimos la webcam

	VideoCapture cap;

	cap.open(0);
	if (!cap.isOpened())
	{
		printf("\nNo se puede abrir la cámara\n");
		return -1;
	}
	MyBGSubtractorColor prueba(cap);
        int cont = 0;
        while (frame.empty()&& cont < 2000 ) {

                cap >> frame;
                ++cont;
        }
        if (cont >= 2000) {
                printf("No se ha podido leer un frame válido\n");
                exit(-1);
        }

	// Creamos las ventanas que vamos a usar en la aplicación

	namedWindow("Reconocimiento");
	namedWindow("Fondo");

        // creamos el objeto para la substracción de fondo
	
	// creamos el objeto para el reconocimiento de gestos

	// iniciamos el proceso de obtención del modelo del fondo
	
		prueba.LearnModel();

	for (;;)
	{
		cap >> frame;
		//flip(frame, frame, 1);
		if (frame.empty())
		{
			printf("Leído frame vacío\n");
			continue;
		}
		int c = cvWaitKey(40);
		if ((char)c == 'q') break;
		prueba.ObtainBGMask(frame, bgmask);
		// obtenemos la máscara del fondo con el frame actual
                
                // CODIGO 2.1
                // limpiar la máscara del fondo de ruido
                //...


		// deteccion de las características de la mano

                // mostramos el resultado de la sobstracción de fondo
		
                // mostramos el resultado del reconocimento de gestos

		imshow("Reconocimiento", frame);
		imshow("HOla", bgmask);

		
	}
	
	destroyWindow("Reconocimiento");
	destroyWindow("Fondo");
	cap.release();
	return 0;
}
