#pragma once
#include "wx/wx.h"
#include "wx/spinctrl.h"

class RandomizeWithSeedDialog : public wxDialog {

public:
	RandomizeWithSeedDialog(wxWindow* parent);

private:
	wxSpinCtrl* spinCtrlSeed = nullptr;
	unsigned int mRandomNumber = 7519745;
	
};