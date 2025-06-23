#include "MainWindow.h"
#include "play.xpm"
#include "pause.xpm"
#include "next.xpm"
#include "trash.xpm"
#include "wx/numdlg.h"

// Event table for MainWindow - this is a static event handler
wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_SIZE(MainWindow::OnSizeChange)
	EVT_MENU(10021, MainWindow::OnClickPlayButton)
	EVT_MENU(10022, MainWindow::OnClickPauseButton)
	EVT_MENU(10023, MainWindow::OnClickNextButton)
	EVT_MENU(10024, MainWindow::OnClickTrashButton)
	EVT_TIMER(30001, MainWindow::OnTimer)
	EVT_MENU(11000, MainWindow::OnSettingsDialogUI)
	EVT_MENU(10025, MainWindow::OnToggleShowNeighbors)
	EVT_MENU(10026, MainWindow::OnRandomizeMenu)
	EVT_MENU(10027, MainWindow::OnRandomizeMenuWithSeed)
	EVT_MENU(wxID_OPEN, MainWindow::OnOpenClicked)
	EVT_MENU(wxID_SAVE, MainWindow::OnSaveClicked)
	EVT_MENU(wxID_SAVEAS, MainWindow::OnSaveAsClicked)
	EVT_MENU(wxID_EXIT, MainWindow::OnExitClicked)
	EVT_MENU(10028, MainWindow::OnFiniteCheckBox)
	EVT_MENU(10029, MainWindow::OnToroidalCheckBox)
	EVT_MENU(10030, MainWindow::OnResetToDefault)
	EVT_MENU(10031, MainWindow::OnImportGameBoard)
	EVT_MENU(10032, MainWindow::OnShowGrid)
	EVT_MENU(10033, MainWindow::OnShowThickGrid)
	EVT_MENU(10034, MainWindow::OnShowHUD)
wxEND_EVENT_TABLE()

