#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

#define MAX_NAME 50
#define MAX_IC 20
#define MAX_PHONE 15
#define MAX_EMAIL 50
#define MAX_DATE 20
#define MAX_TIME 10
#define MAX_DEPT 30
#define MAX_STATUS 20
#define MAX_FILENAME 100

typedef struct Appointment {
    int patientId;
    char patientName[MAX_NAME + 1];
    char icNumber[MAX_IC + 1];
    char phoneNumber[MAX_PHONE + 1];
    char email[MAX_EMAIL + 1];
    char appointmentDate[MAX_DATE + 1];
    char appointmentTime[MAX_TIME + 1];
    char doctorName[MAX_NAME + 1];
    char department[MAX_DEPT + 1];
    char status[MAX_STATUS + 1];
    struct Appointment* next;
} Appointment;

typedef struct {
    Appointment* head;
    int nextId;
} AppointmentList;

void initList(AppointmentList* list) {
    list->head = NULL;
    list->nextId = 1;
}

int isEmpty(AppointmentList* list) {
    return list->head == NULL;
}

char* getValidStatus(int choice) {
    static char status[MAX_STATUS + 1];
    switch(choice) {
        case 1: strcpy(status, "Pending"); break;
        case 2: strcpy(status, "Confirmed"); break;
        case 3: strcpy(status, "Completed"); break;
        case 4: strcpy(status, "Cancelled"); break;
        default: strcpy(status, "Pending");
    }
    return status;
}

void inputString(char* dest, int maxLen, const char* prompt) {
    printf("%s", prompt);
    fgets(dest, maxLen + 1, stdin);
    dest[strcspn(dest, "\n")] = '\0';
    while(strlen(dest) == 0) {
        printf("Input cannot be empty. %s", prompt);
        fgets(dest, maxLen + 1, stdin);
        dest[strcspn(dest, "\n")] = '\0';
    }
}

void inputInt(int* dest, const char* prompt) {
    printf("%s", prompt);
    char buffer[50];
    fgets(buffer, 50, stdin);
    while(sscanf(buffer, "%d", dest) != 1 || *dest <= 0) {
        printf("Invalid input. %s", prompt);
        fgets(buffer, 50, stdin);
    }
}

void inputMenuChoice(int* dest, int min, int max) {
    printf("Enter choice (%d-%d): ", min, max);
    char buffer[50];
    fgets(buffer, 50, stdin);
    while(sscanf(buffer, "%d", dest) != 1 || *dest < min || *dest > max) {
        printf("Invalid choice. Enter choice (%d-%d): ", min, max);
        fgets(buffer, 50, stdin);
    }
}

void inputStatus(char* status) {
    printf("Select Status:\n");
    printf("1. Pending\n");
    printf("2. Confirmed\n");
    printf("3. Completed\n");
    printf("4. Cancelled\n");
    int choice;
    inputMenuChoice(&choice, 1, 4);
    strcpy(status, getValidStatus(choice));
}

Appointment* createAppointment(AppointmentList* list) {
    Appointment* newNode = (Appointment*)malloc(sizeof(Appointment));
    if(!newNode) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    newNode->patientId = list->nextId++;
    inputString(newNode->patientName, MAX_NAME, "Enter Patient Name: ");
    inputString(newNode->icNumber, MAX_IC, "Enter IC Number: ");
    inputString(newNode->phoneNumber, MAX_PHONE, "Enter Phone Number: ");
    inputString(newNode->email, MAX_EMAIL, "Enter Email: ");
    inputString(newNode->appointmentDate, MAX_DATE, "Enter Appointment Date: ");
    inputString(newNode->appointmentTime, MAX_TIME, "Enter Appointment Time: ");
    inputString(newNode->doctorName, MAX_NAME, "Enter Doctor Name: ");
    inputString(newNode->department, MAX_DEPT, "Enter Department: ");
    inputStatus(newNode->status);

    newNode->next = NULL;
    return newNode;
}

