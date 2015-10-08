// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include "BME465_Template.hpp"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------
#include "resources/lp.xpm"
#include "resources/hp.xpm"
#include "resources/undo.xpm"

#include "wx/image.h"


// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(MENU_FILE_OPEN, MyFrame::OnLoad)
EVT_MENU(MENU_FILE_QUIT, MyFrame::OnQuit)
EVT_MENU(MENU_HELP_ABOUT, MyFrame::OnAbout)
EVT_MENU(ID_ToGray, MyFrame::OnToGray)

EVT_PAINT(MyFrame::OnPaint)
EVT_MENU(MENU_FILTER_LP, MyFrame::OnFilter)
EVT_MENU(MENU_FILTER_HP, MyFrame::OnFilter)
EVT_MENU(MENU_FILTER_SORBEL, MyFrame::OnFilter)
EVT_MENU(MENU_FILTER_MIN, MyFrame::OnFilter)
EVT_MENU(MENU_FILTER_MED, MyFrame::OnFilter)
EVT_MENU(MENU_FILTER_MAX, MyFrame::OnFilter)


EVT_MENU(MENU_FILTER_UNDO, MyFrame::OnFilter)


END_EVENT_TABLE()


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
	// create the main application window
	MyFrame *frame = new MyFrame(_T("BME 465 Template App"),
		wxPoint(50, 50), wxSize(800, 600));

	// and show it (the frames, unlike simple controls, are not shown when
	// created initially)
	frame->Show(TRUE);

	// Enable all image handlers - TIFF is among them
	::wxInitAllImageHandlers();

	// success: wxApp::OnRun() will be called which will enter the main message
	// loop and the application will run. If we returned FALSE here, the
	// application would exit immediately.
	return TRUE;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------
// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
	: wxFrame(NULL, -1, title, pos, size, style)
{
	// set the frame icon
	SetIcon(wxICON(mondrian));

#if wxUSE_MENUS
	// create a menu bar
	wxMenu *fileMenu = new wxMenu;
	wxMenu *image_processMenu = new wxMenu;
	wxMenu *filterMenu = new wxMenu;
	wxMenu *nonlinearMenu = new wxMenu;


	// the "About" item should be in the help menu
	wxMenu *helpMenu = new wxMenu;
	fileMenu->Append(MENU_FILE_OPEN, _T("&Open...\tCtrl-O"), _T("Open a new Image"));
	fileMenu->Append(MENU_FILE_QUIT, _T("E&xit\tAlt-X"), _T("Quit this program"));

	//build filterMenu
	filterMenu->Append(MENU_FILTER_LP, _T("&Lowpass Filter\tAlt-L"), _T("Lowpass Filter"));
	filterMenu->Append(MENU_FILTER_HP, _T("&Highpass Filter\tAlt-H"), _T("Highpass Filter"));
	filterMenu->Append(MENU_FILTER_SORBEL, wxT("Edge Detection (Sorbel)"), wxT("Edge Detection (Sorbel)"));

	nonlinearMenu->Append(MENU_FILTER_MIN, wxT("M&inimum Filter\tAlt-I"), wxT("Minimum Filter")); // TODO: Add hotkeys!
	nonlinearMenu->Append(MENU_FILTER_MED, wxT("M&edian Filter\tAlt-E"), wxT("Median Filter"));
	nonlinearMenu->Append(MENU_FILTER_MAX, wxT("M&aximum Filter\tAlt-A"), wxT("Maximum Filter"));

	filterMenu->Append(MENU_FILTER_NONLINEAR, wxT("Nonlinear"), nonlinearMenu, wxT("Nonlinear Filters"));
	image_processMenu->Append(MENU_FILTER, _T("Fil&ters\tAlt-T"), filterMenu, _T("Filter Menu"));

	helpMenu->Append(MENU_HELP_ABOUT, _T("&About...\tF1"), _T("Show about dialog"));

	// now append the freshly created menu to the menu bar...
	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, _T("&File"));
	menuBar->Append(image_processMenu, _T("&Image Process"));
	menuBar->Append(helpMenu, _T("&Help"));


	// ... and attach this menu bar to the frame
	SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_TOOLBAR
	wxToolBar *MyToolBar = new wxToolBar(this, wxID_ANY);

	MyToolBar->AddTool(MENU_FILTER_LP, wxT("Low Pass Filter"), lp_xpm);
	MyToolBar->AddTool(MENU_FILTER_HP, wxT("High Pass Filter"), hp_xpm);


	/*MyToolBar->AddTool(MENU_FILTER_MIN, wxT("Minimum Filter"), lp_xpm);
	MyToolBar->AddTool(MENU_FILTER_MED, wxT("Median Filter"), lp_xpm);
	MyToolBar->AddTool(MENU_FILTER_MAX, wxT("Maximum Filter"), lp_xpm);*/

	MyToolBar->AddTool(MENU_FILTER_UNDO, wxT("Undo"), undo_xpm);

	MyToolBar->Realize();
	SetToolBar(MyToolBar);

