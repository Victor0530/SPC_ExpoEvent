#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <regex>
#include <limits>
#include <sstream>
#include <unistd.h>
#include <direct.h>

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

    string line;

    // ===== Load Attendees =====
    ifstream inFile("attendees.txt");
    if (!inFile.is_open()) {
        cout << "Could not open attendees.txt" << endl;
    } else {
        while (getline(inFile, line)) {
            stringstream ss(line);
            string id, name, email, password;

            if (getline(ss, id, ',') &&
                getline(ss, name, ',') &&
                getline(ss, email, ',') &&
                getline(ss, password)) 
            {
                credentials.push_back({email, password, "Attendee"});
            }
        }
        inFile.close();
    }

    // ===== Load Exhibitors =====
    inFile.open("exhibitors.txt");
    if (!inFile.is_open()) {
        cout << "Could not open exhibitors.txt" << endl;
    } else {
        while (getline(inFile, line)) {
            stringstream ss(line);
            string id, companyName, email, password;

            if (getline(ss, id, ',') &&
                getline(ss, companyName, ',') &&
                getline(ss, email, ',') &&
                getline(ss, password)) 
            {
                credentials.push_back({email, password, "Exhibitor"});
            }
        }
        inFile.close();
    }

    // ===== Load Admins =====
    inFile.open("admins.txt");
    if (!inFile.is_open()) {
        cout << "Could not open admins.txt" << endl;
    } else {
        while (getline(inFile, line)) {
            stringstream ss(line);
            string email, password;

            if (getline(ss, email, ',') &&
                getline(ss, password)) 
            {
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

    const string adminVerificationPassword = "secureAdmin123";
    int choice;

    while (true)
    {
        cout << "=======================================================\n";
        cout << "||                    Sign Up Menu                   ||\n";
        cout << "=======================================================\n";
        cout << "|| Register as:                                      ||\n";
        cout << "|| 1. Attendee                                       ||\n";
        cout << "|| 2. Exhibitor                                      ||\n";
        cout << "|| 3. Admin                                          ||\n";
        cout << "|| 0. Return to Main Menu                            ||\n";
        cout << "=======================================================\n";
        cout << "Choice: ";

        cin >> choice;
        cout << endl;

        if (cin.fail() || choice < 0 || choice > 3) {
            cin.clear();
            clearInputBuffer();
            cout << "=======================================================\n";
            cout << "||         Invalid Choice, please try again!          ||\n";
            cout << "=======================================================\n";
            cout << "Press Enter to continue...";
            cin.get();
        } else {
            break;
        }
        
    }

    // Verify if user has the permission to create an admin account
    if(choice == 3) {
        cout << "Enter admin verification password: ";
        string verificationPassword;
        getline(cin, verificationPassword);
        if (verificationPassword != adminVerificationPassword) {
            cout << "======================================\n";
            cout << "|| Incorrect admin password.        ||\n";
            cout << "|| Admin account creation denied.   ||\n";
            cout << "======================================\n";

            return;
        }

    }

    if(choice == 1) {
        // Attendee signup
        Attendee a;

        // id field (auto generated)
        a.id = generateNextID("attendees.txt", 'A');
        cout << "Generated Attendee ID: " << a.id << endl;

        // name field
        do {
            cout << "Enter Name: "; getline(cin, a.name);

            if(a.name.empty()) {
                cout << "\n==================================================\n";
                cout << "||    Name cannot be empty. Please try again    ||\n";
                cout << "==================================================\n\n";
            }
        } while (a.name.empty());


        // email field
        bool emailExists = false;
        while(true) {
            cout << "Enter Email: "; getline(cin, a.email);
            if (!isValidEmail(a.email)) {
                cout << "\n==============================================\n";
                cout << "||  Invalid email format. Please try again  ||\n";
                cout << "==============================================\n\n";

            } else {
                for (const auto& cred : credentials) {
                    if (cred.email == a.email) {
                        cout << "\n=======================================================\n";
                        cout << "|| Email already exists. Please use a different email. ||\n";
                        cout << "=========================================================\n\n";
                        emailExists = true;
                        break;
                    }
                }

                if(!emailExists) { break; }
            }
        }

        // password field
        do {
            cout << "Enter Password: "; getline(cin, a.password);

            if(a.password.empty()) {
                cout << "\n==================================================\n";
                cout << "||  Password cannot be empty. Please try again  ||\n";
                cout << "==================================================\n\n";
            }
        } while (a.password.empty());
        
        // save to attendees.txt
        ofstream outFile("attendees.txt", ios::app);
        outFile << a.id << "," << a.name << "," << a.email << "," << a.password << "\n";
        outFile.close();

        // save to vector
        credentials.push_back({a.email, a.password, "Attendee"});
        cout << "\nAttendee account created successfully!\n";
    
    } else if(choice == 2) {
        // Exhibitor signup
        Exhibitor e;

        // id field (auto generated)
        e.id = generateNextID("exhibitors.txt", 'E');
        cout << "Generated Exhibitor ID: " << e.id << endl;

        // company name field
        do {
            cout << "Enter Company Name: "; getline(cin, e.companyName);

            if(e.companyName.empty()) {
                cout << "\n====================================================\n";
                cout << "|| Company name cannot be empty. Please try again ||\n";
                cout << "====================================================\n\n";
            }
        } while (e.companyName.empty());
        
        // email field
        bool emailExists = false;
        while(true) {
            cout << "Enter Email: "; getline(cin, e.email);
            if (!isValidEmail(e.email)) {
                cout << "\n==============================================\n";
                cout << "||  Invalid email format. Please try again  ||\n";
                cout << "==============================================\n\n";

            } else {
                for (const auto& cred : credentials) {
                    if (cred.email == e.email) {
                        cout << "\n=======================================================\n";
                        cout << "|| Email already exists. Please use a different email. ||\n";
                        cout << "=========================================================\n\n";
                        emailExists = true;
                        break;
                    }
                }

                if(!emailExists) { break; }
            }
            
            // password field
            do {
                cout << "Enter Password: "; getline(cin, e.password);

                if(e.password.empty()) {
                cout << "\n==================================================\n";
                cout << "||  Password cannot be empty. Please try again  ||\n";
                cout << "==================================================\n\n";
                }
            } while (e.password.empty());
            
            // save to exhibitors.txt
            ofstream outFile("exhibitors.txt", ios::app);
            outFile << e.id << "," << e.companyName << "," << e.email << "," << e.password << "\n";
            outFile.close();

            // save to vector
            credentials.push_back({e.email, e.password, "Exhibitor"});
            cout << "\nExhibitor account created successfully!\n";
        }

    } else if(choice == 3) {
        // Admin signup
        Admin admin;

        // email field
        bool emailExists = false;
        while(true) {
            cout << "Enter Email: "; getline(cin, admin.email);
            for(const auto& cred : credentials) {
                if (cred.email == admin.email) {
                    cout << "\n=======================================================\n";
                    cout << "|| Email already exists. Please use a different email. ||\n";
                    cout << "=========================================================\n\n";
                    emailExists = true;
                    break;
                }
            }

            if(!emailExists) { break; }
        }

        // password field
        do {
            cout << "Enter Password: "; getline(cin, admin.password);

            if(admin.password.empty()) {
            cout << "\n==================================================\n";
            cout << "||  Password cannot be empty. Please try again  ||\n";
            cout << "==================================================\n\n";
            }
        } while (admin.password.empty());

        // save to admins.txt
        ofstream outFile("admins.txt", ios::app);
        outFile << admin.email << "," << admin.password << "\n";
        outFile.close();

        // save to vector
        credentials.push_back({admin.email, admin.password, "Admin"});
        cout << "\nAdmin account created successfully!\n";

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

    while (true) {
        loadCredentials(credentials);
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
