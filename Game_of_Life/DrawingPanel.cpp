#include "DrawingPanel.h"
#include "wx/graphics.h"
#include "wx/dcbuffer.h"
#include "MainWindow.h"

// Event table for DrawingPanel - this is a static event handler
wxBEGIN_EVENT_TABLE(DrawingPanel, wxPanel)
	EVT_PAINT(DrawingPanel::OnPaint)
	EVT_LEFT_UP(DrawingPanel::OnMouseUp)
wxEND_EVENT_TABLE()

DrawingPanel::DrawingPanel(wxFrame* parent, std::vector<std::vector<bool>>& gameBoard, std::vector<std::vector<int>>& neighborCounts)
	: wxPanel(parent, wxID_ANY, wxPoint(0, 0)), m2DGameBoardDrawingPannel(gameBoard), m2DBoardNeighborCounts(neighborCounts)
{
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	

	// dynamic binding to the event handler macros Paint handler - wxEVT_PAINT
	//this->Bind(wxEVT_PAINT, &DrawingPanel::OnPaint, this);

	// This event handler will be called after a mouse button is clicked.
	// after the left mouse button is clicked wxEVT_LEFT_UP, it will call OnMouseUP() function
	//this->Bind(wxEVT_LEFT_UP, &DrawingPanel::OnMouseUp, this);
}

DrawingPanel::~DrawingPanel()
{
}

void DrawingPanel::SettingTheSize(wxSize& size)
{	
	SetSize(size);	
	Refresh();
}

// The easiest way is likely to create a setter for the settings pointer.
void DrawingPanel::SetGridSize(int gridSize)
{	
	mPtrGameSetting->GridSize = gridSize;		
}

void DrawingPanel::OnMouseUp(wxMouseEvent& event)
{
	// When the mouse event handler method is implemented, the wxMouseEvent arguments 
	// gives two important pieces of information. It has a method for GetX() and GetY()
	mPoint2D.x = event.GetX();
	mPoint2D.y = event.GetY();

	wxSize panelSize = GetClientSize();
	//wxSize panelSize = GetSize();
	
	int gridWidth = m2DGameBoardDrawingPannel.size(); //  returns the number of rows or size of row
	int gridHeight = m2DGameBoardDrawingPannel[0].size(); //  returns the number of columns or size of column in row[0]
	
	//int gridWidth = mPtrGameSetting->GridSize; 
	//int gridHeight = mPtrGameSetting->GridSize; //  returns the number of columns or size of column in row[0]

	//const int CELL_WIDTH = panelSize.GetWidth() / mGridSize; 
	//const int CELL_HEIGHT = panelSize.GetHeight() / mGridSize;

	int cellWidth = panelSize.GetWidth() / gridWidth;
	int cellHeight = panelSize.GetHeight() / gridHeight;

	// Calculate the row and column that was clicked
	//int row = mPoint2D.x / CELL_WIDTH;
	//int col = mPoint2D.y / CELL_HEIGHT;

	// Row is hight, Y-coordinate, because positive number goint to the bottom. 
	// Col is width, X-coordinate, because positive number going to the right

	// Calculate the row and column that was clicked
	// The row and column that was clicked can be calculated by dividing the x coordinate
	// of the click by the cell width and the y coordinate of the click by the cell height.
	int col = mPoint2D.x / cellWidth;
	int row = mPoint2D.y / cellHeight;

	// We just calculated and recorded X and Y coordinate where the mouse clicked on the board
	// Now we need to find the exact location in the game board
	if (row >= 0 && col >= 0)
	{
		
		// Toggle the cell value true/false - if it is true it becomes false and if it is false it becomes true. 
		// this is the example:  isFill != isFill
		m2DGameBoardDrawingPannel[row][col] = !m2DGameBoardDrawingPannel[row][col];
	}
	

	MainWindow* mainWindow = dynamic_cast<MainWindow*>(GetParent());
	mainWindow->UpdateNighborCount2DVector();
	Refresh();	

}

void DrawingPanel::SetSettings(GameSettings* gameSettings)
{	
	mPtrGameSetting = gameSettings;
}

void DrawingPanel::SetSettingsGridSize(int gridSize)
{
	// mPtrGameSetting->GridSize = gridSize;
}

void DrawingPanel::SetSettingsLivingCellColor(wxColor color)
{
	mPtrGameSetting->SetColorForLivingCell(color);
}

void DrawingPanel::SetSettingsDeadCellColor(wxColor color)
{
	mPtrGameSetting->SetColorForDeadCell(color);
}