MainWindow::MainWindow() : wxFrame(nullptr, wxID_ANY, "Game of Life", wxPoint(300, 300), wxSize(600, 600)), mLivingCells(0), mGeneration(0)
{
	// Load settings from the file
	mGameSettings.Load();

	SetSize(mGameSettings.winWidth, mGameSettings.winHeight);	

	viewMenu = new wxMenu(); // Allocate the view menu

	// Now create the mShowNeighborsItem after viewMenu is initialized
	mShowNeighborsItems = new wxMenuItem(viewMenu, 10025, "Show Living Neighbors", "", wxITEM_CHECK);
	mFiniteOption = new wxMenuItem(viewMenu, 10028, "Finite", "", wxITEM_CHECK);
	mTorodialOption = new wxMenuItem(viewMenu, 10029, "Torodial", "", wxITEM_CHECK);
	wxMenuItem* ShowGridCheckBox = new wxMenuItem(viewMenu, 10032, "Show Grid", wxEmptyString, wxITEM_CHECK);
	wxMenuItem* ShowThickCheckBox = new wxMenuItem(viewMenu, 10033, "Show 10 x 10 grid", wxEmptyString, wxITEM_CHECK);
	wxMenuItem* showHUDCheckBox = new wxMenuItem(viewMenu, 10034, "Show HUD", wxEmptyString, wxITEM_CHECK);
	
	// Append the item to the View menu
	viewMenu->Append(mShowNeighborsItems);
	viewMenu->Append(mFiniteOption);
	viewMenu->Append(mTorodialOption);
	viewMenu->Append(ShowGridCheckBox);
	viewMenu->Append(ShowThickCheckBox);
	viewMenu->Append(showHUDCheckBox);

	// After the item is instantiated, it is necessary to call SetCheckable(true) on the item.
	mShowNeighborsItems->SetCheckable(true);
	mShowNeighborsItems->Check(mGameSettings.IsShowNeighborCount);		

	mFiniteOption->SetCheckable(true);
	mTorodialOption->SetCheckable(true);

	mFiniteOption->Check(mGameSettings.IsUniverseFinite);
	mTorodialOption->Check(mGameSettings.IsUniverseToroidal);
		
	ShowGridCheckBox->SetCheckable(true);
	ShowGridCheckBox->Check(mGameSettings.mShowGrid);

	ShowThickCheckBox->SetCheckable(true);
	ShowThickCheckBox->Check(mGameSettings.mShowThickGrid);

	showHUDCheckBox->Check(mGameSettings.mShowHUD);

	wxMenu* randomizeMenu = new wxMenu();
	mRandomizeMenuItem = new wxMenuItem(randomizeMenu, 10026, "Randomize (Time)", "", wxITEM_NORMAL);
	mRandomizeMenuItemWithSeed = new wxMenuItem(randomizeMenu, 10027, "Randomize (Seed)", "", wxITEM_NORMAL);
	randomizeMenu->Append(mRandomizeMenuItem);
	randomizeMenu->Append(mRandomizeMenuItemWithSeed);


	// Initialize the menu bar and view menu
	mMenuBarWX = new wxMenuBar(); // Allocate the menu bar	

	wxMenu* fileMenu = new wxMenu();
	fileMenu->Append(wxID_NEW);
	fileMenu->Append(wxID_OPEN);
	fileMenu->Append(wxID_SAVE);
	fileMenu->Append(wxID_SAVEAS);
	fileMenu->Append(10031, "Import");
	fileMenu->Append(wxID_EXIT, "Exit");
	
	// Create the Options menu and append it to the menu bar
	wxMenu* optionsMenu = new wxMenu();
	wxMenuItem* mResetSettingCheckBox = new wxMenuItem(viewMenu, 10030, "Reset to Default");

	// Append the "Settings" option to the Options menu
	optionsMenu->Append(11000, "Setting");
	optionsMenu->Append(mResetSettingCheckBox);

	// Append the menus to the menu bar
	mMenuBarWX->Append(fileMenu, "File");
	mMenuBarWX->Append(optionsMenu, "Options");	
	mMenuBarWX->Append(randomizeMenu, "Randomize");
	mMenuBarWX->Append(viewMenu, "View");

	// Set the menu bar to the main window
	SetMenuBar(mMenuBarWX);
	
	// Initialize other components...
	statusBar = CreateStatusBar(2);
	toolBar = CreateToolBar();
	timerFromWx = new wxTimer(this, 30001); // heap, needs to deallocate

	wxBitmap playBmp(play_xpm);
	wxBitmap pauseBmp(pause_xpm);
	wxBitmap nextBmp(next_xpm);
	wxBitmap trashBmp(trash_xpm);

	toolBar->AddTool(10021, "Play", playBmp);
	toolBar->AddTool(10022, "Pause", pauseBmp);
	toolBar->AddTool(10023, "Next", nextBmp);
	toolBar->AddTool(10024, "Trash", trashBmp);
	toolBar->Realize(); // render toolbar

	toolBar->Bind(wxEVT_MOTION, &MainWindow::OnMouseHoverToolBar, this);


	// Create the DrawingPanel without any border
	mDrawingPanel = new DrawingPanel(this, m2DGameBoard, m2DBoardsNeighborCounts);
	mDrawingPanel->SetSettings(&mGameSettings);

	// Initialize grid and status bar
	InitializeGrid();
	
	UpdateStatusBar();

	// Layout refresh
	this->Layout();	

}

MainWindow::~MainWindow()
{
	delete timerFromWx;
	timerFromWx = nullptr;
}

// Event Handler member function
void MainWindow::OnSizeChange(wxSizeEvent& event)
{
	// save the size of the window as a wxSize
	wxSize updateSize = event.GetSize();
	
	mGameSettings.winWidth = event.GetSize().x;
	mGameSettings.winHeight = event.GetSize().y;
	mGameSettings.Save();

	// add a check to make sure that the drawing panel is not nullptr. 
	if (mDrawingPanel != nullptr) {
		
		// not the best method here
		// when setting is changing the size of the grid we need to change the m2D_GameBoard too.
		// m2D_GameBoard.resize(mGameSetting.GridSize, std::vector<bool>(mGameSetting.GridSize)); // should I change it here ???????

		// Set the size of the drawing panel to match the window size
		mDrawingPanel->SettingTheSize(updateSize);	
				
	}

	// In order to stop other events from being processed when resizing happens, 
	// add event.Skip() as the final line in the size change method.  
	// If you forget this step, you will run into issues with your status bar.
	// Ensure other handlers are called
	event.Skip();

	}

