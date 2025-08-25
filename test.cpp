#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <regex>
#include <limits>
using namespace std;

// ==========================
// STRUCT DEFINITIONS
// ==========================
struct Attendee {
    string id;
    string name;
    string email;
    string password;
};

struct Exhibitor {
    string id;
    string companyName;
    string email;
    string password;
};

struct Admin {
    string email;
    string password;
};

struct UserCredential {
    string email;
    string password;
    string userType;
};

struct LoginDetails {
    string email;
    string password;
};

// ==========================
// HELPER FUNCTIONS
// ==========================
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool isValidEmail(const string &email) {
    const regex pattern(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
    return regex_match(email, pattern);
}

string generateNextID(const string &filename, char prefix) {
    ifstream inFile(filename);
    string line, lastID = "";
    while (getline(inFile, line)) {
        size_t pos = line.find(',');
        if (pos != string::npos) {
            lastID = line.substr(0, pos); // first field is always ID
        }
    }
    inFile.close();

    if (lastID.empty()) {
        return string(1, prefix) + "001"; // e.g., A001 if no record yet
    } else {
        int num = stoi(lastID.substr(1)); // skip prefix
        num++;
        stringstream ss;
        ss << prefix << setw(3) << setfill('0') << num;
        return ss.str();
    }
}

// ==========================
// FILE HANDLING
// ==========================
void loadCredentials(vector<UserCredential> &credentials) {
    credentials.clear();

    // Load Attendees
    ifstream inFile("attendees.txt");
    if (inFile.good()) {
        string line;
        while (getline(inFile, line)) {
            // Format: id,name,email,password
            size_t pos1 = line.find(',');
            size_t pos2 = line.find(',', pos1 + 1);
            size_t pos3 = line.find(',', pos2 + 1);

            if (pos1 != string::npos && pos2 != string::npos && pos3 != string::npos) {
                string email = line.substr(pos2 + 1, pos3 - pos2 - 1);
                string password = line.substr(pos3 + 1);
                credentials.push_back({email, password, "Attendee"});
            }
        }
        inFile.close();
    }

    // Load Exhibitors
    inFile.open("exhibitors.txt");
    if (inFile.good()) {
        string line;
        while (getline(inFile, line)) {
            // Format: id,companyName,email,password
            size_t pos1 = line.find(',');
            size_t pos2 = line.find(',', pos1 + 1);
            size_t pos3 = line.find(',', pos2 + 1);

            if (pos1 != string::npos && pos2 != string::npos && pos3 != string::npos) {
                string email = line.substr(pos2 + 1, pos3 - pos2 - 1);
                string password = line.substr(pos3 + 1);
                credentials.push_back({email, password, "Exhibitor"});
            }
        }
        inFile.close();
    }

    // Load Admins
    inFile.open("admins.txt");
    if (inFile.good()) {
        string line;
        while (getline(inFile, line)) {
            // Format: email,password
            size_t pos = line.find(',');
            if (pos != string::npos) {
                string email = line.substr(0, pos);
                string password = line.substr(pos + 1);
                credentials.push_back({email, password, "Admin"});
            }
        }
        inFile.close();
    }
}

// ==========================
// LOGIN & SIGNUP
// ==========================
bool login(vector<UserCredential> &credentials, const string &role) {
    const int maxAttempts = 3;
    int attempts = 0;

    while (attempts < maxAttempts) {
        cout << "============================================\n";
        cout << "||     " << right << setw(13) << role << " Login Page           ||\n";
        cout << "============================================\n";
        LoginDetails user;
        cout << "Enter email: ";
        getline(cin, user.email);
        cout << "Enter password: ";
        getline(cin, user.password);

        for (auto &cred : credentials) {
            if (cred.userType == role && cred.email == user.email && cred.password == user.password) {
                cout << "\n============================================\n";
                cout << "||  Login successful! Welcome, " << left << setw(10) << role << " ||\n";
                cout << "============================================\n";
                return true;
            }
        }
        cout << "\nInvalid credentials. Attempts left: " << maxAttempts - attempts - 1 << "\n";
        attempts++;
    }
    cout << "===============================================\n";
    cout << "||" << right << setw(10) << role << " login failed after 3 attempts   ||\n";
    cout << "===============================================\n";
    return false;
}

void signUp(vector<UserCredential> &credentials) {

    string choice;

    while (true)
    {
        cout << "=======================================================\n";
        cout << "||                    Sign Up Menu                   ||\n";
        cout << "=======================================================\n";
        cout << "|| Register as:                                      ||\n";
        cout << "|| 1. Attendee                                       ||\n";
        cout << "|| 2. Exhibitor                                      ||\n";
        cout << "|| 0. Return to Main Menu                            ||\n";
        cout << "=======================================================\n";
        cout << "Choice: ";

        getline(cin, choice);
        cout << endl;

        if(stoi(choice) >= 0 && stoi(choice) <= 2) {
            break;
        } else {
            cout << "=======================================================\n";
            cout << "||         Invalid Choice, please try again!          ||\n";
            cout << "=======================================================\n";
        }

    }
    

    if (choice == "1") {
        // Attendee signup
        Attendee a;
        a.id = generateNextID("attendees.txt", 'A');
        cout << "Generated Attendee ID: " << a.id << endl;
        cout << "Enter Full Name: "; getline(cin, a.name);
        cout << "Enter Email: "; getline(cin, a.email);
        if (!isValidEmail(a.email)) { cout << "Invalid email format!\n"; return; }
        cout << "Enter Password: "; getline(cin, a.password);

        ofstream outFile("attendees.txt", ios::app);
        outFile << a.id << "," << a.name << "," << a.email << "," << a.password << "\n";
        outFile.close();

        credentials.push_back({a.email, a.password, "Attendee"});
        cout << "\nAttendee account created successfully!\n";
    } 
    else if (choice == "2") {
        // Exhibitor signup
        Exhibitor e;
        e.id = generateNextID("exhibitors.txt", 'E');
        cout << "Generated Exhibitor ID: " << e.id << endl;
        cout << "Enter Company Name: "; getline(cin, e.companyName);
        cout << "Enter Email: "; getline(cin, e.email);
        if (!isValidEmail(e.email)) { cout << "Invalid email format!\n"; return; }
        cout << "Enter Password: "; getline(cin, e.password);

        ofstream outFile("exhibitors.txt", ios::app);
        outFile << e.id << "," << e.companyName << "," << e.email << "," << e.password << "\n";
        outFile.close();

        credentials.push_back({e.email, e.password, "Exhibitor"});
        cout << "\nExhibitor account created successfully!\n";
    } 
    else if (choice == "0") {
        cout << "Returning to main menu.\n";
    }
}

// ==========================
// MENUS
// ==========================
void mainLogo() {
    cout << "============================================================\n";
    cout << "||                                                        ||\n";
    cout << "||          _____                    _____                ||\n";
    cout << "||         /\\    \\                  /\\    \\               ||\n";
    cout << "||        /::\\    \\                /::\\____\\              ||\n";
    cout << "||       /::::\\    \\              /:::/    /              ||\n";
    cout << "||      /::::::\\    \\            /:::/    /               ||\n";
    cout << "||     /:::/\\:::\\    \\          /:::/    /                ||\n";
    cout << "||    /:::/  \\:::\\    \\        /:::/____/                 ||\n";
    cout << "||   /:::/    \\:::\\    \\       ||::|    |                 ||\n";
    cout << "||  /:::/    / \\:::\\    \\      ||::|    |     _____       ||\n";
    cout << "|| /:::/    /   \\:::\\    \\     ||::|    |    /\\    \\      ||\n";
    cout << "||/:::/____/     \\:::\\____\\    ||::|    |   /:::\\____\\    ||\n";
    cout << "||\\:::\\    \\      \\::/    /    ||::|    |  /:::/    /     ||\n";
    cout << "|| \\:::\\    \\      \\/____/     ||::|    | /:::/    /      ||\n";
    cout << "||  \\:::\\    \\                 ||::|____|/:::/    /       ||\n";
    cout << "||   \\:::\\    \\                ||:::::::::::/    /        ||\n";
    cout << "||    \\:::\\    \\               \\::::::::::/____/          ||\n";
    cout << "||     \\:::\\    \\               ~~~~~~~~~~                ||\n";
    cout << "||      \\:::\\    \\                                        ||\n";
    cout << "||       \\:::\\____\\                                       ||\n";
    cout << "||        \\::/    /                                       ||\n";
    cout << "||         \\/____/                                        ||\n";
    cout << "||                                                        ||\n";
    cout << "============================================================\n";
    cout << "||                       WELCOME TO                       ||\n";
    cout << "||              EXPO EVENT MANAGEMENT SYSTEM              ||\n";
    cout << "============================================================\n\n";

}

void mainMenu() {
    vector<UserCredential> credentials;
    loadCredentials(credentials);

    while (true) {
        mainLogo();
        cout << "============================================================\n";
        cout << "|| Please select one of the options below for logins:     ||\n";
        cout << "============================================================\n";
        cout << "|| 1. Attendee Login                                      ||\n";
        cout << "|| 2. Exhibitor Login                                     ||\n";
        cout << "|| 3. Admin Login                                         ||\n";
        cout << "|| 4. Sign Up                                             ||\n";
        cout << "|| 0. Exit                                                ||\n";
        cout << "============================================================\n";
        cout << "Choice: ";

        string choice; getline(cin, choice);
        cout << endl;

        if (choice == "1") login(credentials, "Attendee");
        else if (choice == "2") login(credentials, "Exhibitor");
        else if (choice == "3") login(credentials, "Admin");
        else if (choice == "4") signUp(credentials);
        else if (choice == "0") {
            cout << "============================================================\n";
            cout << "||     Exiting... Thank you for using the system!         ||\n";
            cout << "============================================================\n";
            break;
        }
        else {
            cout << "============================================================\n";
            cout << "||          Invalid Choice, please try again!             ||\n";
            cout << "============================================================\n";
        }

        cout << "\nPress Enter to continue...";
        cin.ignore();

    }
}

// ==========================
// ENTRY POINT
// ==========================
int main() {
    mainMenu();
    
    return 0;
}
