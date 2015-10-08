// This file provides the basic routines for image processing

#include "image_processor.hpp"
#include "BME465_Template.hpp"
#include "util.h"
#include <vector> //FOR MEDIAN FILTER
#include <algorithm> //FOR SORT()
#include <cmath>

// Implementation of the routines
void wxImage2grayBuffer(const wxImage* img, int* buffer)
{
	unsigned long pix_index = 0, byte_index = 0;
	unsigned long buffer_len;
	unsigned char* img_buffer;

	buffer_len = img->GetWidth()* img->GetHeight();
	img_buffer = img->GetData();

	for (pix_index = 0, byte_index = 0; pix_index < buffer_len; pix_index++)
	{
		int sum = 0;
		sum = img_buffer[byte_index++];
		sum += img_buffer[byte_index++];
		sum += img_buffer[byte_index++];

		buffer[pix_index] = sum / 3;
	}

	return;
} // end of wxImage2grayBuffer(...)

wxImage* grayBuffer2wxImage(const int* buffer, int width, int height)
{
	unsigned long pix_index = 0, byte_index = 0;
	unsigned long buffer_len;
	unsigned char* img_buffer;

	wxImage* img = new wxImage(width, height);
	buffer_len = width*height;
	img_buffer = new unsigned char[width*height * 3];

	for (pix_index = 0, byte_index = 0; pix_index < buffer_len; pix_index++)
	{
		img_buffer[byte_index++] = (unsigned char)buffer[pix_index];
		img_buffer[byte_index++] = (unsigned char)buffer[pix_index];
		img_buffer[byte_index++] = (unsigned char)buffer[pix_index];
	}

	img->SetData(img_buffer);

	return img;
} // end of grayBuffer2wxImage(...)
wxImage* colorBuffer2wxImage(const int* buffer, int width, int height)
{
	unsigned long pix_index = 0, byte_index = 0;
	unsigned long buffer_len;
	unsigned char* img_buffer;
	double PI = 3.1415;

	wxImage* img = new wxImage(width, height);
	buffer_len = width*height;
	img_buffer = new unsigned char[width*height * 3];

	for (pix_index = 0, byte_index = 0; pix_index < buffer_len; pix_index++)
	{
		if (buffer[pix_index] > 0)
		{
			img_buffer[byte_index++] = 255 - (unsigned char)(buffer[pix_index] * 255 / PI);
			img_buffer[byte_index++] = (unsigned char)(0);
			img_buffer[byte_index++] = 255 - (unsigned char)(buffer[pix_index] * 255 / PI);
		}
		if (buffer[pix_index] < 0)
		{
			img_buffer[byte_index++] = (unsigned char)(0);
			img_buffer[byte_index++] = (unsigned char)(buffer[pix_index] * 255 / PI);
			img_buffer[byte_index++] = (unsigned char)(0);
		}
		if (buffer[pix_index] == 0)
		{
			img_buffer[byte_index++] = (unsigned char)(0);
			img_buffer[byte_index++] = (unsigned char)(0);
			img_buffer[byte_index++] = (unsigned char)(0);
		}


	}

	img->SetData(img_buffer);

	return img;
} // end of grayBuffer2wxImage(...)


void wxImage2colorBuffer(const wxImage* img, int* buffer)
{
	unsigned long pix_index = 0, byte_index = 0;
	unsigned long buffer_len;
	unsigned char* img_buffer;

	double PI = 3.1415;
	buffer_len = img->GetWidth()* img->GetHeight();
	img_buffer = img->GetData();

	for (pix_index = 0, byte_index = 0; pix_index < buffer_len; pix_index++)
	{
		int sum = 0;
		sum = img_buffer[byte_index++] * 255 / PI;
		sum += img_buffer[byte_index++] * 255 / PI;
		sum += img_buffer[byte_index++] * 255 / PI;

		buffer[pix_index] = 255 - sum / 3;
	}

	return;
}

// Process a wxImage and generate an int array of its values, to be passed into a filter.
// UNUSED
int* createFilterTemp(wxImage *pImage) {
	int h = pImage->GetHeight();
	int w = pImage->GetWidth();

	int* pTemp = new int[w * h];

	wxImage2grayBuffer(pImage, pTemp);

	return pTemp;
}