// Event Handler member functions
void MainWindow::OnClickPlayButton(wxCommandEvent& event)
{
	//timerFromWx->Start(mTimeInterval);
	timerFromWx->Start(mGameSettings.TimeInterval);	
}

void MainWindow::OnClickPauseButton(wxCommandEvent& event)
{
	timerFromWx->Stop();
}

void MainWindow::OnClickNextButton(wxCommandEvent& event)
{
	nextGenerationOfCell();
}

void MainWindow::OnTimer(wxTimerEvent& event)
{
	// when OnTimer event function is called 
	// nextGenerationOfCell() function is called. 
	nextGenerationOfCell();
}

void MainWindow::OnSettingsDialogUI(wxCommandEvent& event)
{
	SettingDialogUI *settingsDialogUI = new SettingDialogUI(this, &mGameSettings);

	if (settingsDialogUI->ShowModal() == wxID_OK) {
		InitializeGrid();		
		mGameSettings.Save();	
		mDrawingPanel->Refresh();
	}
}

void MainWindow::OnToggleShowNeighbors(wxCommandEvent& event){	
	
	if (event.IsChecked()) {		
		//mShowNeighborsItems->Check(true);	
		mGameSettings.IsShowNeighborCount = mShowNeighborsItems->IsChecked();	
		mGameSettings.Save();	
	}
	else {
		//mShowNeighborsItems->Check(false);
		mGameSettings.IsShowNeighborCount = mShowNeighborsItems->IsChecked();
		mGameSettings.Save();
	};
	
	mDrawingPanel->Refresh();
}

void MainWindow::OnClickTrashButton(wxCommandEvent& event)
{
	// The clear button should reset the game board to all values being false.  
	// The living cell count and generation count should also be set to zero.
		

	// stop the timer from generating next generation 
	if (timerFromWx->IsRunning()) timerFromWx->Stop();

	this->mLivingCells = 0;
	this->mGeneration = 0;

	// Reset m2D_GameBoard and m2DBoardsNeighborCounts
	for (int row = 0; row < mGameSettings.GridSize; ++row) {
		for (int col = 0; col < mGameSettings.GridSize; ++col) {
			m2DGameBoard[row][col] = false;
			m2DBoardsNeighborCounts[row][col] = false; // Reset the neighbor counts
		}
	}

	UpdateStatusBar();

	// Finally, the drawing panel will need to be refreshed.
	mDrawingPanel->Refresh();
		
}

void MainWindow::InitializeGrid()
{
	// https://www2.cs.arizona.edu/~mercer/compfun3/presentations/13_VectorOfVectors.pdf
	//						row	  ,		column
	m2DGameBoard.resize(mGameSettings.GridSize, std::vector<bool>(mGameSettings.GridSize));

	// Resize mNeighborCounts to match the grid size
	m2DBoardsNeighborCounts.resize(mGameSettings.GridSize);
	for (int i = 0; i < mGameSettings.GridSize; ++i) {
		m2DGameBoard[i].resize(mGameSettings.GridSize, false);
		m2DBoardsNeighborCounts[i].resize(mGameSettings.GridSize, 0);
	}

}

