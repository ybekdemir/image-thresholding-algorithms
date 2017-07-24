/*
Yilmaz BEKDEMIR
040090507
Multimedia Computing HW-2
*/

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\opencv.hpp>
#include <math.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <time.h>

using namespace cv;
using namespace std;


void Otsu(Mat img, int selection);
void Kmeans(Mat img, int selection);

int  main(int argc, char *argv[]) {


	//Controlling argument count
	if (argc != 3)
	{
		cout << " Missing arguements" << endl;
		return -1;
	}

	//Reading foreground and background selection
	//Selection must be 1 or 0.
	int  selection = atoi(argv[2]);
	cout << selection << endl;
	if (!(selection == 0 || selection == 1))
	{
		cout << "Selection must be 0 or 1." << endl;
		return -1;
	}

	//Reading image path.
	char* path = argv[1];
	Mat img;
	img = imread(path, CV_LOAD_IMAGE_COLOR);

	if (img.data == NULL){
		cout << "Error: Image can not be read!!" << endl;
		return -1;
	}
	//Calling otsu method.
	Otsu(img, selection);
	Mat img2;
	img2 = imread(path, CV_LOAD_IMAGE_COLOR);
	//Calling K-Means method.
	Kmeans(img2, selection);

	waitKey(0);

	return 0;

}

