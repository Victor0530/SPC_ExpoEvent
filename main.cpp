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

struct TicketType {
    string type;
    double price;
    int capacity;
    int sold;
};

struct Ticket {
    string userEmail;
    string ticketID;
    string eventName;
    string ticketType;
    double amount;
};

struct BoothType {
    string boothID;
    double price;
    bool isRented;
};

struct Booth {
    string userEmail;
    string venueID;
    string boothID;
    double amount;
    bool isRented;
};

struct Venue {
    string venueID;
    string eventName;
    int rows;
    int columns;
    vector<TicketType> ticketType;
    vector<BoothType> boothType;
    bool isAvailable;
};

struct Session {
    string sessionID;
    string venueID;
    string exhibitorEmail;
    string topic;
    string timeSlot;
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

int getValidatedChoice(int min, int max, string prompt) {
    int choice;
    while (true) {
        cout << prompt;
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

string generateUserID(const string &filename, char prefix) {
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

bool isDigits(const string &s) {
    return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}

bool isValidBoothID(const Venue& venue, const string& boothID) {
    if (boothID.length() < 2 || !isDigits(boothID.substr(1))) {
        return false;
    }
    char col = toupper(boothID[0]);
    int row = 0;
    try { row = stoi(boothID.substr(1)); } catch (...) { return false; }
    if (col < 'A' || col >= 'A' + venue.columns) return false;
    if (row < 1 || row > venue.rows) return false;
    return true;
}

string generateTicketID(const vector<Ticket>& tickets) {
    int maxID = 0;
    for (const auto& t : tickets) {
        if (t.ticketID.size() > 1 && t.ticketID[0] == 'T') {
            try {
                int num = stoi(t.ticketID.substr(1));
                if (num > maxID) maxID = num;
            } catch (...) {}
        }
    }
    return "T" + to_string(maxID + 1);
}

string generateSessionID() {
    ifstream file("sessions.txt");
    int maxID = 0;

    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string id;
            getline(ss, id, ','); // sessionID is the first field

            if (!id.empty() && id[0] == 'S') {
                try {
                    int num = stoi(id.substr(1)); // remove 'S' and convert to int
                    if (num > maxID) {
                        maxID = num;
                    }
                } catch (...) {
                    // ignore malformed IDs
                }
            }
        }
        file.close();
    }

    return "S" + to_string(maxID + 1);
}

int timeToInt(const string& t) {
    int hours = stoi(t.substr(0, 2));
    int mins = stoi(t.substr(3, 2));
    return hours * 100 + mins;
}

bool isValidTimeSlot(const string& slot) {
    regex pattern(R"(^\d{2}:\d{2}-\d{2}:\d{2}$)");
    if (!regex_match(slot, pattern)) return false;

    string startStr = slot.substr(0, 5);
    string endStr   = slot.substr(6, 5);

    int sh = stoi(startStr.substr(0, 2));
    int sm = stoi(startStr.substr(3, 2));
    int eh = stoi(endStr.substr(0, 2));
    int em = stoi(endStr.substr(3, 2));

    // Hours/minutes range check
    if (sh < 0 || sh > 23 || eh < 0 || eh > 23) return false;
    if (sm < 0 || sm > 59 || em < 0 || em > 59) return false;

    // Start must be before end
    return timeToInt(startStr) < timeToInt(endStr);
}

bool isOverlap(const string& slot1, const string& slot2) {
    // Expect format "HH:MM-HH:MM"
    int start1 = timeToInt(slot1.substr(0, 5));
    int end1   = timeToInt(slot1.substr(6, 5));

    int start2 = timeToInt(slot2.substr(0, 5));
    int end2   = timeToInt(slot2.substr(6, 5));

    return (start1 < end2 && start2 < end1);
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

void saveVenues(const vector<Venue>& venues) {
    ofstream file("venue.txt");
    for (const auto& v : venues) {
        file << v.venueID << "," 
             << v.eventName << "," 
             << v.rows << "," 
             << v.columns << "," 
             << v.ticketType.size() << "," 
             << v.boothType.size() << "," 
             << (v.isAvailable ? "1" : "0");

        // Save each ticket type on the same line
        for (const auto& t : v.ticketType) {
            file << "," << t.type 
                 << "," << t.price 
                 << "," << t.capacity 
                 << "," << t.sold;
        }
        for (const auto& b : v.boothType) {
            file << "," << b.boothID 
                 << "," << b.price 
                 << "," << (b.isRented ? "1" : "0");
        }
        file << "\n";
    }
    file.close();
}

void loadVenues(vector<Venue>& venues) {
    venues.clear();
    ifstream file("venue.txt");
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        Venue v;
        string rows, cols, avail, ticketCountStr, boothCountStr;
        getline(ss, v.venueID, ',');
        getline(ss, v.eventName, ',');
        getline(ss, rows, ',');
        getline(ss, cols, ',');
        getline(ss, ticketCountStr, ',');
        getline(ss, boothCountStr, ',');
        getline(ss, avail, ',');

        v.rows = stoi(rows);
        v.columns = stoi(cols);
        v.isAvailable = (avail == "1");

        int ticketCount = stoi(ticketCountStr);
        v.ticketType.clear();
        for (int i = 0; i < ticketCount; ++i) {
            TicketType t;
            string priceStr, capacityStr, soldStr;

            if (!getline(ss, t.type, ',')) break;
            if (!getline(ss, priceStr, ',')) break;
            if (!getline(ss, capacityStr, ',')) break;
            if (!getline(ss, soldStr, ',')) break;

            t.price = stod(priceStr);
            t.capacity = stoi(capacityStr);
            t.sold = stoi(soldStr);

            v.ticketType.push_back(t);
        }

        int boothCount = stoi(boothCountStr);
        v.boothType.clear();
        for (int i = 0; i < boothCount; ++i) {
            BoothType b;
            string priceStr, rentedStr;

            if (!getline(ss, b.boothID, ',')) break;
            if (!getline(ss, priceStr, ',')) break;
            if (!getline(ss, rentedStr, ',')) break;
            
            b.price = stod(priceStr);
            b.isRented = (rentedStr == "1");
            v.boothType.push_back(b);
        }
        venues.push_back(v);
    }
    file.close();
}

void saveTicket(const Ticket& ticket) {
    ofstream file("ticket.txt", ios::app);
    file << ticket.userEmail << ","
         << ticket.ticketID << ","
         << ticket.eventName << ","
         << ticket.ticketType << ","
         << ticket.amount << endl;
    file.close();
}

void loadTickets(vector<Ticket>& tickets) {
    tickets.clear();
    ifstream file("ticket.txt");
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        Ticket t;
        string amountStr;

        getline(ss, t.userEmail, ',');
        getline(ss, t.ticketID, ',');
        getline(ss, t.eventName, ',');
        getline(ss, t.ticketType, ',');
        getline(ss, amountStr);

        try { 
            t.amount = stod(amountStr);
        } catch (...) { 
            t.amount = 0.0; 
        }
        tickets.push_back(t);
    }
    file.close();
}

void saveBooth(const Booth& booth) {
    ofstream file("booth.txt", ios::app);
    file << booth.userEmail << "," 
         << booth.venueID << "," 
         << booth.boothID << "," 
         << (booth.isRented ? "1" : "0") << "," 
         << booth.amount << endl;
    file.close();
}

void loadBooths(vector<Booth>& booths) {
    booths.clear();
    ifstream file("booth.txt");
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        Booth b;
        string rentedStr, amountStr;

        getline(ss, b.userEmail, ',');
        getline(ss, b.venueID, ',');
        getline(ss, b.boothID, ',');
        getline(ss, rentedStr, ',');
        getline(ss, amountStr);

        try{
            b.amount = stod(amountStr);
        }catch(...){
            b.amount = 0.0;
        }

        b.isRented = (rentedStr == "1");
        booths.push_back(b);
    }
    file.close();
}

