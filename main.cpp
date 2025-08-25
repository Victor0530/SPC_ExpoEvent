#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <limits>

using namespace std;

struct UserCredential {
    string email;
    string password;
    string userType;
};

struct loginDetails {
    string email;
    string password;
};

struct Announcement {
    int index;
    string userType;
    string title;
    string content;
};

void clearInputBuffer();
bool isValidEmail(const string&);
void loadCredentials(vector<UserCredential>&);
void initializeUserDatabase(vector<UserCredential>&);
void signUp(vector<UserCredential>&);
bool attendeeLogin(vector<UserCredential>&);
bool exhibitorLogin(vector<UserCredential>&);
bool adminLogin(vector<UserCredential>&);
void mainLogo();
void mainMenu();
bool adminMenu();
bool adminSelection();
void AnnouncementSelection();
void loadAnnouncements(vector<Announcement>&);
void saveAnnouncements(const vector<Announcement>&);
void postAnnouncement(vector<Announcement>&);
void editAnnouncement(vector<Announcement>&);
void deleteAnnouncement(vector<Announcement>&);
void viewAnnouncement(const vector<Announcement>&, const string&);

bool adminMenu() {
    
    return adminSelection();
}

bool adminSelection() {
    vector<string> adminOption = {
        "Announcement",
        "Function 1",
        "Function 2",
        "Function 3",
        "Back to Login Screen"
    };

    while (true) {
        
        cout << "======================================\n";
        cout << "||          Admin Menu              ||\n";
        cout << "======================================\n";
        for (int i = 0; i < adminOption.size(); i++) {
            cout << "||" << setw(2) << i + 1 << ". " << left << setw(30) << adminOption[i] << "||\n";
        }
        cout << "======================================\n";
        cout << "Option ->";
        int adminInput;
        cin >> adminInput;

        if (cin.fail()) {
            cin.clear();
            clearInputBuffer();
            cout << "Invalid input. Please enter a number.\n";
            cout << "Press Enter to continue...\n";
            cin.get();
            continue;
        }

        clearInputBuffer();
        switch (adminInput) {
        case 1:
            AnnouncementSelection();
            break;
        case 2:
            
            cout << "======================================\n";
            cout << "||           Function 1             ||\n";
            cout << "======================================\n";
            cout << "|| Press Enter to continue...       ||\n";
            cout << "======================================\n";
            cin.get();
            break;
        case 3:
            
            cout << "======================================\n";
            cout << "||           Function 2             ||\n";
            cout << "======================================\n";
            cout << "|| Press Enter to continue...       ||\n";
            cout << "======================================\n";
            cin.get();
            break;
        case 4:
            
            cout << "======================================\n";
            cout << "||           Function 3             ||\n";
            cout << "======================================\n";
            cout << "|| Press Enter to continue...       ||\n";
            cout << "======================================\n";
            cin.get();
            break;
        case 5:
            
            return true;
        default:
            cout << "Invalid input entered, please try again!!!!\n";
            cout << "Press Enter to continue...\n";
            cin.get();
            break;
        }
    }
    return false;
}

bool attendeeMenu() {
    vector<string> attendeeOption = {
        "View Announcements",
        "Attendee Function 1",
        "Attendee Function 2",
        "Back to Login Screen"
    };

    vector<Announcement> announcements;
    loadAnnouncements(announcements);

    while (true) {
        
        cout << "======================================\n";
        cout << "||         Attendee Menu            ||\n";
        cout << "======================================\n";
        for (int i = 0; i < attendeeOption.size(); i++) {
            cout << "||" << setw(2) << i + 1 << ". " << left << setw(30) << attendeeOption[i] << "||\n";
        }
        cout << "======================================\n";
        cout << "   Option -> ";
        int attendeeInput;
        cin >> attendeeInput;

        if (cin.fail()) {
            cin.clear();
            clearInputBuffer();
            cout << "|| Invalid input. Please enter a number. ||\n";
            cout << "======================================\n";
            cout << "|| Press Enter to continue...        ||\n";
            cout << "======================================\n";
            cin.get();
            continue;
        }

        clearInputBuffer();
        switch (attendeeInput) {
        case 1:
            viewAnnouncement(announcements, "Attendee");
            break;
        case 2:
            
            cout << "======================================\n";
            cout << "||       Attendee Function 1         ||\n";
            cout << "======================================\n";
            cout << "|| Press Enter to continue...        ||\n";
            cout << "======================================\n";
            cin.get();
            break;
        case 3:
            
            cout << "======================================\n";
            cout << "||       Attendee Function 2         ||\n";
            cout << "======================================\n";
            cout << "|| Press Enter to continue...        ||\n";
            cout << "======================================\n";
            cin.get();
            break;
        case 4:
            
            return true;
        default:
            cout << "|| Invalid input entered, please try again!!!! ||\n";
            cout << "======================================\n";
            cout << "|| Press Enter to continue...        ||\n";
            cout << "======================================\n";
            cin.get();
            break;
        }
    }
    return false;
}

