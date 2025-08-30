#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <regex>
#include <limits>
#include <sstream>
#include <set>
#include <algorithm>
#include <cctype>

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

struct Announcement {
    int index;
    string userType;
    string title;
    string content;
};

struct Feedback {
    string email;
    string eventName;
    int rating;
    string comment;
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

int getValidatedChoice(int min, int max) {
    int choice;
    while (true) {
        cout << "Choice: ";

        if (!(cin >> choice)) {
            // Input is not an integer
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between " << min << " and " << max << ".\n\n";
            continue;
        }

        if (choice < min || choice > max) {
            cout << "Invalid choice. Please enter a number between " << min << " and " << max << ".\n\n";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear leftover newline
        return choice;
    }
}

string generateNextID(const string &filename, char prefix) {
    ifstream inFile(filename);
    string line;
    int maxNum = 0;

    while (getline(inFile, line)) {
        size_t pos = line.find(',');
        if (pos != string::npos) {
            string id = line.substr(0, pos); // first field is always ID
            if (id[0] == prefix) {
                try {
                    int num = stoi(id.substr(1)); // skip prefix
                    if (num > maxNum) maxNum = num; // keep track of largest number
                } catch (...) {
                    // ignore malformed IDs
                }
            }
        }
    }
    inFile.close();

    // Generate next ID
    int nextNum = maxNum + 1;
    stringstream ss;
    ss << prefix << setw(3) << setfill('0') << nextNum;
    return ss.str();
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

Attendee findAttendee(string email) {
    Attendee foundAttendee;

    ifstream inFile("attendees.txt");

    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        Attendee a;
        getline(ss, a.id, ',');
        getline(ss, a.name, ',');
        getline(ss, a.email, ',');
        getline(ss, a.password, ',');

        if(a.email == email) {
            foundAttendee = a;
        }
    }

    return foundAttendee;
}

Exhibitor findExhibitor(string email) {
    Exhibitor foundExhibitor;

    ifstream inFile("exhibitors.txt");

    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        Exhibitor e;
        getline(ss, e.id, ',');
        getline(ss, e.companyName, ',');
        getline(ss, e.email, ',');
        getline(ss, e.password, ',');

        if(e.email == email) {
            foundExhibitor = e;
        }
    }

    return foundExhibitor;
}

Admin findAdmin(string email) {
    Admin foundAdmin;

    ifstream inFile("admins.txt");

    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        Admin ad;
        getline(ss, ad.email, ',');
        getline(ss, ad.password, ',');

        if(ad.email == email) {
            foundAdmin = ad;
        }
    }

    return foundAdmin;
}

void loadAnnouncements(vector<Announcement>& announcements) {
    announcements.clear();
    ifstream inFile("announcements.txt");
    if (!inFile.is_open()) {
        return;
    }

    string line;
    while (getline(inFile, line)) {
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        size_t pos3 = line.find(',', pos2 + 1);
        if (pos1 != string::npos && pos2 != string::npos && pos3 != string::npos) {
            Announcement ann;
            ann.index = stoi(line.substr(0, pos1));
            ann.userType = line.substr(pos1 + 1, pos2 - pos1 - 1);
            ann.title = line.substr(pos2 + 1, pos3 - pos2 - 1);
            ann.content = line.substr(pos3 + 1);
            announcements.push_back(ann);
        }
    }
    inFile.close();
}

void saveAnnouncements(const vector<Announcement>& announcements) {
    ofstream outFile("announcements.txt");
    for (const auto& a : announcements) {
        outFile << a.index << "," << a.userType << "," << a.title << "," << a.content << "\n";
    }
    outFile.close();
}

void saveFeedback(Feedback feedback) {
    ofstream file("feedback.txt", ios::app);
    if (file.is_open()) {
        file << feedback.email << ",";
        file << feedback.eventName << ",";
        file << feedback.rating << ",";
        file << feedback.comment << "\n";
        file.close();
        cout << "Feedback saved successfully." << endl;
    } else {
        cout << "Unable to open file for writing." << endl;
    }
}

vector<Feedback> loadFeedbacks() {
    vector<Feedback> feedbacks;
    ifstream file("feedback.txt");
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        Feedback fb;
        string ratingStr;

        getline(ss, fb.email, ',');
        getline(ss, fb.eventName, ',');
        getline(ss, ratingStr, ',');
        getline(ss, fb.comment);

        try {
            fb.rating = stoi(ratingStr);
        } catch (...) {
            fb.rating = 0;
        }

        if (!fb.email.empty()) {
            feedbacks.push_back(fb);
        }
    }
    return feedbacks;
}