#endif

#if wxUSE_STATUSBAR
	// create a status bar just for fun (by default with 1 pane only)
	CreateStatusBar(2);
	SetStatusText(_T("Welcome to BME 465!"));
#endif // wxUSE_STATUSBAR

	pImage = NULL;

	// set the slider stuff to 0

}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//slider frame constructor


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	// TRUE is to force the frame to close
	Close(TRUE);
}


void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxString msg;
	msg.Printf(_T("This is the About dialog for BME 465.\n")
		_T("Welcome to %s"), wxVERSION_STRING);

	wxMessageBox(msg, _T("About BME 465"), wxOK | wxICON_INFORMATION, this);
}
void MyFrame::OnLoad(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog fileDialog(NULL, "Choose a file to load ...", "", "", "*.bmp;*.tif;*.gif;*.jpg", wxFD_OPEN, wxDefaultPosition);

	if (fileDialog.ShowModal() == wxID_OK) {
		wxString fileName = fileDialog.GetPath();
		wxString NameofFile = fileDialog.GetFilename();

		//Update the view of main frame
		if (pImage != NULL) delete pImage;
		pImage = new wxImage(fileName);

		if (pImage == NULL)
			wxMessageBox("File Failed to Open!", wxT("Error"), wxOK | wxICON_INFORMATION, this);
		else {
			masterImage = pImage;
			SetTitle(fileName);
			Refresh();
		}
	}

	fileDialog.Destroy();
}
void MyFrame::OnToGray(wxCommandEvent& event)
{
	if (pImage == NULL) {
		wxMessageBox("Image is not loaded yet!", _T("Error"), wxOK | wxICON_INFORMATION, this);
		return;
	}

	int* buffer = new int[pImage->GetWidth()*pImage->GetHeight()];
	wxImage2grayBuffer(pImage, buffer);

	wxImage* temp = grayBuffer2wxImage(buffer, pImage->GetWidth(), pImage->GetHeight());
	delete pImage;
	pImage = temp;
	delete buffer;

	Refresh();
	return;
}

void MyFrame::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);

	if (pImage == NULL) {
		event.Skip();
		return;
	}

	wxBitmap tempBitmap(*pImage);
	SetClientSize(tempBitmap.GetWidth()*1.25, tempBitmap.GetHeight()*1.25 + 10);
	int spacingW = ceilf((tempBitmap.GetWidth()*1.25 - tempBitmap.GetWidth()) / 2);
	int spacingH = ceilf((tempBitmap.GetHeight()*1.25 - tempBitmap.GetHeight()) / 2);

	dc.DrawBitmap(tempBitmap, spacingW, spacingH + 25, TRUE);
}

void MyFrame::OnFilter(wxCommandEvent& event)
{
	if (pImage == NULL) {
		wxMessageBox("Image is not loaded yet!", "Error", wxOK | wxICON_INFORMATION, this);
		return;
	}

	wxImage *Filtered = NULL;

	// Switch our toolbar request to do what we really want.
	switch (event.GetId()) {
	case MENU_FILTER_LP: Filtered = LowPass(pImage); break;
	case MENU_FILTER_HP: Filtered = HighPass(pImage); break;
	case MENU_FILTER_SORBEL: Filtered = sobelFilter(pImage); break;
	case MENU_FILTER_MIN: Filtered = nonLinFilter(pImage, NONLIN_MIN); break;
	case MENU_FILTER_MED: Filtered = nonLinFilter(pImage, NONLIN_MEDIAN); break;
	case MENU_FILTER_MAX: Filtered = nonLinFilter(pImage, NONLIN_MAX); break;
	case MENU_FILTER_UNDO:
		Filtered = copy(masterImage);

		delete pImage;
		break;
	}

	pImage = Filtered;

	Refresh();
}