bool exhibitorMenu() {
    vector<string> exhibitorOption = {
        "View Announcements",
        "Exhibitor Function 1",
        "Exhibitor Function 2",
        "Back to Login Screen"
    };

    vector<Announcement> announcements;
    loadAnnouncements(announcements);

    while (true) {
        
        cout << "======================================\n";
        cout << "||        Exhibitor Menu            ||\n";
        cout << "======================================\n";
        for (int i = 0; i < exhibitorOption.size(); i++) {
            cout << "||" << setw(2) << i + 1 << ". " << left << setw(30) << exhibitorOption[i] << "||\n";
        }
        cout << "======================================\n";
        cout << "Option ->";
        int exhibitorInput;
        cin >> exhibitorInput;

        if (cin.fail()) {
            cin.clear();
            clearInputBuffer();
            cout << "|| Invalid input. Please enter a number. ||\n";
            cout << "======================================\n";
            cout << "|| Press Enter to continue...        ||\n";
            cout << "======================================\n";
            cin.get();
            continue;
        }

        clearInputBuffer();
        switch (exhibitorInput) {
        case 1:
            viewAnnouncement(announcements, "Exhibitor");
            break;
        case 2:
            
            cout << "======================================\n";
            cout << "||       Exhibitor Function 1        ||\n";
            cout << "======================================\n";
            cout << "|| Press Enter to continue...        ||\n";
            cout << "======================================\n";
            cin.get();
            break;
        case 3:
            
            cout << "======================================\n";
            cout << "||       Exhibitor Function 2        ||\n";
            cout << "======================================\n";
            cout << "|| Press Enter to continue...        ||\n";
            cout << "======================================\n";
            cin.get();
            break;
        case 4:
            
            return true;
        default:
            cout << "|| Invalid input entered, please try again!!!! ||\n";
            cout << "======================================\n";
            cout << "|| Press Enter to continue...        ||\n";
            cout << "======================================\n";
            cin.get();
            break;
        }
    }
    return false;
}

void AnnouncementSelection() {
    vector<string> annc = {
        "Post Announcement",
        "Edit Announcement",
        "Delete Announcement",
        "View Announcement",
        "Back"
    };
    vector<Announcement> announcements;
    loadAnnouncements(announcements);

    while (true) {
        
        cout << "======================================\n";
        cout << "||      Announcement Menu           ||\n";
        cout << "======================================\n";
        for (int i = 0; i < annc.size(); i++) {
            cout << "||" << setw(2) << i + 1 << ". " << left << setw(30) << annc[i] << "||\n";
        }
        cout << "======================================\n";
        cout << "Option ->";
        string input;
        getline(cin, input);
        int announceOption;
        try {
            announceOption = stoi(input);
        }
        catch (...) {
            cout << "Invalid input. Please enter a number.\n";
            cout << "Press Enter to continue...\n";
            cin.get();
            continue;
        }

        switch (announceOption) {
        case 1:
            postAnnouncement(announcements);
            break;
        case 2:
            editAnnouncement(announcements);
            break;
        case 3:
            deleteAnnouncement(announcements);
            break;
        case 4:
            viewAnnouncement(announcements, "Admin");
            break;
        case 5:
            
            return;
        default:
            cout << "Invalid input entered, please try again!!!!\n";
            cout << "Press Enter to continue...\n";
            cin.get();
            break;
        }
    }
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
    if (!outFile.is_open()) {
        cout << "======================================\n";
        cout << "||Error: Could not save announcements.||\n";
        cout << "======================================\n";
        return;
    }
    for (const auto& ann : announcements) {
        outFile << ann.index << "," << ann.userType << "," << ann.title << "," << ann.content << "\n";
    }
    outFile.close();
}

void postAnnouncement(vector<Announcement>& announcements) {
    
    clearInputBuffer();
    cout << "======================================\n";
    cout << "||        Post Announcement         ||\n";
    cout << "======================================\n";
    cout << "|| Select the recipient user type:  ||\n";
    cout << "|| 1. Attendee                      ||\n";
    cout << "|| 2. Exhibitor                     ||\n";
    cout << "|| 3. Both                          ||\n";
    cout << "|| 0. Back                          ||\n";
    cout << "======================================\n";
    cout << "Option ->";
    string input;
    getline(cin, input);

    if (input == "0") {
        
        return;
    }

    int userTypeOption;
    try {
        userTypeOption = stoi(input);
    }
    catch (...) {
        cout << "Invalid input. Returning to announcement menu.\n";
        cout << "Press Enter to continue...\n";
        cin.get();
        
        return;
    }

    if (userTypeOption < 1 || userTypeOption > 3) {
        cout << "Invalid option. Returning to announcement menu.\n";
        cout << "|| Press Enter to continue...        ||\n";
        cin.get();
        
        return;
    }

    Announcement newAnn;
    int maxIndex = 0;
    for (const auto& ann : announcements) {
        if (ann.index > maxIndex) maxIndex = ann.index;
    }
    newAnn.index = maxIndex + 1;

    if (userTypeOption == 1) newAnn.userType = "Attendee";
    else if (userTypeOption == 2) newAnn.userType = "Exhibitor";
    else newAnn.userType = "Both";
    
    cout << "Title:";
    getline(cin, newAnn.title);
    cout << "Content:";
    getline(cin, newAnn.content);
    cout << "\n======================================\n";

    announcements.push_back(newAnn);
    saveAnnouncements(announcements);
    cout << "||Announcement posted!!!!           ||\n";
    cout << "======================================\n";
    cout << "||Press Enter to continue...        ||\n";
    cout << "======================================\n";
    cin.get();
    
}

