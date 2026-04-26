//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Eduardo Hernandez
// Date        : April 18, 2026
// Course      : CS 300 DSA: Analysis & Design [12263]
// Purpose     : This program serves as an advising assistance tool for ABCU. 
//               It utilizes a Binary Search Tree (BST) to manage a course 
//               curriculum. The application allows users to load course 
//               data from a CSV, display an alphanumerically sorted list of 
//               all courses, and search for specific course details 
//               including titles and prerequisites.
//============================================================================


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// ============================================================================
// Structure Definitions
// ============================================================================

/**
 * Custom data type used to store individual course details and requirements.
 */
struct Course {
    string courseNumber;          // Unique identifier (e.g., CSCI100)
    string courseName;            // Full title of the course
    vector<string> prerequisites; // List of required course numbers
};

/**
 * Internal structure for the Binary Search Tree nodes.
 */
struct Node {
    Course course;
    Node* left;
    Node* right;

    // Default constructor: Initializes child pointers to nullptr
    Node() {
        left = nullptr;
        right = nullptr;
    }

    // Overloaded constructor for ease of use during insertion
    Node(Course aCourse) : Node() {
        this->course = aCourse;
    }
};

// ============================================================================
// Binary Search Tree Class Implementation
// ============================================================================

class BinarySearchTree {

private:
    Node* root;

    // Recursive helper methods
    void addNode(Node* node, Course course);
    void inOrder(Node* node);
    void destroyRecursive(Node* node);

public:
    BinarySearchTree();
    virtual ~BinarySearchTree();
    void Insert(Course course);
    void PrintAll();
    void Search(string courseNumber);
};

/**
 * Constructor
 */
BinarySearchTree::BinarySearchTree() {
    root = nullptr;
}

/**
 * Destructor: Automatically triggers memory cleanup upon exit.
 */
BinarySearchTree::~BinarySearchTree() {
    // Post-order traversal to delete nodes
    destroyRecursive(root);
}

/**
 * Helper to recursively deallocate tree memory.
 */
void BinarySearchTree::destroyRecursive(Node* node) {
    if (node != nullptr) {
        destroyRecursive(node->left);
        destroyRecursive(node->right);
        delete node;
    }
}

/**
 * Public Insert method.
 */
void BinarySearchTree::Insert(Course course) {
    if (root == nullptr) {
        root = new Node(course);
    }
    else {
        this->addNode(root, course);
    }
}

/**
 * Recursive addNode to maintain alphanumeric BST properties.
 */
void BinarySearchTree::addNode(Node* node, Course course) {
    // Compare course numbers to determine branch (Left < Root < Right)
    if (course.courseNumber.compare(node->course.courseNumber) < 0) {
        if (node->left == nullptr) {
            node->left = new Node(course);
        }
        else {
            this->addNode(node->left, course);
        }
    }
    else {
        if (node->right == nullptr) {
            node->right = new Node(course);
        }
        else {
            this->addNode(node->right, course);
        }
    }
}

/**
 * Public method to print the full sorted curriculum.
 */
void BinarySearchTree::PrintAll() {
    this->inOrder(root);
}

/**
 * Recursive In-Order Traversal (Left, Root, Right).
 * This satisfies the requirement for an alphanumerically sorted list.
 */
void BinarySearchTree::inOrder(Node* node) {
    if (node != nullptr) {
        inOrder(node->left);
        cout << node->course.courseNumber << ", " << node->course.courseName << endl;
        inOrder(node->right);
    }
}

/**
 * Search logic to locate a specific course by ID.
 * Performance: O(log n).
 */
void BinarySearchTree::Search(string courseNumber) {
    Node* current = root;

    while (current != nullptr) {
        if (current->course.courseNumber.compare(courseNumber) == 0) {
            cout << current->course.courseNumber << ", " << current->course.courseName << endl;
            cout << "Prerequisites: ";

            if (current->course.prerequisites.empty()) {
                cout << "None";
            }
            else {
                for (size_t i = 0; i < current->course.prerequisites.size(); ++i) {
                    cout << current->course.prerequisites[i]
                        << (i == current->course.prerequisites.size() - 1 ? "" : ", ");
                }
            }
            cout << endl;
            return;
        }

        if (courseNumber.compare(current->course.courseNumber) < 0) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }
    cout << "Course " << courseNumber << " not found." << endl;
}

// ============================================================================
// Data Loading Functions
// ============================================================================

/**
 * Implementation of the "LoadAndValidate" algorithm.
 * Opens the file, tokenizes data, and performs validation checks.
 */
void loadCourses(string csvPath, BinarySearchTree* bst) {
    ifstream file(csvPath);
    if (!file.is_open()) {
        cout << "Error: File '" << csvPath << "' could not be opened." << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        vector<string> tokens;

        // Tokenize by comma
        while (getline(ss, token, ',')) {
            // 1. CLEANING: Remove carriage returns (\r) often found in Windows CSVs
            token.erase(remove(token.begin(), token.end(), '\r'), token.end());

            // 2. VALIDATION: Only add tokens that aren't empty (handles trailing commas)
            if (!token.empty()) {
                tokens.push_back(token);
            }
        }

        // Validate line format: Minimum of Course ID and Name required.
        if (tokens.size() < 2) {
            continue;
        }

        Course course;
        course.courseNumber = tokens[0];
        course.courseName = tokens[1];

        // 3. DYNAMIC PREREQUISITES: Add index 2 onwards if they exist.
        for (size_t i = 2; i < tokens.size(); ++i) {
            course.prerequisites.push_back(tokens[i]);
        }

        bst->Insert(course);
    }
    file.close();
    cout << "Data loaded successfully into the structure." << endl;
}

// ============================================================================
// User Interface (Main)
// ============================================================================

int main() {
    // Instantiate the Binary Search Tree on the heap
    BinarySearchTree* bst = new BinarySearchTree();
    string csvPath, courseKey;
    int choice = 0;

    cout << "Welcome to the course planner." << endl;

    // Main Menu Loop
    while (choice != 9) {
        cout << "\n1. Load Data Structure" << endl;
        cout << "2. Print Course List" << endl;
        cout << "3. Print Course" << endl;
        cout << "9. Exit" << endl;
        cout << "\nWhat would you like to do? ";

        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a menu number." << endl;
            cin.clear();
            cin.ignore(100, '\n');
            continue;
        }

        switch (choice) {
        case 1:
            cout << "Enter the file name: ";
            // Discard the newline character left in the buffer by 'cin >> choice'
            // This prevents getline() from accidentally reading an empty string.
            cin.ignore(); 
            getline(cin, csvPath); // Read the full line of text, allowing for spaces in filenames
            loadCourses(csvPath, bst);
            break;

        case 2:
            cout << "\nHere is a sample schedule:" << endl;
            bst->PrintAll();
            break;

        case 3:
            cout << "What course do you want to know about? ";
            cin >> courseKey;
            // Normalize search to uppercase for alphanumeric matching
            transform(courseKey.begin(), courseKey.end(), courseKey.begin(), ::toupper);
            bst->Search(courseKey);
            break;

        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            cout << choice << " is not a valid option." << endl;
        }
    }

    delete bst; // Clean up memory via destructor
    return 0;
}