void saveSessions(const vector<Session>& sessions) {
    ofstream file("sessions.txt");
    if (!file.is_open()) {
        cout << "Error opening sessions.txt for writing.\n";
        return;
    }
    for (const auto& s : sessions) {
        file << s.sessionID << ","
             << s.venueID << ","
             << s.exhibitorEmail << ","
             << s.topic << ","
             << s.timeSlot << endl;
    }
    file.close();
}

void loadSessions(vector<Session>& sessions) {
    ifstream file("sessions.txt");
    string line;

    if (!file) {
        cout << "No sessions found yet.\n";
        return;
    }

    while (getline(file, line)) {
        stringstream ss(line);
        Session s;
        getline(ss, s.sessionID, ',');
        getline(ss, s.venueID, ',');
        getline(ss, s.exhibitorEmail, ',');
        getline(ss, s.topic, ',');
        getline(ss, s.timeSlot, ',');

        if (!s.sessionID.empty()) { // safety check
            sessions.push_back(s);
        }
    }
    file.close();
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
        a.id = generateUserID("attendees.txt", 'A');
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
        e.id = generateUserID("exhibitors.txt", 'E');
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
    
    int choice = getValidatedChoice(0, 3, "Choice: "); 

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
        int choice = getValidatedChoice(0,3, "Choice: ");

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
    int choice = getValidatedChoice(0, 3, "Choice: ");    

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
    int choice = getValidatedChoice(0, 3, "Choice: ");    

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

    int choice = getValidatedChoice(0, 4, "Choice: ");

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
void submitFeedback(const string& email, vector<Ticket> &tickets) {
        // Only allow feedback for attended events
        vector<string> attendedEvents;
        for (const auto& t : tickets) {
            if (t.userEmail == email) {
                if (find(attendedEvents.begin(), attendedEvents.end(), t.eventName) == attendedEvents.end()) {
                    attendedEvents.push_back(t.eventName);
                }
            }
        }
        
        int eventChoice;

        if (attendedEvents.empty()) {
            cout << "You have not attended any events yet. Cannot submit feedback.\n";
            return;

        } else {
            cout << "Events you attended:\n";
            for (size_t i = 0; i < attendedEvents.size(); ++i) {
                cout << i + 1 << ". " << attendedEvents[i] << endl;
            }

            eventChoice = getValidatedChoice(0, attendedEvents.size(), "Select event to submit feedback for (0 to cancel): ");

            if (eventChoice == 0) {
                cout << "Cancelled.\n";
                return;
            }
        }

    Feedback feedback;
    feedback.email = email;
    feedback.eventName = attendedEvents[eventChoice - 1];

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
    choice = getValidatedChoice(0, feedbacks.size(), "Enter which feedback you want to delete (0 to cancel): ");

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

    int choice = getValidatedChoice(0, 2, "Choice: ");

    switch (choice)
    {
        case 1: viewFeedbacks(); break;
        case 2: deleteFeedback(); break;
        case 0: return;
    }

}

// ==========================
// TICKET MODULE
// ==========================
void purchaseTicket(vector<Venue>& venues, vector<Ticket>& tickets, const string& email) {
    cout << "Available Events:\n";
    for (size_t i = 0; i < venues.size(); ++i) {
        if (!venues[i].isAvailable && !venues[i].eventName.empty())
            cout << i+1 << ". " << venues[i].eventName << " (Venue " << venues[i].venueID << ")\n";
    }

    int choice = getValidatedChoice(0, venues.size(), "Select event (0 to cancel): ");

    if(choice == 0) {
        cout << "Purchasing ticket cancelled.\n";
        return;
    }

    Venue& v = venues[choice-1];
    if(v.isAvailable) return;
    cout << "Ticket Types:\n";
    for (size_t i = 0; i < v.ticketType.size(); ++i) {
        cout << i+1 << ". " << v.ticketType[i].type << " (RM" << v.ticketType[i].price << ", Remaining: " << (v.ticketType[i].capacity - v.ticketType[i].sold) << ")\n";
    }

    int ttype = getValidatedChoice(0, v.ticketType.size(), "Select ticket type (0 to cancel): ");
    if(ttype == 0) {
        cout << "Purchasing ticket cancelled.\n";
        return;
    }
    
    int qty = getValidatedChoice(1, (v.ticketType[ttype-1].capacity - v.ticketType[ttype-1].sold), "Quantity: ");
    for (int i = 0; i < qty; ++i) {
        Ticket t;
        t.userEmail = email;
        t.eventName = v.eventName;
        t.ticketType = v.ticketType[ttype-1].type;
        t.amount = v.ticketType[ttype-1].price;
        t.ticketID = generateTicketID(tickets);
        tickets.push_back(t);
        saveTicket(t);
    }

    v.ticketType[ttype-1].sold += qty;
    saveVenues(venues);
    cout << "Tickets purchased successfully!\n";
}

void viewPurchasedTickets(const vector<Ticket>& tickets, const string& email) {
    cout << "Your Tickets:\n";
    bool found = false;
    for (const auto& t : tickets) {
        if (t.userEmail == email) {
            cout << "Ticket ID: " << t.ticketID << ", Event: " << t.eventName << ", Type: " << t.ticketType << ", Amount: RM" << t.amount << endl;
            found = true;
        }
    }
    if (!found) cout << "No tickets found.\n";
    cout << "Press Enter to continue...";
    cin.ignore();
}

void refundTicket(const string& email) {
    vector<Ticket> tickets;
    loadTickets(tickets);

    // Show user's tickets
    vector<Ticket> userTickets;
    for (const auto& t : tickets) {
        if (t.userEmail == email) userTickets.push_back(t);
    }
    if (userTickets.empty()) {
        cout << "You have no tickets to refund.\n";
        return;
    }
    cout << "Your tickets:\n";
    for (size_t i = 0; i < userTickets.size(); ++i) {
        cout << i+1 << ". Ticket ID: " << userTickets[i].ticketID
             << ", Event: " << userTickets[i].eventName
             << ", Type: " << userTickets[i].ticketType
             << ", Amount: RM" << fixed << setprecision(2) << userTickets[i].amount << endl;
    }
    
    int choice = getValidatedChoice(0, (int)userTickets.size(), "Enter which ticket you to refund (0 to cancel): ");
    if (choice == 0) {
        cout << "Refund cancelled.\n";
        return;
    }

    string ticketID = userTickets[choice-1].ticketID;
    string eventName = userTickets[choice-1].eventName;
    string ticketType = userTickets[choice-1].ticketType;
    double amount = userTickets[choice-1].amount;

    // Remove ticket from tickets vector
    vector<Ticket> updatedTickets;
    bool found = false;
    for (const auto& t : tickets) {
        if (t.ticketID == ticketID && t.userEmail == email) {
            found = true;
            // Write refund log
            ofstream refundFile("Ticketrefunds.txt", ios::app);
            refundFile << t.userEmail << "," << t.ticketID << "," << t.eventName << "," << t.ticketType << "," << t.amount << ",REFUNDED\n";
            refundFile.close();
            cout << "Refund amount: RM" << fixed << setprecision(2) << t.amount << endl;
        } else {
            updatedTickets.push_back(t);
        }
    }
    if (!found) {
        cout << "Ticket not found or not owned by user.\n";
        return;
    }
    // Save updated tickets
    ofstream outFile("ticket.txt");
    for (const auto& t : updatedTickets) {
        outFile << t.userEmail << "," << t.ticketID << "," << t.eventName << "," << t.ticketType << "," << t.amount << endl;
    }
    outFile.close();

    // Update venue ticketType sold count
    vector<Venue> venues;
    loadVenues(venues);
    for (auto& v : venues) {
        if (v.eventName == eventName) {
            for (auto& tt : v.ticketType) {
                if (tt.type == ticketType && tt.sold > 0) {
                    tt.sold -= 1;
                }
            }
            break;
        }
    }
    saveVenues(venues);
}

void attendeeTicketSelection(const string& email) {
    cout << "===========================================\n";
    cout << "||             Ticket Menu               ||\n";
    cout << "===========================================\n";
    cout << "|| 1. View Purchased Tickets             ||\n";
    cout << "|| 2. Refund Ticket                      ||\n";
    cout << "|| 0. Back                               ||\n";
    cout << "===========================================\n";

    int choice = getValidatedChoice(0, 2, "Choice: ");

    vector<Ticket> tickets; loadTickets(tickets);

    switch (choice)
    {
        case 1: viewPurchasedTickets(tickets, email); break;
        case 2: refundTicket(email); break;
        case 0: return;
    }
}

// ==========================
// BOOTH MODULE
// ==========================
void displayBoothLayout(const Venue& venue) {
    cout << "Booth layout for " << venue.eventName << ": " << endl;
    vector<Booth> booths;
    loadBooths(booths);
    cout << "    ";
    for (int j =  0; j < venue.columns; j++) {
        char colLetter = 'A' + j;
        cout << colLetter << "    ";
    }
    cout << endl;
    for (int i = 0; i < venue.rows; i++) {
        cout << i+1 << "  ";
        for (int j = 0; j < venue.columns; j++) {
            char colLetter = 'A' + j;
            string boothID = string(1, colLetter) + to_string(i + 1);
            bool booked = false;
            for (const auto& b : booths) {
                if (b.boothID == boothID && b.venueID == venue.venueID && b.isRented) {
                    booked = true;
                    break;
                }
            }
            cout << (booked ? "[x]" : "[ ]");
            if (j < venue.columns - 1) cout << "  ";
        }
        cout << endl;
    }
}

void bookBooth(vector<Venue>& venues, vector<Booth>& booths, const string& email) {
    cout << "Available Venues:\n";
    for (size_t i = 0; i < venues.size(); ++i) {
        if (!venues[i].isAvailable && !venues[i].eventName.empty())
            cout << i+1 << ". " << venues[i].eventName << " (Venue " << venues[i].venueID << ")\n";
    }
    int choice = getValidatedChoice(0, venues.size(), "Select venue (0 to cancel): ");
    if (choice == 0) return;
    
    Venue& v = venues[choice-1];

    if(v.isAvailable) return;

    displayBoothLayout(v);

    string boothID; 
    while(true) {
        bool sameBooth = false;
        cout << "Enter booth ID to rent (e.g. A1, B2): ";
        getline(cin, boothID);
        
        boothID[0] = toupper(boothID[0]);

        if (!isValidBoothID(v, boothID)) {
            cout << "Invalid booth ID.\n\n";
            continue;
        }
        for (const auto& booked : booths) {
            if (booked.venueID == v.venueID && booked.boothID == boothID && booked.isRented) {
                cout << "Booth already booked.\n\n";
                sameBooth = true;
            }
        }

        if (!sameBooth)
        {
            break;
        }        
    }

    double price = 0.0;
    for (const auto& b : v.boothType) {
        if (b.boothID == boothID) price = b.price;
    }
    cout << "Booth price: RM" << fixed << setprecision(2) << price << endl;
    
    char confirm;
    while(true) {
        cout << "Confirm rental of booth " << boothID << "? (y/n): ";
        cin >> confirm;
        cin.ignore();

        if(confirm == 'Y' || confirm == 'N' || confirm == 'y' || confirm == 'n') break;

        cout << "Invalid input. Please try again.\n\n";
    }

    if(confirm == 'N' || confirm == 'n') {
        cout << "Booth rental cancelled.\n";
        return;
    }

    Booth b;
    b.userEmail = email;
    b.venueID = v.venueID;
    b.boothID = boothID;
    b.amount = price;
    b.isRented = true;
    booths.push_back(b);
    saveBooth(b);

    cout << "Booth booked successfully!\n";
}

void viewBooth(const string& email){
    vector<Booth> booths;
    bool found = false;
    loadBooths(booths);
    for (const auto& booth : booths) {
        if (booth.userEmail == email && booth.isRented) {
            found = true;
            cout << endl;
            cout << "Booth ID: " << booth.boothID << endl;
            cout << "Venue ID: " << booth.venueID << endl;
            cout << "Amount: $" << fixed << setprecision(2) << booth.amount << endl;
            cout << "--------------------------" << endl;
        }
    }
    if (!found) {
        cout << "No booths found for this email." << endl;
    }

    cout << "\nPress Enter to continue...\n";
    cin.ignore();

}

void viewAllBookedBooths(const string &venueID) {
    vector<Booth> booths;
    loadBooths(booths);

    for (const auto& b : booths) {
        if (b.isRented && b.venueID == venueID) {
            cout << "Booth ID: " << b.boothID << ", User Email: " << b.userEmail << endl;
        }
    }
}

void refundBooth(const string& email) {
    vector<Booth> booths;
    loadBooths(booths);

    // Show user's booths
    vector<Booth> userBooths;
    for (const auto& b : booths) {
        if (b.userEmail == email && b.isRented) userBooths.push_back(b);
    }
    if (userBooths.empty()) {
        cout << "You have no booths to refund.\n";
        return;
    }
    cout << "Your rented booths:\n";
    for (size_t i = 0; i < userBooths.size(); ++i) {
        cout << i+1 << ". Booth ID: " << userBooths[i].boothID
             << ", Venue ID: " << userBooths[i].venueID
             << ", Amount: RM" << fixed << setprecision(2) << userBooths[i].amount << endl;
    }

    int choice = getValidatedChoice(0, (int)userBooths.size(), "Enter which booth you want to refund (0 to cancel): ");
    if (choice == 0) {
        cout << "Refund cancelled.\n";
        return;
    }

    string boothID = userBooths[choice-1].boothID;
    string venueID = userBooths[choice-1].venueID;
    double amount = userBooths[choice-1].amount;

    // Remove booth from booths vector (set isRented to false)
    vector<Booth> updatedBooths;
    bool found = false;
    for (auto& b : booths) {
        if (b.boothID == boothID && b.userEmail == email && b.venueID == venueID && b.isRented) {
            found = true;
            // Write refund log
            ofstream refundFile("boothRefunds.txt", ios::app);
            refundFile << b.userEmail << "," << b.boothID << "," << b.venueID << "," << b.amount << ",REFUNDED\n";
            refundFile.close();
            cout << "Refund amount: RM" << fixed << setprecision(2) << b.amount << endl;
            b.isRented = false; // Mark as not rented
        }
        updatedBooths.push_back(b);
    }
    if (!found) {
        cout << "Booth not found or not owned by user.\n";
        return;
    }
    // Save updated booths
    ofstream outFile("booth.txt");
    for (const auto& b : updatedBooths) {
        outFile << b.userEmail << "," << b.venueID << "," << b.boothID << "," << (b.isRented ? "1" : "0") << "," << b.amount << endl;
    }
    outFile.close();

    // Update venue boothType isRented status
    vector<Venue> venues;
    loadVenues(venues);
    for (auto& v : venues) {
        if (v.venueID == venueID) {
            for (auto& bt : v.boothType) {
                if (bt.boothID == boothID) {
                    bt.isRented = false;
                }
            }
            break;
        }
    }
    saveVenues(venues);
}

void exhibitorBoothSelection(const string& email) {
    cout << "===========================================\n";
    cout << "||              Booth Menu               ||\n";
    cout << "===========================================\n";
    cout << "|| 1. View Booked Booths                 ||\n";
    cout << "|| 2. Refund Booth                       ||\n";
    cout << "|| 0. Back                               ||\n";
    cout << "===========================================\n";

    int choice = getValidatedChoice(0, 2, "Choice: ");

    switch (choice)
    {
        case 1: viewBooth(email); break;
        case 2: refundBooth(email); break;
        case 0: return;
    }
}

// ==========================
// VENUE MODULE
// ==========================
void makeNewVenueBooking(Venue& venue) {
    cin.clear();
    cout << "Please enter the details for the new venue booking. (0 to cancel)" << endl;
    string eventName;

    do{
        cout << "\nEnter the name for the event: ";
        getline(cin, eventName);
        if (eventName == "0") {
            cout << "Booking cancelled.\n" << endl;
            return;
        }

        if(eventName.empty()) {
            cout << "Event name cannot be empty, please try again.\n";
        }
    } while(eventName.empty());

    venue.eventName = eventName;


    cout << "\nEnter the number of rows and columns of booths to be added (Maximum 10 rows and 10 columns).\n";
    int rows = getValidatedChoice(0, 10, "Rows: ");
    if (rows == 0 ) {
        cout << "Booking cancelled." << endl;
        return;
    }
    venue.rows = rows;

    int cols = getValidatedChoice(0, 10, "Columns: ");
    if (cols == 0) {
        cout << "Booking cancelled." << endl;
        return;
    }
    venue.columns = cols;

    double boothPrice;
    while(true) {
        cout << "\nEnter booth price (fixed for the event): RM ";

        if(!(cin >> boothPrice)) {
            cout << "Invalid booth price, please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        break;
    }

    if (boothPrice <= 0) {
        cout << "Booking cancelled." << endl;
        return;
    }

    venue.boothType.clear();
    for (int i = 0; i < venue.rows; i++) {
        for (int j = 0; j < venue.columns; j++) {
            BoothType bt;
            bt.boothID = string(1, 'A' + j) + to_string(i + 1);
            bt.price = boothPrice;
            bt.isRented = false; 
            venue.boothType.push_back(bt);
        }
    }

    int ticketTypes;
    while(true) {
        cout << "\nEnter the number of ticket types: ";

        if(!(cin >> ticketTypes)) {
            cout << "Invalid ticket type, please enter a valid number.\n\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
       
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        break;
    }

    if (ticketTypes <= 0) {
        cout << "Booking cancelled." << endl;
        return;
    }
    
    venue.ticketType.clear();
    for (int i = 0; i < ticketTypes; ++i) {
        TicketType t;
        cout << "\nEnter details for ticket type " << i + 1 << ":" << endl;

        cout << "Type: ";
        getline(cin, t.type); 

        cout << "Price: RM";
        cin >> t.price;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

        cout << "Capacity: ";
        cin >> t.capacity;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

        t.sold = 0;
        venue.ticketType.push_back(t);
    }
    if (venue.ticketType.empty()) {
        cout << "No ticket types added. Booking cancelled." << endl;
        return;
    }
    venue.isAvailable = false;
    cout << "New event created successfully." << endl;
}

void viewVenueAvailability(vector<Venue>& venues) {
    cout << "Venues: " << endl;
    for (size_t i = 0; i < venues.size(); i++) {
        if (venues[i].isAvailable) {
            cout << i + 1 << ". Venue " << venues[i].venueID << endl;
        }
    }

    int choice;
    while(true) {
        cout << "Enter the venue no. of your choice (enter 0 to cancel): ";
        if(!(cin >> choice) || choice < 0 || choice > venues.size() || !venues[choice - 1].isAvailable) {
            cout << "Invalid or already booked venue." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        break;

    }

    if (choice == 0) {
        cout << "Booking cancelled." << endl;
        cin.ignore();
        return;
    }

    makeNewVenueBooking(venues[choice - 1]);
    saveVenues(venues);
    cout << "Venue " << venues[choice - 1].venueID << " booked successfully." << endl;
}

void viewAllVenue(const vector<Venue>& venues) {
    bool hasVenue = false;
    for (const auto& v : venues) {
        if (!v.eventName.empty()) {
            hasVenue = true;
            break;
        }
    }
    if (!hasVenue) {
        cout << "No venues have been created yet." << endl;
        return;
    }
    for (const auto& v : venues) {
        if (!v.venueID.empty() && !v.isAvailable) {
            cout << "Venue ID: " << v.venueID << endl;
            cout << "Event Name: " << v.eventName << endl;
            displayBoothLayout(v);
            cout << endl;
            viewAllBookedBooths(v.venueID);
            cin.ignore();
        }
    }

    cout << "\nPress Enter to continue...\n";
    cin.ignore();

}

void closeEvent(const string& venueID, vector<Venue>& venues) {
    // Find the event name for the given venue ID
    string targetEventName = "";
    for (const auto& v : venues) {
        if (v.venueID == venueID) {
            targetEventName = v.eventName;
            break;
        }
    }

    if (targetEventName.empty()) {
        cout << "Event not found." << endl;
        return;
    }

    // Remove all tickets for this event
    vector<Ticket> tickets;
    loadTickets(tickets);
    ofstream ticketFile("ticket.txt");
    for (const auto& t : tickets) {
        if (t.eventName != targetEventName) { // keep only unrelated tickets
            ticketFile << t.userEmail << ","
                       << t.ticketID << ","
                       << t.eventName << ","
                       << t.ticketType << ","
                       << t.amount << endl;
        }
    }
    ticketFile.close();

    // Remove all booths for this venue
    vector<Booth> booths;
    loadBooths(booths);
    ofstream boothFile("booth.txt");
    for (const auto& b : booths) {
        if (b.venueID != venueID) {
            boothFile << b.userEmail << ","
                      << b.venueID << ","
                      << b.boothID << ","
                      << (b.isRented ? "1" : "0") << ","
                      << b.amount << endl;
        }
    }
    boothFile.close();

    // Remove all sessions for this venue
    vector<Session> sessions;
    loadSessions(sessions);
    ofstream sessionFile("sessions.txt");
    for (const auto& s : sessions) {
        if (s.venueID != venueID) {
            sessionFile << s.sessionID << ","
                       << s.venueID << ","
                       << s.exhibitorEmail << ","
                       << s.topic << ","
                       << s.timeSlot << endl;
        }
    }
    sessionFile.close();

    // Reset the venue
    for (auto& v : venues) {
        if (v.venueID == venueID) {
            v.eventName = "";
            v.rows = 0;
            v.columns = 0;
            v.isAvailable = true;
            v.ticketType.clear();
            v.boothType.clear();
        }
    }
    saveVenues(venues);

    cout << "Event " << targetEventName << " has been closed and cleared.\n";
}

void closeEventMenu(vector<Venue>& venues) {
    // Gather all venues with active events
    vector<int> closableIndexes;
    cout << "\nVenues with active events:\n";
    for (size_t i = 0; i < venues.size(); ++i) {
        if (!venues[i].isAvailable && !venues[i].eventName.empty()) {
            cout << closableIndexes.size() + 1 << ". Venue ID: " << venues[i].venueID
                 << " | Event Name: " << venues[i].eventName << endl;
            closableIndexes.push_back(i);
        }
    }
    if (closableIndexes.empty()) {
        cout << "No active events to close.\n";
        return;
    }

    int closeChoice = getValidatedChoice(0, (int)closableIndexes.size(), "Select the event to close (0 to cancel): ");
    
    if (closeChoice == 0) {
        cout << "Cancelled.\n";
        return;
    }

    int venueIdx = closableIndexes[closeChoice - 1];
    closeEvent(venues[venueIdx].venueID, venues);
}

void adminEventSelection() {
    cout << "===========================================\n";
    cout << "||              Event Menu               ||\n";
    cout << "===========================================\n";
    cout << "|| 1. View All Venues & Booked Booths    ||\n";
    cout << "|| 2. Create New Event                   ||\n";
    cout << "|| 3. Close Event                        ||\n";
    cout << "|| 0. Back                               ||\n";
    cout << "===========================================\n";

    int choice = getValidatedChoice(0, 3, "Choice: ");

    vector<Venue> venues; loadVenues(venues);       

    switch (choice)
    {
        case 1: viewAllVenue(venues); break;
        case 2: viewVenueAvailability(venues); break;
        case 3: closeEventMenu(venues); break;
        case 0: return;
    }
 
}

// ==========================
// SESSION SCHEDULING MODULE
// ==========================
void scheduleSession(const string &email) {
    vector<Booth> booths;
    loadBooths(booths);

    vector<Session> sessions;
    loadSessions(sessions);

    Session s;
    s.sessionID = generateSessionID();
    s.exhibitorEmail = email;

    // Validate venue
    string venueID;
    bool validVenue = false;
    while (!validVenue) {
        cout << "Enter Venue ID where you want to schedule a session (0 to cancel): ";
        getline(cin, venueID);

        if(venueID == "0") return;

        // Check exhibitor has booth in that venue
        bool hasBooth = false;
        for (const auto& booth : booths) {
            if (booth.userEmail == email && booth.venueID == venueID) {
                hasBooth = true;
                break;
            }
        }

        if (!hasBooth) {
            cout << "You do not have a booth in this venue. Please try again.\n\n";
            continue;
        }

        // Check if exhibitor already scheduled a session in this venue
        bool alreadyScheduled = false;
        for (const auto& sess : sessions) {
            if (sess.exhibitorEmail == email && sess.venueID == venueID) {
                alreadyScheduled = true;
                break;
            }
        }

        if (alreadyScheduled) {
            cout << "You already scheduled a session in this venue.\n";
            return; // exit scheduling
        }

        validVenue = true;
    }
    s.venueID = venueID;

    do
    {
        cout << "Enter Session Topic: ";
        getline(cin, s.topic);

        if(!s.topic.empty()) break;

        cout << "Session topic cannot be empty. Please try again.\n\n";
 
    } while (s.topic.empty());
    

    // Validate timeslot
    string timeSlot;
    while (true) {
        cout << "Enter time slot (format HH:MM-HH:MM): ";
        getline(cin, timeSlot);

        if (!isValidTimeSlot(timeSlot)) {
            cout << "Invalid format or time range. Example valid input: 09:00-10:30\n\n";
            continue;
        }

        // Clash check
        bool clash = false;
        for (const auto& sess : sessions) {
            if (sess.venueID == venueID && isOverlap(sess.timeSlot, timeSlot)) {
                clash = true;
                break;
            }
        }

        if (clash) {
            cout << "This time slot overlaps with another session in this venue. Please try again.\n";
            continue;
        }

        break; // valid and no clash
    }
    s.timeSlot = timeSlot;

    // Save session
    sessions.push_back(s);
    saveSessions(sessions);
    cout << "Session scheduled successfully! Session ID: " << s.sessionID << endl;
}

void viewAllSessions() {
    vector<Session> sessions;
    loadSessions(sessions);

    if (sessions.empty()) {
        cout << "No sessions scheduled yet.\n";
        return;
    }

    cout << "All Scheduled Sessions:";
    for (const auto& s : sessions) {
        cout << "\n---------------------------------------------------------\n";
        cout << "SessionID: " << s.sessionID
             << "\nExhibitor: " << s.exhibitorEmail
             << "\nVenue: " << s.venueID
             << "\nTopic: " << s.topic
             << "\nTime: " << s.timeSlot;
        cout << "\n\n---------------------------------------------------------\n";

    }

    cout << "\nPress Enter to continue...\n";
    cin.ignore();

}

void viewSessionsByExhibitor(const string& email) {
    vector<Session> sessions;
    loadSessions(sessions);

    cout << "Your Scheduled Sessions:\n";
    bool found = false;
    for (const auto& s : sessions) {
        if (s.exhibitorEmail == email) {   // only show their own sessions
            cout << "[" << s.sessionID << "] "
                 << s.topic
                 << " (Venue " << s.venueID 
                 << ", Time: " << s.timeSlot << ")\n";
            found = true;
        }
    }

    if (!found) {
        cout << "You have not scheduled any sessions yet.\n";
    }

    cout << "\nPress Enter to continue...\n";
    cin.ignore();

}

void viewSessionsByAttendee(const string& email) {
    // 1. Load attendee's tickets
    vector<Ticket> tickets;
    loadTickets(tickets);

    // 2. Load venues 
    vector<Venue> venues;
    loadVenues(venues);  // 

    // 3. Collect all venueIDs that the attendee joined (via eventName in tickets)
    set<string> joinedVenueIDs;
    for (const auto& t : tickets) {
        if (t.userEmail == email) {
            for (const auto& v : venues) {
                if (v.eventName == t.eventName) {
                    joinedVenueIDs.insert(v.venueID);
                }
            }
        }
    }

    if (joinedVenueIDs.empty()) {
        cout << "You have not joined any events yet.\n";
        return;
    }

    // 4. Load sessions
    vector<Session> sessions;
    loadSessions(sessions);

    // 5. Display only sessions matching the attendee's joined venues
    cout << "Sessions available for events you joined:\n";
    bool found = false;
    for (const auto& s : sessions) {
        if (joinedVenueIDs.find(s.venueID) != joinedVenueIDs.end()) {
            cout << "[" << s.sessionID << "] "
                 << s.topic << " (Venue " << s.venueID
                 << ", Time: " << s.timeSlot << ")\n";
            found = true;
        }
    }

    if (!found) {
        cout << "No sessions scheduled yet for the events you joined.\n";
    }

    cout << "\nPress Enter to continue...\n";
    cin.ignore();

}

void updateSession(const string& email) {
    vector<Session> sessions;
    loadSessions(sessions);

    // Show exhibitor's sessions
    vector<int> ownedIndexes;
    cout << "Your Scheduled Sessions:\n";
    for (int i = 0; i < sessions.size(); i++) {
        if (sessions[i].exhibitorEmail == email) {
            cout << i+1 << ". [" << sessions[i].sessionID << "] "
                 << sessions[i].topic << " at Venue " << sessions[i].venueID
                 << " (" << sessions[i].timeSlot << ")\n";
            ownedIndexes.push_back(i);
        }
    }

    if (ownedIndexes.empty()) {
        cout << "You have no sessions to update.\n";
        return;
    }

    int choice = getValidatedChoice(0, ownedIndexes.size(), "Enter the number of the session you want to update (0 to cancel): ");
    if (choice == 0) {
        cout << "Update cancelled.\n";
        return;
    }

    int idx = ownedIndexes[choice - 1];

    cout << "Enter new topic (leave empty to keep current): ";
    string newTopic;
    getline(cin, newTopic);
    if (!newTopic.empty()) sessions[idx].topic = newTopic;

    // Validate new time slot
    string newSlot;
    while (true) {
        cout << "Enter new time slot (HH:MM-HH:MM, leave empty to keep current): ";
        getline(cin, newSlot);

        if (newSlot.empty()) break;

        if (!isValidTimeSlot(newSlot)) {
            cout << "Invalid time format. Example: 09:00-10:30\n";
            continue;
        }

        // Check clash with other sessions
        bool clash = false;
        for (int j = 0; j < sessions.size(); j++) {
            if (j != idx && sessions[j].venueID == sessions[idx].venueID &&
                isOverlap(sessions[j].timeSlot, newSlot)) {
                clash = true;
                break;
            }
        }

        if (clash) {
            cout << "Time slot overlaps with another session. Please try again.\n";
            continue;
        }

        sessions[idx].timeSlot = newSlot;
        break;
    }

    saveSessions(sessions);
    cout << "Session updated successfully.\n";
}

void deleteSession(const string& email) {
    vector<Session> sessions;
    loadSessions(sessions);

    // Show exhibitor's sessions
    vector<int> ownedIndexes;
    cout << "Your Scheduled Sessions:\n";
    for (int i = 0; i < sessions.size(); i++) {
        if (sessions[i].exhibitorEmail == email) {
            cout << i+1 << ". [" << sessions[i].sessionID << "] "
                 << sessions[i].topic<< " at Venue " << sessions[i].venueID
                 << " (" << sessions[i].timeSlot << ")\n";
            ownedIndexes.push_back(i);
        }
    }

    if (ownedIndexes.empty()) {
        cout << "You have no sessions to delete.\n";
        return;
    }

    int choice = getValidatedChoice(0, ownedIndexes.size(), "Enter the number of the session you want to delete (0 to cancel): ");
    if (choice == 0) {
        cout << "Deletion cancelled.\n";
        return;
    }

    int idx = ownedIndexes[choice - 1];
    cout << "Are you sure you want to delete session [" << sessions[idx].sessionID << "] (Y/N)? ";
    char confirm;
    cin >> confirm;
    cin.ignore();

    if (confirm == 'y' || confirm == 'Y') {
        sessions.erase(sessions.begin() + idx);
        saveSessions(sessions);
        cout << "Session deleted successfully.\n";
    } else {
        cout << "Deletion cancelled.\n";
    }
}

void ExhibitorSessionSelection(const string& email) {
    cout << "===========================================\n";
    cout << "||              Session Menu             ||\n";
    cout << "===========================================\n";
    cout << "|| 1. View All Session Scheduled         ||\n";
    cout << "|| 2. Update Session Detail              ||\n";
    cout << "|| 3. Delete Session                     ||\n";
    cout << "|| 0. Back                               ||\n";
    cout << "===========================================\n";

    int choice = getValidatedChoice(0, 3, "Choice: ");

    switch (choice)
    {
        case 1: viewSessionsByExhibitor(email); break;
        case 2: updateSession(email); break;
        case 3: deleteSession(email); break;
        case 0: return;
    }
 
}

// ==========================
// MONITORING MODULE
// ==========================
void monitorExhibitorStats(const string& exhibitorEmail) {

    vector<Booth> booths; loadBooths(booths);
    vector<Session> sessions; loadSessions(sessions);

    cout << "\n========================================================\n";
    cout << "||         Exhibitor Monitoring Dashboard             ||\n";
    cout << "========================================================\n";

    cout << "Exhibitor: " << exhibitorEmail;

    cout << "\n=============================\n";
    cout << "||       Booth Stats       ||\n";
    cout << "=============================\n";

    bool boothFound = false;
    for (const auto& booth : booths) {
        if (booth.userEmail == exhibitorEmail) {
            cout << "Venue: " << booth.venueID << endl
                 << "Booth: " << booth.boothID << endl
                 << "Amount Paid: RM" << fixed << setprecision(2) << booth.amount << endl
                 << "Status: " << (booth.isRented ? "Rented" : "Refunded") << endl
                 << endl;
            boothFound = true;
        }
    }
    if (!boothFound) {
        cout << "No booths booked yet.\n";
    }

    cout << "==============================\n";
    cout << "||    Scheduled Sessions    ||\n";
    cout << "==============================\n";

    bool sessionFound = false;
    for (const auto& session : sessions) {
        if (session.exhibitorEmail == exhibitorEmail) {
            cout << "Session ID: " << session.sessionID << endl
                 << "Venue: " << session.venueID << endl
                 << "Topic: " << session.topic << endl
                 << "Time Slot: " << session.timeSlot << endl
                 << endl;
            sessionFound = true;
        }
    }
    if (!sessionFound) {
        cout << "No sessions scheduled yet.\n";
    }

    cout << "\nPress Enter to continue...\n";
    cin.ignore();

}

void monitorAdminStats() {

    vector<Ticket> tickets; loadTickets(tickets);
    vector<Booth> booths; loadBooths(booths);
    vector<Session> sessions; loadSessions(sessions);
    vector<Venue> venues; loadVenues(venues);

    if (tickets.empty()) {
        cout << "No ticket sales data available.\n";
        return;
    }

    cout << "\n==================================\n";
    cout << "||     Monitor Venue Stat       ||\n";
    cout << "==================================\n";

    for (size_t i = 0; i < venues.size(); i++) {
        cout << i + 1 << ". " << venues[i].venueID << " - " << venues[i].eventName << endl;
    }

    cout << endl;

    int choice = getValidatedChoice(0, venues.size(), "Enter the number of the venue to view stats: ");
    
    if (choice == 0) {
        cout << "Returning to dashboard.\n\n";
        return;
    }

    Venue selectedVenue = venues[choice - 1];

    cout << "\nMonitoring: Venue " << selectedVenue.venueID << " - " << selectedVenue.eventName << endl;

    // ticket sales
    int ticketCount = 0;
    double ticketRevenue = 0.0;

    for (const auto& ticket : tickets) {
        if (ticket.eventName == selectedVenue.eventName) {
            ticketCount++;
            ticketRevenue += ticket.amount;
        }
    }
    cout << "==========================================\n";
    cout << "||         Ticket Sales Summary         ||\n";
    cout << "==========================================\n";
    cout << "Total Tickets Sold: " << ticketCount << endl;
    cout << "Total Ticket Revenue: RM " << fixed << setprecision(2) << ticketRevenue << endl;

    // booth sales
    int boothCount = 0;
    double boothRevenue = 0.0;
    
    for (const auto& booth : booths) {
        if (booth.venueID == selectedVenue.venueID && booth.isRented) {
            boothCount++;
            boothRevenue += booth.amount;
        }
    }
    cout << "==========================================\n";
    cout << "||          Booth Sales Summary         ||\n";
    cout << "==========================================\n";
    cout << "Total Booths Rented: " << boothCount << endl;
    cout << "Total Booth Revenue: RM " << fixed << setprecision(2) << boothRevenue << endl;
    displayBoothLayout(selectedVenue);

    // sessions scheduled
    cout << "==========================================\n";
    cout << "||           Sessions Summary           ||\n";
    cout << "==========================================\n";
    bool foundSession = false;
    for (const auto& session : sessions) {
        if (session.venueID == selectedVenue.venueID) {
            cout << "[" << session.sessionID << "] " << session.topic << " at " << session.timeSlot << " (Exhibitor: " << session.exhibitorEmail << ")\n";
            foundSession = true;
        }
    }
    if (!foundSession) {
        cout << " - No sessions scheduled yet.\n";
    }

    cout << "\nPress Enter to continue...\n";
    cin.ignore();

}

// ==========================
// REPORTING MODULE
// ==========================
void generateEventReport(const string& venueID, const vector<Ticket>& tickets, const vector<Booth>& booths, const vector<Session>& sessions, const vector<Venue>& venues) {
    
    // Find venue
    auto it = find_if(venues.begin(), venues.end(),
        [&](const Venue& v) { return v.venueID == venueID; });

    if (it == venues.end() || it->eventName.empty()) {
        cout << "[ERROR] Venue not found or no event assigned.\n";
        return;
    }

    const Venue& venue = *it;
    cout << "\n=========================================\n";
    cout << "FINAL REPORT FOR EVENT: " << venue.eventName << "\n";
    cout << "Venue ID: " << venue.venueID << "\n";
    cout << "=========================================\n";

    // --- Ticket Summary ---
    int totalTickets = 0;
    double totalTicketRevenue = 0.0;
    for (const auto& t : tickets) {
        if (t.eventName == venue.eventName) { // only this event
            totalTickets++;
            totalTicketRevenue += t.amount;
        }
    }
    cout << "Ticket Sales Summary\n";
    cout << "Total Tickets Sold : " << totalTickets << endl;
    cout << "Total Ticket Revenue: RM " << fixed << setprecision(2) << totalTicketRevenue << "\n";

    // --- Booth Summary ---
    int totalBooths = 0;
    double totalBoothRevenue = 0.0;
    for (const auto& b : booths) {
        if (b.venueID == venue.venueID && b.isRented) {
            totalBooths++;
            totalBoothRevenue += b.amount;
        }
    }
    cout << "\nBooth Rental Summary\n";
    cout << "Total Booths Rented : " << totalBooths << endl;
    cout << "Total Booth Revenue : RM " << fixed << setprecision(2) << totalBoothRevenue << "\n";

    // --- Session Summary ---
    cout << "\nSessions Summary\n";
    bool hasSessions = false;
    for (const auto& s : sessions) {
        if (s.venueID == venue.venueID) {
            cout << "   - [" << s.sessionID << "] "
                 << s.topic << " | Time: " << s.timeSlot
                 << " | Exhibitor: " << s.exhibitorEmail << "\n";
            hasSessions = true;
        }
    }
    if (!hasSessions) cout << "   No sessions scheduled.\n";

    // --- Grand Total ---
    double grandTotal = totalTicketRevenue + totalBoothRevenue;
    cout << "\nGRAND TOTAL REVENUE: RM " << fixed << setprecision(2) << grandTotal << endl;
    cout << "=========================================\n";
    cout << "         END OF FINAL REPORT              \n";
    cout << "=========================================\n";

    cout << "\nPress Enter to continue...\n";
    cin.ignore();

}

void exportReportToFile(const string& venueID, const vector<Ticket>& tickets, const vector<Booth>& booths, const vector<Session>& sessions, const vector<Venue>& venues) {
    
    auto it = find_if(venues.begin(), venues.end(),
        [&](const Venue& v) { return v.venueID == venueID; });

    if (it == venues.end() || it->eventName.empty()) {
        cout << "[ERROR] Venue not found or no event assigned.\n";
        return;
    }


    const Venue& venue = *it;
    const string filename = venue.eventName + " Event Report.txt";
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "[ERROR] Unable to create report file.\n";
        return;
    }

    file << "========== FINAL REPORT ==========\n";
    file << "Event: " << venue.eventName << "\n";
    file << "Venue: " << venue.venueID << "\n";

    // Tickets
    int totalTickets = 0;
    double totalTicketRevenue = 0.0;
    for (const auto& t : tickets) {
        if (t.eventName == venue.eventName) {
            totalTickets++;
            totalTicketRevenue += t.amount;
        }
    }
    file << "\nTicket Sales:\n";
    file << "Total Tickets Sold : " << totalTickets << "\n";
    file << "Total Ticket Revenue: RM " << fixed << setprecision(2) << totalTicketRevenue << "\n";

    // Booths
    int totalBooths = 0;
    double totalBoothRevenue = 0.0;
    for (const auto& b : booths) {
        if (b.venueID == venue.venueID && b.isRented) {
            totalBooths++;
            totalBoothRevenue += b.amount;
        }
    }
    file << "\nBooth Rentals:\n";
    file << "Total Booths Rented : " << totalBooths << "\n";
    file << "Total Booth Revenue : RM " << fixed << setprecision(2) << totalBoothRevenue << "\n";

    // Sessions
    file << "\nSessions Summary:\n";
    bool hasSessions = false;
    for (const auto& s : sessions) {
        if (s.venueID == venue.venueID) {
            file << " - [" << s.sessionID << "] " << s.topic
                 << " | Time: " << s.timeSlot
                 << " | Exhibitor: " << s.exhibitorEmail << "\n";
            hasSessions = true;
        }
    }
    if (!hasSessions) file << "No sessions scheduled.\n";

    // Grand Total
    double grandTotal = totalTicketRevenue + totalBoothRevenue;
    file << "\nGRAND TOTAL REVENUE: RM " << fixed << setprecision(2) << grandTotal << "\n";
    file << "========== END OF REPORT ==========\n";

    file.close();
    cout << "[INFO] Final report for " << venue.eventName << " exported to " << filename << endl;

    cout << "\nPress Enter to continue...\n";
    cin.ignore();

}

void adminReportSelection() {
    vector<Ticket> tickets; loadTickets(tickets);
    vector<Booth> booths; loadBooths(booths);
    vector<Session> sessions; loadSessions(sessions);
    vector<Venue> venues; loadVenues(venues);

    cout << "===========================================\n";
    cout << "||            Reporting Menu             ||\n";
    cout << "===========================================\n";

    cout << "Available Events:\n";
    for (const auto& v : venues) {
        if (!v.eventName.empty())
            cout << " - Venue " << v.venueID << ": " << v.eventName << endl;
    }

    string venueID;
    cout << "\nEnter Venue ID to generate report: ";
    getline(cin, venueID);
    transform(venueID.begin(), venueID.end(), venueID.begin(), ::toupper);

    cout << "1. View Report on Screen\n";
    cout << "2. Export Report to File\n";
    int choice = getValidatedChoice(0, 2, "Choice (0 to return): ");

    switch (choice)
    {
        case 1: generateEventReport(venueID, tickets, booths, sessions, venues); break;
        case 2: exportReportToFile(venueID, tickets, booths, sessions, venues); break;
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
        cout << "|| 6. Manage Purchased Tickets                     ||\n";
        cout << "|| 7. Submit Feedbacks                             ||\n";
        cout << "|| 8. View Available Sessions                      ||\n";
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
        else if (choice == "5") { // Purchase Ticket
            vector<Venue> venues; loadVenues(venues);
            vector<Ticket> tickets; loadTickets(tickets);
            purchaseTicket(venues, tickets, a.email);

        }
        else if (choice == "6") { // Manage Ticket
            attendeeTicketSelection(a.email);
        }
        else if (choice == "7") { // Submit Feedbacks
            vector<Ticket> tickets; loadTickets(tickets);
            submitFeedback(a.email, tickets);

        }
        else if (choice == "8") { // View Sessions
            viewSessionsByAttendee(a.email);
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
        cout << "|| 6. Manage Booked Booths                         ||\n";
        cout << "|| 7. Schedule Sessions                            ||\n";
        cout << "|| 8. Manage Sessions Scheduled                    ||\n";
        cout << "|| 9. Monitor Booth/Session Stats                  ||\n";
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
        else if (choice == "5") { // Book Booth
            vector<Venue> venues; loadVenues(venues);
            vector<Booth> booths; loadBooths(booths);
            bookBooth(venues, booths, e.email);
        }
        else if (choice == "6") { // Manage Booth
            exhibitorBoothSelection(e.email); 
        }
        else if (choice == "7") { // Schedule Session
            scheduleSession(e.email);
        }
        else if (choice == "8") { // Manage Session
            ExhibitorSessionSelection(e.email);
        }
        else if (choice == "9") { // Monitor Stats
            monitorExhibitorStats(e.email);
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
        cout << "|| 4. Manage Event Venues                          ||\n";
        cout << "|| 5. View Sessions                                ||\n"; 
        cout << "|| 6. Manage Feedbacks                             ||\n"; 
        cout << "|| 7. Monitor Ticket/Booth/Session Stats           ||\n"; 
        cout << "|| 8. Manage Reports                               ||\n"; 
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
        else if (choice == "4") { // Manage Event Venues
            adminEventSelection();
        }
        else if (choice == "5") { // View Sessions
            viewAllSessions();
        }
        else if (choice == "6") { // Manage Feedbacks
            adminFeedbackSelection();
        }
        else if (choice == "7") { // Monitor Stats
            monitorAdminStats();
        }
        else if (choice == "8") { // Manage Report
            adminReportSelection();
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
