/*
 * MEDICAL APPOINTMENT SYSTEM
 * Pusat Kesihatan Universiti UTHM (PKU UTHM)
 * Group 4 | BIK10903 Data Structure | Sem 2 2025/2026
 *
 * Data Structures Used:
 *   - Linked List: Dynamic storage of patients and appointments
 *   - Binary Search Tree: Fast patient search by ID
 *   - Queue: Walk-in patient management (FIFO)
 *   - Stack: Undo/Redo operations (LIFO)
 *   - Hash Table: Fast user authentication lookup
 *
 * Compilation: gcc medical_system.c -o medical_system -lm
 */

#define _POSIX_C_SOURCE 199309L
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <sys/stat.h>

#ifdef _WIN32
    #include <conio.h>
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
    #include <termios.h>
    #include <unistd.h>
#endif

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

static int currentYear = 2025;
static int currentSeq = 1;
static int currentAptSeq = 1;

/* ================================================================
 * SHA-256 IMPLEMENTATION (Public Domain)
 * ================================================================ */

typedef struct {
    unsigned char data[64];
    unsigned int datalen;
    unsigned long long bitlen;
    unsigned int state[8];
} SHA256_CTX;

#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))
#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

static const unsigned int k[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

static void sha256_transform(SHA256_CTX *ctx, const unsigned char data[]) {
    unsigned int a,b,c,d,e,f,g,h,i,j,t1,t2,m[64];
    for (i=0,j=0; i<16; ++i,j+=4) m[i]=(data[j]<<24)|(data[j+1]<<16)|(data[j+2]<<8)|(data[j+3]);
    for (; i<64; ++i) m[i]=SIG1(m[i-2])+m[i-7]+SIG0(m[i-15])+m[i-16];
    a=ctx->state[0];b=ctx->state[1];c=ctx->state[2];d=ctx->state[3];
    e=ctx->state[4];f=ctx->state[5];g=ctx->state[6];h=ctx->state[7];
    for (i=0; i<64; ++i) {
        t1=h+EP1(e)+CH(e,f,g)+k[i]+m[i]; t2=EP0(a)+MAJ(a,b,c);
        h=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;
    }
    ctx->state[0]+=a;ctx->state[1]+=b;ctx->state[2]+=c;ctx->state[3]+=d;
    ctx->state[4]+=e;ctx->state[5]+=f;ctx->state[6]+=g;ctx->state[7]+=h;
}

static void sha256_init(SHA256_CTX *ctx) {
    ctx->datalen=0; ctx->bitlen=0;
    ctx->state[0]=0x6a09e667;ctx->state[1]=0xbb67ae85;ctx->state[2]=0x3c6ef372;ctx->state[3]=0xa54ff53a;
    ctx->state[4]=0x510e527f;ctx->state[5]=0x9b05688c;ctx->state[6]=0x1f83d9ab;ctx->state[7]=0x5be0cd19;
}

static void sha256_update(SHA256_CTX *ctx, const unsigned char data[], unsigned int len) {
    for (unsigned int i=0; i<len; ++i) {
        ctx->data[ctx->datalen]=data[i]; ctx->datalen++;
        if (ctx->datalen==64){sha256_transform(ctx,ctx->data);ctx->bitlen+=512;ctx->datalen=0;}
    }
}

static void sha256_final(SHA256_CTX *ctx, unsigned char hash[]) {
    unsigned int i=ctx->datalen;
    if (ctx->datalen<56){ctx->data[i++]=0x80;while(i<56)ctx->data[i++]=0x00;}
    else{ctx->data[i++]=0x80;while(i<64)ctx->data[i++]=0x00;sha256_transform(ctx,ctx->data);memset(ctx->data,0,56);}
    ctx->bitlen+=ctx->datalen*8;ctx->data[63]=ctx->bitlen;ctx->data[62]=ctx->bitlen>>8;
    ctx->data[61]=ctx->bitlen>>16;ctx->data[60]=ctx->bitlen>>24;ctx->data[59]=ctx->bitlen>>32;
    ctx->data[58]=ctx->bitlen>>40;ctx->data[57]=ctx->bitlen>>48;ctx->data[56]=ctx->bitlen>>56;
    sha256_transform(ctx,ctx->data);
    for(i=0;i<4;++i){hash[i]=(ctx->state[0]>>(24-i*8))&0x000000ff;hash[i+4]=(ctx->state[1]>>(24-i*8))&0x000000ff;
    hash[i+8]=(ctx->state[2]>>(24-i*8))&0x000000ff;hash[i+12]=(ctx->state[3]>>(24-i*8))&0x000000ff;
    hash[i+16]=(ctx->state[4]>>(24-i*8))&0x000000ff;hash[i+20]=(ctx->state[5]>>(24-i*8))&0x000000ff;
    hash[i+24]=(ctx->state[6]>>(24-i*8))&0x000000ff;hash[i+28]=(ctx->state[7]>>(24-i*8))&0x000000ff;}
}

void sha256_string(const char *str, char output[65]) {
    SHA256_CTX ctx;
    unsigned char hash[32];
    sha256_init(&ctx);
    sha256_update(&ctx, (unsigned char*)str, strlen(str));
    sha256_final(&ctx, hash);
    for(int i=0;i<32;i++) sprintf(output+i*2,"%02x",hash[i]);
    output[64]='\0';
}

/* ================================================================
 * STRUCT DEFINITIONS
 * ================================================================ */

typedef struct Patient {
    char patientID[MAX_ID]; char name[MAX_NAME]; char icNumber[MAX_IC];
    char phone[MAX_PHONE]; char email[MAX_EMAIL]; char faculty[MAX_FACULTY];
    char program[MAX_PROGRAM]; char gender[MAX_GENDER]; int age;
    char bloodType[MAX_BLOOD]; char allergies[MAX_ALLERGIES];
    char emergencyContact[MAX_EMERGENCY];
    struct Patient* next;
    struct Patient* left; struct Patient* right;
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

typedef struct {
    QueueNode* front; QueueNode* rear; int size;
} Queue;

typedef struct StackNode {
    int actionType;  // 0=insert,1=delete,2=update
    char actionData[LINE_SIZE];
    char targetID[MAX_ID];
    int entityType;  // 0=patient,1=appointment
    struct StackNode* next;
} StackNode;

typedef struct {
    StackNode* top; int count;
} Stack;

typedef struct PatientNode {
    char patientID[MAX_ID];
    struct PatientNode* next;
} PatientNode;

typedef struct {
    PatientNode* table[HASH_TABLE_SIZE];
} HashTable;

/* ================================================================
 * GLOBAL VARIABLES
 * ================================================================ */

Patient* patientList = NULL;
Patient* patientBST = NULL;
Appointment* appointmentList = NULL;
User* userList = NULL;
Schedule* scheduleList = NULL;
Queue walkInQueue = {NULL, NULL, 0};
Stack undoStack = {NULL, 0};
Stack redoStack = {NULL, 0};
HashTable patientHash;
User* currentUser = NULL;
time_t lastActivity;
int sessionTimeout = 600;
int auditEnabled = 1;

/* ================================================================
 * FORWARD DECLARATIONS
 * ================================================================ */

void clearScreen();
void printHeader();
void printFooter();
void printBreadcrumb(const char* path);
void showLoadingBar(const char* msg, int percent);
void showSpinner();
void typewriter(const char* text, int delayMs);
void printSuccess(const char* msg);
void printError(const char* msg);
void printWarning(const char* msg);
void printInfo(const char* msg);
void initStructures();
void freeAll();
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
int stringToInt(const char* str);
int isLeapYear(int year);
int daysInMonth(int month, int year);
int dateToInt(const char* date);
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

void displayPatientMenu();
void displayAppointmentMenu();
void displayReportMenu();
void displaySearchMenu();
void displayQueueMenu();
void displayAdminMenu();
void mainMenu();
void manageUsers();

void generateMC();
void statisticsDashboard();
void searchByDateRange();
void advancedSearch();

/* ================================================================
 * ANSI COLORS & CONSOLE UTILITIES
 * ================================================================ */

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"

void clearScreen() {
    system(CLEAR);
}

void printHeader() {
    printf("%s%s", BOLD, CYAN);
    printf("╔══════════════════════════════════════════════════════════════════════╗\n");
    printf("║              PUSAT KESIHATAN UNIVERSITI UTHM                          ║\n");
    printf("║              Medical Appointment System v2.0                         ║\n");
    if (currentUser) {
        printf("║  Logged in as: %-20s                           ║\n",
               currentUser->fullName);
    }
    printf("╚══════════════════════════════════════════════════════════════════════╝%s\n", RESET);
}

void printFooter() {
    printf("%s%s", DIM, CYAN);
    printf("╔══════════════════════════════════════════════════════════════════════╗\n");
    printf("║  PKU UTHM Medical System v2.0  |  Group 4  |  BIK10903                 ║\n");
    printf("╚══════════════════════════════════════════════════════════════════════╝%s\n", RESET);
}

void printBreadcrumb(const char* path) {
    printf("%s%s >>> %s%s\n\n", BOLD, BLUE, path, RESET);
}

void showLoadingBar(const char* msg, int percent) {
    int barWidth = 20;
    int pos = barWidth * percent / 100;
    printf("\r%s%-40s%s [", DIM, msg, RESET);
    for (int i = 0; i < barWidth; i++) {
        if (i < pos) printf("%s█%s", GREEN, RESET);
        else printf("░");
    }
    printf("] %d%%", percent);
    fflush(stdout);
}

void showSpinner() {
    static const char spin[] = "⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏";
    static int i = 0;
    printf("\r%s%c Processing...%s", YELLOW, spin[i++ % 10], RESET);
    fflush(stdout);
}

void typewriter(const char* text, int delayMs) {
    for (size_t i = 0; i < strlen(text); i++) {
        putchar(text[i]);
        fflush(stdout);
        usleep(delayMs * 1000);
    }
}

void printSuccess(const char* msg) {
    printf("%s✓ SUCCESS:%s %s\n", GREEN, RESET, msg);
}

void printError(const char* msg) {
    printf("%s✗ ERROR:%s %s\n", RED, RESET, msg);
}

void printWarning(const char* msg) {
    printf("%s⚠ WARNING:%s %s\n", YELLOW, RESET, msg);
}

void printInfo(const char* msg) {
    printf("%sℹ INFO:%s %s\n", BLUE, RESET, msg);
}

/* ================================================================
 * INPUT & VALIDATION HELPERS
 * ================================================================ */

#define ANSI_BLUE "\033[34m"
#define ANSI_RESET "\033[0m"

char* getCurrentTimestamp() {
    static char buf[MAX_TIMESTAMP];
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
    return buf;
}

void trimNewline(char* str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len-1]=='\n'||str[len-1]=='\r'||str[len-1]==' ')) str[--len]='\0';
}

