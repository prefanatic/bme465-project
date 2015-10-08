// Image_processer.hpp

#ifndef IMAGE_PROCESSOR_HPP
#define IMAGE_PROCESSOR_HPP

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include <vector>
#include <cmath>
#include "wx/image.h"


// Definitions of the routines
void wxImage2grayBuffer(const wxImage* img, int* buffer);
wxImage* grayBuffer2wxImage(const int* buffer, int width, int height);
void wxImage2colorBuffer(const wxImage* img, int* buffer);
wxImage* colorBuffer2wxImage(const int* buffer, int width, int height);
wxImage* LowPass(wxImage *pImage);
wxImage* HighPass(wxImage *pImage);
wxImage* nonLinFilter(wxImage* pImage, int FILTER_TYPE);
wxImage* sobelFilter(wxImage* pImage);
wxImage* copy(wxImage *masterImage);

#endif
