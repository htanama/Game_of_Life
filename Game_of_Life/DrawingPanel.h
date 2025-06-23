#pragma once

#include "wx/wx.h"
#include "GameSettings.h"

class DrawingPanel : public wxPanel
{
public:
	wxFrame* _parent = nullptr;

	DrawingPanel(wxFrame* parent, std::vector<std::vector<bool>>& gameBoard,
		std::vector<std::vector<int>>& neighborCounts);
	~DrawingPanel();

	void SettingTheSize(wxSize& size);
	void SetGridSize(int gridSize);
	void OnMouseUp(wxMouseEvent& event);

	void SetSettings(GameSettings* mGameSetting);
	
	void SetSettingsGridSize(int gridSize);
	void SetSettingsLivingCellColor(wxColor color);
	void SetSettingsDeadCellColor(wxColor color);	

private:

	// pointer only storing the address of the object
	GameSettings* mPtrGameSetting = nullptr;

	void OnPaint(wxPaintEvent& event);
	std::vector<std::vector<bool>>& m2DGameBoardDrawingPannel;
	std::vector<std::vector<int>>& m2DBoardNeighborCounts;
	
	//int mGridSize;

	// A wxPoint is a useful data structure for graphics operations.
	// It contains integer x and y members.See wxRealPoint for a floating point version.
	wxPoint mPoint2D;

	wxDECLARE_EVENT_TABLE();
};