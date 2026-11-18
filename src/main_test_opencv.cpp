//////////////////////////////////////////////////////////////////////////
// Option Images
// Projet, s�ance 1
// th�me : premier pas en OpenCV
// contenu : charge, affiche, r�duction, calcul et affichage d'histogramme
// version : 17.1128
//////////////////////////////////////////////////////////////////////////


#include <iostream>
using namespace std;

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;

#include "histogram.hpp"


int main (void) {

	//charge et affiche l'image (� MODIFIER) :
	string imName = "../Lenna.png";
	Mat im = imread(imName);
	Mat im2;
	if(im.data == nullptr){
		cerr << "Image not found: "<< imName << endl;
		waitKey(0);
		//system("pause");
		exit(EXIT_FAILURE);
	}
	imshow("exemple1", im);

	//applique une reduction de taille d'un facteur 5
	//ici modifier pour ne reduire qu'a l'affichage 
	//comme demande dans l'enonce
	int reduction = 5;
	Size tailleReduite(im.cols/reduction, im.rows/reduction);
	Mat imreduite = Mat(tailleReduite,CV_8UC3); //cree une image � 3 canaux de profondeur 8 bits chacuns
	resize(im,imreduite,tailleReduite);
	imshow("image reduite", imreduite);

	//computeHistogram("histogramme", im);

	cvtColor(im,im2,COLOR_BGR2HSV);
	
	vector<Mat> channels;
	split(im2,channels);


	Mat im3;
	threshold(channels[1], im3, 125, 255 ,THRESH_TOZERO);
	imshow("thresh",im3);


	//imshow("chanh", channels[0]);
	imshow("chans", channels[1]);
	//imshow("chanv", channels[2]);


	//extract roi 
	Mat roi;
	//Rect t = selectROI(im);
	
	Rect t(Point(250,200),Point(300,400));

	Mat crop = im(t);
	imshow("t",crop);

	//termine le programme lorsqu'une touche est frappee
	waitKey(0);
	//system("pause");
	return EXIT_SUCCESS;
}