// Edge detection.
wxImage* sobelFilter(wxImage* pImage) {
	int h = pImage->GetHeight();
	int w = pImage->GetWidth();

	int* pTemp = new int[w * h];
	int* pRes = new int[w * h];

	wxImage2grayBuffer(pImage, pTemp);

	int x, y;
	unsigned long i;
	double val;

	for (x = 0; x < w; x++) {
		for (y = 0; y < h; y++) {

			i = (y * w) + x;
			if (x != 0 && y != 0 && x != (w - 1) && y != (h - 1)) {
				val =
					abs(pTemp[i - w - 1] * (-1) + pTemp[i - w + 1] + pTemp[i - 1] * (-2) + pTemp[i + 1] * 2 + pTemp[i + w - 1] * (-1) + pTemp[i + w + 1])
					+ abs(pTemp[i - w - 1] * (-1) + pTemp[i - w] * (-2) + pTemp[i - w + 1] * (-1) + pTemp[i + w - 1] + pTemp[i + w] * 2 + pTemp[i + w + 1]);
				
				if (val < 0) val = 0;
				if (val > 255) val = 255;

				pRes[i] = (int) val;
			}
			else {
				pRes[i] = pTemp[i];
			}
		}
	}

	wxImage* temp = grayBuffer2wxImage(pRes, w, h);

	delete pTemp;
	delete pRes;
	return temp;
}
// Median filter
wxImage* nonLinFilter(wxImage* pImage, int FILTER_TYPE) {
	int h = pImage->GetHeight();
	int w = pImage->GetWidth();

	int* pTemp = new int[w * h];
	int* pRes = new int[w * h];

	wxImage2grayBuffer(pImage, pTemp);

	// Create the vector of ints.
	std::vector<int> valArr(9);
	double val;
	int x, y;
	unsigned long i;

	for (x = 0; x < w; x++) {
		for (y = 0; y < h; y++) {

			i = (y * w) + x;
			if (x != 0 && y != 0 && x != (w - 1) && y != (h - 1)) {
				valArr[0] = pTemp[i - w - 1];
				valArr[1] = pTemp[i - w];
				valArr[2] = pTemp[i - w + 1];

				valArr[3] = pTemp[i - 1];
				valArr[4] = pTemp[i];
				valArr[5] = pTemp[i + 1];

				valArr[6] = pTemp[i + w - 1];
				valArr[7] = pTemp[i + w];
				valArr[8] = pTemp[i + w + 1];

				sort(valArr.begin(), valArr.end());

				switch (FILTER_TYPE) {
				case NONLIN_MIN: val = valArr[0]; break;
				case NONLIN_MEDIAN: val = valArr[4]; break;
				case NONLIN_MAX: val = valArr[8]; break;
				}

				pRes[i] = clamp(val, 0, 255);

			}
			else {
				pRes[i] = pTemp[i];
			}
		}
	}

	wxImage* temp = grayBuffer2wxImage(pRes, w, h);

	delete pTemp;
	delete pRes;
	return temp;
}


wxImage * HighPass(wxImage *pImage) {
	int height = pImage->GetHeight();
	int width = pImage->GetWidth();

	int* pTemp = new int[width * height];
	int* pResult = new int[width * height];

	wxImage2grayBuffer(pImage, pTemp);

	// We don't want to keep creating these variables within this loop.  Thats a scary thing maan.
	int x, y;
	unsigned long i;
	double val;

	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {

			i = (y * width) + x;
			if (x != 0 && y != 0 && x != (width - 1) && y != (height - 1)) {

				val = (
					pTemp[i - width - 1] + pTemp[i - width] + pTemp[i - width + 1]
					+ pTemp[i - 1] + pTemp[i] + pTemp[i + 1]
					+ pTemp[i + width - 1] + pTemp[i + width] + pTemp[i + width + 1]
					) / 9.0;


				pResult[i] = 2 * pTemp[i] - (int)val;
				pResult[i] = clamp(pResult[i], 0, 255);
			}
			else {
				pResult[i] = pTemp[i];
			}
		}
	}

	wxImage* temp = grayBuffer2wxImage(pResult, width, height);

	delete pTemp;
	delete pResult;
	return temp;
}


wxImage * LowPass(wxImage *pImage)
{
	int x, y;
	int height = pImage->GetHeight();
	int width = pImage->GetWidth();
	int* pTemp;
	int* pResult;

	pTemp = new int[width*height];
	pResult = new int[width*height];
	wxImage2grayBuffer(pImage, pTemp);

	for (x = 0;x < width;x++)
	{
		for (y = 0;y < height;y++)
		{
			if (x != 0 && y != 0 && x != (width - 1) && y != (height - 1))
			{
				unsigned long index;
				index = (unsigned long)y*width + x;
				double value = 1.0 / 9.0*
					(float(pTemp[index - width - 1]) + float(pTemp[index - width]) + float(pTemp[index - width + 1])
						+ float(pTemp[index - 1]) + float(pTemp[index]) + float(pTemp[index + 1])
						+ float(pTemp[index + width - 1]) + float(pTemp[index + width]) + float(pTemp[index + width + 1])
						);
				pResult[(y)*width + (x)] = (int)(value);
			}
			else
			{
				pResult[y*width + x] = pTemp[y*width + x];
			}
		}
	}

	wxImage* temp;
	temp = grayBuffer2wxImage(pResult, width, height);
	delete pResult;
	return temp;
}



wxImage *copy(wxImage *masterImage) {
	int x, y;
	int height = masterImage->GetHeight();
	int width = masterImage->GetWidth();
	int* pTemp;
	int* pResult;

	pTemp = new int[width*height];
	pResult = new int[width*height];

	wxImage2grayBuffer(masterImage, pTemp);

	for (x = 0;x < width;x++) {
		for (y = 0;y < height;y++) {
			unsigned long index;
			index = (unsigned long)y*width + x;
			pResult[index] = pTemp[index];
		}
	}

	wxImage* temp;
	temp = grayBuffer2wxImage(pResult, width, height);
	delete pResult;
	return temp;
}
