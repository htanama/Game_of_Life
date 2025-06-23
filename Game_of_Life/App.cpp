#include "App.h"

wxIMPLEMENT_APP(App);

App::App() : mainWindow(nullptr)
{

}

App::~App() {

}

bool App::OnInit() {
	mainWindow = new MainWindow();
	mainWindow->Show(true);
	return true;
}