void viewAnnouncement(const vector<Announcement>& announcements, const string& userType) {
    
    cout << "======================================\n";
    cout << "||       View Announcements         ||\n";
    cout << "======================================\n";
    if (announcements.empty()) {
        cout << "|| No announcements available.       ||\n";
        cout << "======================================\n";
        cout << "|| Press Enter to continue...        ||\n";
        cout << "======================================\n";
        cin.get();
        
        return;
    }

    if (userType == "Admin") {
        cout << "|| Select the usertype              ||\n";
        cout << "|| 1. Attendee                      ||\n";
        cout << "|| 2. Exhibitor                     ||\n";
        cout << "|| 3. Both                          ||\n";
        cout << "|| 0. Back                          ||\n";
        cout << "======================================\n";
        cout << "Option ->";
        string input;
        getline(cin, input);
        

        if (input == "0") {
            
            return;
        }

        int userTypeOption;
        try {
            userTypeOption = stoi(input);
        }
        catch (...) {
            cout << "|| Invalid input. Returning to menu. ||\n";
            cout << "======================================\n";
            cout << "|| Press Enter to continue...        ||\n";
            cout << "======================================\n";
            cin.get();
            
            return;
        }

        if (userTypeOption < 1 || userTypeOption > 3) {
            cout << "|| Invalid option.                  ||\n";
            cout << "======================================\n";
            cout << "|| Press Enter to continue...       ||\n";
            cout << "======================================\n";
            cin.get();
            
            return;
        }

        string filterType;
        if (userTypeOption == 1) filterType = "Attendee";
        else if (userTypeOption == 2) filterType = "Exhibitor";
        else filterType = "Both";

        bool found = false;
        cout << "||Current Announcements for " << left << setw(10) << filterType << "||\n";
        cout << "======================================\n\n";
        for (const auto& ann : announcements) {
            if (ann.userType == filterType || ann.userType == "Both" || filterType == "Both") {
                cout << "Index: " << left << setw(29) << ann.index << "\n";
                cout << "User Type: " << left << setw(25) << ann.userType << "\n";
                cout << "Title: " << left << setw(29) << ann.title << "\n";
                cout << "Content: " << left << setw(27) << ann.content << "\n\n";
                cout << "======================================\n\n";
                found = true;
            }
        }
        if (!found) {
            cout << "======================================\n";
            cout << "||No announcements found            ||\n";
            cout << "======================================\n";
        }
    }
    else {
        bool found = false;
        cout << "||Current Announcements for " << left << setw(9) << userType << "||\n";
        cout << "======================================\n\n";
        for (const auto& ann : announcements) {
            if (ann.userType == userType || ann.userType == "Both") {
                cout << "Index: " << left << setw(29) << ann.index << "\n";
                cout << "User Type: " << left << setw(25) << ann.userType << "\n";
                cout << "Title: " << left << setw(29) << ann.title << "\n";
                cout << "Content: " << left << setw(27) << ann.content << "\n\n";
                cout << "======================================\n\n";
                found = true;
            }
        }
        if (!found) {
            cout << "======================================\n";
            cout << "||No announcements found            ||\n";
            cout << "======================================\n";
        }
    }
    cout << "\nPress Enter to continue...\n";
    cin.get();
    
}

