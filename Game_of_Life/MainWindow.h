#pragma once
#pragma warning(disable : 4996)

//#include "wx/vector.h"
#include <vector>
#include "wx/wx.h"
#include "DrawingPanel.h"
#include "GameSettings.h"
#include "SettingDialogUI.h"
#include "RandomizeWithSeedDialog.h"

class MainWindow : public wxFrame
{
public:
	MainWindow();
	~MainWindow();

	// Event Handler member functions
	void OnSizeChange(wxSizeEvent& event);
	void OnClickPlayButton(wxCommandEvent& event);
	void OnClickPauseButton(wxCommandEvent& event);
	void OnClickNextButton(wxCommandEvent& event);
	void OnClickTrashButton(wxCommandEvent& event);	
	void OnTimer(wxTimerEvent& event);
	void OnSettingsDialogUI(wxCommandEvent& event);
	void OnToggleShowNeighbors(wxCommandEvent& event);
	void OnRandomizeMenu(wxCommandEvent& event);
	void OnRandomizeMenuWithSeed(wxCommandEvent& event);
	void OnMouseHoverToolBar(wxMouseEvent& event);
	void OnFiniteCheckBox(wxCommandEvent& event);
	void OnToroidalCheckBox(wxCommandEvent& event);
	void OnResetToDefault(wxCommandEvent& event);
	void OnImportGameBoard(wxCommandEvent& event);
	void OnShowGrid(wxCommandEvent& event);
	void OnShowThickGrid(wxCommandEvent& event);
	void OnShowHUD(wxCommandEvent& event);

	// Create a method in the main window header file with a return type of void that will initialize the grid.
	void InitializeGrid();
	int  CalculateNeighborCount(int x, int y);
	void nextGenerationOfCell();	
	void UpdateNighborCount2DVector();


	void OnOpenClicked(wxCommandEvent& event);
	void OnSaveClicked(wxCommandEvent& event);
	void OnSaveAsClicked(wxCommandEvent& event);
	void OnExitClicked(wxCommandEvent& event);

	int GetGeneration() const;
	int GetLivingCells() const;

private:

	// GameSetting contains the data for grid size, time interval
	// color for living and dead cell. 
	GameSettings mGameSettings;
	
	// hard coded rectangle/square grid 15x15
	//const int GRID_SIZE = 25;

	DrawingPanel* mDrawingPanel = nullptr;	

	// Each box is going to have two possible states. It's either on or off. 
	// This can best be represented by a bool, where false meaning the box is dead 
	// and true meaning that the box is alive.
	std::vector<std::vector<bool>> m2DGameBoard; // this is the main vector
	
	int mGeneration, mLivingCells;
	//int mTimeInterval = 50;

	wxMenu* viewMenu = nullptr;
	// Add this to keep track of the menu item state
	wxMenuItem* mShowNeighborsItems = nullptr;
	
	wxMenuItem* mRandomizeMenuItem = nullptr;
	int mRandomizeNumber; 

	wxMenuItem* mRandomizeMenuItemWithSeed = nullptr;
	int mRandomizeNumberWithSeed;

	std::vector<std::vector<int>> m2DBoardsNeighborCounts;

	// wxMenuBar* will represent the entire menu bar
	wxMenuBar* mMenuBarWX = nullptr;

	wxStatusBar* statusBar = nullptr;
	wxToolBar* toolBar = nullptr;
	wxTimer* timerFromWx = nullptr;	

	wxString mCurrentFileName;

	wxMenuItem* mFiniteOption = nullptr;
	wxMenuItem* mTorodialOption = nullptr;

	void UpdateStatusBar();

	wxDECLARE_EVENT_TABLE();
};