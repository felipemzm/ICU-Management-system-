#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <stack>
#include <string>
#include <limits>
using namespace std; 

// Patient Information , stores basic information and vitals
struct Patient{
     int id;
    string name;
    int bedNumber;
    string diagnosis;
    string codeStatus;
     int age;
    double height; 
    double weight; 
    string bloodType; 
};
// Nurses Task Information 
struct Task {
    int time;
    string description;  
};

// Create Nodes for Task
struct TaskNode {
    Task task;
    TaskNode* left;
    TaskNode* right;

    TaskNode( const Task & t) : task(t), left(nullptr), right(nullptr){}
};

// Tree for Tasks 
class TaskTree{
private: 
    TaskNode* root;

    //Decide where each task goes in the tree based on time 
    TaskNode* insert(TaskNode* node, const Task & task){
        if(!node) return new TaskNode(task);
        if(task.time < node->task.time)
            node->left = insert(node->left, task);
        else    
            node->right = insert(node->right, task);
        return node;
    }
    //Print Tasks In Order  of time 
    void inOrder(TaskNode* node) const{
        if(!node) return;
        inOrder(node->left);
        cout << " " << node->task.time <<" "<< node->task.description << "\n" ;
        inOrder(node->right);
    }
    // clear nodes in the tree 
    void clear(TaskNode* node){
        if(!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

// Create empty tree 
public: 
    TaskTree() : root(nullptr) {}

    ~TaskTree(){
        clear(root);
    }
//function called when inserting tasks into the tree
    void insertTask(const Task & task){
        root = insert(root, task);
    }
// function called when there are tasks, and prints them out in order 
    void printTasksInOrder() const{
        cout << "Upcoming ICU Tasks (by time):\n ";
        inOrder(root);
    }
};

class ICU_Unit{
private: 
    vector<Patient> patients;                    
    unordered_map< int, Patient*> bedToPatient;  
    queue<string> alertQueue;                   
    stack<string> actionHistory;                
    TaskTree taskTree;                               
    vector<vector<int>> icuGraph;               

public: 
    ICU_Unit(int numRooms = 0){
        icuGraph.resize(numRooms);
    }
    // Add hallways between two rooms(edges) in the graph 
    void addHallway(int roomA, int roomB){
        if(roomA < 0 || roomB < 0 || roomA >= (int)icuGraph.size() || roomB >= (int)icuGraph.size()) return; 
        icuGraph[roomA].push_back(roomB);
        icuGraph[roomB].push_back(roomA); 
    }
    //Adds new patients to the ICU 
    void addPatient(const Patient & p ){
        patients.push_back(p);
        bedToPatient[p.bedNumber] = &patients.back();
        actionHistory.push("Added Patient  " + p.name + "to bed " + to_string(p.bedNumber));
    }


    // Find where a Patient is by their bed Number, uses hash table to keep it fast 
    Patient* findPatientByBed(int bedNumber){
        auto  iterator = bedToPatient.find(bedNumber);
        if(iterator != bedToPatient.end())
            return iterator->second;
        return nullptr;
    }
    // List all of the patitents that are currently in the ICU 
    void listPatients() const{
        cout << "Patients Currently In The ICU:\n ";
        for(const auto & p : patients){
            cout << "ID: " << p.id 
                 << ", Name: " << p.name
                 << ", Bed: " << p.bedNumber
                 << ", Diagnosis: " <<p.diagnosis
                 << ", Code: " << p.codeStatus 
                 << ", Blood Type: " <<p.bloodType
                 << ", Age: " <<p.age
                 << ", Height: " <<p.height
                 << ", Weight: " << p.weight
                 << " \n";
        }
    }
    // Adds alerts manually, such as fluids being low to the queue 
    void addAlert(const string & alert){
        alertQueue.push(alert);
        actionHistory.push("Alert Queued: " + alert);
    }
    // CLicked when an alert is done, moving to the next one
    void processNextAlert(){
        if(alertQueue.empty()){
            cout << " No alerts in queue. \n";
            return; 
        }
        
        string alert = alertQueue.front();
        alertQueue.pop();

        cout << "Processing alert: " << alert << "\n";
        actionHistory.push("Processed alert: " + alert);
    }
    // undo last action, only prints for now
    void undoLastAction(){
        if(actionHistory.empty()){
            cout << " No actions to undo. \n";
            return;
        }
        string last = actionHistory.top();
        actionHistory.pop();

        cout << "Undoing: " + last << "\n";
    }
    // Schedule tasks with a time, and a description and save them 
    void scheduleTask(int time, const string & description){
        Task t{time, description};
        taskTree.insertTask(t);
        actionHistory.push("Scheduled Task: " + description);
    }
    // List all cyurrently avaliable tasks
    void showTasks() const{
        taskTree.printTasksInOrder();
    }
    // Pathway using graph from one room to another , using the edges that are connected
    void fromRoom(int startRoom) const{
        if(startRoom < 0 || startRoom >= (int)icuGraph.size()){
            cout << "Not A Valid Starting Room. \n ";
            return;
        }
        
        vector<bool> visited(icuGraph.size(), false);
        queue<int> q; 

        visited[startRoom] = true;
        q.push(startRoom);

        cout << "Starting from room:  " << startRoom << ":";

        while(!q.empty()){
            int room = q.front();
            q.pop();
            cout << room << " ";
            
            for(int neighbor : icuGraph[room]){
                if(!visited[neighbor]){
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }
        cout << " \n";
    }
};

// Menu printing for various options 
void printMenu(){
    cout << "\n===== ICU Patient Management Menu ====\n ";
    cout << "1. Add Patients \n";
    cout << "2. List Patients \n";
    cout << "3. Find Patient by Bed Number \n";
    cout << "4. Schedule Task \n";
    cout << "5. Display Scheduled Tasks \n"; 
    cout << "6. Add Alerts \n";
    cout << "7. Process Next Alert \n";
    cout << "8. Undo Last Action \n";
    cout << "9. Run Route To Room \n ";
    cout << "0. Exit \n";
    cout << "Enter Choice:  ";
}
// The only valid integer range , everything else laeds to an error 
int rangeInt(int minVal, int maxVal){
    int choice;
    while(true){
        if(cin >> choice){
            if(choice >= minVal && choice <= maxVal)
                return choice;
            
            cout << "Enter A Number Between " <<  minVal << " and " << maxVal << ".\n";
        
        }
        else{
            cout << "Enter A Valid Number.\n";
            cin.clear();
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int main() {
    ICU_Unit icu(20);  // 20 rooms 
   
    for(int i =1; i < 20; i++){
        icu.addHallway(0 , i);
    }
// Main menu loop
    while(true){
        printMenu();

         int choice = rangeInt(0,9);
        // Exit Program 
        if(choice == 0){
            cout <<"Exiting Program. \n";
            break;
        }
        // Add Patient Information
        if(choice == 1){
            Patient p; 
            cout << " Enter Patient ID: \n";
            cin >> p.id;
            cin.ignore();

            cout <<"Enter Patient Name: \n ";
            getline(cin, p.name);

            cout <<"Enter Bed Number: \n";
            cin >> p.bedNumber;
            cin.ignore();

            cout <<"Enter Diagnosis: \n ";
            getline(cin, p.diagnosis);

            cout <<"Enter Code Status: \n ";
            getline (cin, p.codeStatus);

            cout <<"Enter Age: \n ";
            cin >> p.age;

            cout <<"Enter Height: \n ";
            cin >> p.height;
           
            cout <<"Enter Weight: \n ";
            cin >> p.weight;

            cout <<"Enter Blood Type: \n ";
            cin >> p.bloodType;

            icu.addPatient(p);
            cout <<"Patient Added";
        }// List all the Patients 
        else if( choice == 2){
            icu.listPatients();
        }
        else if(choice == 3){ // Find patient by bed Number 
            int bed; 
            cout << "Enter Bed Number: ";
            cin >> bed;

            Patient* found = icu.findPatientByBed(bed);
            if(found){
                cout << "Found Patient: \n";
                cout << "ID: " << found->id << "\n";
                cout << "Name: " << found->name << "\n";
                cout << "Diagnosis: " << found->diagnosis << "\n";
                cout << "Code Status: " << found->codeStatus << "\n";
                cout << "Age: " << found->age << "\n";
                cout << "Height: " << found->height << "\n";
                cout << "Weight: " << found->weight << "\n";
                cout << "Blood Type: " << found->bloodType << "\n";  
            }else {
                cout << "No Patient Occupying Bed. \n";
            }
        }
        else if(choice == 4 ){ // Schedule tasks 
            int time; 
            string description; 
            cout << "Enter Task time: ";
            cin >> time; 
            cin.ignore();
            cout << "Enter Task Description:  ";
            getline(cin, description);
            icu.scheduleTask(time, description);
            cout << "Task Scheduled Successfully \n";
        }
        else if(choice == 5){ // Show all Tasks
            icu.showTasks();
        }
        else if(choice == 6){ // Add Alerts 
            string alert; 
            cin.ignore();
            cout << "Enter Alert Description: ";
            getline(cin, alert);
            icu.addAlert(alert);
            cout << "Alert Added To Queue.\n";
        }
        else if (choice == 7){ // Process Alerts in queue 
            icu.processNextAlert();
        }
        else if(choice == 8){ // Undo last action
            icu.undoLastAction();
        }
        else if(choice == 9){ // Traverse through ICU with graph and connected edges
            int room;
            cout << "Enter Starting Room: ";
            cin >> room;
            icu.fromRoom(room);
        }
     }
     return 0;
    }