void editAnnouncement(vector<Announcement>& announcements) {
    
    cout << "======================================\n";
    cout << "||        Edit Announcement         ||\n";
    cout << "======================================\n";
    if (announcements.empty()) {
        cout << "|| No announcements available.       ||\n";
        cout << "======================================\n";
        cout << "|| Press Enter to continue...        ||\n";
        cout << "======================================\n";
        cin.get();
        
        return;
    }

    cout << "|| Select the usertype:             ||\n";
    cout << "|| 1. Attendee                      ||\n";
    cout << "|| 2. Exhibitor                     ||\n";
    cout << "|| 3. Both                          ||\n";
    cout << "|| 0. Back                          ||\n";
    cout << "======================================\n";
    cout << "Option ->";
    string input;
    getline(cin, input);
    

    if (input == "0") {
        
        return;
    }

    int userTypeOption;
    try {
        userTypeOption = stoi(input);
    }
    catch (...) {
        cout << "Invalid input. Returning to announcement menu.\n";
        cout << "Press Enter to continue...\n";
        cin.get();
        
        return;
    }

    if (userTypeOption < 1 || userTypeOption > 3) {
        cout << "Invalid option. Returning to announcement menu.\n";
        cout << "Press Enter to continue...\n";
        cin.get();
        
        return;
    }

    string filterType;
    if (userTypeOption == 1) filterType = "Attendee";
    else if (userTypeOption == 2) filterType = "Exhibitor";
    else filterType = "Both";

    bool found = false;
    cout << "||Current Announcements for " << left << setw(9) << filterType << "||\n";
    cout << "======================================\n\n";
    for (const auto& ann : announcements) {
        if (ann.userType == filterType || ann.userType == "Both" || filterType == "Both") {
            cout << "Index: " << left << setw(29) << ann.index << "\n";
            cout << "User Type: " << left << setw(25) << ann.userType << "\n";
            cout << "Title: " << left << setw(29) << ann.title << "\n";
            cout << "Content: " << left << setw(27) << ann.content << "\n";
            cout << "\n======================================\n\n";
            found = true;
        }
    }
    if (!found) {
        cout << "||No announcements found            ||\n";
        cout << "======================================\n";
        cout << "|| Press Enter to continue...       ||\n";
        cout << "======================================\n";
        cin.get();
        
        return;
    }

    cout << "\nEnter the index of the announcement to edit (or 0 to go back): ";
    getline(cin, input);

    if (input == "0") {
        
        return;
    }

    int index;
    try {
        index = stoi(input);
    }
    catch (...) {
        cout << "\nInvalid input. Returning to announcement menu.\n";
        cout << "Press Enter to continue...\n";
        cin.get();
        
        return;
    }

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
            
            cout << "\nEnter new title (leave blank to keep current): ";
            string newTitle;
            getline(cin, newTitle);
            if (!newTitle.empty()) {
                ann.title = newTitle;
            }
            cout << "\nEnter new content (leave blank to keep current): ";
            string newContent;
            getline(cin, newContent);
            if (!newContent.empty()) {
                ann.content = newContent;
            }
            cout << "\n======================================\n";
            saveAnnouncements(announcements);
            cout << "||Announcement updated successfully! ||\n";
            cout << "======================================\n";
            cout << "|| Press Enter to continue...        ||\n";
            cout << "======================================\n";
            cin.get();
            
            return;
        }
    }
    cout << "|| Announcement not found           ||\n";
    cout << "======================================\n";
    cout << "|| Press Enter to continue...       ||\n";
    cout << "======================================\n";
    cin.get();
    
}

