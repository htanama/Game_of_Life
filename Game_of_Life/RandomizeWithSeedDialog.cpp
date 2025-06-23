#include "RandomizeWithSeedDialog.h"

RandomizeWithSeedDialog::RandomizeWithSeedDialog(wxWindow* parent)
	:wxDialog(parent, wxID_ANY, "Randomize")
{
	// Main box sizer for vertical layout
	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);


	wxBoxSizer* boxSizerSeed = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* staticTextSeed = new wxStaticText(this, wxID_ANY, "SEED", wxPoint(20, 5), wxSize(100, -1));
	wxStaticText* staticTextEnterSeed = new wxStaticText(this, wxID_ANY, "Enter seed:");
	wxSpinCtrl* spinCtrlGridSize = new wxSpinCtrl(this, 10051, wxString::Format("%d", mRandomNumber), wxDefaultPosition, wxSize(150, -1));
	spinCtrlGridSize->SetRange(1, 9999999);
	
	boxSizerSeed->Add(staticTextEnterSeed, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
	boxSizerSeed->Add(spinCtrlGridSize, 0, wxEXPAND | wxALL, 30);
	
	topSizer->Add(boxSizerSeed, 0, wxEXPAND | wxALL, 10);

	// Add the OK and CANCEL buttons to the mainSizer
	wxSizer* buttonSizer = CreateButtonSizer(wxOK | wxCANCEL);
	topSizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxALL, 10);

	SetSizerAndFit(topSizer);

}