// x and y are the location of the current living cell 
// this function is use to check the surrounding eight boxes arround the living cell in the middle. 
int MainWindow::CalculateNeighborCount(int x, int y)
{
	int count = 0;

	// Finite and Toroidal Universe
	for (int i = -1; i <= 1; ++i) { // row, y-coordinate
		for (int j = -1; j <= 1; ++j) { // col, x-coordinate
			// Skip the cell itself
			if (i == 0 && j == 0) continue;

			int row = y + i;
			int col = x + j;

			// https://nostarch.com/download/samples/PythonPlayground_sampleCh3.pdf
			// Toroidal 
			if (mGameSettings.IsUniverseToroidal) {
				// An index of -1 become grid - 1
				if (row < 0) row = mGameSettings.GridSize - 1;
				
				// An index of grid size will become 0 (zero)
				if (row >= mGameSettings.GridSize) row = 0;
				if (col < 0) col = mGameSettings.GridSize - 1;
				if (col >= mGameSettings.GridSize) col = 0;
			}

			// Check bounds make sure it does not go outside m2D_GameBoard grid - GRID_SIZE x GRID_SIZE
			// Double check that the index being check exists before checking the box.
			// It would be easy to call an index that is out of range with this algorithm.
			if (row >= 0 && row < mGameSettings.GridSize && col >= 0 && col < mGameSettings.GridSize) {
				// Checking if a living cell exists around the middle cell
				if (m2DGameBoard[row][col]) {
					++count; // Increment living cell count
				}
			}

		}
	}
	
	return count; // return the number of living cells


	//for (int i = -1; i <= 1; ++i) { // row, y-coordinate
	//	for (int j = -1; j <= 1; ++j) { // col, x-coordinate
	//		//skip the cell itself
	//		// Make sure not to count the box itself, only its neighbors.
	//		// the drawing explanation of this code it is located in the folder
	//		if (i == 0 && j == 0) continue;

	//		int row = y + i;
	//		int col = x + j;

	//		// Check bounds make sure it does not go outside m2D_GameBoard grid - GRID_SIZE x GRID_SIZE
	//		// Double check that the index being check exists before checking the box.
	//		// It would be easy to call an index that is out of range with this algorithm.
	//		if (row >= 0 && row < mGameSettings.GridSize && col >= 0 && col < mGameSettings.GridSize) {
	//			// checking if living cell exist arround the middle cell 
	//			if (m2DGameBoard[row][col]) {
	//				++count; // then add living cells
	//			}
	//		}

	//	}
	//}

	//return count; // return the number of living cells
}

void MainWindow::nextGenerationOfCell()
{
	mLivingCells = 0;
	// Create a variable called sandbox that is the same data type as the game board.
	std::vector<std::vector<bool>> sandbox;

	// The sandbox need to be resized to the grid size as does each individual vector
	// to match the main vector - m2D_GameBoard is the main vector. 
	sandbox.resize(mGameSettings.GridSize, std::vector<bool>(mGameSettings.GridSize));

	sandbox.resize(mGameSettings.GridSize);
	for (int i = 0; i < mGameSettings.GridSize; i++) {
		sandbox[i].resize(mGameSettings.GridSize);
	}

	for (int y = 0; y < mGameSettings.GridSize; ++y) { // y is row the same as i hat variable in matrix mathematics
		for (int x = 0; x < mGameSettings.GridSize; ++x) { // x is col the same as j hat variable in matrix mathematics

			// Living cells with less than 2 living neighbors die in the next generation.
			// I need to count the neighbor to see if there is surrounding living cell.
			int neighboringCells = CalculateNeighborCount(x, y);
			// m2DBoardsNeighborCounts[y][x] = neighboringCells;
			
			// Living cells
			// check if there is a life cell in sandbox[y][x] (return true/false) 
			if (m2DGameBoard[y][x]) {
				// Living cells with less than 2 living neighbors die in the next generation
				// Living cells with more than 3 living neighbors die in the next generation
				if (neighboringCells < 2 || neighboringCells > 3) {
					sandbox[y][x] = false; // cell dies in the next generation						
				}
				// Living cells with 2 or 3 living neighbors live in the next generation.
				else if (neighboringCells == 2 || neighboringCells == 3) {
					sandbox[y][x] = true; // cell live in the next generation	
					++mLivingCells;
				}
			}
			else { // No Living cells or No life cells
				// Dead cells with exactly 3 living neighbors live in the next generation.
				if (neighboringCells == 3) {
					sandbox[y][x] = true; // dead cell live in the next generation
					++mLivingCells;
				}
			}
		}

	}

	sandbox.swap(m2DGameBoard);

	UpdateNighborCount2DVector();
	
	// At the end of this method, make sure to change the generation count, 
	// update the status text, and refresh the drawing panel.
	++mGeneration;	 
	UpdateStatusBar();
	mDrawingPanel->Refresh();
}

void MainWindow::UpdateNighborCount2DVector()
{
	for (int y = 0; y < mGameSettings.GridSize; ++y) { // y is row the same as i hat variable in matrix mathematics
		for (int x = 0; x < mGameSettings.GridSize; ++x) { // x is col the same as j hat variable in matrix mathematics
			int neighboringCells = CalculateNeighborCount(x, y);
			m2DBoardsNeighborCounts[y][x] = neighboringCells;
		}
	}
}

