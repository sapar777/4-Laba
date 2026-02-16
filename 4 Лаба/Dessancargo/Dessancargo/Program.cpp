// Program.cpp
#using <System.dll>
#using <System.Windows.Forms.dll>

#include "MainForm.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace DessanCargoWinForms;

[STAThread]
int main(array<String^>^ args) {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    MainForm^ mainForm = gcnew MainForm();
    Application::Run(mainForm);

    return 0;
}