void toUpperStr(char* str) {
    for (int i=0; str[i]; i++) str[i]=toupper(str[i]);
}

int getchNoEcho() {
#ifdef _WIN32
    return getch();
#else
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

void inputPassword(char* dest, int maxLen) {
    int i = 0;
    char ch;
    while (i < maxLen - 1) {
        ch = getchNoEcho();
        if (ch == '\n' || ch == '\r') break;
        if (ch == 127 || ch == 8) {
            if (i > 0) { i--; printf("\b \b"); }
        } else {
            dest[i++] = ch;
            printf("*");
        }
    }
    dest[i] = '\0';
    printf("\n");
}

static void print_prompt(const char *p) {
    int len = strlen(p);
    while (len > 0 && (p[len-1]==':' || p[len-1]==' ')) len--;
    printf("%.*s (or press Enter to cancel): ", len, p);
}

void inputString(char* dest, int maxLen, const char* prompt) {
    print_prompt(prompt);
    if (fgets(dest, maxLen, stdin)) trimNewline(dest);
}

void inputStringOptional(char* dest, int maxLen, const char* prompt) {
    print_prompt(prompt);
    if (fgets(dest, maxLen, stdin)) trimNewline(dest);
}

int inputInt(const char* prompt) {
    char buf[50];
    int val;
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, sizeof(buf), stdin)) {
            trimNewline(buf);
            if (sscanf(buf, "%d", &val)==1 && val>=0) return val;
        }
        printf("Invalid input. Please enter a valid number.\n");
    }
}

int inputIntRange(const char* prompt, int min, int max) {
    int val;
    while (1) {
        val = inputInt(prompt);
        if (val>=min && val<=max) return val;
        printf("Please enter a value between %d and %d.\n", min, max);
    }
}

void inputMenuChoice(int* dest, int min, int max) {
    *dest = inputIntRange("Enter your choice: ", min, max);
}

void pressEnterToContinue() {
    printf("\nPress Enter to continue...");
    int c;
    do { c = getchar(); } while (c != '\n' && c != EOF);
}

/* ================================================================
 * VALIDATION FUNCTIONS
 * ================================================================ */

int isLeapYear(int year) {
    return (year%4==0 && year%100!=0) || (year%400==0);
}

int daysInMonth(int month, int year) {
    static int days[]={31,28,31,30,31,30,31,31,30,31,30,31};
    if (month==2 && isLeapYear(year)) return 29;
    return days[month-1];
}

int validateDate(const char* date) {
    if (strlen(date)!=10 || date[2]!='/' || date[5]!='/') return 0;
    int d,m,y;
    if (sscanf(date,"%d/%d/%d",&d,&m,&y)!=3) return 0;
    if (m<1||m>12||d<1||d>daysInMonth(m,y)) return 0;
    return 1;
}

int validateTime(const char* time) {
    if (strlen(time)<7||strlen(time)>9) return 0;
    int h,m;
    char ampm[3];
    if (sscanf(time,"%d:%d %2s",&h,&m,ampm)!=3) return 0;
    if (h<1||h>12||m<0||m>59) return 0;
    toUpperStr(ampm);
    if (strcmp(ampm,"AM")!=0&&strcmp(ampm,"PM")!=0) return 0;
    return 1;
}

int validateEmail(const char* email) {
    const char* at = strchr(email,'@');
    if (!at) return 0;
    const char* dot = strrchr(at,'.');
    if (!dot||dot==at+1||*(dot+1)=='\0') return 0;
    return 1;
}

int validatePhone(const char* phone) {
    int len=strlen(phone);
    if (len<10||len>15) return 0;
    if (phone[0]!='0'||phone[1]!='1') return 0;
    for (int i=0;i<len;i++) if(!isdigit(phone[i])) return 0;
    return 1;
}

int validateIC(const char* ic) {
    if (strlen(ic)!=14||ic[6]!='-'||ic[9]!='-') return 0;
    for(int i=0;i<14;i++) if(i!=6&&i!=9&&!isdigit(ic[i])) return 0;
    return 1;
}

int validateAge(int age) {
    return age>=1&&age<=120;
}

int validateBloodType(const char* bt) {
    const char* valid[]={"A+","A-","B+","B-","AB+","AB-","O+","O-","None",NULL};
    for(int i=0;valid[i];i++) if(strcasecmp(bt,valid[i])==0) return 1;
    return 0;
}

int validateGender(const char* g) {
    const char* valid[]={"Male","Female","Other",NULL};
    for(int i=0;valid[i];i++) if(strcasecmp(g,valid[i])==0) return 1;
    return 0;
}

int dateCompare(const char* d1, const char* d2) {
    int dd1,mm1,yy1,dd2,mm2,yy2;
    sscanf(d1,"%d/%d/%d",&dd1,&mm1,&yy1);
    sscanf(d2,"%d/%d/%d",&dd2,&mm2,&yy2);
    if (yy1!=yy2) return yy1-yy2;
    if (mm1!=mm2) return mm1-mm2;
    return dd1-dd2;
}

/* ================================================================
 * HASH TABLE (Patient ID lookup)
 * ================================================================ */

int hashFunction(const char* key) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++)) hash = ((hash << 5) + hash) + c;
    return hash % HASH_TABLE_SIZE;
}

void hashInsert(const char* patientID) {
    int idx = hashFunction(patientID);
    PatientNode* newNode = (PatientNode*)malloc(sizeof(PatientNode));
    if (!newNode) return;
    strcpy(newNode->patientID, patientID);
    newNode->next = patientHash.table[idx];
    patientHash.table[idx] = newNode;
}

int hashSearch(const char* patientID) {
    int idx = hashFunction(patientID);
    PatientNode* cur = patientHash.table[idx];
    while (cur) {
        if (strcmp(cur->patientID, patientID)==0) return 1;
        cur = cur->next;
    }
    return 0;
}

