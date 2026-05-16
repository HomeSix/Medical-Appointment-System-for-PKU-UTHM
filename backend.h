#ifndef BACKEND_H
#define BACKEND_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#define MAX_ID 15
#define MAX_NAME 50
#define MAX_IC 20
#define MAX_PHONE 15
#define MAX_EMAIL 50
#define MAX_FACULTY 30
#define MAX_PROGRAM 30
#define MAX_GENDER 10
#define MAX_BLOOD 5
#define MAX_ALLERGIES 100
#define MAX_EMERGENCY 15
#define MAX_DATE 15
#define MAX_TIME 10
#define MAX_DEPT 30
#define MAX_DOCTOR 50
#define MAX_STATUS 15
#define MAX_SYMPTOMS 200
#define MAX_DIAGNOSIS 200
#define MAX_PRESCRIPTION 200
#define MAX_REMARKS 200
#define MAX_USERNAME 30
#define MAX_PASSWORD 65
#define MAX_ROLE 15
#define MAX_FULLNAME 50
#define MAX_TIMESTAMP 25
#define MAX_DAYS 50
#define LINE_SIZE 600
#define HASH_TABLE_SIZE 101
#define MAX_UNDO 10
#define FILE_DIR "data/"

typedef struct Patient {
    char patientID[MAX_ID]; char name[MAX_NAME]; char icNumber[MAX_IC];
    char phone[MAX_PHONE]; char email[MAX_EMAIL]; char faculty[MAX_FACULTY];
    char program[MAX_PROGRAM]; char gender[MAX_GENDER]; int age;
    char bloodType[MAX_BLOOD]; char allergies[MAX_ALLERGIES];
    char emergencyContact[MAX_EMERGENCY];
    struct Patient* next; struct Patient* left; struct Patient* right;
} Patient;

typedef struct Appointment {
    char appointmentID[MAX_ID]; char patientID[MAX_ID];
    char date[MAX_DATE]; char time[MAX_TIME]; char department[MAX_DEPT];
    char doctorName[MAX_DOCTOR]; char status[MAX_STATUS];
    char symptoms[MAX_SYMPTOMS]; char diagnosis[MAX_DIAGNOSIS];
    char prescription[MAX_PRESCRIPTION]; char remarks[MAX_REMARKS];
    struct Appointment* next;
} Appointment;

typedef struct User {
    char username[MAX_USERNAME]; char password[MAX_PASSWORD];
    char role[MAX_ROLE]; char fullName[MAX_FULLNAME];
    char lastLogin[MAX_TIMESTAMP]; int isActive;
    struct User* next;
} User;

typedef struct Schedule {
    char department[MAX_DEPT]; char doctorName[MAX_DOCTOR];
    char availableDays[MAX_DAYS]; char timeStart[MAX_TIME];
    char timeEnd[MAX_TIME]; int maxPatientsPerDay; int currentBookings;
    struct Schedule* next;
} Schedule;

typedef struct QueueNode {
    char patientID[MAX_ID]; char name[MAX_NAME];
    char timeAdded[MAX_TIMESTAMP]; int isEmergency;
    char department[MAX_DEPT];
    struct QueueNode* next;
} QueueNode;

typedef struct { QueueNode* front; QueueNode* rear; int size; } Queue;

typedef struct StackNode {
    int actionType; int entityType;
    char actionData[LINE_SIZE]; char targetID[MAX_ID];
    struct StackNode* next;
} StackNode;

typedef struct { StackNode* top; int count; } Stack;

typedef struct PatientNode {
    char patientID[MAX_ID]; struct PatientNode* next;
} PatientNode;

typedef struct { PatientNode* table[HASH_TABLE_SIZE]; } HashTable;

extern Patient* patientList;
extern Patient* patientBST;
extern Appointment* appointmentList;
extern User* userList;
extern Schedule* scheduleList;
extern Queue walkInQueue;
extern Stack undoStack;
extern Stack redoStack;
extern HashTable patientHash;
extern User* currentUser;
extern time_t lastActivity;
extern int sessionTimeout;
extern int auditEnabled;

char* getCurrentTimestamp();
void appendAuditLog(const char* action, const char* id, const char* status);
void trimNewline(char* str);
void toUpperStr(char* str);

int validateDate(const char* date);
int validateTime(const char* time);
int validateEmail(const char* email);
int validatePhone(const char* phone);
int validateIC(const char* ic);
int validateAge(int age);
int validateBloodType(const char* bt);
int validateGender(const char* g);
int isLeapYear(int year);
int daysInMonth(int month, int year);
int dateCompare(const char* d1, const char* d2);

int hashFunction(const char* key);
void hashInsert(const char* patientID);
int hashSearch(const char* patientID);
void hashRemove(const char* patientID);

void generatePatientID(char* id);
void generateAppointmentID(char* id);

Patient* createPatientNode();
void insertPatientToList(Patient* p);
void insertPatientToBST(Patient** root, Patient* p);
Patient* searchPatientBST(Patient* root, const char* id);
Patient* searchPatientList(const char* id);
void insertPatient();
Patient* findPatientByID(const char* id);
Patient* findPatientByName(const char* name);
void updatePatientRecord();
void removePatientByID();
void displayAllPatients();
void displayPatientInOrder(Patient* root);
void displayPatientHistory();
int countPatients();

Appointment* createAppointmentNode();
void insertAppointmentToList(Appointment* a);
void insertAppointment();
Appointment* findAppointmentByID(const char* id);
void updateAppointment();
void cancelAppointment();
void completeAppointment();
void displayAllAppointments();
void displayAppointmentsByDate();
void displayTodayAppointments();

void enqueueWalkIn();
int dequeueWalkIn(QueueNode* out);
void displayQueueStatus();
void clearQueue();

void pushUndo(int actionType, int entityType, const char* targetID, const char* data);
int popUndo();
void undoLastOperation();
void pushRedo(int actionType, int entityType, const char* targetID, const char* data);
int popRedo();
void redoLastOperation();

void loadUsers();
void loadPatients();
void loadAppointments();
void loadSchedule();
void savePatients();
void saveAppointments();
void saveUsers();
void saveSchedule();
void saveAll();
void backupData();
void restoreFromBackup();

int login();
void logout();
void changePassword();
User* findUser(const char* username);
int authenticate(const char* username, const char* password);
int hasRole(const char* roles);

void generateMC();
void statisticsDashboard();
void searchByDateRange();
void advancedSearch();

void freeAll();

#endif
