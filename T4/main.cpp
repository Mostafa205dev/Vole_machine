#include "Vole_machine.h"
#include "Vole_machine.cpp"
#include <fstream>
#include <string>

using namespace std;

vector<string> loadProgramFromFile(const string &filePath)
{
    vector<string> program;
    ifstream file(filePath);
    string instruction;

    if (!file.is_open())
    {
        cerr << "Failed to open file: " << filePath << "\n";
        return program;
    }

    while (file >> instruction)
    {
        program.push_back(instruction);
    }

    file.close();
    return program;
}

int main()
{
    Machine voleMachine;       // Initialize the Vole machine object
    vector<string> program(1); // Stores the loaded program
    bool exitFlag = false;
    int choice;
    while (!exitFlag)
    {
        cout << "\n--- Vole Machine Simulator ---" << "\n";
        cout << "1. Load Program from File" << "\n";
        cout << "2. Execute Program Step-by-Step" << "\n";
        cout << "3. Display Machine Status" << "\n";
        cout << "4. Exit" << "\n";
        cout << "Enter your choice: ";
        cin >> choice;
        if (choice == 1)
        { // Load instruction file
            string filePath;
            cout << "Enter file path: ";
            cin >> filePath;
            program = loadProgramFromFile(filePath);

            if (!program.empty())
            {
                voleMachine.loadProgram(program);
                cout << "\nProgram loaded successfully." << "\n";
                voleMachine.getPC().setValue(16);
                voleMachine.start();
            }
        }
        else if (choice == 2)
        { // Execute
            if (program.empty())
            {
                cout << "No program loaded! Please load a program first." << "\n";
            }
            else
            {
                // Step-by-step execution
                while (voleMachine.running())
                {
                    voleMachine.fetchAndExecute();
                    voleMachine.displayStatus();
                    if (voleMachine.getRunning() == false)
                    {
                        cout << "\n END OF PROGRAM\n";
                        break;
                    }

                    char stepChoice;
                    cout << "Press 'n' to continue, or 'q' to stop: ";
                    cin >> stepChoice;

                    if (stepChoice == 'q')
                    {
                        break;
                    }
                }
            }
        }
        else if (choice == 3)
        { // Display
            // Display the current status of the machine
            voleMachine.displayStatus();
        }
        else if (choice == 4)
        { // Exit
            exitFlag = true;
        }
        else
        { // Invalid choice
            cout << "Invalid choice. Please try again." << "\n";
        }
    }

    cout << "Exiting simulator. Goodbye!" << "\n";

    return 0;
}