void hashRemove(const char* patientID) {
    int idx = hashFunction(patientID);
    PatientNode* cur = patientHash.table[idx];
    PatientNode* prev = NULL;
    while (cur) {
        if (strcmp(cur->patientID, patientID)==0) {
            if (prev) prev->next = cur->next;
            else patientHash.table[idx] = cur->next;
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

/* ================================================================
 * ID GENERATORS
 * ================================================================ */

void generatePatientID(char* id) {
    snprintf(id, MAX_ID, "PKU%02d%04d", currentYear % 100, currentSeq++);
    while (hashSearch(id)) {
        snprintf(id, MAX_ID, "PKU%02d%04d", currentYear % 100, currentSeq++);
    }
}

void generateAppointmentID(char* id) {
    int y = currentYear % 100;
    snprintf(id, MAX_ID, "APT%02d%04d", y, currentAptSeq++);
}

/* ================================================================
 * PATIENT OPERATIONS
 * ================================================================ */

Patient* createPatientNode() {
    Patient* p = (Patient*)malloc(sizeof(Patient));
    if (!p) return NULL;
    memset(p, 0, sizeof(Patient));
    p->next = NULL; p->left = NULL; p->right = NULL;
    return p;
}

void insertPatientToList(Patient* p) {
    p->next = patientList;
    patientList = p;
}

void insertPatientToBST(Patient** root, Patient* p) {
    if (!(*root)) { *root = p; return; }
    if (strcmp(p->patientID, (*root)->patientID) < 0)
        insertPatientToBST(&(*root)->left, p);
    else
        insertPatientToBST(&(*root)->right, p);
}

Patient* searchPatientBST(Patient* root, const char* id) {
    if (!root || strcmp(root->patientID, id)==0) return root;
    if (strcmp(id, root->patientID) < 0)
        return searchPatientBST(root->left, id);
    return searchPatientBST(root->right, id);
}

Patient* searchPatientList(const char* id) {
    Patient* cur = patientList;
    while (cur) {
        if (strcmp(cur->patientID, id)==0) return cur;
        cur = cur->next;
    }
    return NULL;
}

Patient* findPatientByID(const char* id) {
    Patient* result = searchPatientBST(patientBST, id);
    if (!result) result = searchPatientList(id);
    return result;
}

Patient* findPatientByName(const char* name) {
    Patient* cur = patientList;
    while (cur) {
        if (strstr(cur->name, name)) return cur;
        cur = cur->next;
    }
    return NULL;
}

void insertPatient() {
    Patient* p = createPatientNode();
    if (!p) { printError("Memory allocation failed!"); return; }

    generatePatientID(p->patientID);
    inputString(p->name, MAX_NAME, "Enter Full Name");
    if (strlen(p->name)==0) { free(p); printInfo("Cancelled."); return; }
    while (1) {
        inputString(p->icNumber, MAX_IC, "Enter IC Number (000000-00-0000)");
        if (strlen(p->icNumber)==0) { free(p); printInfo("Cancelled."); return; }
        if (validateIC(p->icNumber)) break;
        printError("Invalid IC format! Use: 000000-00-0000");
    }
    while (1) {
        inputString(p->phone, MAX_PHONE, "Enter Phone Number (01xxxxxxxxx)");
        if (strlen(p->phone)==0) { free(p); printInfo("Cancelled."); return; }
        if (validatePhone(p->phone)) break;
        printError("Invalid phone! Must be 10-11 digits starting with 01");
    }
    while (1) {
        inputString(p->email, MAX_EMAIL, "Enter Email");
        if (strlen(p->email)==0) { free(p); printInfo("Cancelled."); return; }
        if (validateEmail(p->email)) break;
        printError("Invalid email format!");
    }
    inputString(p->faculty, MAX_FACULTY, "Enter Faculty");
    if (strlen(p->faculty)==0) { free(p); printInfo("Cancelled."); return; }
    inputString(p->program, MAX_PROGRAM, "Enter Program");
    if (strlen(p->program)==0) { free(p); printInfo("Cancelled."); return; }
    while (1) {
        inputString(p->gender, MAX_GENDER, "Enter Gender (Male/Female/Other)");
        if (strlen(p->gender)==0) { free(p); printInfo("Cancelled."); return; }
        if (validateGender(p->gender)) break;
        printError("Invalid gender! Use Male, Female, or Other");
    }
    p->age = inputIntRange("Enter Age (1-120)", 1, 120);
    while (1) {
        inputString(p->bloodType, MAX_BLOOD, "Enter Blood Type (A+/A-/B+/B-/AB+/AB-/O+/O-)");
        if (strlen(p->bloodType)==0) { free(p); printInfo("Cancelled."); return; }
        if (validateBloodType(p->bloodType)) break;
        printError("Invalid blood type!");
    }
    inputStringOptional(p->allergies, MAX_ALLERGIES, "Enter Allergies (or None)");
    if (strlen(p->allergies)==0) strcpy(p->allergies, "None");
    inputString(p->emergencyContact, MAX_EMERGENCY, "Enter Emergency Contact");
    if (strlen(p->emergencyContact)==0) { free(p); printInfo("Cancelled."); return; }

    insertPatientToList(p);
    insertPatientToBST(&patientBST, p);
    hashInsert(p->patientID);

    char log[100]; snprintf(log, sizeof(log), "INSERT_PATIENT | ID: %s", p->patientID);
    appendAuditLog(log, p->patientID, "SUCCESS");
    printSuccess("Patient added successfully!");
    printf("Patient ID: %s\n", p->patientID);
}

void updatePatientRecord() {
    char id[MAX_ID];
    inputString(id, MAX_ID, "Enter Patient ID to update");
    if (strlen(id)==0) { printInfo("Cancelled."); return; }
    Patient* p = findPatientByID(id);
    if (!p) { printError("Patient not found!"); return; }

    printf("\n%s--- Current Details ---%s\n", BOLD, RESET);
    printf("Name: %s\nIC: %s\nPhone: %s\nEmail: %s\nFaculty: %s\nProgram: %s\n",
           p->name, p->icNumber, p->phone, p->email, p->faculty, p->program);
    printf("Gender: %s\nAge: %d\nBlood: %s\nAllergies: %s\nEmergency: %s\n",
           p->gender, p->age, p->bloodType, p->allergies, p->emergencyContact);

    printf("\nEnter new details (press Enter to keep current):\n");
    char temp[MAX_NAME];
    inputStringOptional(temp, MAX_NAME, "Name: "); if(strlen(temp)) strcpy(p->name,temp);
    inputStringOptional(temp, MAX_IC, "IC: "); if(strlen(temp)) strcpy(p->icNumber,temp);
    inputStringOptional(temp, MAX_PHONE, "Phone: "); if(strlen(temp)) strcpy(p->phone,temp);
    inputStringOptional(temp, MAX_EMAIL, "Email: "); if(strlen(temp)) strcpy(p->email,temp);
    inputStringOptional(temp, MAX_FACULTY, "Faculty: "); if(strlen(temp)) strcpy(p->faculty,temp);
    inputStringOptional(temp, MAX_PROGRAM, "Program: "); if(strlen(temp)) strcpy(p->program,temp);
    inputStringOptional(temp, MAX_GENDER, "Gender: "); if(strlen(temp)) strcpy(p->gender,temp);
    int newAge = inputInt("Age (0 to keep): "); if(newAge>0) p->age=newAge;
    inputStringOptional(temp, MAX_BLOOD, "Blood Type: "); if(strlen(temp)) strcpy(p->bloodType,temp);
    inputStringOptional(temp, MAX_ALLERGIES, "Allergies: "); if(strlen(temp)) strcpy(p->allergies,temp);
    inputStringOptional(temp, MAX_EMERGENCY, "Emergency Contact: "); if(strlen(temp)) strcpy(p->emergencyContact,temp);
    savePatients();

    char log[100]; snprintf(log, sizeof(log), "UPDATE_PATIENT | ID: %s", id);
    appendAuditLog(log, id, "SUCCESS");
    printSuccess("Patient record updated!");
}

void removePatientByID() {
    char id[MAX_ID];
    inputString(id, MAX_ID, "Enter Patient ID to remove");
    if (strlen(id)==0) { printInfo("Cancelled."); return; }
    Patient* p = findPatientByID(id);
    if (!p) { printError("Patient not found!"); return; }

    printf("Are you sure you want to delete patient %s (%s)? (y/n): ", p->patientID, p->name);
    char confirm = getchar(); while(getchar()!='\n');
    if (tolower(confirm)!='y') { printInfo("Operation cancelled."); return; }

    Patient* cur = patientList;
    Patient* prev = NULL;
    while (cur) {
        if (strcmp(cur->patientID, id)==0) {
            if (prev) prev->next = cur->next;
            else patientList = cur->next;
            break;
        }
        prev = cur; cur = cur->next;
    }

    hashRemove(id);
    char log[100]; snprintf(log, sizeof(log), "DELETE_PATIENT | ID: %s", id);
    appendAuditLog(log, id, "SUCCESS");
    printSuccess("Patient removed!");
    free(p);
}

void displayPatientInOrder(Patient* root) {
    if (!root) return;
    displayPatientInOrder(root->left);
    printf("%-12s %-25s %-15s %-10s %-3d %-5s %-10s\n",
           root->patientID, root->name, root->icNumber, root->gender, root->age, root->bloodType, root->phone);
    displayPatientInOrder(root->right);
}

void displayAllPatients() {
    if (!patientBST) { printInfo("No patients registered."); return; }
    printf("\n%s%-12s %-25s %-15s %-10s %-3s %-5s %-10s%s\n", BOLD,
           "Patient ID", "Name", "IC Number", "Gender", "Age", "Blood", "Phone", RESET);
    printf("%s----------------------------------------------------------------------------------------%s\n", DIM, RESET);
    displayPatientInOrder(patientBST);
}

void displayPatientHistory() {
    char id[MAX_ID];
    inputString(id, MAX_ID, "Enter Patient ID");
    if (strlen(id)==0) { printInfo("Cancelled."); return; }
    Patient* p = findPatientByID(id);
    if (!p) { printError("Patient not found!"); return; }

    printf("\n%sAppointment History for %s (%s)%s\n", BOLD, p->name, p->patientID, RESET);
    printf("%s%-15s %-12s %-10s %-25s %-15s %-15s%s\n", BOLD,
           "Apt ID", "Date", "Time", "Doctor", "Department", "Status", RESET);
    printf("%s----------------------------------------------------------------------------------------%s\n", DIM, RESET);

    Appointment* cur = appointmentList;
    int found = 0;
    while (cur) {
        if (strcmp(cur->patientID, id)==0) {
            printf("%-15s %-12s %-10s %-25s %-15s %-15s\n",
                   cur->appointmentID, cur->date, cur->time,
                   cur->doctorName, cur->department, cur->status);
            found = 1;
        }
        cur = cur->next;
    }
    if (!found) printInfo("No appointment history found.");
}

int countPatients() {
    int count = 0;
    Patient* cur = patientList;
    while (cur) { count++; cur = cur->next; }
    return count;
}

/* ================================================================
 * APPOINTMENT OPERATIONS
 * ================================================================ */

Appointment* createAppointmentNode() {
    Appointment* a = (Appointment*)malloc(sizeof(Appointment));
    if (!a) return NULL;
    memset(a, 0, sizeof(Appointment));
    a->next = NULL;
    return a;
}

void insertAppointmentToList(Appointment* a) {
    if (!appointmentList || dateCompare(a->date, appointmentList->date) < 0 ||
        (dateCompare(a->date, appointmentList->date)==0 && strcmp(a->time, appointmentList->time)<0)) {
        a->next = appointmentList;
        appointmentList = a;
        return;
    }
    Appointment* cur = appointmentList;
    while (cur->next) {
        int cmp = dateCompare(a->date, cur->next->date);
        if (cmp<0 || (cmp==0 && strcmp(a->time, cur->next->time)<0)) break;
        cur = cur->next;
    }
    a->next = cur->next;
    cur->next = a;
}

int hasConflict(const char* date, const char* time, const char* doctor, const char* patientID) {
    Appointment* cur = appointmentList;
    while (cur) {
        if (strcmp(cur->doctorName, doctor)==0 && strcmp(cur->date, date)==0 &&
            strcmp(cur->time, time)==0 && strcmp(cur->status, "Scheduled")==0) {
            return 1;
        }
        if (strcmp(cur->patientID, patientID)==0 && strcmp(cur->date, date)==0 &&
            strcmp(cur->time, time)==0 && strcmp(cur->status, "Scheduled")==0) {
            printWarning("Patient already has an appointment at this time!");
            return 1;
        }
        cur = cur->next;
    }
    return 0;
}

void insertAppointment() {
    Appointment* a = createAppointmentNode();
    if (!a) { printError("Memory allocation failed!"); return; }

    generateAppointmentID(a->appointmentID);
    strcpy(a->status, "Scheduled");
    strcpy(a->diagnosis, "Pending");
    strcpy(a->prescription, "Pending");

    inputString(a->patientID, MAX_ID, "Enter Patient ID");
    if (strlen(a->patientID)==0) { free(a); printInfo("Cancelled."); return; }
    if (!findPatientByID(a->patientID)) {
        printError("Patient not found! Please register patient first."); free(a); return;
    }

    while (1) {
        inputString(a->date, MAX_DATE, "Enter Date (DD/MM/YYYY)");
        if (strlen(a->date)==0) { free(a); printInfo("Cancelled."); return; }
        if (validateDate(a->date)) break;
        printError("Invalid date! Use DD/MM/YYYY format.");
    }
    while (1) {
        inputString(a->time, MAX_TIME, "Enter Time (HH:MM AM/PM)");
        if (strlen(a->time)==0) { free(a); printInfo("Cancelled."); return; }
        if (validateTime(a->time)) break;
        printError("Invalid time! Use HH:MM AM/PM.");
    }

    inputString(a->department, MAX_DEPT, "Enter Department");
    if (strlen(a->department)==0) { free(a); printInfo("Cancelled."); return; }
    inputString(a->doctorName, MAX_DOCTOR, "Enter Doctor Name");
    if (strlen(a->doctorName)==0) { free(a); printInfo("Cancelled."); return; }

    if (hasConflict(a->date, a->time, a->doctorName, a->patientID)) {
        printError("Time conflict detected!"); free(a); return;
    }

    inputStringOptional(a->symptoms, MAX_SYMPTOMS, "Enter Symptoms: ");
    if (strlen(a->symptoms)==0) strcpy(a->symptoms, "Not specified");
    if (strlen(a->remarks)==0) strcpy(a->remarks, "N/A");

    insertAppointmentToList(a);
    char log[100]; snprintf(log, sizeof(log), "INSERT_APPOINTMENT | ID: %s", a->appointmentID);
    appendAuditLog(log, a->appointmentID, "SUCCESS");
    printSuccess("Appointment booked!");
    printf("Appointment ID: %s\n", a->appointmentID);
}

Appointment* findAppointmentByID(const char* id) {
    Appointment* cur = appointmentList;
    while (cur) {
        if (strcmp(cur->appointmentID, id)==0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void updateAppointment() {
    char id[MAX_ID];
    inputString(id, MAX_ID, "Enter Appointment ID to update");
    if (strlen(id)==0) { printInfo("Cancelled."); return; }
    Appointment* a = findAppointmentByID(id);
    if (!a) { printError("Appointment not found!"); return; }

    printf("\n%s--- Current Details ---%s\n", BOLD, RESET);
    printf("ID: %s\nPatient: %s\nDate: %s\nTime: %s\nDoctor: %s\nDept: %s\nStatus: %s\n",
           a->appointmentID, a->patientID, a->date, a->time, a->doctorName, a->department, a->status);

    printf("\nEnter new details (press Enter to keep current):\n");
    char temp[MAX_DATE];
    inputStringOptional(temp, MAX_DATE, "Date: "); if(strlen(temp)) strcpy(a->date,temp);
    inputStringOptional(temp, MAX_TIME, "Time: "); if(strlen(temp)) strcpy(a->time,temp);
    inputStringOptional(temp, MAX_DEPT, "Department: "); if(strlen(temp)) strcpy(a->department,temp);
    inputStringOptional(temp, MAX_DOCTOR, "Doctor: "); if(strlen(temp)) strcpy(a->doctorName,temp);
    inputStringOptional(temp, MAX_SYMPTOMS, "Symptoms: "); if(strlen(temp)) strcpy(a->symptoms,temp);
    saveAppointments();

    char log[100]; snprintf(log, sizeof(log), "UPDATE_APPOINTMENT | ID: %s", id);
    appendAuditLog(log, id, "SUCCESS");
    printSuccess("Appointment updated!");
}

void cancelAppointment() {
    char id[MAX_ID];
    inputString(id, MAX_ID, "Enter Appointment ID to cancel");
    if (strlen(id)==0) { printInfo("Cancelled."); return; }
    Appointment* a = findAppointmentByID(id);
    if (!a) { printError("Appointment not found!"); return; }

    printf("Cancel appointment %s for patient %s? (y/n): ", a->appointmentID, a->patientID);
    char confirm = getchar(); while(getchar()!='\n');
    if (tolower(confirm)!='y') { printInfo("Cancellation aborted."); return; }

    strcpy(a->status, "Cancelled");
    saveAppointments();
    char log[100]; snprintf(log, sizeof(log), "CANCEL_APPOINTMENT | ID: %s", id);
    appendAuditLog(log, id, "SUCCESS");
    printSuccess("Appointment cancelled!");
}

void completeAppointment() {
    char id[MAX_ID];
    inputString(id, MAX_ID, "Enter Appointment ID to complete");
    if (strlen(id)==0) { printInfo("Cancelled."); return; }
    Appointment* a = findAppointmentByID(id);
    if (!a) { printError("Appointment not found!"); return; }

    if (strcmp(a->status, "Scheduled")!=0) {
        printWarning("Appointment is not in Scheduled status!"); return;
    }

    inputStringOptional(a->diagnosis, MAX_DIAGNOSIS, "Enter Diagnosis: ");
    inputStringOptional(a->prescription, MAX_PRESCRIPTION, "Enter Prescription: ");
    inputStringOptional(a->remarks, MAX_REMARKS, "Enter Remarks: ");
    strcpy(a->status, "Completed");
    saveAppointments();

    char log[100]; snprintf(log, sizeof(log), "COMPLETE_APPOINTMENT | ID: %s", id);
    appendAuditLog(log, id, "SUCCESS");
    printSuccess("Appointment completed!");

    printf("Generate Medical Certificate? (y/n): ");
    char confirm = getchar(); while(getchar()!='\n');
    if (tolower(confirm)=='y') {
        Patient* p = findPatientByID(a->patientID);
        if (p) {
            mkdir("data/mc", 0755);
            char mcFile[100];
            snprintf(mcFile, sizeof(mcFile), "data/mc/MC_%s.txt", a->appointmentID);
            FILE* f = fopen(mcFile, "w");
            if (f) {
                fprintf(f, "═══════════════════════════════════════════════════════════\n");
                fprintf(f, "          PUSAT KESIHATAN UNIVERSITI UTHM\n");
                fprintf(f, "              MEDICAL CERTIFICATE\n");
                fprintf(f, "═══════════════════════════════════════════════════════════\n\n");
                fprintf(f, "Date Issued: %s\n\n", getCurrentTimestamp());
                fprintf(f, "This is to certify that:\n\n");
                fprintf(f, "  Name:            %s\n", p->name);
                fprintf(f, "  IC Number:       %s\n", p->icNumber);
                fprintf(f, "  Patient ID:      %s\n\n", p->patientID);
                fprintf(f, "was examined at PKU UTHM on %s.\n\n", a->date);
                fprintf(f, "  Diagnosis:  %s\n", a->diagnosis);
                fprintf(f, "  Remarks:    %s\n\n", a->remarks);
                fprintf(f, "  Doctor:     %s\n", a->doctorName);
                fprintf(f, "  Department: %s\n\n", a->department);
                fprintf(f, "═══════════════════════════════════════════════════════════\n");
                fprintf(f, "  Generated: %s\n", getCurrentTimestamp());
                fprintf(f, "═══════════════════════════════════════════════════════════\n");
                fclose(f);
                printSuccess("Medical Certificate generated!");
                printf("File: %s\n", mcFile);
            }
        }
    }
}

void displayAllAppointments() {
    if (!appointmentList) { printInfo("No appointments."); return; }
    printf("\n%s%-15s %-12s %-12s %-10s %-25s %-15s %-15s%s\n", BOLD,
           "Apt ID", "Patient ID", "Date", "Time", "Doctor", "Department", "Status", RESET);
    printf("%s--------------------------------------------------------------------------------------------------%s\n", DIM, RESET);
    Appointment* cur = appointmentList;
    while (cur) {
        printf("%-15s %-12s %-12s %-10s %-25s %-15s %-15s\n",
               cur->appointmentID, cur->patientID, cur->date, cur->time,
               cur->doctorName, cur->department, cur->status);
        cur = cur->next;
    }
}

void displayAppointmentsByDate() {
    char date[MAX_DATE];
    inputString(date, MAX_DATE, "Enter Date (DD/MM/YYYY)");
    if (strlen(date)==0) { printInfo("Cancelled."); return; }
    if (!validateDate(date)) { printError("Invalid date!"); return; }

    printf("\n%sAppointments on %s%s\n", BOLD, date, RESET);
    printf("%s%-15s %-12s %-10s %-25s %-15s %-15s%s\n", BOLD,
           "Apt ID", "Patient ID", "Time", "Doctor", "Department", "Status", RESET);
    Appointment* cur = appointmentList;
    int found = 0;
    while (cur) {
        if (strcmp(cur->date, date)==0) {
            printf("%-15s %-12s %-10s %-25s %-15s %-15s\n",
                   cur->appointmentID, cur->patientID, cur->time,
                   cur->doctorName, cur->department, cur->status);
            found = 1;
        }
        cur = cur->next;
    }
    if (!found) printInfo("No appointments on this date.");
}

void displayTodayAppointments() {
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    char date[MAX_DATE];
    int day = tm->tm_mday, mon = tm->tm_mon + 1, yr = tm->tm_year + 1900;
    snprintf(date, MAX_DATE, "%02d/%02d/%04d", day, mon, yr);

    printf("\n%sToday's Appointments (%s)%s\n", BOLD, date, RESET);
    printf("%s%-15s %-12s %-10s %-25s %-15s %-15s%s\n", BOLD,
           "Apt ID", "Patient ID", "Time", "Doctor", "Department", "Status", RESET);
    Appointment* cur = appointmentList;
    int found = 0;
    while (cur) {
        if (strcmp(cur->date, date)==0) {
            printf("%-15s %-12s %-10s %-25s %-15s %-15s\n",
                   cur->appointmentID, cur->patientID, cur->time,
                   cur->doctorName, cur->department, cur->status);
            found = 1;
        }
        cur = cur->next;
    }
    if (!found) printInfo("No appointments scheduled for today.");
}

/* ================================================================
 * QUEUE OPERATIONS (Walk-in Queue)
 * ================================================================ */

void enqueueWalkIn() {
    QueueNode* q = (QueueNode*)malloc(sizeof(QueueNode));
    if (!q) { printError("Memory allocation failed!"); return; }
    inputString(q->patientID, MAX_ID, "Enter Patient ID");
    if (strlen(q->patientID)==0) { free(q); printInfo("Cancelled."); return; }
    Patient* p = findPatientByID(q->patientID);
    if (!p) { printError("Patient not found!"); free(q); return; }
    strcpy(q->name, p->name);
    strcpy(q->timeAdded, getCurrentTimestamp());
    q->isEmergency = 0;
    inputStringOptional(q->department, MAX_DEPT, "Enter Department: ");
    q->next = NULL;

    printf("Is this an emergency? (y/n): ");
    char c = getchar(); while(getchar()!='\n');
    if (tolower(c)=='y') q->isEmergency = 1;

    if (q->isEmergency) {
        QueueNode* cur = walkInQueue.front;
        QueueNode* prev = NULL;
        while (cur && !cur->isEmergency) { prev = cur; cur = cur->next; }
        if (!prev) { q->next = walkInQueue.front; walkInQueue.front = q; }
        else { q->next = cur; prev->next = q; }
        if (!q->next) walkInQueue.rear = q;
        printWarning("Emergency case placed at front of queue!");
    } else {
        if (!walkInQueue.rear) { walkInQueue.front = walkInQueue.rear = q; }
        else { walkInQueue.rear->next = q; walkInQueue.rear = q; }
    }
    walkInQueue.size++;
    printSuccess("Added to walk-in queue!");
}

int dequeueWalkIn(QueueNode* out) {
    if (!walkInQueue.front) { printError("Queue is empty!"); return 0; }
    QueueNode* temp = walkInQueue.front;
    *out = *temp;
    walkInQueue.front = temp->next;
    if (!walkInQueue.front) walkInQueue.rear = NULL;
    walkInQueue.size--;
    free(temp);
    return 1;
}

void displayQueueStatus() {
    if (!walkInQueue.front) { printInfo("Walk-in queue is empty."); return; }
    printf("\n%sWalk-in Queue Status (%d patients)%s\n", BOLD, walkInQueue.size, RESET);
    printf("%s%-12s %-25s %-10s %-20s %-15s%s\n", BOLD,
           "Patient ID", "Name", "Emergency", "Time Added", "Department", RESET);
    QueueNode* cur = walkInQueue.front;
    int pos = 1;
    while (cur) {
        printf("%-2d. %-12s %-25s %-10s %-20s %-15s\n",
               pos++, cur->patientID, cur->name,
               cur->isEmergency ? "🚨 YES" : "No",
               cur->timeAdded, cur->department);
        cur = cur->next;
    }
}

void clearQueue() {
    if (!walkInQueue.front) { printInfo("Queue already empty."); return; }
    printf("Clear entire queue? (y/n): ");
    char c = getchar(); while(getchar()!='\n');
    if (tolower(c)!='y') return;
    while (walkInQueue.front) {
        QueueNode* temp = walkInQueue.front;
        walkInQueue.front = temp->next;
        free(temp);
    }
    walkInQueue.rear = NULL;
    walkInQueue.size = 0;
    printSuccess("Queue cleared!");
}

/* ================================================================
 * STACK OPERATIONS (Undo/Redo)
 * ================================================================ */

void pushUndo(int actionType, int entityType, const char* targetID, const char* data) {
    if (undoStack.count >= MAX_UNDO) {
        StackNode* temp = undoStack.top;
        while (temp && temp->next && temp->next->next) temp = temp->next;
        if (temp && temp->next) { free(temp->next); temp->next = NULL; undoStack.count--; }
    }
    StackNode* s = (StackNode*)malloc(sizeof(StackNode));
    if (!s) return;
    s->actionType = actionType; s->entityType = entityType;
    strcpy(s->targetID, targetID); strcpy(s->actionData, data);
    s->next = undoStack.top;
    undoStack.top = s;
    undoStack.count++;
}

int popUndo() {
    if (!undoStack.top) { printInfo("Nothing to undo."); return 0; }
    StackNode* temp = undoStack.top;
    undoStack.top = temp->next;
    undoStack.count--;

    pushRedo(temp->actionType, temp->entityType, temp->targetID, temp->actionData);

    if (temp->actionType == 0 && temp->entityType == 0) {
        // Undo insert patient -> delete
        removePatientByID();
    } else if (temp->actionType == 1 && temp->entityType == 0) {
        // Undo delete patient -> re-insert (simplified)
        printInfo("Cannot fully undo patient deletion automatically.");
    }
    free(temp);
    return 1;
}

void pushRedo(int actionType, int entityType, const char* targetID, const char* data) {
    if (redoStack.count >= MAX_UNDO) {
        StackNode* temp = redoStack.top;
        while (temp && temp->next && temp->next->next) temp = temp->next;
        if (temp && temp->next) { free(temp->next); temp->next = NULL; redoStack.count--; }
    }
    StackNode* s = (StackNode*)malloc(sizeof(StackNode));
    if (!s) return;
    s->actionType = actionType; s->entityType = entityType;
    strcpy(s->targetID, targetID); strcpy(s->actionData, data);
    s->next = redoStack.top; redoStack.top = s; redoStack.count++;
}

int popRedo() {
    if (!redoStack.top) { printInfo("Nothing to redo."); return 0; }
    StackNode* temp = redoStack.top;
    redoStack.top = temp->next; redoStack.count--;
    pushUndo(temp->actionType, temp->entityType, temp->targetID, temp->actionData);
    printInfo("Redo operation: would re-apply previous action.");
    free(temp);
    return 1;
}

void undoLastOperation() { popUndo(); }
void redoLastOperation() { popRedo(); }

/* ================================================================
 * FILE I/O OPERATIONS
 * ================================================================ */

void appendAuditLog(const char* action, const char* id, const char* status) {
    (void)id;
    if (!auditEnabled) return;
    char path[100]; snprintf(path, sizeof(path), "%saudit_log.txt", FILE_DIR);
    FILE* f = fopen(path, "a");
    if (!f) return;
    fprintf(f, "[%s] USER: %s | ACTION: %s | STATUS: %s\n",
            getCurrentTimestamp(),
            currentUser ? currentUser->username : "SYSTEM",
            action, status);
    fclose(f);
}

void loadUsers() {
    char path[100]; snprintf(path, sizeof(path), "%susers.txt", FILE_DIR);
    FILE* f = fopen(path, "r");
    if (!f) return;
    char line[LINE_SIZE];
    while (fgets(line, sizeof(line), f)) {
        trimNewline(line);
        if (strlen(line)==0) continue;
        User* u = (User*)malloc(sizeof(User));
        if (!u) continue;
        if (sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%d",
                   u->username, u->password, u->role, u->fullName,
                   u->lastLogin, &u->isActive) == 6) {
            u->next = userList;
            userList = u;
        } else free(u);
    }
    fclose(f);
}

void loadPatients() {
    char path[100]; snprintf(path, sizeof(path), "%spatients.txt", FILE_DIR);
    FILE* f = fopen(path, "r");
    if (!f) return;
    char line[LINE_SIZE];
    while (fgets(line, sizeof(line), f)) {
        trimNewline(line); if (strlen(line)==0) continue;
        Patient* p = createPatientNode();
        if (!p) continue;
        if (sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%d|%[^|]|%[^|]|%[^|]",
                   p->patientID, p->name, p->icNumber, p->phone, p->email,
                   p->faculty, p->program, p->gender, &p->age,
                   p->bloodType, p->allergies, p->emergencyContact) == 12) {
            insertPatientToList(p);
            insertPatientToBST(&patientBST, p);
            hashInsert(p->patientID);
        } else free(p);
    }
    Patient* cur = patientList;
    int maxSeq = 0;
    while (cur) {
        int seq, yr;
        if (sscanf(cur->patientID, "PKU%2d%4d", &yr, &seq) == 2) {
            if (seq > maxSeq) maxSeq = seq;
        }
        cur = cur->next;
    }
    if (maxSeq >= currentSeq) currentSeq = maxSeq + 1;
    fclose(f);
}

void loadAppointments() {
    char path[100]; snprintf(path, sizeof(path), "%sappointments.txt", FILE_DIR);
    FILE* f = fopen(path, "r");
    if (!f) return;
    char line[LINE_SIZE];
    while (fgets(line, sizeof(line), f)) {
        trimNewline(line); if (strlen(line)==0) continue;
        Appointment* a = createAppointmentNode();
        if (!a) continue;
        if (sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]",
                   a->appointmentID, a->patientID, a->date, a->time,
                   a->department, a->doctorName, a->status,
                   a->symptoms, a->diagnosis, a->prescription, a->remarks) == 11) {
            insertAppointmentToList(a);
        } else free(a);
    }
    fclose(f);

    Appointment *acur = appointmentList;
    int maxAptSeq = 0;
    while (acur) {
        int seq, yr;
        if (sscanf(acur->appointmentID, "APT%2d%4d", &yr, &seq) == 2) {
            if (seq > maxAptSeq) maxAptSeq = seq;
        }
        acur = acur->next;
    }
    if (maxAptSeq >= currentAptSeq) currentAptSeq = maxAptSeq + 1;
}

void loadSchedule() {
    char path[100]; snprintf(path, sizeof(path), "%sschedule.txt", FILE_DIR);
    FILE* f = fopen(path, "r");
    if (!f) return;
    char line[LINE_SIZE];
    while (fgets(line, sizeof(line), f)) {
        trimNewline(line); if (strlen(line)==0) continue;
        Schedule* s = (Schedule*)malloc(sizeof(Schedule));
        if (!s) continue;
        if (sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%d|%d",
                   s->department, s->doctorName, s->availableDays,
                   s->timeStart, s->timeEnd,
                   &s->maxPatientsPerDay, &s->currentBookings) == 7) {
            s->next = scheduleList;
            scheduleList = s;
        } else free(s);
    }
    fclose(f);
}

void savePatients() {
    char path[100]; snprintf(path, sizeof(path), "%spatients.txt", FILE_DIR);
    FILE* f = fopen(path, "w");
    if (!f) { printError("Could not open patients.txt for writing!"); return; }
    Patient* cur = patientList;
    while (cur) {
        fprintf(f, "%s|%s|%s|%s|%s|%s|%s|%s|%d|%s|%s|%s\n",
                cur->patientID, cur->name, cur->icNumber, cur->phone,
                cur->email, cur->faculty, cur->program, cur->gender,
                cur->age, cur->bloodType, cur->allergies, cur->emergencyContact);
        cur = cur->next;
    }
    fclose(f);
}

static const char* nz(const char* s) { return (s && strlen(s) > 0) ? s : "N/A"; }

void saveAppointments() {
    char path[100]; snprintf(path, sizeof(path), "%sappointments.txt", FILE_DIR);
    FILE* f = fopen(path, "w");
    if (!f) { printError("Could not open appointments.txt for writing!"); return; }
    Appointment* cur = appointmentList;
    while (cur) {
        fprintf(f, "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
                cur->appointmentID, cur->patientID, cur->date, cur->time,
                cur->department, cur->doctorName, cur->status,
                nz(cur->symptoms), nz(cur->diagnosis), nz(cur->prescription), nz(cur->remarks));
        cur = cur->next;
    }
    fclose(f);
}

void saveUsers() {
    char path[100]; snprintf(path, sizeof(path), "%susers.txt", FILE_DIR);
    FILE* f = fopen(path, "w");
    if (!f) return;
    User* cur = userList;
    while (cur) {
        fprintf(f, "%s|%s|%s|%s|%s|%d\n",
                cur->username, cur->password, cur->role, cur->fullName,
                cur->lastLogin, cur->isActive);
        cur = cur->next;
    }
    fclose(f);
}

void saveSchedule() {
    char path[100]; snprintf(path, sizeof(path), "%sschedule.txt", FILE_DIR);
    FILE* f = fopen(path, "w");
    if (!f) return;
    Schedule* cur = scheduleList;
    while (cur) {
        fprintf(f, "%s|%s|%s|%s|%s|%d|%d\n",
                cur->department, cur->doctorName, cur->availableDays,
                cur->timeStart, cur->timeEnd,
                cur->maxPatientsPerDay, cur->currentBookings);
        cur = cur->next;
    }
    fclose(f);
}

void saveAll() {
    showLoadingBar("Saving patient records...", 25);
    savePatients();
    showLoadingBar("Saving appointments...", 50);
    saveAppointments();
    showLoadingBar("Saving user accounts...", 75);
    saveUsers();
    showLoadingBar("Saving schedules...", 100);
    saveSchedule();
    printf("\n");
    printSuccess("All data saved successfully!");
}

void backupData() {
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    char folder[100];
    snprintf(folder, sizeof(folder), "%sbackups/%04d%02d%02d_%02d%02d%02d",
             FILE_DIR, tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
             tm->tm_hour, tm->tm_min, tm->tm_sec);

    char cmd[200];
    snprintf(cmd, sizeof(cmd), "mkdir -p %s", folder);
    system(cmd);

    showLoadingBar("Backing up patients...", 25);
    snprintf(cmd, sizeof(cmd), "cp %spatients.txt %s/", FILE_DIR, folder); system(cmd);
    showLoadingBar("Backing up appointments...", 50);
    snprintf(cmd, sizeof(cmd), "cp %sappointments.txt %s/", FILE_DIR, folder); system(cmd);
    showLoadingBar("Backing up users...", 75);
    snprintf(cmd, sizeof(cmd), "cp %susers.txt %s/", FILE_DIR, folder); system(cmd);
    showLoadingBar("Backup complete!", 100);
    printf("\n");
    printSuccess("Backup created!");
    printf("Location: %s\n", folder);
    char log[200]; snprintf(log, sizeof(log), "BACKUP_DATA | LOCATION: %s", folder);
    appendAuditLog(log, "", "SUCCESS");
}

void restoreFromBackup() {
    char folder[100];
    inputString(folder, 100, "Enter backup folder path: ");

    char cmd[200];
    snprintf(cmd, sizeof(cmd), "cp %s/patients.txt %spatients.txt", folder, FILE_DIR); system(cmd);
    snprintf(cmd, sizeof(cmd), "cp %s/appointments.txt %sappointments.txt", folder, FILE_DIR); system(cmd);
    snprintf(cmd, sizeof(cmd), "cp %s/users.txt %susers.txt", folder, FILE_DIR); system(cmd);

    // Reload all data
    freeAll();
    initStructures();
    loadUsers();
    loadPatients();
    loadAppointments();
    loadSchedule();

    char log[200]; snprintf(log, sizeof(log), "RESTORE_DATA | FROM: %s", folder);
    appendAuditLog(log, "", "SUCCESS");
    printSuccess("Data restored from backup! Please log in again.");
}

/* ================================================================
 * USER AUTHENTICATION
 * ================================================================ */

User* findUser(const char* username) {
    User* cur = userList;
    while (cur) {
        if (strcasecmp(cur->username, username)==0) return cur;
        cur = cur->next;
    }
    return NULL;
}

int authenticate(const char* username, const char* password) {
    User* u = findUser(username);
    if (!u || !u->isActive) return 0;
    char hash[65];
    sha256_string(password, hash);
    return strcmp(u->password, hash)==0;
}

int login() {
    clearScreen();
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    int attempts = 0;

    printHeader();
    printf("\n");
    printf("╔══════════════════════════════════════════╗\n");
    printf("║              USER LOGIN                   ║\n");
    printf("╠══════════════════════════════════════════╣\n");

    while (attempts < 3) {
        printf("║\n");
        inputString(username, MAX_USERNAME, "  Username: ");
        printf("  Password: "); inputPassword(password, MAX_PASSWORD);
        printf("║\n");

        if (authenticate(username, password)) {
            currentUser = findUser(username);
            strcpy(currentUser->lastLogin, getCurrentTimestamp());
            lastActivity = time(NULL);
            appendAuditLog("LOGIN", username, "SUCCESS");
            printf("╠══════════════════════════════════════════╣\n");
            printf("║  %s✓ Login successful!%s                    ║\n", GREEN, RESET);
            printf("║  Welcome, %s%s%s                         ║\n", BOLD, currentUser->fullName, RESET);
            printf("╚══════════════════════════════════════════╝\n\n");
            usleep(1500000);
            return 1;
        }
        attempts++;
        printf("╠══════════════════════════════════════════╣\n");
        printf("║  %s✗ Invalid credentials! Attempt %d/3%s     ║\n", RED, attempts, RESET);
        printf("╚══════════════════════════════════════════╝\n\n");
        appendAuditLog("LOGIN_FAILED", username, "FAILED");
        if (attempts >= 3) {
            User* u = findUser(username);
            if (u) u->isActive = 0;
            saveUsers();
            printError("Account locked due to 3 failed attempts!");
            return 0;
        }
    }
    return 0;
}

void logout() {
    if (currentUser) {
        appendAuditLog("LOGOUT", currentUser->username, "SUCCESS");
        currentUser = NULL;
    }
    printInfo("Logged out successfully.");
}

void changePassword() {
    if (!currentUser) return;
    char oldPass[MAX_PASSWORD], newPass[MAX_PASSWORD], confirm[MAX_PASSWORD];
    printf("Enter current password: "); inputPassword(oldPass, MAX_PASSWORD);
    char hash[65]; sha256_string(oldPass, hash);
    if (strcmp(currentUser->password, hash)!=0) {
        printError("Current password is incorrect!"); return;
    }
    printf("Enter new password: "); inputPassword(newPass, MAX_PASSWORD);
    printf("Confirm new password: "); inputPassword(confirm, MAX_PASSWORD);
    if (strcmp(newPass, confirm)!=0) { printError("Passwords do not match!"); return; }
    sha256_string(newPass, currentUser->password);
    saveUsers();
    appendAuditLog("CHANGE_PASSWORD", currentUser->username, "SUCCESS");
    printSuccess("Password changed successfully!");
}

/* ================================================================
 * SEARCH FUNCTIONS
 * ================================================================ */

void advancedSearch() {
    printf("\n%s=== Advanced Search ===%s\n", BOLD, RESET);
    printf("1. Search Patients\n");
    printf("2. Search Appointments\n");
    printf("3. Search by Date Range\n");
    printf("4. Back\n");
    int choice = inputIntRange("Choice: ", 1, 4);

    if (choice == 1) {
        char name[MAX_NAME];
        inputString(name, MAX_NAME, "Enter patient name to search");
        if (strlen(name)==0) { printInfo("Cancelled."); return; }
        printf("\n%sSearch Results for '%s'%s\n", BOLD, name, RESET);
        printf("%s%-12s %-25s %-15s %-15s%s\n", BOLD, "ID", "Name", "IC", "Phone", RESET);
        Patient* cur = patientList;
        int found=0;
        while(cur) {
            if(strstr(cur->name, name)) {
                printf("%-12s %-25s %-15s %s\n", cur->patientID, cur->name, cur->icNumber, cur->phone);
                found=1;
            }
            cur=cur->next;
        }
        if(!found) printInfo("No patients found.");
    } else if (choice == 2) {
        char id[MAX_ID];
        inputString(id, MAX_ID, "Enter Appointment ID");
        if (strlen(id)==0) { printInfo("Cancelled."); return; }
        Appointment* a = findAppointmentByID(id);
        if (a) {
            printf("\n%sAppointment Found:%s\n", BOLD, RESET);
            printf("ID: %s\nPatient: %s\nDate: %s\nTime: %s\nDoctor: %s\nDept: %s\nStatus: %s\n",
                   a->appointmentID, a->patientID, a->date, a->time,
                   a->doctorName, a->department, a->status);
            if (strlen(a->diagnosis) > 0 && strcmp(a->diagnosis, "Pending")!=0)
                printf("Diagnosis: %s\n", a->diagnosis);
            if (strlen(a->prescription) > 0 && strcmp(a->prescription, "Pending")!=0)
                printf("Prescription: %s\n", a->prescription);
        } else printError("Appointment not found!");
    } else if (choice == 3) {
        searchByDateRange();
    }
}

void searchByDateRange() {
    char start[MAX_DATE], end[MAX_DATE];
    inputString(start, MAX_DATE, "Start Date (DD/MM/YYYY)");
    if (strlen(start)==0) { printInfo("Cancelled."); return; }
    inputString(end, MAX_DATE, "End Date (DD/MM/YYYY)");
    if (strlen(end)==0) { printInfo("Cancelled."); return; }
    if (!validateDate(start) || !validateDate(end)) {
        printError("Invalid date format!"); return;
    }
    printf("\n%sAppointments from %s to %s%s\n", BOLD, start, end, RESET);
    printf("%s%-15s %-12s %-12s %-10s %-25s %-15s%s\n", BOLD,
           "Apt ID", "Patient", "Date", "Time", "Doctor", "Status", RESET);
    Appointment* cur = appointmentList;
    int found=0;
    while(cur) {
        if(dateCompare(cur->date, start)>=0 && dateCompare(cur->date, end)<=0) {
            printf("%-15s %-12s %-12s %-10s %-25s %-15s\n",
                   cur->appointmentID, cur->patientID, cur->date, cur->time,
                   cur->doctorName, cur->status);
            found=1;
        }
        cur=cur->next;
    }
    if(!found) printInfo("No appointments in this date range.");
}

/* ================================================================
 * REPORTS & STATISTICS
 * ================================================================ */

void statisticsDashboard() {
    int totalP = countPatients();
    int totalA = 0, scheduled = 0, completed = 0, cancelled = 0, noshow = 0;
    Appointment* cur = appointmentList;
    while (cur) {
        totalA++;
        if (strcmp(cur->status,"Scheduled")==0) scheduled++;
        else if (strcmp(cur->status,"Completed")==0) completed++;
        else if (strcmp(cur->status,"Cancelled")==0) cancelled++;
        else if (strcmp(cur->status,"No-Show")==0) noshow++;
        cur = cur->next;
    }

    printf("\n%s╔══════════════════════════════════════════════╗\n", BOLD);
    printf("║      📊  PKU UTHM STATISTICS DASHBOARD     ║\n");
    printf("╠══════════════════════════════════════════════╣\n");
    printf("║  👥 Total Patients:   %-4d                       ║\n", totalP);
    printf("║  📋 Total Appointments: %-4d                    ║\n", totalA);
    printf("╠══════════════════════════════════════════════╣\n");
    printf("║  By Status:                                 ║\n");
    if (totalA > 0) {
        printf("║    🟢 Scheduled: %-4d (%.1f%%)                  ║\n", scheduled, (float)scheduled/totalA*100);
        printf("║    ✅ Completed: %-4d (%.1f%%)                  ║\n", completed, (float)completed/totalA*100);
        printf("║    ❌ Cancelled: %-4d (%.1f%%)                  ║\n", cancelled, (float)cancelled/totalA*100);
        if (noshow>0) printf("║    ⚠ No-Show:   %-4d (%.1f%%)                  ║\n", noshow, (float)noshow/totalA*100);
    } else printf("║    No appointment data.                       ║\n");
    printf("╚══════════════════════════════════════════════╝%s\n", RESET);

    // Department distribution
    printf("\n%sDepartment Distribution:%s\n", BOLD, RESET);
    Schedule* sc = scheduleList;
    while (sc) {
        int deptCount = 0;
        cur = appointmentList;
        while (cur) {
            if (strcmp(cur->department, sc->department)==0) deptCount++;
            cur = cur->next;
        }
        printf("  %-20s: %d appointments\n", sc->department, deptCount);
        sc = sc->next;
    }
}

/* ================================================================
 * MEDICAL CERTIFICATE GENERATION
 * ================================================================ */

void generateMC() {
    char aptID[MAX_ID];
    inputString(aptID, MAX_ID, "Enter Appointment ID");
    if (strlen(aptID)==0) { printInfo("Cancelled."); return; }
    Appointment* a = findAppointmentByID(aptID);
    if (!a) { printError("Appointment not found!"); return; }
    if (strcmp(a->status, "Completed")!=0) {
        printWarning("Appointment must be completed first!"); return;
    }
    Patient* p = findPatientByID(a->patientID);
    if (!p) { printError("Patient not found!"); return; }

    mkdir("data/mc", 0755);
    char mcFile[100];
    snprintf(mcFile, sizeof(mcFile), "data/mc/MC_%s.txt", aptID);
    FILE* f = fopen(mcFile, "w");
    if (!f) { printError("Could not create MC file!"); return; }

    fprintf(f, "═══════════════════════════════════════════════════════════\n");
    fprintf(f, "          PUSAT KESIHATAN UNIVERSITI UTHM\n");
    fprintf(f, "              MEDICAL CERTIFICATE\n");
    fprintf(f, "═══════════════════════════════════════════════════════════\n\n");
    fprintf(f, "Date Issued: %s\n\n", getCurrentTimestamp());
    fprintf(f, "This is to certify that:\n\n");
    fprintf(f, "  Name:            %s\n", p->name);
    fprintf(f, "  IC Number:       %s\n", p->icNumber);
    fprintf(f, "  Patient ID:      %s\n", p->patientID);
    fprintf(f, "  Faculty/Program: %s / %s\n\n", p->faculty, p->program);
    fprintf(f, "was examined at Pusat Kesihatan Universiti UTHM on %s.\n\n", a->date);
    fprintf(f, "  Diagnosis:  %s\n", a->diagnosis);
    fprintf(f, "  Prescription: %s\n", a->prescription);
    fprintf(f, "  Remarks:    %s\n\n", a->remarks);
    fprintf(f, "  Doctor:     %s\n", a->doctorName);
    fprintf(f, "  Department: %s\n\n", a->department);
    fprintf(f, "═══════════════════════════════════════════════════════════\n");
    fprintf(f, "  Generated: %s\n", getCurrentTimestamp());
    fprintf(f, "═══════════════════════════════════════════════════════════\n");
    fclose(f);

    printSuccess("Medical Certificate generated!");
    printf("File: %s\n", mcFile);
    appendAuditLog("GENERATE_MC", aptID, "SUCCESS");
}

/* ================================================================
 * USER MANAGEMENT (Admin)
 * ================================================================ */

void manageUsers() {
    while (1) {
        clearScreen(); printHeader();
        printBreadcrumb("Main Menu > Admin > User Management");
        printf("  [1] List All Users\n");
        printf("  [2] Add New User\n");
        printf("  [3] Deactivate User\n");
        printf("  [4] Reactivate User\n");
        printf("  [5] Back\n");
        int choice = inputIntRange("Choice: ", 1, 5);
        if (choice == 5) break;

        if (choice == 1) {
            printf("\n%s%-15s %-25s %-15s %-20s %-8s%s\n", BOLD,
                   "Username", "Full Name", "Role", "Last Login", "Active", RESET);
            User* cur = userList;
            while (cur) {
                printf("%-15s %-25s %-15s %-20s %-8s\n",
                       cur->username, cur->fullName, cur->role,
                       cur->lastLogin, cur->isActive ? "Yes" : "No");
                cur = cur->next;
            }
            pressEnterToContinue();
        } else if (choice == 2) {
            User* u = (User*)malloc(sizeof(User));
            if (!u) continue;
            inputString(u->username, MAX_USERNAME, "Username: ");
            if (findUser(u->username)) {
                printError("Username already exists!"); free(u); continue;
            }
            char pass[MAX_PASSWORD];
            printf("Password: "); inputPassword(pass, MAX_PASSWORD);
            sha256_string(pass, u->password);
            strcpy(u->role, "admin");
            inputString(u->fullName, MAX_FULLNAME, "Full Name: ");
            strcpy(u->lastLogin, "Never");
            u->isActive = 1;
            u->next = userList;
            userList = u;
            saveUsers();
            printSuccess("User added!");
            appendAuditLog("ADD_USER", u->username, "SUCCESS");
        } else if (choice == 3 || choice == 4) {
            char uname[MAX_USERNAME];
            inputString(uname, MAX_USERNAME, "Username: ");
            User* u = findUser(uname);
            if (!u) { printError("User not found!"); continue; }
            u->isActive = (choice == 4) ? 1 : 0;
            saveUsers();
            printSuccess(u->isActive ? "User reactivated!" : "User deactivated!");
        }
    }
}

/* ================================================================
 * MENU SYSTEMS
 * ================================================================ */

int checkSession() {
    if (!currentUser) return 0;
    time_t now = time(NULL);
    if (difftime(now, lastActivity) > sessionTimeout) {
        printWarning("Session expired due to inactivity!");
        logout();
        return 0;
    }
    lastActivity = time(NULL);
    return 1;
}

int hasRole(const char* roles) {
    if (!currentUser) return 0;
    char buf[100]; strcpy(buf, roles);
    char* tok = strtok(buf, ",");
    while (tok) {
        while (*tok==' ') tok++;
        if (strcmp(currentUser->role, tok)==0) return 1;
        tok = strtok(NULL, ",");
    }
    return 0;
}

void patientMenu() {
    while (1) {
        if (!checkSession()) return;
        clearScreen(); printHeader();
        printBreadcrumb("Main Menu > Patient Management");
        printf("  [1] Add New Patient\n");
        printf("  [2] View Patient by ID\n");
        printf("  [3] Update Patient Record\n");
        printf("  [4] Remove Patient\n");
        printf("  [5] List All Patients\n");
        printf("  [6] Search Patient by Name\n");
        printf("  [7] View Patient Appointment History\n");
        printf("  [8] Back to Main Menu\n");
        int choice = inputIntRange("Choice: ", 1, 8);
        if (choice == 8) break;
        switch (choice) {
            case 1: insertPatient(); break;
            case 2: {
                char id[MAX_ID];
                inputString(id, MAX_ID, "Enter Patient ID");
                if (strlen(id)==0) { printInfo("Cancelled."); break; }
                Patient* p = findPatientByID(id);
                if (!p) { printError("Patient not found!"); break; }
                printf("\n%sPatient Details:%s\n", BOLD, RESET);
                printf("ID: %s\nName: %s\nIC: %s\nPhone: %s\nEmail: %s\nFaculty: %s\nProgram: %s\n",
                       p->patientID, p->name, p->icNumber, p->phone, p->email, p->faculty, p->program);
                printf("Gender: %s\nAge: %d\nBlood: %s\nAllergies: %s\nEmergency: %s\n",
                       p->gender, p->age, p->bloodType, p->allergies, p->emergencyContact);
                break;
            }
            case 3: updatePatientRecord(); break;
            case 4: removePatientByID(); break;
            case 5: displayAllPatients(); break;
            case 6: {
                char name[MAX_NAME];
                inputString(name, MAX_NAME, "Enter name");
                if (strlen(name)==0) { printInfo("Cancelled."); break; }
                Patient* p = findPatientByName(name);
                if (!p) { printInfo("No patient found with that name."); break; }
                printf("Found: %s (%s)\n", p->name, p->patientID);
                break;
            }
            case 7: displayPatientHistory(); break;
        }
        if (choice>=1 && choice<=7) pressEnterToContinue();
    }
}

void appointmentMenu() {
    while (1) {
        if (!checkSession()) return;
        clearScreen(); printHeader();
        printBreadcrumb("Main Menu > Appointment Management");
        printf("  [1] Book New Appointment\n");
        printf("  [2] View Appointment by ID\n");
        printf("  [3] Reschedule Appointment\n");
        printf("  [4] Cancel Appointment\n");
        printf("  [5] Complete Appointment (Add Diagnosis)\n");
        printf("  [6] List Today's Appointments\n");
        printf("  [7] List All Appointments\n");
        printf("  [8] Generate Medical Certificate\n");
        printf("  [9] Back to Main Menu\n");
        int choice = inputIntRange("Choice: ", 1, 9);
        if (choice == 9) break;
        switch (choice) {
            case 1: insertAppointment(); break;
            case 2: {
                char id[MAX_ID];
                inputString(id, MAX_ID, "Enter Appointment ID");
                if (strlen(id)==0) { printInfo("Cancelled."); break; }
                Appointment* a = findAppointmentByID(id);
                if (!a) { printError("Not found!"); break; }
                printf("ID: %s\nPatient: %s\nDate: %s\nTime: %s\nDoctor: %s\nDept: %s\nStatus: %s\n",
                       a->appointmentID, a->patientID, a->date, a->time,
                       a->doctorName, a->department, a->status);
                break;
            }
            case 3: updateAppointment(); break;
            case 4: cancelAppointment(); break;
            case 5: completeAppointment(); break;
            case 6: displayTodayAppointments(); break;
            case 7: displayAllAppointments(); break;
            case 8: generateMC(); break;
        }
        if (choice>=1 && choice<=8) pressEnterToContinue();
    }
}

void reportMenu() {
    while (1) {
        if (!checkSession()) return;
        clearScreen(); printHeader();
        printBreadcrumb("Main Menu > Reports & Statistics");
        printf("  [1] Dashboard Statistics\n");
        printf("  [2] Department-wise Distribution\n");
        printf("  [3] Patient Demographics\n");
        printf("  [4] Back\n");
        int choice = inputIntRange("Choice: ", 1, 4);
        if (choice == 4) break;
        switch (choice) {
            case 1: statisticsDashboard(); break;
            case 2: {
                printf("\n");
                Schedule* sc = scheduleList;
                while (sc) {
                    int count=0;
                    Appointment* cur = appointmentList;
                    while (cur) { if(strcmp(cur->department, sc->department)==0) count++; cur=cur->next; }
                    printf("  %-20s: %d appointments (Dr. %s)\n", sc->department, count, sc->doctorName);
                    sc = sc->next;
                }
                break;
            }
            case 3: {
                int male=0,female=0,other=0;
                Patient* cur = patientList;
                while (cur) {
                    if (strcmp(cur->gender,"Male")==0) male++;
                    else if (strcmp(cur->gender,"Female")==0) female++;
                    else other++;
                    cur=cur->next;
                }
                int total = male+female+other;
                if (total>0) {
                    printf("\n  Male:   %d (%.1f%%)\n", male, (float)male/total*100);
                    printf("  Female: %d (%.1f%%)\n", female, (float)female/total*100);
                    if (other>0) printf("  Other:  %d (%.1f%%)\n", other, (float)other/total*100);
                }
                break;
            }
        }
        if (choice>=1 && choice<=3) pressEnterToContinue();
    }
}

void searchMenu() {
    while (1) {
        if (!checkSession()) return;
        clearScreen(); printHeader();
        printBreadcrumb("Main Menu > Search");
        printf("  [1] Quick Search by ID\n");
        printf("  [2] Advanced Search\n");
        printf("  [3] Search by Date Range\n");
        printf("  [4] Back\n");
        int choice = inputIntRange("Choice: ", 1, 4);
        if (choice == 4) break;
        switch (choice) {
            case 1: {
                char id[MAX_ID];
                inputString(id, MAX_ID, "Enter Patient ID");
                if (strlen(id)==0) { printInfo("Cancelled."); break; }
                Patient* p = findPatientByID(id);
                if (p) printf("Found: %s - %s\n", p->patientID, p->name);
                else printError("Patient not found!");
                break;
            }
            case 2: advancedSearch(); break;
            case 3: searchByDateRange(); break;
        }
        if (choice>=1 && choice<=3) pressEnterToContinue();
    }
}

void queueMenu() {
    while (1) {
        if (!checkSession()) return;
        clearScreen(); printHeader();
        printBreadcrumb("Main Menu > Walk-in Queue");
        printf("  [1] Add Patient to Queue\n");
        printf("  [2] Call Next Patient\n");
        printf("  [3] View Queue Status\n");
        printf("  [4] Clear Queue\n");
        printf("  [5] Back\n");
        int choice = inputIntRange("Choice: ", 1, 5);
        if (choice == 5) break;
        switch (choice) {
            case 1: enqueueWalkIn(); break;
            case 2: {
                QueueNode out;
                if (dequeueWalkIn(&out)) {
                    printf("\n%s=== NEXT PATIENT ===%s\n", BOLD, RESET);
                    printf("Patient: %s (%s)\n", out.name, out.patientID);
                    if (strlen(out.department)) printf("Department: %s\n", out.department);
                    printf("Time added: %s\n", out.timeAdded);
                    if (out.isEmergency) printWarning("EMERGENCY CASE!");
                }
                break;
            }
            case 3: displayQueueStatus(); break;
            case 4: clearQueue(); break;
        }
        if (choice>=1 && choice<=4) pressEnterToContinue();
    }
}

void adminMenu() {
    while (1) {
        if (!checkSession()) return;
        clearScreen(); printHeader();
        printBreadcrumb("Main Menu > Admin Settings");
        printf("  [1] Manage User Accounts\n");
        printf("  [2] Doctor Schedule Management\n");
        printf("  [3] View Audit Log\n");
        printf("  [4] Backup Data\n");
        printf("  [5] Restore Data\n");
        printf("  [6] Save All Data\n");
        printf("  [7] Change Password\n");
        printf("  [8] Back\n");
        int choice = inputIntRange("Choice: ", 1, 8);
        if (choice == 8) break;
        switch (choice) {
            case 1: manageUsers(); break;
            case 2: {
                Schedule* sc = scheduleList;
                printf("\n%sDoctor Schedules:%s\n", BOLD, RESET);
                while (sc) {
                    printf("%-20s %-20s Days: %-15s %s-%s (Max: %d, Booked: %d)\n",
                           sc->department, sc->doctorName, sc->availableDays,
                           sc->timeStart, sc->timeEnd, sc->maxPatientsPerDay, sc->currentBookings);
                    sc = sc->next;
                }
                break;
            }
            case 3: {
                char path[100]; snprintf(path, sizeof(path), "%saudit_log.txt", FILE_DIR);
                FILE* f = fopen(path, "r");
                if (!f) { printInfo("No audit log found."); break; }
                char line[LINE_SIZE];
                printf("\n%sAudit Log:%s\n", BOLD, RESET);
                while (fgets(line, sizeof(line), f)) printf("%s", line);
                fclose(f);
                break;
            }
            case 4: backupData(); break;
            case 5: restoreFromBackup(); return;
            case 6: saveAll(); break;
            case 7: changePassword(); break;
        }
        if (choice>=1 && choice<=7) pressEnterToContinue();
    }
}

void mainMenu() {
    while (1) {
        if (!checkSession() || !currentUser) { logout(); return; }

        clearScreen(); printHeader();
        printf("\n");
        printf("╔══════════════════════════════════════════════╗\n");
        printf("║              MAIN MENU                       ║\n");
        printf("╠══════════════════════════════════════════════╣\n");
        printf("║  [1] 👤 Patient Management                   ║\n");
        printf("║  [2] 📅 Appointment Management               ║\n");
        printf("║  [3] 📊 Reports & Statistics                 ║\n");
        printf("║  [4] 🔍 Search Records                       ║\n");
        printf("║  [5] 🏥 Walk-in Queue                        ║\n");
        printf("║  [6] ⚙  Admin Settings                       ║\n");
        printf("║  [7] 💾 Save All Data                        ║\n");
        printf("║  [8] 🔄 Undo Last Operation                  ║\n");
        printf("║  [9] 🔁 Redo Last Operation                  ║\n");
        printf("║  [0] 🚪 Logout                               ║\n");
        printf("╚══════════════════════════════════════════════╝\n");
        printf("User: %s | %s\n",
               currentUser->username, getCurrentTimestamp());

        int choice = inputIntRange("Enter choice: ", 0, 9);
        if (choice == 0) { logout(); return; }

        switch (choice) {
            case 1: patientMenu(); break;
            case 2: appointmentMenu(); break;
            case 3: reportMenu(); break;
            case 4: searchMenu(); break;
            case 5: queueMenu(); break;
            case 6: adminMenu(); break;
            case 7: saveAll(); break;
            case 8: undoLastOperation(); break;
            case 9: popRedo(); break;
        }
        if (choice>=1 && choice<=9 && choice!=6) pressEnterToContinue();
    }
}

/* ================================================================
 * INITIALIZATION & STARTUP
 * ================================================================ */

void startupAnimation() {
    printf("\n%s╔═══════════════════════════════════════════════════════════════╗%s\n", CYAN, RESET);
    printf("%s║     PUSAT KESIHATAN UNIVERSITI UTHM                          ║%s\n", CYAN, RESET);
    printf("%s║        Medical Appointment System v2.0                        ║%s\n", CYAN, RESET);
    printf("%s╚═══════════════════════════════════════════════════════════════╝%s\n\n", CYAN, RESET);

    typewriter("Initializing PKU UTHM Medical System...\n\n", 5);
    usleep(300000);

    showLoadingBar("Loading configuration...", 10); usleep(200000);
    showLoadingBar("Initializing data structures...", 25); usleep(200000);
    showLoadingBar("Loading patient database...", 40); usleep(300000);
    showLoadingBar("Loading appointment schedules...", 55); usleep(300000);
    showLoadingBar("Loading user accounts...", 70); usleep(200000);
    showLoadingBar("Loading doctor schedules...", 85); usleep(200000);
    showLoadingBar("Finalizing setup...", 100); usleep(300000);
    printf("\n\n%s✓ System Ready!%s\n\n", GREEN, RESET);
    usleep(500000);
}

void initStructures() {
    memset(&patientHash, 0, sizeof(HashTable));
    currentSeq = 1;

    showSpinner();
    loadUsers();
    showSpinner();
    loadPatients();
    showSpinner();
    loadAppointments();
    showSpinner();
    loadSchedule();
}

void freePatients(Patient* root) {
    if (!root) return;
    freePatients(root->left);
    freePatients(root->right);
    free(root);
}

void freeAppointments() {
    Appointment* cur = appointmentList;
    while (cur) {
        Appointment* temp = cur;
        cur = cur->next;
        free(temp);
    }
    appointmentList = NULL;
}

void freeUsers() {
    User* cur = userList;
    while (cur) {
        User* temp = cur;
        cur = cur->next;
        free(temp);
    }
    userList = NULL;
}

void freeSchedules() {
    Schedule* cur = scheduleList;
    while (cur) {
        Schedule* temp = cur;
        cur = cur->next;
        free(temp);
    }
    scheduleList = NULL;
}

void freeQueue() {
    while (walkInQueue.front) {
        QueueNode* temp = walkInQueue.front;
        walkInQueue.front = temp->next;
        free(temp);
    }
    walkInQueue.rear = NULL;
    walkInQueue.size = 0;
}

void freeStack(Stack* s) {
    while (s->top) {
        StackNode* temp = s->top;
        s->top = temp->next;
        free(temp);
    }
    s->count = 0;
}

void freeHashTable() {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        PatientNode* cur = patientHash.table[i];
        while (cur) {
            PatientNode* temp = cur;
            cur = cur->next;
            free(temp);
        }
        patientHash.table[i] = NULL;
    }
}

void freeAll() {
    freePatients(patientBST);
    patientBST = NULL;
    patientList = NULL;
    freeAppointments();
    freeUsers();
    freeSchedules();
    freeQueue();
    freeStack(&undoStack);
    freeStack(&redoStack);
    freeHashTable();
}

/* ================================================================
 * MAIN FUNCTION (Console mode)
 * ================================================================ */

#ifndef GTK_BUILD
int main() {
    clearScreen();
    startupAnimation();

    initStructures();

    while (1) {
        if (!currentUser) {
            if (!login()) {
                printf("Exiting system. Goodbye!\n");
                break;
            }
        }
        mainMenu();

        if (!currentUser) {
            printf("\nPress Enter to return to login screen...");
            while(getchar()!='\n' && getchar()!=EOF);
            getchar();
        }
    }

    saveAll();
    printInfo("Cleaning up resources...");
    freeAll();
    printf("\n%sThank you for using PKU UTHM Medical System!%s\n", BOLD, GREEN);
    return 0;
}
#endif