void MainWindow::OnOpenClicked(wxCommandEvent& event)
{	
	wxFileDialog fileDialog(this, "Open File", "", "",
	"Game of Life File (*.cells)|*.cells", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (fileDialog.ShowModal() == wxID_CANCEL)
		return;

	// Clear the existing game board
	m2DGameBoard.clear();
	m2DGameBoard.resize(0);

	// Open the selected file
	std::ifstream fileStream;
	fileStream.open(fileDialog.GetPath().ToStdString());

	if (fileStream.is_open()) {
		std::string buffer;

		int numCols = 0;
		int numRows = 0;

		// Determine the grid size
		while (std::getline(fileStream, buffer)) {
			if (buffer.empty()) break;

			numRows++;  // Count the number of rows

			// Determine the maximum number of columns
			if (buffer.size() > numCols) {
				numCols = buffer.size();
			}
		}
		
		// initial the game board grid base on the file
		m2DGameBoard.resize(numRows, std::vector<bool>(numCols, false));
		m2DBoardsNeighborCounts.resize(numRows, std::vector<int>(numCols, false));
		
		// Resize the game board based correctly
		m2DGameBoard.resize(numRows);
		for (int i = 0; i < numCols; ++i) {
			try {
				m2DGameBoard[i].resize(numCols, false);
				m2DBoardsNeighborCounts[i].resize(numCols, 0);
			}
			catch (std::runtime_error& e) {
				wxMessageBox("vector out of range");
			}
		}

		// Update the grid size in the settings to match the file
		mGameSettings.GridSize = numRows;		
		
		// Clear any error flags and return to the beginning of the file
		fileStream.clear();
		fileStream.seekg(0);

		// Populate the game board
		int rowIndex = 0;
		while (std::getline(fileStream, buffer)) {
			if (buffer.empty()) continue;

			for (int colIndex = 0; colIndex < buffer.size(); ++colIndex) {
				if (buffer[colIndex] == '*') {
					m2DGameBoard[rowIndex][colIndex] = true;
				}
			}
			rowIndex++;
		}
	}
	else {
		wxMessageBox("Failed to open the file", "Error");
		return;
	}

	fileStream.close();

	// Update the neighbor count and refresh the display
	UpdateNighborCount2DVector();
	this->Refresh();
	event.Skip();
	


	// from school
	// wxFileDialog fileDialog(this, "Open File", "", "",
	// 	"Game of Life File (*.cells)|*.cells", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	// if (fileDialog.ShowModal() == wxID_CANCEL)
	// 	return; 

	// for (int i = 0; i < m2DGameBoard.size(); ++i) {
	// 	m2DGameBoard[i].clear();
	// 	m2DGameBoard.resize(0);
	// }
	// m2DGameBoard.clear();
	// m2DGameBoard.resize(0);

	// std::string buffer;
	// std::ifstream fileStream;
	// int index = 0;
	// fileStream.open((std::string)fileDialog.GetPath());
	// if (fileStream.is_open()) {
	// 	while (!fileStream.eof()) {
	// 		std::getline(fileStream, buffer);
			
	// 		if (buffer.size() == 0) break;
			
	// 		if (m2DGameBoard.size() == 0) {
	// 			m2DGameBoard.resize(buffer.size());
	// 			mGameSettings.GridSize = buffer.size();
	// 		}				

	// 		m2DGameBoard[index].resize(buffer.size());
	// 		for (int i = 0; i < buffer.size(); ++i) {
	// 			if (buffer[i] == '*') {
	// 				m2DGameBoard[index][i] = true;
	// 			}
	// 			else {
	// 				m2DGameBoard[index][i] = false;
	// 			}
	// 		}
	// 		++index;
	// 	}

	// 	fileStream.close();
	// }	

	// UpdateNeighborCountVector();	
	// Refresh();

	// event.Skip();
}

void MainWindow::OnSaveClicked(wxCommandEvent& event)
{
	wxFileDialog fileDialog(this, "Save Game of Life File", wxEmptyString, wxEmptyString, "Game of Life File (*.cells) | *.cells", wxFD_SAVE);

	if (fileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}

	// If we have a file name stored, use it directly
	if (!mCurrentFileName.IsEmpty()) {

		std::ofstream fileStream;
		fileStream.open(mCurrentFileName.ToStdString());

		if (fileStream.is_open()) {
			for (int i = 0; i < m2DGameBoard.size(); ++i) {
				for (int j = 0; j < m2DGameBoard[i].size(); ++j) {
					if (m2DGameBoard[i][j]) {
						fileStream << "*";
					}
					else {
						fileStream << ".";
					}
				}
				fileStream << "\n";
			}

			fileStream.close();
			wxMessageBox("File is Saved", "Save File");
			return;
		}
	}

	// If there is no file name saved, then Save will behave exactly like Save As.
	OnSaveAsClicked(event);
	event.Skip();
}

void MainWindow::OnSaveAsClicked(wxCommandEvent& event)
{
	wxFileDialog fileDialog(this, "Save Game of Life File", wxEmptyString, wxEmptyString, "Game of Life File (*.cells) | *.cells", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (fileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}

	// Save the file name in memory
	mCurrentFileName = fileDialog.GetPath();

	// Now save the file using the selected file name
	std::ofstream fileStream;
	fileStream.open(mCurrentFileName.ToStdString());
	if (fileStream.is_open()) {
		for (int i = 0; i < m2DGameBoard.size(); ++i) {
			for (int j = 0; j < m2DGameBoard[i].size(); ++j) {
				if (m2DGameBoard[i][j]) {
					fileStream << "*";
				}
				else {
					fileStream << ".";
				}
			}
			fileStream << "\n";
		}

		fileStream.close();
		wxMessageBox("File is Saved", "Save File");
	}
	event.Skip();
}

void MainWindow::OnExitClicked(wxCommandEvent& event)
{
	this->Close();
}

int MainWindow::GetGeneration() const
{
	return mGeneration;
}

int MainWindow::GetLivingCells() const
{
	return mLivingCells;
}

void MainWindow::OnRandomizeMenu(wxCommandEvent& event)
{
	srand(time(NULL));

	// accessing the row m2D_GameBoard 
	for (int i = 0; i < m2DGameBoard.size(); ++i) {
		// Going through each column in row i of vector m2D_GameBoard
		for (int j = 0; j < m2DGameBoard[i].size(); ++j) {
			int num = rand();
			if (num % 100 < 45) {
				m2DGameBoard[i][j] = true;
			}
			else
				m2DGameBoard[i][j] = false;
		}
	}

	UpdateNighborCount2DVector();
	Refresh();
}

void MainWindow::OnRandomizeMenuWithSeed(wxCommandEvent& event)
{
	//RandomizeWithSeedDialog* randomizeWithSeedDialog = new RandomizeWithSeedDialog(this);
	//randomizeWithSeedDialog->ShowModal();
		
	long seed = wxGetNumberFromUser(
		"Seed ", "Enter Seed ", "Randomize",
		0, 7436298, LONG_MAX, this);
	


	if (seed != -1)  
	{
		srand(seed);
		// accessing the row m2D_GameBoard 
		for (int i = 0; i < m2DGameBoard.size(); ++i) {
			// Going through each column in row i of vector m2D_GameBoard
			for (int j = 0; j < m2DGameBoard[i].size(); ++j) {
				int num = rand();
				if (num % 100 < 20) { // create 20% living cell
					m2DGameBoard[i][j] = true;
				}
				else
					m2DGameBoard[i][j] = false; // create 80% dead cell
			}
		}
	}

	UpdateNighborCount2DVector();
	Refresh();	
}

void MainWindow::OnMouseHoverToolBar(wxMouseEvent& event)
{
	if (event.GetEventObject() == toolBar)
		UpdateStatusBar();

	event.Skip();
}

void MainWindow::OnFiniteCheckBox(wxCommandEvent& event)
{
	if (event.IsChecked()) {
		mGameSettings.IsUniverseFinite = event.IsChecked();
		mGameSettings.IsUniverseToroidal = false;
		mTorodialOption->Check(false);

		mGameSettings.Save();
	}
	UpdateNighborCount2DVector();
	mDrawingPanel->Refresh();
}

void MainWindow::OnToroidalCheckBox(wxCommandEvent& event)
{
	if (event.IsChecked()) {
		mGameSettings.IsUniverseToroidal = event.IsChecked();
		mGameSettings.IsUniverseFinite = false;
		mFiniteOption->Check(false);

		mGameSettings.Save();
	}
	UpdateNighborCount2DVector();
	mDrawingPanel->Refresh();
}

void MainWindow::UpdateStatusBar()
{
	// This will stop the status bar text from disapring
	statusBar->SetStatusText(" ", 0);

	// wxString statusText;
	// statusBar->SetStatusText(statusText);
	// if you need to concatenate SetStatusText() function we need to convert any other data type to string
	statusBar->SetStatusText("Generation = " + std::to_string(mGeneration), 0);
	statusBar->SetStatusText("Living Cells = " + std::to_string(mLivingCells), 1);
	// statusBar->SetStatusText(wxString::Format(" Generation = %d		\t\t  Living Cells = %d", mGeneration, mLivingCells)); // c-style printf
}

void MainWindow::OnResetToDefault(wxCommandEvent& event)
{
	mGameSettings.RedDeadCell = 255;
	mGameSettings.GreenDeadCell = 255;
	mGameSettings.BlueDeadCell = 255;
	mGameSettings.AlphaDeadCell = 255;

	mGameSettings.RedLivingCell = 128;
	mGameSettings.GreenLivingCell = 128;
	mGameSettings.BlueLivingCell = 128;
	mGameSettings.AlphaLivingCell = 255;

	//mGameSettings.GridSize = 30; // the GridSize can be change/modify
	
	mGameSettings.GridSize = 30;
	mGameSettings.GridSize = 30;
	mGameSettings.TimeInterval = 50; // the TimeInterval can be change/modify

	mGameSettings.IsShowNeighborCount = true;
	mGameSettings.IsUniverseFinite = true; // The universe should currently be defined as finite.
	mGameSettings.IsUniverseToroidal = false;
	mGameSettings.mShowGrid = true;
	mGameSettings.mShowThickGrid = true;
	mGameSettings.mShowHUD = true;

	mShowNeighborsItems->Check(mGameSettings.IsShowNeighborCount);
	mFiniteOption->Check(mGameSettings.IsUniverseFinite);
	mTorodialOption->Check(mGameSettings.IsUniverseToroidal);
	
	this->SetSize(600, 600); // set the window size
	
	InitializeGrid();

	mGameSettings.Save();
	wxMessageBox("Reset To Default", "Message");
	
	mDrawingPanel->Refresh();
}

void MainWindow::OnImportGameBoard(wxCommandEvent& event)
{
	wxFileDialog fileDialog(this, "Import Game of Life Pattern", "", "",
		"Game of Life File (*.cells)|*.cells", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (fileDialog.ShowModal() == wxID_CANCEL)
		return;
	
	// Clear the existing game board
	m2DGameBoard.clear();
	m2DGameBoard.resize(0);

	std::string buffer;
	std::ifstream fileStream(fileDialog.GetPath().ToStdString());

	if (fileStream.is_open()) {

		int patternRows = 0;
		int patternCols = 0;

		//Determine the pattern size
		while (std::getline(fileStream, buffer)) {
			if (buffer.empty()) continue;

			patternRows++;
			if (buffer.size() > patternCols) {
				patternCols = buffer.size();
			}
		}

			
		// Calculate the offsets to center the pattern within the existing grid (optional)
		int rowOffset = (mGameSettings.GridSize - patternRows) / 2;
		int colOffset = (mGameSettings.GridSize - patternCols) / 2;

		// Ensure offsets are not negative (in case the pattern is larger than the grid)
		if (rowOffset < 0) rowOffset = 0;
		if (colOffset < 0) colOffset = 0;

		// Clear any error flags or if it reach nullterminator '\0'
		fileStream.clear();
		// start the pointer to the begining of the file to iterate from the start
		fileStream.seekg(0);

		// Import the pattern into the existing grid
		int rowIndex = 0;
		while (std::getline(fileStream, buffer)) {
			if (buffer.empty()) continue;

			// Using offsets so patterns can fit into the existing grid dimensions			
			if (rowOffset + rowIndex >= mGameSettings.GridSize) break;

			for (int colIndex = 0; colIndex < buffer.size(); ++colIndex) {
				if (colOffset + colIndex >= mGameSettings.GridSize) break;

				if (buffer[colIndex] == '*') {
					m2DGameBoard[rowOffset + rowIndex][colOffset + colIndex] = true;
				}
				else {
					m2DGameBoard[rowOffset + rowIndex][colOffset + colIndex] = false;  // Ensure no other cells are affected
				}
			}
			rowIndex++;
		}
	}
	else {
		wxMessageBox("Failed to open the file.", "Error", wxICON_ERROR);
		return;
	}

	fileStream.close();

	// Update the neighbor counts and refresh the display
	UpdateNighborCount2DVector();
	this->Refresh();
	event.Skip();





	// ALMOST WORKING
	//wxFileDialog fileDialog(this, "Import Pattern", "", "",
	//	"Game of Life File (*.cells)|*.cells", wxFD_OPEN);

	//if (fileDialog.ShowModal() == wxID_CANCEL)
	//	return;

	////Clear the existing game board
	//m2DGameBoard.clear();
	//m2DGameBoard.resize(0);

	//std::string buffer;
	//std::ifstream fileStream(fileDialog.GetPath().ToStdString());

	//if (fileStream.is_open()) {
	//	int patternRows = 0;
	//	int patternCols = 0;

	//	// determine the size of the pattern
	//	while (std::getline(fileStream, buffer)) {
	//		if (buffer.empty()) break;

	//		patternRows++; //count the number of row

	//		if (patternCols < buffer.size()) {
	//			patternCols = buffer.size(); // Update the column count
	//		}			
	//	}

	//	
	//	int gridRows = m2DGameBoard.size();
	//	int gridCols = m2DGameBoard[0].size(); // since each column is the same on each row, subscript zero is okay.

	//	// Calculating the starting position to center of the pattern
	//	int startRow = (gridRows - patternRows) / 2;
	//	int startCol = (gridCols - patternCols) / 2;

	//	// If the pattern is larger than the grid, adjust the start positions to 0
	//	if (startRow < 0) startRow = 0;
	//	if (startCol < 0) startCol = 0;

	//	// Clear any error flags and return to the beginning of the file
	//	fileStream.clear();
	//	fileStream.seekg(0);

	//	// Second pass: Populate the game board with the pattern
	//	int rowIndex = 0;
	//	while (std::getline(fileStream, buffer)) {
	//		if (buffer.empty()) break;

	//		int colIndex = 0;
	//		for (int i = 0; i < buffer.size(); ++i) {
	//			// Check bounds before assigning to avoid out-of-range errors
	//			if ((startRow + rowIndex) < gridRows && (startCol + colIndex) < gridCols) {
	//				if (buffer[i] == '*') {
	//					m2DGameBoard[startRow + rowIndex][startCol + colIndex] = true;
	//				}
	//				else {
	//					m2DGameBoard[startRow + rowIndex][startCol + colIndex] = false;
	//				}
	//			}
	//			colIndex++;
	//		}
	//		rowIndex++;
	//	}

	//	fileStream.close();
	//}
	//else {
	//	wxMessageBox("Failed to open the file", "Error");
	//	return;
	//}

	//// Update the neighbor count and refresh the display
	//UpdateNighborCount2DVector();
	//this->Refresh();

	//event.Skip();

}

void MainWindow::OnShowGrid(wxCommandEvent& event)
{
	mGameSettings.mShowGrid = event.IsChecked();
	mGameSettings.Save();
	mDrawingPanel->Refresh();
}

void MainWindow::OnShowThickGrid(wxCommandEvent& event)
{
	mGameSettings.mShowThickGrid = event.IsChecked();
	mGameSettings.Save();
	mDrawingPanel->Refresh();
}

void MainWindow::OnShowHUD(wxCommandEvent& event)
{
	mGameSettings.mShowHUD = !mGameSettings.mShowHUD;
	mGameSettings.Save();
	mDrawingPanel->Refresh();
}