void deleteAnnouncement(vector<Announcement>& announcements) {
    
    cout << "======================================\n";
    cout << "||       Delete Announcement        ||\n";
    cout << "======================================\n";
    if (announcements.empty()) {
        cout << "|| No announcements available.      ||\n";
        cout << "======================================\n";
        cout << "|| Press Enter to continue...       ||\n";
        cout << "======================================\n";
        cin.get();
        
        return;
    }

    cout << "|| Select the usertype:             ||\n";
    cout << "|| 1. Attendee                      ||\n";
    cout << "|| 2. Exhibitor                     ||\n";
    cout << "|| 3. Both                          ||\n";
    cout << "|| 0. Back                          ||\n";
    cout << "======================================\n";
    cout << "Option ->";
    string input;
    getline(cin, input);
    

    if (input == "0") {
        
        return;
    }

    int userTypeOption;
    try {
        userTypeOption = stoi(input);
    }
    catch (...) {
        cout << "Invalid input. Returning to announcement menu.\n";
        cout << "Press Enter to continue...\n";
        cin.get();
        
        return;
    }

    if (userTypeOption < 1 || userTypeOption > 3) {
        cout << "Invalid option. Returning to announcement menu.\n";
        cout << "Press Enter to continue...\n";
        cin.get();
        
        return;
    }

    string filterType;
    if (userTypeOption == 1) filterType = "Attendee";
    else if (userTypeOption == 2) filterType = "Exhibitor";
    else filterType = "Both";

    bool found = false;
    cout << "||Current Announcements for " << left << setw(9) << filterType << "||\n";
    cout << "======================================\n\n";
    for (const auto& ann : announcements) {
        if (ann.userType == filterType || ann.userType == "Both" || filterType == "Both") {
            cout << "Index: " << left << setw(29) << ann.index << "\n";
            cout << "User Type: " << left << setw(25) << ann.userType << "\n";
            cout << "Title: " << left << setw(29) << ann.title << "\n";
            cout << "Content: " << left << setw(27) << ann.content << "\n";
            cout << "\n======================================\n\n";
            found = true;
        }
    }
    if (!found) {
        cout << "======================================\n";
        cout << "|| No announcements found           ||\n";
        cout << "======================================\n";
        cout << "|| Press Enter to continue...       ||\n";
        cout << "======================================\n";
        cin.get();
        
        return;
    }

    cout << "\nEnter the index of the announcement to delete (or 0 to go back): ";
    getline(cin, input);
    

    if (input == "0") {
        
        return;
    }

    int index;
    try {
        index = stoi(input);
    }
    catch (...) {
        cout << "Invalid input. Returning to announcement menu.\n";
        cout << "|| Press Enter to continue...        ||\n";
        cin.get();
        
        return;
    }

    for (auto it = announcements.begin(); it != announcements.end(); ++it) {
        if (it->index == index && (it->userType == filterType || it->userType == "Both" || filterType == "Both")) {
            announcements.erase(it);
            saveAnnouncements(announcements);
            cout << "======================================\n";
            cout << "||Announcement deleted successfully! ||\n";
            cout << "======================================\n";
            cout << "|| Press Enter to continue...        ||\n";
            cout << "======================================\n";
            cin.get();
            
            return;
        }
    }
    cout << "\n======================================\n";
    cout << "|| Announcement not found           ||\n";
    cout << "======================================\n";
    cout << "|| Press Enter to continue...       ||\n";
    cout << "======================================\n";
    cin.get();
    
}

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool isValidEmail(const string& email) {
    const regex pattern(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
    return regex_match(email, pattern);
}

void loadCredentials(vector<UserCredential>& credentials) {
    credentials.clear();
    vector<pair<string, string>> files = {
        {"attendees.txt", "Attendee"},
        {"exhibitors.txt", "Exhibitor"},
        {"admins.txt", "Admin"}
    };

    for (const auto& file : files) {
        ifstream inFile(file.first);
        if (!inFile.is_open()) {
            cout << "======================================\n";
            cout << "|| Warning: Could not open " << left << setw(10) << file.first << "||\n";
            cout << "======================================\n";
            continue;
        }
        string line;
        while (getline(inFile, line)) {
            size_t delimiterPos = line.find(',');
            if (delimiterPos != string::npos && !line.empty()) {
                UserCredential cred;
                cred.email = line.substr(0, delimiterPos);
                cred.password = line.substr(delimiterPos + 1);
                cred.userType = file.second;
                credentials.push_back(cred);
            }
        }
        inFile.close();
    }
}

void initializeUserDatabase(vector<UserCredential>& credentials) {
    ifstream attendeeCheck("attendees.txt");
    if (!attendeeCheck.good()) {
        ofstream attendeeFile("attendees.txt");
        attendeeFile << "lmao@gmail.com,abcdefg\n";
        attendeeFile << "yes@gmail.com,123456789\n";
        attendeeFile.close();
    }

    ifstream exhibitorCheck("exhibitors.txt");
    if (!exhibitorCheck.good()) {
        ofstream exhibitorFile("exhibitors.txt");
        exhibitorFile << "exhibitor1@gmail.com,pass123\n";
        exhibitorFile << "exhibitor2@gmail.com,pass456\n";
        exhibitorFile.close();
    }

    ifstream adminCheck("admins.txt");
    if (!adminCheck.good()) {
        ofstream adminFile("admins.txt");
        adminFile << "admin1@gmail.com,adminpass1\n";
        adminFile << "admin2@gmail.com,adminpass2\n";
        adminFile.close();
    }

    loadCredentials(credentials);
}

void signUp(vector<UserCredential>& credentials) {
    
    vector<string> newAcc = { "Attendee", "Exhibitor", "Admin", "Back to Main Menu" };
    const string adminVerificationPassword = "secureAdmin123";

    cout << "======================================\n";
    cout << "||         Sign Up Menu             ||\n";
    cout << "======================================\n";
    cout << "||Select one of the options below:  ||\n";
    for (int i = 0; i < newAcc.size(); i++) {
        cout << "||" << setw(2) << i + 1 << ". " << left << setw(30) << newAcc[i] << "||\n";
    }
    cout << "======================================\n";
    cout << "Option ->";
    int option;
    cin >> option;
    

    if (cin.fail() || option < 1 || option > 4) {
        cin.clear();
        clearInputBuffer();
        cout << "Invalid option. Returning to main menu.\n";
        cout << "Press Enter to continue...\n";
        cin.get();
        
        return;
    }

    if (option == 4) {
        clearInputBuffer();
        
        return;
    }

    clearInputBuffer();

    loginDetails newUser;

    if (option == 3) {
        cout << "Enter admin verification password: ";
        string verificationPassword;
        getline(cin, verificationPassword);
        if (verificationPassword != adminVerificationPassword) {
            cout << "======================================\n";
            cout << "|| Incorrect admin password.        ||\n";
            cout << "|| Admin account creation denied.   ||\n";
            cout << "======================================\n";
            cout << "|| Press Enter to continue...       ||\n";
            cout << "======================================\n";
            cin.get();
            
            return;
        }
    }
    cout << "======================================\n";
    cout << "Enter your email: ";
    getline(cin, newUser.email);

    if (!isValidEmail(newUser.email)) {
        cout << "\n======================================\n";
        cout << "|| Invalid email format.            ||\n";
        cout << "======================================\n";
        cout << "|| Press Enter to continue...       ||\n";
        cout << "======================================\n";
        cin.get();
        
        return;
    }

    for (const auto& cred : credentials) {
        if (cred.email == newUser.email) {
            cout << "\n=======================================================\n";
            cout << "|| Email already exists. Please use a different email. ||\n";
            cout << "=========================================================\n";
            cout << "|| Press Enter to continue...        ||\n";
            cout << "======================================\n";
            cin.get();
            
            return;
        }
    }

    cout << "Enter your password:";
    getline(cin, newUser.password);
    cout << "======================================\n";

    string fileName;
    if (option == 1) fileName = "attendees.txt";
    else if (option == 2) fileName = "exhibitors.txt";
    else if (option == 3) fileName = "admins.txt";

    ofstream outFile(fileName, ios::app);
    if (!outFile.is_open()) {
        cout << "|| Error: Could not open " << left << setw(10) << fileName << " for writing. ||\n";
        cout << "======================================\n";
        cout << "|| Press Enter to continue...        ||\n";
        cout << "======================================\n";
        cin.get();
        
        return;
    }
    outFile << newUser.email << "," << newUser.password << "\n";
    outFile.close();

    UserCredential newCred;
    newCred.email = newUser.email;
    newCred.password = newUser.password;
    newCred.userType = newAcc[option - 1];
    credentials.push_back(newCred);

    cout << "\n" << left << setw(2) << newAcc[option - 1] << " account created successfully!\n";
    cout << "Press Enter to continue...\n";
    cin.get();
    
}

bool attendeeLogin(vector<UserCredential>& credentials) {
    const int maxAttempts = 3;
    int attempts = 0;
    bool showError = false;

    while (attempts < maxAttempts) {
        
        loginDetails user;

        cout << "======================================\n";
        cout << "||         Attendee Login           ||\n";
        cout << "======================================\n";
        cout << "Attempt " << (attempts + 1) << " of " << maxAttempts << "\n";
        if (showError) {
            cout << "Invalid email format. Please enter a valid email.\n";
        }
        cout << "Enter your attendee email:";
        getline(cin, user.email);

        if (!isValidEmail(user.email)) {
            cout << "Invalid email format. Please enter a valid email. \n";
            cout << "Press Enter to continue...        \n";
            attempts++;
            showError = true;
            cin.get();
            continue;
        }

        cout << "Enter your attendee password:";
        getline(cin, user.password);

        bool loginStatus = false;
        for (const auto& cred : credentials) {
            if (cred.userType == "Attendee" && cred.email == user.email && cred.password == user.password) {
                loginStatus = true;
                break;
            }
        }

        if (loginStatus) {
            cout << "\n======================================\n";
            cout << "|| Attendee Login Success!!!        ||\n";
            cout << "======================================\n";
            cout << "|| Press Enter to continue...       ||\n";
            cout << "======================================\n";
            cin.get();
            
            return attendeeMenu();
        }
        else {
            cout << "Incorrect attendee email or password. Please try again!!!\n";
            cout << "Press Enter to try again...\n";
            cin.get();
            
            attempts++;
            showError = false;
        }
    }

    
    cout << "======================================\n";
    cout << "|| Too many failed attempts. Attendee login failed. ||\n";
    cout << "|| Do you want to register a new account? (y/Y/n/N): ";
    string response;
    getline(cin, response);

    while (response != "y" && response != "Y" && response != "n" && response != "N") {
        
        cout << "======================================\n";
        cout << "|| Invalid input. Please enter y/Y for yes or n/N for no. ||\n";
        cout << "|| Do you want to register a new account? (y/Y/n/N): ";
        getline(cin, response);
    }

    if (response == "y" || response == "Y") {
        signUp(credentials);

        
        cout << "======================================\n";
        cout << "|| Would you like to try logging in again? (y/Y/n/N): ";
        getline(cin, response);
        while (response != "y" && response != "Y" && response != "n" && response != "N") {
            
            cout << "======================================\n";
            cout << "|| Invalid input. Please enter y/Y for yes or n/N for no. ||\n";
            cout << "|| Would you like to try logging in again? (y/Y/n/N): ";
            getline(cin, response);
        }
        if (response == "y" || response == "Y") {
            return attendeeLogin(credentials);
        }
    }

    
    return false;
}

bool exhibitorLogin(vector<UserCredential>& credentials) {
    const int maxAttempts = 3;
    int attempts = 0;
    bool showError = false;

    while (attempts < maxAttempts) {
        
        loginDetails exhibitor;

        cout << "======================================\n";
        cout << "||        Exhibitor Login           ||\n";
        cout << "======================================\n";
        cout << "Attempt " << (attempts + 1) << " of " << maxAttempts << "\n";
        if (showError) {
            cout << "Invalid email format. Please enter a valid email.\n";
        }
        cout << "Enter your exhibitor email:";
        getline(cin, exhibitor.email);

        if (!isValidEmail(exhibitor.email)) {
            cout << "Invalid email format. Please enter a valid email.\n";
            cout << "Press Enter to continue...\n";
            attempts++;
            showError = true;
            cin.get();
            continue;
        }

        cout << "Enter your exhibitor password:";
        getline(cin, exhibitor.password);

        bool loginStatus = false;
        for (const auto& cred : credentials) {
            if (cred.userType == "Exhibitor" && cred.email == exhibitor.email && cred.password == exhibitor.password) {
                loginStatus = true;
                break;
            }
        }

        if (loginStatus) {
            cout << "\n======================================\n";
            cout << "|| Exhibitor Login Success!!!       ||\n";
            cout << "======================================\n";
            cout << "|| Press Enter to continue...       ||\n";
            cout << "======================================\n";
            cin.get();
            
            return exhibitorMenu();
        }
        else {
            cout << "Incorrect exhibitor email or password. Please try again!!!\n";
            cout << "Press Enter to try again...      \n";
            cin.get();
            
            attempts++;
            showError = false;
        }
    }

    
    cout << "======================================\n";
    cout << "Too many failed attempts. Exhibitor login failed.\n";
    cout << "Do you want to register a new account? (y/Y/n/N): ";
    string response;
    getline(cin, response);

    while (response != "y" && response != "Y" && response != "n" && response != "N") {
        
        cout << "======================================\n";
        cout << "Invalid input. Please enter y/Y for yes or n/N for no.\n";
        cout << "Do you want to register a new account? (y/Y/n/N): ";
        getline(cin, response);
    }

    if (response == "y" || response == "Y") {
        signUp(credentials);
        
        cout << "======================================\n";
        cout << "Would you like to try logging in again? (y/Y/n/N): ";
        getline(cin, response);
        while (response != "y" && response != "Y" && response != "n" && response != "N") {
            
            cout << "======================================\n";
            cout << "Invalid input. Please enter y/Y for yes or n/N for no.\n";
            cout << "Would you like to try logging in again? (y/Y/n/N): ";
            getline(cin, response);
        }
        if (response == "y" || response == "Y") {
            return exhibitorLogin(credentials);
        }
    }

    
    return false;
}

bool adminLogin(vector<UserCredential>& credentials) {
    const int maxAttempts = 3;
    int attempts = 0;
    bool showError = false;

    while (attempts < maxAttempts) {
        
        loginDetails admin;

        cout << "======================================\n";
        cout << "||          Admin Login             ||\n";
        cout << "======================================\n";
        cout << "Attempt " << (attempts + 1) << " of " << maxAttempts << "\n";
        if (showError) {
            cout << "Invalid email format. Please enter a valid email.\n";
        }
        cout << "Enter your admin email:";
        getline(cin, admin.email);

        if (!isValidEmail(admin.email)) {
            cout << "Invalid email format. Please enter a valid email.\n";
            cout << "Press Enter to continue...\n";
            attempts++;
            showError = true;
            cin.get();
            continue;
        }

        cout << "Enter your admin password:";
        getline(cin, admin.password);

        bool loginStatus = false;
        for (const auto& cred : credentials) {
            if (cred.userType == "Admin" && cred.email == admin.email && cred.password == admin.password) {
                loginStatus = true;
                break;
            }
        }

        if (loginStatus) {
            cout << "\n======================================\n";
            cout << "|| Admin Login Success!!!            ||\n";
            cout << "======================================\n";
            cout << "|| Press Enter to continue...        ||\n";
            cout << "======================================\n";
            cin.get();
            
            if (adminMenu()) {
                return false;
            }
            return true;
        }
        else {
            cout << "Incorrect admin email or password. Please try again!!!\n";
            cout << "Press Enter to try again...\n";
            cin.get();
            
            attempts++;
            showError = false;
        }
    }

    
    cout << "======================================\n";
    cout << "Too many failed attempts. Admin login failed.\n";
    cout << "Do you want to register a new account? (y/Y/n/N): ";
    string response;
    getline(cin, response);

    while (response != "y" && response != "Y" && response != "n" && response != "N") {
        
        cout << "======================================\n";
        cout << "Invalid input. Please enter y/Y for yes or n/N for no.\n";
        cout << "Do you want to register a new account? (y/Y/n/N): ";
        getline(cin, response);
    }

    if (response == "y" || response == "Y") {
        signUp(credentials);

        
        cout << "======================================\n";
        cout << "Would you like to try logging in again? (y/Y/n/N): ";
        getline(cin, response);
        while (response != "y" && response != "Y" && response != "n" && response != "N") {
            
            cout << "======================================\n";
            cout << "Invalid input. Please enter y/Y for yes or n/N for no.\n";
            cout << "Would you like to try logging in again? (y/Y/n/N): ";
            getline(cin, response);
        }
        if (response == "y" || response == "Y") {
            return adminLogin(credentials);
        }
    }

    
    return false;
}

void mainLogo() {
    cout << "============================================================\n";
    cout << "||                       Welcome to                        ||\n";
    cout << "||          _____                    _____                 ||\n";
    cout << "||         /\\    \\                  /\\    \\                ||\n";
    cout << "||        /::\\    \\                /::\\____\\               ||\n";
    cout << "||       /::::\\    \\              /:::/    /               ||\n";
    cout << "||      /::::::\\    \\            /:::/    /                ||\n";
    cout << "||     /:::/\\:::\\    \\          /:::/    /                 ||\n";
    cout << "||    /:::/  \\:::\\    \\        /:::/____/                  ||\n";
    cout << "||   /:::/    \\:::\\    \\       ||::|    |                  ||\n";
    cout << "||  /:::/    / \\:::\\    \\      ||::|    |     _____        ||\n";
    cout << "|| /:::/    /   \\:::\\    \\     ||::|    |    /\\    \\       ||\n";
    cout << "||/:::/____/     \\:::\\____\\    ||::|    |   /:::\\____\\     ||\n";
    cout << "||\\:::\\    \\      \\::/    /    ||::|    |  /:::/    /      ||\n";
    cout << "|| \\:::\\    \\      \\/____/     ||::|    | /:::/    /       ||\n";
    cout << "||  \\:::\\    \\                 ||::|____|/:::/    /        ||\n";
    cout << "||   \\:::\\    \\                ||:::::::::::/    /         ||\n";
    cout << "||    \\:::\\    \\               \\::::::::::/____/           ||\n";
    cout << "||     \\:::\\    \\               ~~~~~~~~~~                 ||\n";
    cout << "||      \\:::\\    \\                                         ||\n";
    cout << "||       \\:::\\____\\                                        ||\n";
    cout << "||        \\::/    /                                        ||\n";
    cout << "||         \\/____/                                         ||\n";
    cout << "||                                                         ||\n";
    cout << "============================================================\n";
}

void mainMenu() {
    vector<UserCredential> credentials;
    initializeUserDatabase(credentials);

    vector<string> menuTemplate = {
        "Attendee",
        "Exhibitor",
        "Admin",
        "New User? Sign Up Here!!!",
        "Exit"
    };

    while (true) {
        
        mainLogo();
        cout << "|| Please select one of the options below for logins:      ||\n";
        cout << "============================================================\n";
        for (int i = 0; i < menuTemplate.size(); i++) {
            cout << "||" << setw(1) << i + 1 << ". " << left << setw(46) << menuTemplate[i] << "        ||\n";
        }
        cout << "============================================================\n";
        cout << "Option -> ";
        int menuOption;
        cin >> menuOption;

        if (cin.fail()) {
            cin.clear();
            clearInputBuffer();
            cout << "Invalid input type. Please enter a number.               \n";
            cout << "Press Enter to continue...                               \n";
            cin.get();
            continue;
        }

        clearInputBuffer();
        
        switch (menuOption) {
        case 1:
            cout << "============================================================\n";
            cout << "|| You have selected " << left << setw(29) << menuTemplate[0] << " login       ||\n";
            cout << "============================================================\n\n";
            attendeeLogin(credentials);
            break;
        case 2:
            cout << "============================================================\n";
            cout << "|| You have selected " << left << setw(29) << menuTemplate[1] << " login       ||\n";
            cout << "============================================================\n\n";
            exhibitorLogin(credentials);
            break;
        case 3:
            cout << "============================================================\n";
            cout << "|| You have selected " << left << setw(29) << menuTemplate[2] << " login       ||\n";
            cout << "============================================================\n\n";
            if (!adminLogin(credentials)) {
                break;
            }
            break;
        case 4:
            signUp(credentials);
            break;
        case 5:
            cout << "============================================================\n";
            cout << "|| Thank you for using our system!!!                      ||\n";
            cout << "============================================================\n";
            return;
        default:
            cout << "Invalid option. Please select a number between 1 and 5. \n";
            cout << "Press Enter to continue...                        \n";
            cin.get();
            break;
        }
    }
}

int main() {
    mainMenu();
    return 0;
}