void insertSorted(AppointmentList* list, Appointment* newNode) {
    if(isEmpty(list) || strcasecmp(newNode->patientName, list->head->patientName) < 0) {
        newNode->next = list->head;
        list->head = newNode;
    } else {
        Appointment* current = list->head;
        while(current->next && strcasecmp(newNode->patientName, current->next->patientName) > 0) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

void addAppointment(AppointmentList* list) {
    Appointment* newNode = createAppointment(list);
    if(!newNode) return;

    insertSorted(list, newNode);
    printf("Appointment added successfully with Patient ID: %d\n", newNode->patientId);
}

int removeAppointment(AppointmentList* list) {
    if(isEmpty(list)) {
        printf("No appointments to remove.\n");
        return 0;
    }

    int patientId;
    inputInt(&patientId, "Enter Patient ID to remove: ");

    Appointment* current = list->head;
    Appointment* prev = NULL;

    while(current) {
        if(current->patientId == patientId) {
            if(prev) {
                prev->next = current->next;
            } else {
                list->head = current->next;
            }
            printf("Appointment for %s (ID: %d) has been removed.\n", 
                   current->patientName, current->patientId);
            free(current);
            return 1;
        }
        prev = current;
        current = current->next;
    }

    printf("Appointment with Patient ID %d not found.\n", patientId);
    return 0;
}

void updateAppointment(AppointmentList* list) {
    if(isEmpty(list)) {
        printf("No appointments to update.\n");
        return;
    }

    int patientId;
    inputInt(&patientId, "Enter Patient ID to update: ");

    Appointment* current = list->head;
    while(current) {
        if(current->patientId == patientId) {
            printf("\n--- Current Details ---\n");
            printf("Patient Name: %s\n", current->patientName);
            printf("IC Number: %s\n", current->icNumber);
            printf("Phone Number: %s\n", current->phoneNumber);
            printf("Email: %s\n", current->email);
            printf("Appointment Date: %s\n", current->appointmentDate);
            printf("Appointment Time: %s\n", current->appointmentTime);
            printf("Doctor Name: %s\n", current->doctorName);
            printf("Department: %s\n", current->department);
            printf("Status: %s\n", current->status);
            printf("------------------------\n\n");

            printf("Enter new details (press Enter to keep current value):\n");

            char temp[MAX_NAME + 1];
            inputString(temp, MAX_NAME, "Enter new Patient Name: ");
            if(strlen(temp) > 0) strcpy(current->patientName, temp);

            inputString(temp, MAX_IC, "Enter new IC Number: ");
            if(strlen(temp) > 0) strcpy(current->icNumber, temp);

            inputString(temp, MAX_PHONE, "Enter new Phone Number: ");
            if(strlen(temp) > 0) strcpy(current->phoneNumber, temp);

            inputString(temp, MAX_EMAIL, "Enter new Email: ");
            if(strlen(temp) > 0) strcpy(current->email, temp);

            inputString(temp, MAX_DATE, "Enter new Appointment Date: ");
            if(strlen(temp) > 0) strcpy(current->appointmentDate, temp);

            inputString(temp, MAX_TIME, "Enter new Appointment Time: ");
            if(strlen(temp) > 0) strcpy(current->appointmentTime, temp);

            inputString(temp, MAX_NAME, "Enter new Doctor Name: ");
            if(strlen(temp) > 0) strcpy(current->doctorName, temp);

            inputString(temp, MAX_DEPT, "Enter new Department: ");
            if(strlen(temp) > 0) strcpy(current->department, temp);

            inputStatus(current->status);

            printf("Appointment updated successfully!\n");
            return;
        }
        current = current->next;
    }

    printf("Appointment with Patient ID %d not found.\n", patientId);
}

void searchAppointment(AppointmentList* list) {
    if(isEmpty(list)) {
        printf("No appointments to search.\n");
        return;
    }

    int patientId;
    inputInt(&patientId, "Enter Patient ID to search: ");

    Appointment* current = list->head;
    while(current) {
        if(current->patientId == patientId) {
            printf("\n===== Appointment Found =====\n");
            printf("Patient ID: %d\n", current->patientId);
            printf("Patient Name: %s\n", current->patientName);
            printf("IC Number: %s\n", current->icNumber);
            printf("Phone Number: %s\n", current->phoneNumber);
            printf("Email: %s\n", current->email);
            printf("Appointment Date: %s\n", current->appointmentDate);
            printf("Appointment Time: %s\n", current->appointmentTime);
            printf("Doctor Name: %s\n", current->doctorName);
            printf("Department: %s\n", current->department);
            printf("Status: %s\n", current->status);
            printf("=============================\n\n");
            return;
        }
        current = current->next;
    }

    printf("Appointment with Patient ID %d not found.\n", patientId);
}

void listAllAppointments(AppointmentList* list) {
    if(isEmpty(list)) {
        printf("No appointments to display.\n");
        return;
    }

    printf("\n========== All Appointments ==========\n");
    printf("%-5s %-20s %-15s %-15s %-12s %-10s\n", 
           "ID", "Patient Name", "Doctor", "Department", "Date", "Status");
    printf("--------------------------------------------------------------------------------\n");

    Appointment* current = list->head;
    while(current) {
        printf("%-5d %-20s %-15s %-15s %-12s %-10s\n",
               current->patientId,
               current->patientName,
               current->doctorName,
               current->department,
               current->appointmentDate,
               current->status);
        current = current->next;
    }
    printf("======================================\n\n");
}

void filterByDepartment(AppointmentList* list) {
    if(isEmpty(list)) {
        printf("No appointments to filter.\n");
        return;
    }

    char department[MAX_DEPT + 1];
    inputString(department, MAX_DEPT, "Enter Department to filter: ");

    printf("\n======= Appointments in %s Department =======\n", department);
    printf("%-5s %-20s %-15s %-12s %-10s\n", 
           "ID", "Patient Name", "Doctor", "Date", "Status");
    printf("--------------------------------------------------------------------------------\n");

    int found = 0;
    Appointment* current = list->head;
    while(current) {
        if(strcasecmp(current->department, department) == 0) {
            printf("%-5d %-20s %-15s %-12s %-10s\n",
                   current->patientId,
                   current->patientName,
                   current->doctorName,
                   current->appointmentDate,
                   current->status);
            found = 1;
        }
        current = current->next;
    }

    if(!found) {
        printf("No appointments found in %s department.\n", department);
    }
    printf("=============================================\n\n");
}

void filterByDate(AppointmentList* list) {
    if(isEmpty(list)) {
        printf("No appointments to filter.\n");
        return;
    }

    char date[MAX_DATE + 1];
    inputString(date, MAX_DATE, "Enter Date to filter (YYYY-MM-DD): ");

    printf("\n======= Appointments on %s =======\n", date);
    printf("%-5s %-20s %-15s %-15s %-10s\n", 
           "ID", "Patient Name", "Doctor", "Department", "Status");
    printf("--------------------------------------------------------------------------------\n");

    int found = 0;
    Appointment* current = list->head;
    while(current) {
        if(strcmp(current->appointmentDate, date) == 0) {
            printf("%-5d %-20s %-15s %-15s %-10s\n",
                   current->patientId,
                   current->patientName,
                   current->doctorName,
                   current->department,
                   current->status);
            found = 1;
        }
        current = current->next;
    }

    if(!found) {
        printf("No appointments found on %s.\n", date);
    }
    printf("=====================================\n\n");
}

int countAppointments(AppointmentList* list) {
    int count = 0;
    Appointment* current = list->head;
    while(current) {
        count++;
        current = current->next;
    }
    return count;
}

void showStatistics(AppointmentList* list) {
    if(isEmpty(list)) {
        printf("No appointments to show statistics.\n");
        return;
    }

    int total = countAppointments(list);
    int pending = 0, confirmed = 0, completed = 0, cancelled = 0;

    Appointment* current = list->head;
    while(current) {
        if(strcasecmp(current->status, "Pending") == 0) pending++;
        else if(strcasecmp(current->status, "Confirmed") == 0) confirmed++;
        else if(strcasecmp(current->status, "Completed") == 0) completed++;
        else if(strcasecmp(current->status, "Cancelled") == 0) cancelled++;
        current = current->next;
    }

    printf("\n========== Appointment Statistics ==========\n");
    printf("Total Appointments: %d\n\n", total);
    printf("Status Breakdown:\n");
    printf("  Pending:    %d (%.1f%%)\n", pending, (float)pending / total * 100);
    printf("  Confirmed:  %d (%.1f%%)\n", confirmed, (float)confirmed / total * 100);
    printf("  Completed:  %d (%.1f%%)\n", completed, (float)completed / total * 100);
    printf("  Cancelled:  %d (%.1f%%)\n", cancelled, (float)cancelled / total * 100);
    printf("=============================================\n\n");
}

int saveToFile(AppointmentList* list) {
    if(isEmpty(list)) {
        printf("No appointments to save.\n");
        return 0;
    }

    char filename[MAX_FILENAME + 1];
    inputString(filename, MAX_FILENAME, "Enter filename to save: ");

    FILE* file = fopen(filename, "w");
    if(!file) {
        printf("Error: Could not open file for writing.\n");
        return 0;
    }

    Appointment* current = list->head;
    while(current) {
        fprintf(file, "%d|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
                current->patientId,
                current->patientName,
                current->icNumber,
                current->phoneNumber,
                current->email,
                current->appointmentDate,
                current->appointmentTime,
                current->doctorName,
                current->department,
                current->status);
        current = current->next;
    }

    fclose(file);
    printf("Appointments saved to %s successfully!\n", filename);
    return 1;
}

void loadFromFile(AppointmentList* list) {
    char filename[MAX_FILENAME + 1];
    inputString(filename, MAX_FILENAME, "Enter filename to load: ");

    FILE* file = fopen(filename, "r");
    if(!file) {
        printf("Error: Could not open file %s.\n", filename);
        return;
    }

    char line[500];
    int maxId = 0;

    while(fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        if(strlen(line) == 0) continue;

        Appointment* newNode = (Appointment*)malloc(sizeof(Appointment));
        if(!newNode) continue;

        sscanf(line, "%d|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]",
               &newNode->patientId,
               newNode->patientName,
               newNode->icNumber,
               newNode->phoneNumber,
               newNode->email,
               newNode->appointmentDate,
               newNode->appointmentTime,
               newNode->doctorName,
               newNode->department,
               newNode->status);

        if(newNode->patientId > maxId) {
            maxId = newNode->patientId;
        }

        newNode->next = NULL;
        insertSorted(list, newNode);
    }

    list->nextId = maxId + 1;
    fclose(file);
    printf("Appointments loaded from %s successfully!\n", filename);
}

void clearAllRecords(AppointmentList* list) {
    if(isEmpty(list)) {
        printf("No appointments to clear.\n");
        return;
    }

    printf("Are you sure you want to delete ALL appointments? (y/n): ");
    char confirm;
    scanf("%c", &confirm);
    while(getchar() != '\n');

    if(tolower(confirm) == 'y') {
        Appointment* current = list->head;
        while(current) {
            Appointment* temp = current;
            current = current->next;
            free(temp);
        }
        list->head = NULL;
        list->nextId = 1;
        printf("All appointments have been cleared.\n");
    } else {
        printf("Operation cancelled.\n");
    }
}

void displayMenu() {
    printf("\n=== Medical Appointment System ===\n");
    printf("1. Add New Appointment\n");
    printf("2. Remove Appointment\n");
    printf("3. Update Appointment\n");
    printf("4. Search Appointment\n");
    printf("5. View All Appointments\n");
    printf("6. Filter by Department\n");
    printf("7. Filter by Date\n");
    printf("8. Appointment Statistics\n");
    printf("9. Save to File\n");
    printf("10. Load from File\n");
    printf("11. Clear All Records\n");
    printf("12. Exit\n");
    printf("==================================\n");
}

int main() {
    AppointmentList list;
    initList(&list);

    int choice;
    while(1) {
        displayMenu();
        inputMenuChoice(&choice, 1, 12);

        switch(choice) {
            case 1:
                addAppointment(&list);
                break;
            case 2:
                removeAppointment(&list);
                break;
            case 3:
                updateAppointment(&list);
                break;
            case 4:
                searchAppointment(&list);
                break;
            case 5:
                listAllAppointments(&list);
                break;
            case 6:
                filterByDepartment(&list);
                break;
            case 7:
                filterByDate(&list);
                break;
            case 8:
                showStatistics(&list);
                break;
            case 9:
                saveToFile(&list);
                break;
            case 10:
                loadFromFile(&list);
                break;
            case 11:
                clearAllRecords(&list);
                break;
            case 12:
                printf("Thank you for using Medical Appointment System. Goodbye!\n");
                while(list.head) {
                    Appointment* temp = list.head;
                    list.head = list.head->next;
                    free(temp);
                }
                return 0;
        }
    }

    return 0;
}