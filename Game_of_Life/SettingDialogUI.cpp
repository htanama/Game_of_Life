#include "SettingDialogUI.h"

wxBEGIN_EVENT_TABLE(SettingDialogUI, wxDialog)
	EVT_SPINCTRL(10051, SettingDialogUI::OnGridSizeSpinChanged)
	EVT_SPINCTRL(10052, SettingDialogUI::OnIntervalSpinChanged)
	EVT_COLOURPICKER_CHANGED(10053, SettingDialogUI::OnLivingColorPickChanged)
	EVT_COLOURPICKER_CHANGED(10054, SettingDialogUI::OnDeadColorPickChanged)	
	EVT_BUTTON(wxID_OK, SettingDialogUI::OnOkButtonClicked)
	EVT_BUTTON(wxID_CANCEL, SettingDialogUI::OnCancelButtonClicked)
wxEND_EVENT_TABLE()

SettingDialogUI::SettingDialogUI(wxWindow* parent, GameSettings* gameSettings)
	:wxDialog (parent, wxID_ANY, "Setting Dialog UI"), mGameSetting(gameSettings), mOriginalSettings(*gameSettings)
{

	// Main box sizer for vertical layout
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	// Child sizer for the Grid Size
	wxBoxSizer* boxSizerGridSize = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* staticTextGridSize = new wxStaticText(this, wxID_ANY, "Grid Size:");
	spinCtrlGridSize = new wxSpinCtrl(this, 10051, wxString::Format("%d", gameSettings->GridSize));
	//Bind(wxEVT_SPINCTRL, &SettingDialogUI::OnGridSizeSpin, this, m_spinCtrl->GetId());	
	boxSizerGridSize->Add(staticTextGridSize, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
	boxSizerGridSize->Add(spinCtrlGridSize, 1, wxLEFT, 110);

	// Child sizer for the Interval Size
	wxBoxSizer* boxSizerInterval = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* staticTextIntervalTime = new wxStaticText(this, wxID_ANY, "Time Interval:");
	spinCtrlTimeInterval = new wxSpinCtrl(this, 10052, wxString::Format("%d", gameSettings->TimeInterval));
	spinCtrlTimeInterval->SetRange(1, 9000); // setting the range of minimum value and maximum value of time interval
	boxSizerInterval->Add(staticTextIntervalTime, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
	boxSizerInterval->Add(spinCtrlTimeInterval, 1, wxLEFT, 85);

	// Child sizer for Living Cell Color
	wxBoxSizer* boxSizerLivingCellColor = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* staticTextLivingCellColor = new wxStaticText(this, wxID_ANY, "Living Cell Color:");
	livingColorPicker = new wxColourPickerCtrl(this, 10053, gameSettings->GetColorForLivingCell());
	boxSizerLivingCellColor->Add(staticTextLivingCellColor, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
	boxSizerLivingCellColor->Add(livingColorPicker, 1, wxLEFT, 65);

	// Child sizer for Dead Cell Color
	wxBoxSizer* boxSizerDeadCellColor = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* staticTextDeadCellColor = new wxStaticText(this, wxID_ANY, "Dead Cell Color");
	deadColorPicker = new wxColourPickerCtrl(this, 10054, gameSettings->GetColorForDeadCell());
	boxSizerDeadCellColor->Add(staticTextDeadCellColor, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
	boxSizerDeadCellColor->Add(deadColorPicker, 1, wxLEFT, 71);

	// Add all child wxBoxSizer to the wxBoxSizer mainSizer (i.e. boxSizerGridSize, boxSizerLivingCellColor, etc)
	mainSizer->Add(boxSizerGridSize, 0, wxEXPAND | wxALL, 10);
	mainSizer->Add(boxSizerInterval, 0, wxEXPAND | wxALL, 10);
	mainSizer->Add(boxSizerLivingCellColor, 0, wxEXPAND | wxALL, 10);
	mainSizer->Add(boxSizerDeadCellColor, 0, wxEXPAND | wxALL, 10);

	// Add the OK and CANCEL buttons to the mainSizer
	wxSizer* buttonSizer = CreateButtonSizer(wxOK | wxCANCEL);
	mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxALL, 10);

	// Set the mainSizer
	SetSizer(mainSizer);
	mainSizer->Fit(this);
}

SettingDialogUI::~SettingDialogUI()
{
	//delete mMainBoxSizerWX;
	//mMainBoxSizerWX = nullptr;
}

void SettingDialogUI::OnGridSizeSpinChanged(wxSpinEvent& event)
{
	// https://docs.wxwidgets.org/latest/classwx_spin_ctrl.html#aa8ec6e624432bf37b0172a15a913f015
	// Gets the value of the spin control and put it in the mGameSetting->GridSize
	mGameSetting->GridSize = event.GetValue();	
	
}

void SettingDialogUI::OnIntervalSpinChanged(wxSpinEvent& event)
{
	mGameSetting->TimeInterval = event.GetValue();
}

void SettingDialogUI::OnLivingColorPickChanged(wxColourPickerEvent& event)
{
	// https://docs.wxwidgets.org/stable/classwx_colour_picker_ctrl.html
	// wxColour GetColour () const // Returns the currently selected colour.
	// Get the color value from the wxColourPickerCtrl and assign it to Living Cell color in the game setting struct
	mGameSetting->SetColorForLivingCell(event.GetColour());
}

void SettingDialogUI::OnDeadColorPickChanged(wxColourPickerEvent& event)
{
	// https://docs.wxwidgets.org/stable/classwx_colour_picker_ctrl.html
	// wxColour GetColour () const // Returns the currently selected colour.
	// Get the color value from the wxColourPickerCtrl and assign it to Living Cell color in the game setting struct
	mGameSetting->SetColorForDeadCell(event.GetColour());
}

void SettingDialogUI::OnOkButtonClicked(wxCommandEvent& event)
{
	/*/ Apply the changes to the settings object
	mGameSetting->GridSize = spinCtrlGridSize->GetValue();
	mGameSetting->TimeInterval = spinCtrlTimeInterval->GetValue();
	mGameSetting->SetColorForLivingCell(livingColorPicker->GetColour());
	mGameSetting->SetColorForDeadCell(deadColorPicker->GetColour());
	*/

	// https://docs.wxwidgets.org/latest/classwx_dialog.html#a6e078c3d0653f75ad3c34a37c0b54637
	// 	Ends a modal dialog, passing a value to be returned from the ShowModal() invocation. More...
	EndModal(wxID_OK);
}

void SettingDialogUI::OnCancelButtonClicked(wxCommandEvent& event)
{
	*mGameSetting = mOriginalSettings;
	EndModal(wxID_CANCEL);
}