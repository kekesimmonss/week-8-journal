
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cctype>

using namespace std;

// Stores information about an individual course.
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Stores courses by their course number.
unordered_map<string, Course> courseMap;

// Converts course numbers to uppercase.
string toUpperCase(string text) {
    transform(text.begin(), text.end(), text.begin(),
        [](unsigned char c) {
            return static_cast<char>(toupper(c));
        });
    return text;
}

// Removes unnecessary spaces from text.
string trim(const string& text) {
    size_t start = text.find_first_not_of(" \t\r\n");

    if (start == string::npos) {
        return "";
    }

    size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}

// Loads course information from a comma-separated file.
bool loadCourses(const string& filename) {
    ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        cout << "Error: Unable to open file "
             << filename << ".\n";
        return false;
    }

    unordered_map<string, Course> loadedCourses;
    string line;
    int lineNumber = 0;

    while (getline(inputFile, line)) {
        lineNumber++;

        if (trim(line).empty()) {
            continue;
        }

        stringstream lineStream(line);
        vector<string> fields;
        string field;

        while (getline(lineStream, field, ',')) {
            fields.push_back(trim(field));
        }

        // A valid row must contain a course number and title.
        if (fields.size() < 2 ||
            fields[0].empty() || fields[1].empty()) {
            cout << "Warning: Invalid data on line "
                 << lineNumber << ". Skipping row.\n";
            continue;
        }

        Course course;
        course.courseNumber = toUpperCase(fields[0]);
        course.courseTitle = fields[1];

        // Store prerequisite course numbers.
        for (size_t i = 2; i < fields.size(); i++) {
            if (!fields[i].empty()) {
                course.prerequisites.push_back(
                    toUpperCase(fields[i]));
            }
        }

        // Do not load duplicate course numbers.
        if (loadedCourses.find(course.courseNumber)
            != loadedCourses.end()) {
            cout << "Warning: Duplicate course "
                 << course.courseNumber
                 << ". Skipping duplicate.\n";
            continue;
        }

        loadedCourses[course.courseNumber] = course;
    }

    inputFile.close();

    if (loadedCourses.empty()) {
        cout << "Error: No valid courses were loaded.\n";
        return false;
    }

    // Confirm that prerequisite courses exist.
    for (const auto& entry : loadedCourses) {
        for (const string& prerequisite :
             entry.second.prerequisites) {
            if (loadedCourses.find(prerequisite)
                == loadedCourses.end()) {
                cout << "Warning: Prerequisite "
                     << prerequisite << " for "
                     << entry.first
                     << " was not found in the file.\n";
            }
        }
    }

    courseMap = loadedCourses;

    cout << "Successfully loaded "
         << courseMap.size() << " courses.\n";

    return true;
}

// Returns course numbers in alphanumeric order.
vector<string> getSortedCourseNumbers() {
    vector<string> courseNumbers;

    for (const auto& entry : courseMap) {
        courseNumbers.push_back(entry.first);
    }

    sort(courseNumbers.begin(), courseNumbers.end());

    return courseNumbers;
}

// Prints the complete course list in sorted order.
void printCourseList() {
    if (courseMap.empty()) {
        cout << "Please load the course data first.\n";
        return;
    }

    cout << "\nHere is a sample schedule:\n";

    vector<string> sortedCourses = getSortedCourseNumbers();

    for (const string& number : sortedCourses) {
        const Course& course = courseMap.at(number);

        cout << course.courseNumber << ", "
             << course.courseTitle << '\n';
    }
}

// Prints a course title and its prerequisite information.
void printCourseInformation() {
    if (courseMap.empty()) {
        cout << "Please load the course data first.\n";
        return;
    }

    string courseNumber;

    cout << "What course do you want to know about? ";
    cin >> courseNumber;

    courseNumber = toUpperCase(courseNumber);

    auto courseFound = courseMap.find(courseNumber);

    if (courseFound == courseMap.end()) {
        cout << "Course not found: "
             << courseNumber << '\n';
        return;
    }

    const Course& course = courseFound->second;

    cout << course.courseNumber << ", "
         << course.courseTitle << '\n';

    if (course.prerequisites.empty()) {
        cout << "Prerequisites: None\n";
        return;
    }

    cout << "Prerequisites:\n";

    for (const string& prerequisite :
         course.prerequisites) {
        auto prerequisiteFound = courseMap.find(prerequisite);

        if (prerequisiteFound != courseMap.end()) {
            const Course& prerequisiteCourse =
                prerequisiteFound->second;

            cout << prerequisiteCourse.courseNumber
                 << ", " << prerequisiteCourse.courseTitle
                 << '\n';
        } else {
            cout << prerequisite
                 << " (course information unavailable)\n";
        }
    }
}

// Displays the program menu.
void displayMenu() {
    cout << "\nWelcome to the course planner.\n";
    cout << "1. Load Data Structure.\n";
    cout << "2. Print Course List.\n";
    cout << "3. Print Course.\n";
    cout << "9. Exit\n";
    cout << "What would you like to do? ";
}

int main() {
    int choice = 0;
    string filename;

    while (choice != 9) {
        displayMenu();

        // Validate menu input.
        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        switch (choice) {
            case 1:
                cout << "Enter the course data filename: ";
                cin >> filename;
                loadCourses(filename);
                break;

            case 2:
                printCourseList();
                break;

            case 3:
                printCourseInformation();
                break;

            case 9:
                cout << "Thank you for using the course planner!\n";
                break;

            default:
                cout << choice << " is not a valid option.\n";
                break;
        }
    }

    return 0;
}