// ==========================
// LOGIN MODULE
// ==========================
string login(vector<UserCredential> &credentials, const string &role) {
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
                return user.email;
            }
        }
        cout << "\nInvalid credentials. Attempts left: " << maxAttempts - attempts - 1 << "\n";
        attempts++;
    }
    cout << "===============================================\n";
    cout << "||" << right << setw(10) << role << " login failed after 3 attempts   ||\n";
    cout << "===============================================\n";
    return "";
}

// ==========================
// SIGN UP MODULE
// ==========================
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
        } else {
            break;
        }
        
    }
    clearInputBuffer();

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
        while(true) {
            bool emailExists = false;
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
        while(true) {
            bool emailExists = false;
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

    } else if(choice == 3) {
        // Admin signup
        Admin admin;

        // email field
        while(true) {
            bool emailExists = false;
            cout << "Enter Email: "; getline(cin, admin.email);
            if (!isValidEmail(admin.email)) {
                cout << "\n==============================================\n";
                cout << "||  Invalid email format. Please try again  ||\n";
                cout << "==============================================\n\n";

            } else {
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
// USER PROFILES
// ==========================
void viewAttendeeProfile(Attendee &a) {
    cout << "\n============================================================\n";
    cout << "||                   ATTENDEE PROFILE                     ||\n";
    cout << "============================================================\n";
    cout << "|| ID        : " << left << setw(43) << a.id << "||\n";
    cout << "|| Name      : " << left << setw(43) << a.name << "||\n";
    cout << "|| Email     : " << left << setw(43) << a.email << "||\n";
    cout << "|| Password  : " << left << setw(43) << a.password << "||\n";
    cout << "============================================================\n\n";
}

void updateAttendeeProfile(Attendee &a, vector<UserCredential> &credentials) {
    viewAttendeeProfile(a);

    cout << "===================================\n";
    cout << "||  Updating Attendee's Profile  ||\n";
    cout << "===================================\n\n";

    vector<Attendee> attendees;
    ifstream inFile("attendees.txt");
    string line;

    // Load attendees into memory
    while (getline(inFile, line)) {
        stringstream ss(line);
        Attendee a;
        getline(ss, a.id, ',');
        getline(ss, a.name, ',');
        getline(ss, a.email, ',');
        getline(ss, a.password);
        attendees.push_back(a);
    }
    inFile.close();

    for(auto &attendee : attendees) {
        if(attendee.email == a.email) {
            cout << "Enter new name (leave blank to keep current): ";
            string newName; getline(cin, newName);
            if(!newName.empty()) {
                a.name = newName;
                attendee.name = newName;
            }

            cout << "Enter new password (leave blank to keep current): ";
            string newPass; getline(cin, newPass);
            if(!newPass.empty()) {
                a.password = newPass;
                attendee.password = newPass;
            }
        }
    }

    // Update vector
    for (auto &cred : credentials) {
        if (cred.email == a.email && cred.userType == "Attendee") {
            cred.password = a.password;
        }
    }

    // Truncate the text file 
    ofstream outFile("attendees.txt", ios::trunc);
    for (auto &attendee : attendees) {
        outFile << attendee.id << "," << attendee.name << "," << attendee.email << "," << attendee.password << "\n";
    }
    outFile.close();

    cout << "\n=======================================\n";
    cout << "|| Profile updated successfully!     ||\n";
    cout << "=======================================\n";
}

void deleteAttendeeProfile(Attendee &a) {
    cout << "\n==============================================================\n";
    cout << "|| WARNING, ONCE ACCOUNT IS DELETED, ALL DATA WILL BE LOST. ||\n";
    cout << "==============================================================\n";

    string confirmation; 
    while (true) {
        cout << "\nAre you sure you want to delete your account? (Y/N): ";
        getline(cin, confirmation);

        if(confirmation == "N" || confirmation == "n") {
            cout << "Returning to dashboard.\n";
            return;
        } else if (confirmation == "Y" || confirmation == "y") {
            break;
        } else {
            cout << "Invalid input. Please try again.\n";
        }
    }

    // Rewrite text file
    ifstream inFile("attendees.txt");
    if (!inFile.is_open()) {
        cout << "Error opening attendees.txt\n";
        return;
    }

    vector<string> attendees;
    string line;

    while (getline(inFile, line)) {
        stringstream ss(line);
        string id, name, email, password;

        if (getline(ss, id, ',') &&
            getline(ss, name, ',') &&
            getline(ss, email, ',') &&
            getline(ss, password)) 
        {
            // Keep only lines that do not match the attendee's email
            if (email != a.email) {
                attendees.push_back(line);
            }
        }
    }
    inFile.close();

    ofstream outFile("attendees.txt", ios::trunc);
    for (const auto& attendee : attendees) {
        outFile << attendee << "\n";
    }
    outFile.close();

}

void viewExhibitorProfile(Exhibitor &e) {
    cout << "\n============================================================\n";
    cout << "||                  EXHIBITOR PROFILE                     ||\n";
    cout << "============================================================\n";
    cout << "|| ID          : " << left << setw(41) << e.id << "||\n";
    cout << "|| Company     : " << left << setw(41) << e.companyName << "||\n";
    cout << "|| Email       : " << left << setw(41) << e.email << "||\n";
    cout << "|| Password    : " << left << setw(41) << e.password << "||\n";
    cout << "============================================================\n\n";
}

void updateExhibitorProfile(Exhibitor &e, vector<UserCredential> &credentials) {
    viewExhibitorProfile(e);

    cout << "====================================\n";
    cout << "||  Updating Exhibitor's Profile  ||\n";
    cout << "====================================\n\n";

    vector<Exhibitor> exhibitors;
    ifstream inFile("exhibitors.txt");
    string line;

    // Load exhibitor into memory
    while (getline(inFile, line)) {
        stringstream ss(line);
        Exhibitor e;
        getline(ss, e.id, ',');
        getline(ss, e.companyName, ',');
        getline(ss, e.email, ',');
        getline(ss, e.password);
        exhibitors.push_back(e);
    }
    inFile.close();

    for(auto &exhibitor : exhibitors) {
        if(exhibitor.email == e.email) {
            cout << "Enter new company name (leave blank to keep current): ";
            string newName; getline(cin, newName);
            if(!newName.empty()) {
                e.companyName = newName;
                exhibitor.companyName= newName;
            }

            cout << "Enter new password (leave blank to keep current): ";
            string newPass; getline(cin, newPass);
            if(!newPass.empty()) {
                e.password = newPass;
                exhibitor.password = newPass;
            }
        }
    }

    // Update vector
    for (auto &cred : credentials) {
        if (cred.email == e.email && cred.userType == "Attendee") {
            cred.password = e.password;
        }
    }

    // Truncate the text file 
    ofstream outFile("exhibitors.txt", ios::trunc);
    for (auto &exhibitor : exhibitors) {
        outFile << exhibitor.id << "," << exhibitor.companyName<< "," << exhibitor.email << "," << exhibitor.password << "\n";
    }
    outFile.close();

    cout << "\n=======================================\n";
    cout << "|| Profile updated successfully!     ||\n";
    cout << "=======================================\n";
}

void deleteExhibitorProfile(Exhibitor &e) {
    cout << "\n==============================================================\n";
    cout << "|| WARNING, ONCE ACCOUNT IS DELETED, ALL DATA WILL BE LOST. ||\n";
    cout << "==============================================================\n";

    string confirmation; 
    while (true) {
        cout << "\nAre you sure you want to delete your account? (Y/N): ";
        getline(cin, confirmation);

        if(confirmation == "N" || confirmation == "n") {
            cout << "Returning to dashboard.\n";
            return;
        } else if (confirmation == "Y" || confirmation == "y") {
            break;
        } else {
            cout << "Invalid input. Please try again.\n";
        }
    }

    // Rewrite text file
    ifstream inFile("exhibitors.txt");
    if (!inFile.is_open()) {
        cout << "Error opening exhibitors.txt\n";
        return;
    }

    vector<string> exhibitors;
    string line;

    while (getline(inFile, line)) {
        stringstream ss(line);
        string id, name, email, password;

        if (getline(ss, id, ',') &&
            getline(ss, name, ',') &&
            getline(ss, email, ',') &&
            getline(ss, password)) 
        {
            // Keep only lines that do not match the attendee's email
            if (email != e.email) {
                exhibitors.push_back(line);
            }
        }
    }
    inFile.close();

    ofstream outFile("exhibitors.txt", ios::trunc);
    for (const auto& exhibitor : exhibitors) {
        outFile << exhibitor << "\n";
    }
    outFile.close();

}

void viewAdminProfile(Admin &ad) {
    cout << "\n============================================================\n";
    cout << "||                     ADMIN PROFILE                      ||\n";
    cout << "============================================================\n";
    cout << "|| Email     : " << left << setw(43) << ad.email << "||\n";
    cout << "|| Password  : " << left << setw(43) << ad.password << "||\n";
    cout << "============================================================\n\n";
}

void updateAdminProfile(Admin &ad, vector<UserCredential> &credentials) {
    viewAdminProfile(ad);

    cout << "================================\n";
    cout << "||  Updating Admin's Profile  ||\n";
    cout << "================================\n\n";

    vector<Admin> admins;
    ifstream inFile("admins.txt");
    string line;

    // Load admin into memory
    while (getline(inFile, line)) {
        stringstream ss(line);
        Admin ad;
        getline(ss, ad.email, ',');
        getline(ss, ad.password);
        admins.push_back(ad);
    }
    inFile.close();

    for(auto &admin : admins) {
        if(admin.email == ad.email) {
            cout << "Enter new password (leave blank to keep current): ";
            string newPass; getline(cin, newPass);
            if(!newPass.empty()) {
                ad.password = newPass;
                admin.password = newPass;
            }
        }
    }

    // Update vector
    for (auto &cred : credentials) {
        if (cred.email == ad.email && cred.userType == "Attendee") {
            cred.password = ad.password;
        }
    }

    // Truncate the text file 
    ofstream outFile("admins.txt", ios::trunc);
    for (auto &admin : admins) {
        outFile << admin.email << "," << admin.password << "\n";
    }
    outFile.close();

    cout << "\n=======================================\n";
    cout << "|| Profile updated successfully!     ||\n";
    cout << "=======================================\n";
}

// ==========================
// MARKETING MODULE (ANNOUNCEMENT)
// ==========================
void postAnnouncement(vector<Announcement>& announcements) {
    Announcement a;

    cout << "======================================\n";
    cout << "||        Post Announcement         ||\n";
    cout << "======================================\n";
    cout << "|| Select the recipient user type:  ||\n";
    cout << "|| 1. Attendee                      ||\n";
    cout << "|| 2. Exhibitor                     ||\n";
    cout << "|| 3. Both                          ||\n";
    cout << "|| 0. Back                          ||\n";
    cout << "======================================\n";

    int choice = getValidatedChoice(0, 3); 

    if(choice == 0) return;
    else if (choice == 1) a.userType = "Attendee";
    else if (choice == 2) a.userType = "Exhibitor";
    else a.userType = "Both";

    while(true)
    {
        cout << "Enter title (Maximum 20 Characters): ";
        getline(cin, a.title);

        if(a.title == "") {
            cout << "Title cannot be empty. Please try again.\n\n";
        } else {
            if(a.title.length() <= 20) {
                break;
            } else {
                cout << "Title has more than 20 characters. Please try again.\n\n";
            }
        }

    }

    cout << endl;
    
    while(true)
    {
        cout << "Enter content (Maximum 50 Characters): ";
        getline(cin, a.content);

        if(a.content == "") {
            cout << "Content cannot be empty. Please try again.\n\n";
        } else {
            if(a.content.length() <= 50) {
                break;
            } else {
                cout << "Content has more than 50 characters. Please try again.\n\n";
            }
        }

    }

    // Assign new index
    int maxIndex = 0;
    for (auto& an : announcements) {
        if (an.index > maxIndex) maxIndex = an.index;
    }
    a.index = maxIndex + 1;

    announcements.push_back(a);
    saveAnnouncements(announcements);

    cout << "\n======================================\n";
    cout << "|| Announcement posted!!!!          ||\n";
    cout << "======================================\n";

}

void viewAnnouncement(const vector<Announcement>& announcements, const string& userType) {
    
    cout << "======================================\n";
    cout << "||       View Announcements         ||\n";
    cout << "======================================\n";
    if (announcements.empty()) {
        cout << "|| No announcements available.       ||\n";
        cout << "======================================\n";
        
        return;
    }

    if (userType == "Admin") {
        cout << "|| Select the usertype              ||\n";
        cout << "|| 1. Attendee                      ||\n";
        cout << "|| 2. Exhibitor                     ||\n";
        cout << "|| 3. Both                          ||\n";
        cout << "|| 0. Back                          ||\n";
        cout << "======================================\n";
        int choice = getValidatedChoice(0,3);

        cout << endl;

        if (choice == 0) { return; }

        string filterType;
        if (choice == 1) filterType = "Attendee";
        else if (choice == 2) filterType = "Exhibitor";
        else filterType = "Both";

        bool found = false;
        cout << setfill('=') << setw(65) << "=" << setfill(' ') << endl;
        cout << "||               Current Announcements for " << left << setw(13) << filterType << "       ||\n";
        cout << setfill('=') << setw(65) << "=" << setfill(' ') << endl;
        for (const auto& ann : announcements) {
            if (ann.userType == filterType || ann.userType == "Both" || filterType == "Both") {
                cout << "|| Index: " << left << setw(52) << ann.index << " ||\n";
                cout << "|| User Type: " << left << setw(48) << ann.userType << " ||\n";
                cout << "|| Title: " << left << setw(52) << ann.title << " ||\n";
                cout << "|| Content: " << left << setw(50) << ann.content << " ||\n";
                cout << setfill('=') << setw(65) << "=" << setfill(' ') << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "======================================\n";
            cout << "||     No announcements found       ||\n";
            cout << "======================================\n";
        }
    }
    else {
        bool found = false;
        cout << setfill('=') << setw(65) << "=" << setfill(' ') << endl;
        cout << "||               Current Announcements for " << left << setw(13) << userType << "       ||\n";
        cout << setfill('=') << setw(65) << "=" << setfill(' ') << endl;
        for (const auto& ann : announcements) {
            if (ann.userType == userType || ann.userType == "Both") {
                cout << "|| Title: " << left << setw(52) << ann.title << " ||\n";
                cout << "|| Content: " << left << setw(50) << ann.content << " ||\n";
                cout << setfill('=') << setw(65) << "=" << setfill(' ') << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "======================================\n";
            cout << "||     No announcements found       ||\n";
            cout << "======================================\n";
        }
    }
    cout << "\nPress Enter to continue...\n";
    cin.ignore();
    
}

void editAnnouncement(vector<Announcement>& announcements) {
    
    cout << "======================================\n";
    cout << "||        Edit Announcement         ||\n";
    cout << "======================================\n";
    if (announcements.empty()) {
        cout << "|| No announcements available.       ||\n";
        cout << "======================================\n";
        
        return;
    }

    cout << "|| Select the usertype:             ||\n";
    cout << "|| 1. Attendee                      ||\n";
    cout << "|| 2. Exhibitor                     ||\n";
    cout << "|| 3. Both                          ||\n";
    cout << "|| 0. Back                          ||\n";
    cout << "======================================\n";
    int choice = getValidatedChoice(0, 3);    

    cout << endl;

    if (choice == 0) { return; }

    string filterType;
    if (choice == 1) filterType = "Attendee";
    else if (choice == 2) filterType = "Exhibitor";
    else filterType = "Both";

    bool found = false;
    cout << setfill('=') << setw(65) << "=" << setfill(' ') << endl;
    cout << "||               Current Announcements for " << left << setw(13) << filterType << "       ||\n";
    cout << setfill('=') << setw(65) << "=" << setfill(' ') << endl;
    for (const auto& ann : announcements) {
        if (ann.userType == filterType || ann.userType == "Both" || filterType == "Both") {
            cout << "|| Index: " << left << setw(52) << ann.index << " ||\n";
            cout << "|| User Type: " << left << setw(48) << ann.userType << " ||\n";
            cout << "|| Title: " << left << setw(52) << ann.title << " ||\n";
            cout << "|| Content: " << left << setw(50) << ann.content << " ||\n";
            cout << setfill('=') << setw(65) << "=" << setfill(' ') << endl;
            found = true;
        }
    }
    if (!found) {
        cout << "======================================\n";
        cout << "||     No announcements found       ||\n";
        cout << "======================================\n";
        
        return;
    }

    int index;

    while (true) {
        cout << "\nEnter the index of the announcement to edit (or 0 to go back): ";

        if (!(cin >> index)) {
            // Input is not an integer
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid index. Please enter a valid number.\n";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear leftover newline
        break;
    }

    if (index == 0) { return; }

    for (auto& ann : announcements) {
        if (ann.index == index && (ann.userType == filterType || ann.userType == "Both" || filterType == "Both")) {
            cout << "\nEditing announcement " << index << ": " << ann.title << "\n";
            cout << "Enter new user type (1. Attendee, 2. Exhibitor, 3. Both, leave blank to keep current): ";
            string newUserType;
            getline(cin, newUserType);
            if (!newUserType.empty()) {
                if (newUserType == "1") ann.userType = "Attendee";
                else if (newUserType == "2") ann.userType = "Exhibitor";
                else if (newUserType == "3") ann.userType = "Both";
            }
            
            cout << "\nEnter new title (Maximum 20 Characters, leave blank to keep current): ";
            string newTitle;
            getline(cin, newTitle);
            if (!newTitle.empty()) {
                ann.title = newTitle;
            }
            cout << "\nEnter new content (Maximum 50 Characters, leave blank to keep current): ";
            string newContent;
            getline(cin, newContent);
            if (!newContent.empty()) {
                ann.content = newContent;
            }
            saveAnnouncements(announcements);
            cout << "\n======================================\n";
            cout << "||Announcement updated successfully! ||\n";
            cout << "======================================\n";
            
            return;
        }
    }
    cout << "=====================================================\n";
    cout << "|| Announcement not found, returning to dashboard. ||\n";
    cout << "=====================================================\n";
}

void deleteAnnouncement(vector<Announcement>& announcements) {
    
    cout << "======================================\n";
    cout << "||       Delete Announcement        ||\n";
    cout << "======================================\n";
    if (announcements.empty()) {
        cout << "|| No announcements available.      ||\n";
        cout << "======================================\n";
        
        return;
    }

    cout << "|| Select the usertype:             ||\n";
    cout << "|| 1. Attendee                      ||\n";
    cout << "|| 2. Exhibitor                     ||\n";
    cout << "|| 3. Both                          ||\n";
    cout << "|| 0. Back                          ||\n";
    cout << "======================================\n";
    cout << "Option ->";
    int choice = getValidatedChoice(0, 3);    

    cout << endl;

    if (choice == 0) { return; }

    string filterType;
    if (choice == 1) filterType = "Attendee";
    else if (choice == 2) filterType = "Exhibitor";
    else filterType = "Both";

    bool found = false;
    cout << setfill('=') << setw(65) << "=" << setfill(' ') << endl;
    cout << "||               Current Announcements for " << left << setw(13) << filterType << "       ||\n";
    cout << setfill('=') << setw(65) << "=" << setfill(' ') << endl;
    for (const auto& ann : announcements) {
        if (ann.userType == filterType || ann.userType == "Both" || filterType == "Both") {
            cout << "|| Index: " << left << setw(52) << ann.index << " ||\n";
            cout << "|| User Type: " << left << setw(48) << ann.userType << " ||\n";
            cout << "|| Title: " << left << setw(52) << ann.title << " ||\n";
            cout << "|| Content: " << left << setw(50) << ann.content << " ||\n";
            cout << setfill('=') << setw(65) << "=" << setfill(' ') << endl;
            found = true;
        }
    }
    if (!found) {
        cout << "======================================\n";
        cout << "||     No announcements found       ||\n";
        cout << "======================================\n";
        
        return;
    }

    int index;

    while (true) {
        cout << "\nEnter the index of the announcement to delete (or 0 to go back): ";

        if (!(cin >> index)) {
            // Input is not an integer
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid index. Please enter a valid number.\n";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear leftover newline
        break;
    }

    if (index == 0) { return; }

    for (auto it = announcements.begin(); it != announcements.end(); ++it) {
        if (it->index == index && (it->userType == filterType || it->userType == "Both" || filterType == "Both")) {
            announcements.erase(it);
            saveAnnouncements(announcements);
            cout << "======================================\n";
            cout << "||Announcement deleted successfully! ||\n";
            cout << "======================================\n";
            
            return;
        }
    }
    cout << "=====================================================\n";
    cout << "|| Announcement not found, returning to dashboard. ||\n";
    cout << "=====================================================\n";
    
}

void adminAnnouncementSelection(vector<Announcement> &announcements) {
    cout << "======================================\n";
    cout << "||        Announcement Menu         ||\n";
    cout << "======================================\n";
    cout << "|| 1. Post Announcement             ||\n";
    cout << "|| 2. Edit Announcement             ||\n";
    cout << "|| 3. Delete Announcement           ||\n";
    cout << "|| 4. View Announcement             ||\n";
    cout << "|| 0. Back                          ||\n";
    cout << "======================================\n";

    int choice = getValidatedChoice(0, 4);

    switch (choice)
    {
        case 1: postAnnouncement(announcements); break;
        case 2: editAnnouncement(announcements); break;
        case 3: deleteAnnouncement(announcements); break;
        case 4: viewAnnouncement(announcements, "Admin"); break;
        case 0: return;
    }
}

// ==========================
// FEEDBACK MODULE
// ==========================
void submitFeedback(string email, string eventName) {
    Feedback feedback;
    feedback.email = email;
    feedback.eventName = eventName;

    while (true) {
        cout << "Enter rating (1-5): ";
        cin >> feedback.rating;
        if (cin.fail() || feedback.rating < 1 || feedback.rating > 5) {
            cout << "\n==============================================\n";
            cout << "|| Invalid rating. Please enter 1 to 5 only ||\n";
            cout << "==============================================\n\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            break;
        }
    }

    cin.ignore();
    while(true) {
        cout << "Enter your feedback: ";
        getline(cin, feedback.comment);
        if(feedback.comment == "") {
            cout << "Feedback cannot be empty, please try again.\n\n";
            continue;
        }

        break;
    }
    
    cout << "Thank you for your feedback!\n";
    saveFeedback(feedback);
}

void viewFeedbacks() {
    vector<Feedback> feedbacks = loadFeedbacks();

    if (feedbacks.empty()) {
        cout << "No feedback available." << endl;
    } else {
        cout << "\n===================================================\n";
        cout << "||         Feedback Ratings and Comments         ||\n";
        cout << "===================================================\n";
        for (size_t i = 0; i < feedbacks.size(); ++i) {
            cout << "\n================\n";
            cout << "|| Feedback " << i + 1 << " ||\n";
            cout << "================\n";
            cout << "Email: " << feedbacks[i].email << endl;
            cout << "Event: " << feedbacks[i].eventName << endl;
            cout << "Rating: " << feedbacks[i].rating << endl;
            cout << "Comment: " << feedbacks[i].comment << endl;
        }
    }

    cout << "\nPress Enter to continue...";
    cin.ignore();
}

void deleteFeedback() {
    vector<Feedback> feedbacks = loadFeedbacks();

    if (feedbacks.empty()) {
        cout << "No feedback to delete." << endl;
        return;
    }

    // Display feedbacks
    cout << "\n===================================================\n";
    cout << "||         Feedback Ratings and Comments         ||\n";
    cout << "===================================================\n";
    for (size_t i = 0; i < feedbacks.size(); ++i) {
        cout << "\n================\n";
        cout << "|| Feedback " << i + 1 << " ||\n";
        cout << "================\n";
        cout << "Email: " << feedbacks[i].email << endl;
        cout << "Event: " << feedbacks[i].eventName << endl;
        cout << "Rating: " << feedbacks[i].rating << endl;
        cout << "Comment: " << feedbacks[i].comment << endl;
    }

    // Ask user which to delete
    int choice;
    while (true) {
        cout << "\nEnter the index of the feedback to delete (0 to cancel): ";
        cin >> choice;
        if (cin.fail() || choice < 0 || choice > feedbacks.size()) {
            cout << "Invalid choice. Try again.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear leftover newline
        break;

    }

    if (choice == 0) {
        cout << "Delete cancelled.\n";
        return;
    }

    // Remove selected feedback
    feedbacks.erase(feedbacks.begin() + (choice - 1));

    // Rewrite file
    ofstream outFile("feedback.txt");
    for (const auto& fb : feedbacks) {
        outFile << fb.email << ","
                << fb.eventName << ","
                << fb.rating << ","
                << fb.comment << endl;
    }
    outFile.close();

    cout << "====================================\n";
    cout << "|| Feedback deleted successfully! ||\n";
    cout << "====================================\n\n";

}

void adminFeedbackSelection() {
    cout << "===========================================\n";
    cout << "||             Feedback Menu             ||\n";
    cout << "===========================================\n";
    cout << "|| 1. View Feedbacks                     ||\n";
    cout << "|| 2. Delete Inappropriate Feedback      ||\n";
    cout << "|| 0. Back                               ||\n";
    cout << "===========================================\n";

    int choice = getValidatedChoice(0, 2);

    switch (choice)
    {
        case 1: viewFeedbacks(); break;
        case 2: deleteFeedback(); break;
        case 0: return;
    }

}

// ==========================
// PROFILE DASHBOARD
// ==========================
void attendeeDashboard(Attendee &a, vector<Announcement> &annc, vector<UserCredential> &credentials) {
    while (true) {
        cout << "\n=====================================================\n";
        cout << "||               Attendee Dashboard                ||\n";
        cout << "=====================================================\n";
        cout << "|| 1. View Profile                                 ||\n";
        cout << "|| 2. Update Profile                               ||\n";
        cout << "|| 3. Delete Account                               ||\n";
        cout << "|| 4. View Event Announcements                     ||\n";
        cout << "|| 5. Purchase Tickets                             ||\n";
        cout << "|| 6. View Purchased Tickets                       ||\n";
        cout << "|| 7. Submit Feedbacks                             ||\n";
        cout << "|| 0. Logout                                       ||\n";
        cout << "=====================================================\n";
        cout << "Choice: ";

        string choice;
        getline(cin, choice);

        if (choice == "1") { // View Profile
            viewAttendeeProfile(a);
            cout << "Press Enter to return to dashboard...";
            cin.ignore();
        }
        else if (choice == "2") { // Update Profile
            updateAttendeeProfile(a, credentials);
            
        }
        else if (choice == "3") { // Delete Profile
            deleteAttendeeProfile(a);
            break;
            
        }
        else if (choice == "4") { // View Announcement
            viewAnnouncement(annc, "Attendee");
        }
        else if (choice == "5") {
            
        }
        else if (choice == "6") {
            
        }
        else if (choice == "7") { // Submit Feedbacks

        }
        else if (choice == "0") {
            cout << "Logging out...\n";
            break;
        } else cout << "Invalid choice.\n";
    }
}

void exhibitorDashboard(Exhibitor &e, vector<Announcement> &annc, vector<UserCredential> &credentials) {
    while (true) {
        cout << "\n=====================================================\n";
        cout << "||               Exhibitor Dashboard               ||\n";
        cout << "=====================================================\n";
        cout << "|| 1. View Profile                                 ||\n";
        cout << "|| 2. Update Profile                               ||\n";
        cout << "|| 3. Delete Account                               ||\n";
        cout << "|| 4. View Event Announcements                     ||\n";
        cout << "|| 5. Book Booths                                  ||\n";
        cout << "|| 6. Manage Booth Details                         ||\n";
        cout << "|| 7. Monitor Booth/Session Stats                  ||\n";
        cout << "|| 8. Schedule Sessions                            ||\n";
        cout << "|| 0. Logout                                       ||\n";
        cout << "=====================================================\n";
        cout << "Choice: ";

        string choice;
        getline(cin, choice);

        if (choice == "1") { // View Profile
            viewExhibitorProfile(e);
            cout << "Press Enter to return to dashboard...";
            cin.ignore();
        }
        else if (choice == "2") { // Update Profile
            updateExhibitorProfile(e, credentials);

        }
        else if (choice == "3") { // Delete Profile
            deleteExhibitorProfile(e);
            break;

        }
        else if (choice == "4") { // View Announcement
            viewAnnouncement(annc, "Exhibitor");
        }
        else if (choice == "5") {

        }
        else if (choice == "6") {

        }
        else if (choice == "7") {

        }
        else if (choice == "8") {

        }
        else if (choice == "0") {
            cout << "Logging out...\n";
            break;
        } else cout << "Invalid choice.\n";
    }
}

void adminDashboard(Admin &ad, vector<Announcement> &annc, vector<UserCredential> &credentials) {

    while (true) {
        cout << "\n=====================================================\n";
        cout << "||                 Admin Dashboard                 ||\n";
        cout << "=====================================================\n";
        cout << "|| 1. View Profile                                 ||\n"; 
        cout << "|| 2. Update Profile                               ||\n"; 
        cout << "|| 3. Manage Event Announcements                   ||\n"; 
        cout << "|| 4. Borrow Venue for Event                       ||\n";
        cout << "|| 5. Monitor Ticket/Booth/Session Activity        ||\n"; 
        cout << "|| 6. Generate Reports                             ||\n"; 
        cout << "|| 7. View Sessions                                ||\n"; 
        cout << "|| 8. Manage Feedbacks                             ||\n"; 
        cout << "|| 9. Search Users/Events                          ||\n"; 
        cout << "|| 0. Logout                                       ||\n";
        cout << "=====================================================\n";
        cout << "Choice: ";

        string choice;
        getline(cin, choice);

        if (choice == "1") { // View Profile
            viewAdminProfile(ad);
            cout << "Press Enter to return to dashboard...";
            cin.ignore();
        }
        else if (choice == "2") { // Update Profile
            updateAdminProfile(ad, credentials);
        }
        else if (choice == "3") { // Manage Announcement
            adminAnnouncementSelection(annc);
        }
        else if (choice == "4") { 

        }
        else if (choice == "5") {

        }
        else if (choice == "6") {

        }
        else if (choice == "7") {

        }
        else if (choice == "8") { // Manage Feedbacks
            adminFeedbackSelection();
        }
        else if (choice == "9") {

        }
        else if (choice == "0") {
            cout << "Logging out...\n";
            break;
        } else cout << "Invalid choice.\n";
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
    vector<Announcement> announcements;

    while (true) {
        loadCredentials(credentials);
        loadAnnouncements(announcements);

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

        if (choice == "1") {
            string attendeeEmail = login(credentials, "Attendee");
            if(attendeeEmail != "") {
                Attendee attendee = findAttendee(attendeeEmail);
                attendeeDashboard(attendee, announcements, credentials);

            }

        } else if (choice == "2")  {
            string exhibitorEmail = login(credentials, "Exhibitor");
            if(exhibitorEmail != "") {
                Exhibitor exhibitor = findExhibitor(exhibitorEmail);
                exhibitorDashboard(exhibitor, announcements, credentials);

            }

        } else if (choice == "3") {
            string adminEmail = login(credentials, "Admin");
            if(adminEmail != "") {
                Admin admin = findAdmin(adminEmail);
                adminDashboard(admin, announcements, credentials);
            }

        }else if (choice == "4") signUp(credentials);
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
    }
}

// ==========================
// ENTRY POINT
// ==========================
int main() {
    mainMenu();
    
    return 0;
}