void DrawingPanel::OnPaint(wxPaintEvent& event) {
	
	wxAutoBufferedPaintDC dc(this);
	dc.Clear();

	wxGraphicsContext* graphicContext = wxGraphicsContext::Create(dc);
	
	if (graphicContext) {

		if (mPtrGameSetting->mShowGrid) {
			graphicContext->SetPen(wxPen(wxColor(0, 0, 0, 64)));
		}
		else {
			graphicContext->SetPen(*wxTRANSPARENT_PEN);
		}		
		
		graphicContext->SetBrush(*wxWHITE);	

		// wxSize wxWindow::GetClientSize()	const
		// This is an overloaded member function, provided for convenience. Returns the size of the window 'client area' in pixels.
		wxSize panelSize = GetClientSize();

		// Cell width and height for the small square inside the big rectangle/square. 
		//const int CELL_WIDTH = panelSize.GetWidth() / mPtrGameSetting->GridSize; // the width of the small rectangle/square
		//const int CELL_HEIGHT = panelSize.GetHeight() / mPtrGameSetting->GridSize; // the heigh of the small rectangle/square 	

		// Calculate cell size using floating-point arithmetic to avoid gaps
		float CELL_WIDTH = this->GetSize().x / (float)mPtrGameSetting->GridSize;
		float CELL_HEIGHT = this->GetSize().y / (float)mPtrGameSetting->GridSize;

		wxDouble cellSize = std::min(CELL_WIDTH, CELL_HEIGHT);
		// Calculate an appropriate font size (e.g., 70% of the cell size)
		int fontSize = static_cast<int>(cellSize * 0.7);		
		
		// Set the font and text color
		graphicContext->SetFont(wxFontInfo(fontSize), *wxRED);

		wxString neighborText;
		wxDouble textWidth, textHeight;		
		wxDouble textX;
		wxDouble textY;


		// row and col to draw the big rectangle/square
		for (int row = 0; row < m2DGameBoardDrawingPannel.size(); ++row) {
			for (int col = 0; col < m2DGameBoardDrawingPannel[0].size(); ++col) {

				// there is a gap on between the grid and the wxFrame area 
				// because of this calculation below
				int x = col * CELL_WIDTH;
				int y = row * CELL_HEIGHT;

				//int x = col * CELL_WIDTH; // find the starting point of width 
				//int y = row * CELL_HEIGHT; // find the starting point of height

				// The corresponding bool from the game board needs to be checked to see if it is true or false.
				// before drawing the rectangle. 
				// The iterator from each for loop can be used as an index for one of the square brackets (for the element of the square)
				// Since the game board is a vector of vectors, it will have two sets of square brackets.ex: variable[x][y].
				if (m2DGameBoardDrawingPannel[row][col]) {
					// If the bool is true, SetBrush should be set to* wxLIGHT_GREY.
					//graphicContext->SetBrush(*wxLIGHT_GREY);
					graphicContext->SetBrush(mPtrGameSetting->GetColorForLivingCell());													
				}
				else
				{
					//graphicContext->SetBrush(*wxWHITE_BRUSH);
					graphicContext->SetBrush(mPtrGameSetting->GetColorForDeadCell());
				}
				// The iterator from each for loop can be used as an index for one of the square brackets
				// Since the game board is a vector of vectors, it will have two sets of square brackets. ex: variable[x][y].

				// Rectangle is drawing
				// drawing the small square inside the big rectangle/square
				graphicContext->DrawRectangle(x, y, CELL_WIDTH, CELL_HEIGHT);
						
				neighborText = wxString::Format("%d", m2DBoardNeighborCounts[row][col]);
				graphicContext->GetTextExtent(neighborText, &textWidth, &textHeight);
				textX = x + (CELL_WIDTH - textWidth) / 2;
				textY = y + (CELL_HEIGHT - textHeight) / 2;

				if (m2DBoardNeighborCounts[row][col] > 0 && mPtrGameSetting->IsShowNeighborCount) {					

					graphicContext->DrawText(neighborText, textX, textY);
				}   
			
			
			}
		}
		
		//Draw the thicker 10x10 grid lines if enabled
		if (mPtrGameSetting->mShowThickGrid) {
			dc.SetPen(wxPen(*wxBLACK,2)); // Thicker black lines

			int numThickLinesX = mPtrGameSetting->GridSize / 10;
			int numThickLinesY = mPtrGameSetting->GridSize / 10;
			
			
			// draw horizontal line
			for (int i = 1; i <= numThickLinesY; ++i) {
				wxPoint hStart(0, CELL_HEIGHT * i * 10);
				wxPoint hEnd(this->GetSize().x, CELL_HEIGHT * i * 10);
				dc.DrawLine(hStart, hEnd);
			}

			// draw vertical line
			for (int j = 1; j <= numThickLinesX; ++j) {
				wxPoint vStart(CELL_WIDTH * j * 10, 0);
				wxPoint vEnd(CELL_WIDTH * j * 10, this->GetSize().y);
				dc.DrawLine(vStart, vEnd);
			}

		}

		// Obtain a pointer to MainWindow
		MainWindow* mainWindow = dynamic_cast<MainWindow*>(GetParent());
		if (mainWindow == nullptr) {
			wxLogError("Failed to cast wxFrame to MainWindow");
			return;
		}

		// Draw the HUD if enabled
		if (mPtrGameSetting->mShowHUD) {
			graphicContext->SetFont(wxFontInfo(16), *wxRED); // Set the font size to 16 and color to red

			wxString hudText = wxString::Format("Generations: %d\nLiving Cells: %d\nBoundary: %s\nUniverse Size: %dx%d",
				mainWindow->GetGeneration(),
				mainWindow->GetLivingCells(),
				mPtrGameSetting->IsUniverseToroidal ? "Toroidal" : "Finite",
				mPtrGameSetting->GridSize, mPtrGameSetting->GridSize);

			// Get the size of the text to help position it
			wxDouble textWidth, textHeight;
			graphicContext->GetTextExtent(hudText, &textWidth, &textHeight);

			// Calculate the position for the HUD in the lower left corner
			wxSize panelSize = GetClientSize();
			wxDouble x = 10; // A small margin from the left edge
			wxDouble y = panelSize.GetHeight() - textHeight - 10; // A small margin from the bottom edge

			// Draw the text on the panel
			graphicContext->DrawText(hudText, x, y);
		}

		delete graphicContext;
	}
	else {
		wxLogError("Failed to create wxGraphicsContext on OnPaint Function");
		return;
	}

}