void Otsu(Mat img, int selection)
{

	int i, j, k;
	int size;

	//Histogram and thresholds for R,G and B value calculated seperatly.
	//So , program doesn't convert image tob gray scale.
	//But I saw that otsu with grayscale image more accurate.

	//histogram arrays initialized.
	int * histogramR = new int[256];
	int * histogramG = new int[256];
	int * histogramB = new int[256];

	// Initial values zero
	for (i = 0; i < 256; i++)
	{
		histogramR[i] = 0;
		histogramG[i] = 0;
		histogramB[i] = 0;
	}



	int rowCount = img.rows;
	int columnCount = img.cols;
	size = columnCount * rowCount;


	//Histograms for R, G, B calculated.
	for (i = 0; i < img.rows; i++)
	{

		for (j = 0; j < img.cols; j++)
		{
			Vec3b color = img.at<Vec3b>(Point(j, i));
			histogramR[color[0]]++;
			histogramG[color[1]]++;
			histogramB[color[2]]++;
		}

	}

	//Variables initialized
	double w0R = 0, w0G = 0, w0B = 0;
	float w1R = 0, w1G = 0, w1B = 0;
	float m1R = 0, m1G = 0, m1B = 0;
	float m2R = 0, m2G = 0, m2B = 0;
	float varianceR = 0, varianceG = 0, varianceB = 0;
	float varianceRmax = 0, varianceGmax = 0, varianceBmax = 0;
	float thresholdR = 0, thresholdG = 0, thresholdB = 0;

	//otsu algorithm
	for (j = 0; j < 256; j++)
	{

		w0R = 0;
		w0G = 0;
		w0B = 0;

		//Means for both class calculated.(R,G,B seperatly).
		//WO first class
		for (k = 0; k < j; k++)
		{
			w0R += (histogramR[k]);
			w0G += (histogramG[k]);
			w0B += (histogramB[k]);
		}

		//W1 second class
		w1R = size - w0R;//size of image (w1 = 1- w0)
		w1G = size - w0G;
		w1B = size - w0B;

		int sumR = 0, sumG = 0, sumB = 0;

		//Sum of class for calculating variance for first class
		for (k = 0; k < j; k++)
		{
			sumR += k* (histogramR[k]);
			sumG += k* (histogramG[k]);
			sumB += k* (histogramB[k]);
		}

		//Variances for first class
		m1R = sumR / w0R;
		m1G = sumG / w0G;
		m1B = sumB / w0B;

		sumR = 0;
		sumG = 0;
		sumB = 0;

		//Sum of class for calculating variance for second class
		for (k = j; k < 256; k++)
		{
			sumR += k* histogramR[k];
			sumG += k* histogramG[k];
			sumB += k* histogramB[k];
		}
		//Variances for second class
		m2R = sumR / w1R;
		m2G = sumG / w1G;
		m2B = sumB / w1B;


		//Between class variances for R,G,B
		varianceR = sqrt(w0R*w1R*(m1R - m2R)*(m1R - m2R));
		varianceG = sqrt(w0G*w1G*(m1G - m2G)*(m1G - m2G));
		varianceB = sqrt(w0B*w1B*(m1B - m2B)*(m1B - m2B));

		//Maximum between class variances values assigned to Threshold(R,G,B)
		if (varianceR > varianceRmax){
			thresholdR = j;
			varianceRmax = varianceR;
		}

		if (varianceG > varianceGmax){
			thresholdG = j;
			varianceGmax = varianceG;
		}
		if (varianceB > varianceBmax){
			thresholdB = j;
			varianceBmax = varianceB;
		}
	}

	//We find thresholds for R,g,b seperatly. Now program will apply thresholds to image.

	for (i = 0; i < img.rows; i++)
	{

		for (j = 0; j < img.cols; j++)
		{
			Vec3b color = img.at<Vec3b>(Point(j, i));//Reading pixel values

			//R,g and B values compare with threshold
			//Results of each  channel combine with AND operator
			if (color[0]>thresholdR && color[1]>thresholdG && color[2]>thresholdB)
			{
				if (selection == 1){
					color[0] = 255;
					color[1] = 255;
					color[2] = 255;
				}
				else
				{
					color[0] = 0;
					color[1] = 0;
					color[2] = 0;
				}


			}
			else{
				if (selection == 1){
					color[0] = 0;
					color[1] = 0;
					color[2] = 0;
				}
				else
				{
					color[0] = 255;
					color[1] = 255;
					color[2] = 255;
				}

			}


			img.at<Vec3b>(Point(j, i)) = color;
		}

	}

	//Writing thresholds value to screen
	cout << "Thresholds for Otsu  Algorithm :  T_R =" << thresholdR << " T_G = " << thresholdG << " T_B = " << thresholdB << endl;

	//Show output image on new window
	namedWindow("Otsu", WINDOW_AUTOSIZE);// Create a window for display.
	imshow("Otsu", img);
}
void Kmeans(Mat img, int selection){


	//Variables initialized.
	int i, j;
	int rowCount = img.rows;
	int columnCount = img.cols;
	int size = columnCount * rowCount;

	int R_f = rand() % 256;//R_F initialized
	int R_b = rand() % 256;//R_B initialized
	int sumRF = 0;
	int sumRB = 0;
	int RFCount = 1;
	int RBCount = 1;
	int T = 0;
	int newT = 1;
	for (i = 0; i < (img.rows); i++)
	{

		for (j = 0; j < (img.cols); j++)
		{
			//Image converting to grayscale.
			Vec3b color = img.at<Vec3b>(Point(j, i));

			float grayColor = sqrt((color[0] * color[0] + color[1] * color[1] + color[2] * color[2]) / 3);
			color[0] = grayColor;
			color[1] = grayColor;
			color[2] = grayColor;

			img.at<Vec3b>(Point(j, i)) = color;//Reading pixel values
			sumRF = 0;
			sumRB = 0;
			RFCount = 1;
			RBCount = 1;

			//Graylevel value of pixel compare with  initial R_F and R_B
			if (abs(color[0] - R_f) > abs(color[0] - R_b))
			{
				//If value more close to R_B than pixel selected to Background region
				sumRB += color[0];
				RBCount++;
			}
			else{
				//If value more close to R_f than pixel selected to Foreground region
				sumRF += color[0];
				RFCount++;
			}

		}
	}

	//Inýtial comparasion completed.

	int meanRF = sumRF / RFCount;//Initial Mean for foreground region
	int meanRB = sumRB / RBCount;//Initial Mean for background region
	T = (meanRF + meanRB) / 2; //First Threshold value

	//Note : For foreground and background regions only total pixel count and total value 
	//stored because we need to implement mean of region.


	while (true)
	{//To T remains unchanged


		sumRB = 0;
		sumRF = 0;
		RFCount = 1;
		RBCount = 1;
		for (i = 0; i < (img.rows); i++)
		{

			for (j = 0; j < (img.cols); j++)
			{
				Vec3b color = img.at<Vec3b>(Point(j, i));

				if (color[0]>T)// If color value bigger than threshold than
				{
					//pixel selected to background region
					sumRB += color[0];
					RBCount++;
				}
				else{//If value smaller than threshold than pixel selected to foreground region
					sumRF += color[0];
					RFCount++;
				}

			}
		}
		//New R_B and R_F values calculated for both egion
		R_b = sumRB / RBCount;
		R_f = sumRF / RFCount;

		//New threshold calculated
		newT = (R_f + R_b) / 2;

		//If threshold equal to old threslhold, algorithm finished.
		if (newT == T)
			break;

		T = newT;//Threshold changed if new threshold not equal to old.
	}

	//Program calculated the threshold , now it's apply threshold to image.
	for (i = 0; i < img.rows; i++)
	{

		for (j = 0; j < img.cols; j++)
		{
			Vec3b color = img.at<Vec3b>(Point(j, i));
			if (color[0]>T)//If value bigger than threshold 
			{
				//selection is user input to change background and foreground 
				if (selection == 1){//If bigger value is foregroud
					color[0] = 255;
					color[1] = 255;
					color[2] = 255;

				}
				else{
					color[0] = 0;//If bigger value is background
					color[1] = 0;
					color[2] = 0;
				}

			}
			else{//If value smaller than threshold
				//selection is user input to change background and foreground 
				if (selection == 1){
					color[0] = 0;
					color[1] = 0;
					color[2] = 0;

				}
				else{
					color[0] = 255;
					color[1] = 255;
					color[2] = 255;
				}
			}


			img.at<Vec3b>(Point(j, i)) = color;//color value changed to black or white for binary output
		}

	}

	//Writing Threshold value to console.
	cout << "Threshold for K-Means Algorithm : " << T << endl;

	//Show output image on new window.
	namedWindow("K-Means", WINDOW_AUTOSIZE);// Create a window for display.
	imshow("K-Means", img);
}