#pragma once

#include "wx/wx.h"
#include "GameSettings.h"
#include "wx/spinctrl.h"
#include "wx/clrpicker.h"

class SettingDialogUI : public wxDialog
{
public: 
	SettingDialogUI(wxWindow* parent, GameSettings* gameSetting = nullptr);
	~SettingDialogUI();

	// Setter for the settings object pointer
	void SetSettings(const GameSettings* gameSetting);

	// Event handlers (declared in the header file)
	void OnGridSizeSpinChanged(wxSpinEvent& event);
	void OnIntervalSpinChanged(wxSpinEvent& event);
	void OnLivingColorPickChanged(wxColourPickerEvent& event);
	void OnDeadColorPickChanged(wxColourPickerEvent& event);
	void OnOkButtonClicked(wxCommandEvent& event);
	void OnCancelButtonClicked(wxCommandEvent& event);

private:

	GameSettings* mGameSetting; // // Pointer to the current settings
	GameSettings mOriginalSettings;    // Backup of the original settings

	wxBoxSizer *mainSizer; // Main sizer for the dialog
	wxSpinCtrl* spinCtrlGridSize;
	wxColourPickerCtrl* livingColorPicker; // Color picker for living cells	
	wxColourPickerCtrl* deadColorPicker; // Color picker for dead cells
	wxSpinCtrl* spinCtrlTimeInterval; // Spin control for update interval	

	wxDECLARE_EVENT_TABLE();
};

