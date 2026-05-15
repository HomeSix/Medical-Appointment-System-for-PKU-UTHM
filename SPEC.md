# Medical Appointment System - Specification

## Project Overview
- **Project Name**: Medical Appointment System
- **Type**: C Program with Linked List Data Structure
- **Core Functionality**: A console-based system to manage medical appointments with CRUD operations and additional features

## Fields Required
1. **Patient ID** - Unique identifier (integer)
2. **Patient Name** - Full name (string, max 50 chars)
3. **IC Number** - National ID (string, max 20 chars)
4. **Phone Number** - Contact number (string, max 15 chars)
5. **Email** - Email address (string, max 50 chars)
6. **Appointment Date** - Date of appointment (string, max 20 chars)
7. **Appointment Time** - Time of appointment (string, max 10 chars)
8. **Doctor Name** - Assigned doctor (string, max 50 chars)
9. **Department** - Medical department (string, max 30 chars)
10. **Status** - Appointment status: Pending/Confirmed/Completed/Cancelled (string)

## Data Structure
- **Linked List** - For dynamic insertion, deletion, and alphabetical sorting

## Operations Required
1. **Insert Record** - Add new appointment in sorted alphabetical order by patient name
2. **Remove Record** - Delete appointment by Patient ID
3. **Update Record** - Modify existing appointment by Patient ID
4. **Find Record** - Search appointment by Patient ID
5. **List All Records** - Display all records in alphabetical order by patient name

## Additional Features (for extra marks)
1. **Filter by Department** - Show appointments by department
2. **Filter by Date** - Show appointments on specific date
3. **Count Total Appointments** - Display total number of records
4. **Appointment Statistics** - Show count by status
5. **Save to File** - Export data to text file
6. **Load from File** - Import data from text file
7. **Clear All Records** - Delete all appointments with confirmation

## Menu Structure
```
=== Medical Appointment System ===
1. Add New Appointment
2. Remove Appointment
3. Update Appointment
4. Search Appointment
5. View All Appointments
6. Filter by Department
7. Filter by Date
8. Appointment Statistics
9. Save to File
10. Load from File
11. Clear All Records
12. Exit
```

## File Format
- Text file with pipe-separated values for easy parsing

## Compilation
- Standard C99 compliance
- Single file implementation for simplicity