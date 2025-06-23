#pragma once
#include "wx/wx.h"
#include <fstream>
 
struct GameSettings {
	unsigned int RedDeadCell = 255;
	unsigned int GreenDeadCell = 255;
	unsigned int BlueDeadCell = 255;
	unsigned int AlphaDeadCell = 255;

	unsigned int RedLivingCell = 128;
	unsigned int GreenLivingCell = 128;
	unsigned int BlueLivingCell = 128;
	unsigned int AlphaLivingCell = 255;
	
	
	int GridSize = 15; // the GridSize can be change/modify	

	int TimeInterval = 50; // the TimeInterval can be change/modify
	
	bool IsShowNeighborCount = false;  
	bool IsUniverseFinite = true; // The universe should currently be defined as finite.
	bool IsUniverseToroidal = false;
	
	bool mShowGrid = true;          
	bool mShowThickGrid = true;      
	bool mShowHUD = true;

	int winWidth = 600;
	int winHeight = 600;

	wxColor GetColorForLivingCell() const
	{
		wxColor color(RedLivingCell, GreenLivingCell, BlueLivingCell, AlphaLivingCell);
		return color;
	}

	void SetColorForLivingCell(wxColour color) {
		RedLivingCell = color.GetRed();
		GreenLivingCell = color.GetGreen();
		BlueLivingCell = color.GetBlue();
		AlphaLivingCell = color.GetAlpha();
	}

	wxColor GetColorForDeadCell() const
	{
		wxColor color(RedDeadCell, GreenDeadCell, BlueDeadCell, AlphaDeadCell);
		return color;
	}

	void SetColorForDeadCell(wxColour color) {
		RedDeadCell = color.GetRed();
		GreenDeadCell = color.GetGreen();
		BlueDeadCell = color.GetBlue();
		AlphaDeadCell = color.GetAlpha();
	}

	//// Save the settings to a binary file
	//void Save(const std::string& filename = "settings.bin")
	//{
	//	std::ofstream file(filename, std::ios::out | std::ios::binary);
	//	if (file.is_open())
	//	{
	//		file.write((char*)this, sizeof(GameSettings));
	//		file.close();
	//	}
	//}

	//// Load the settings from a binary file
	//void Load(const std::string& filename = "settings.bin")
	//{
	//	std::ifstream file(filename, std::ios::in | std::ios::binary);
	//	if (file.is_open())
	//	{
	//		file.read((char*)this, sizeof(GameSettings));
	//		file.close();
	//	}
	//}



	// Save the settings to a binary file
	void Save()
	{
		std::ofstream file("settings.bin", std::ios::out | std::ios::binary);
		if (file.is_open())
		{			
			file.write((char*)this, sizeof(GameSettings));
			file.close();
		}
	}

	// Load the settings from a binary file
	void Load()
	{
		std::ifstream file("settings.bin", std::ios::in | std::ios::binary);
		if (file.is_open())
		{
			file.read((char*)this, sizeof(GameSettings));
			file.close();
		}
	}

};