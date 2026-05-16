# Medical Appointment System - PKU UTHM

**Group 4 | BIK10903 Data Structure | Sem 2 2025/2026**

A comprehensive Medical Appointment System for Pusat Kesihatan Universiti UTHM (PKU UTHM), developed in C programming language.

## Features

### Data Structures Used
| Structure | Purpose |
|---|---|
| **Linked List** | Dynamic storage of patient and appointment records |
| **Binary Search Tree (BST)** | Fast patient search by ID (O(log n)) with in-order alphabetical listing |
| **Queue (FIFO)** | Walk-in patient queue management with priority for emergencies |
| **Stack (LIFO)** | Undo/Redo system for last 10 operations |
| **Hash Table** | O(1) average lookup for patient ID verification |

### Core Functionality
- Complete CRUD operations for patients and appointments
- Role-based access control (Admin, Doctor, Staff, Student)
- SHA-256 password hashing and session management
- Appointment conflict detection
- Medical Certificate (MC) generation
- Walk-in queue with emergency priority
- Statistics dashboard and reports
- Advanced search by ID, name, date range
- Data backup and restore
- Audit logging for all operations
- Console GUI with box-drawing characters and ANSI colors
- Loading animations and visual effects

## Default Login Credentials

| Username | Password | Role |
|---|---|---|
| `admin` | `admin` | Administrator |
| `doctor1` | `doctor1` | Doctor |
| `doctor2` | `doctor2` | Doctor |
| `staff1` | `staff1` | Staff |
| `student1` | `student1` | Student (deactivated) |

## Compilation

```bash
# Using Makefile
make clean && make

# Or directly with GCC
gcc -std=c99 -Wall -Wextra -Wno-format-truncation -o medical_system medical_system.c -lm
```

## Usage

```bash
./medical_system
```

## Project Structure

```
medical_system/
├── medical_system.c        # Main source file (single-file implementation)
├── Makefile                # Build configuration
├── README.md               # This file
├── data/
│   ├── patients.txt        # Patient records
│   ├── appointments.txt    # Appointment records
│   ├── users.txt           # User accounts (SHA-256 hashed passwords)
│   ├── schedule.txt        # Doctor schedules
│   ├── audit_log.txt       # Operation audit log
│   └── backups/            # Timestamped backup folder
└── medical_system          # Compiled binary
```

## Video Demonstration

A 15-20 minute video walkthrough is available.
