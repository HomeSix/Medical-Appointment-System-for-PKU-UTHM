# Medical Appointment System - PKU UTHM

## Group 4 | BIK10903 Data Structure | Sem 2 2025/2026

---

## Overview
A comprehensive Medical Appointment System for Pusat Kesihatan Universiti UTHM with **two interfaces**:
- **CLI** (Console) — Full-featured terminal-based interface
- **GUI** (GTK+3) — Modern graphical interface with hospital theme

---

## Data Structures Used

| Structure | Purpose | Justification |
|---|---|---|
| **Linked List (Singly)** | Dynamic storage of patient/appointment records | Efficient insertion/deletion at any position |
| **Binary Search Tree (BST)** | Fast searching by Patient ID, alphabetical in-order traversal | O(log n) average for find/insert/delete |
| **Queue (FIFO)** | Walk-in patient queue management | First-come-first-served with emergency priority |
| **Stack (LIFO)** | Undo/Redo operations (last 10) | Perfect for reversing recent actions |
| **Hash Table** | O(1) patient ID lookup | Fast authentication and duplicate detection |

---

## Compilation

### Prerequisites

**For CLI version (any system):**
```bash
# GCC with C99 support (usually pre-installed on Linux)
gcc --version
```

**For GUI version (Linux with GTK+3):**
```bash
# Install GTK+3 development libraries (Ubuntu/Debian)
sudo apt-get install libgtk-3-dev build-essential pkg-config

# Arch Linux
sudo pacman -S gtk3 base-devel

# Fedora
sudo dnf install gtk3-devel gcc make
```

### Build Commands

```bash
# Build both CLI and GUI versions
make all

# Build only CLI version
make cli

# Build only GUI version
make gui

# Clean build artifacts
make clean
```

### Manual Compilation (without Makefile)

```bash
# CLI version
gcc -std=c99 -Wall -Wextra -Wno-format-truncation -o medical_system medical_system.c -lm

# GUI version
gcc -std=c99 -Wall -Wextra -Wno-format-truncation -DGTK_BUILD \
    $(pkg-config --cflags gtk+-3.0) \
    -o medical_system_gui main.c medical_system_gui.c medical_system.c \
    $(pkg-config --libs gtk+-3.0) -lm
```

---

## Running

### CLI Version
```bash
./medical_system
```
Starts in the terminal with loading animation, then login prompt.

### GUI Version
```bash
./medical_system_gui
```
Opens a graphical window with login screen, animated background.

**Cannot run GUI via SSH without X forwarding** — use a local terminal or VNC.

---

## Default Login Credentials

| Username | Password | Role |
|---|---|---|
| `admin` | `admin` | Administrator (full access) |
| `doctor1` | `doctor1` | Doctor |
| `doctor2` | `doctor2` | Doctor |
| `staff1` | `staff1` | Staff |
| `student1` | `student1` | Student (deactivated account) |

---

## GUI Features (medical_system_gui)

| View | Description |
|---|---|
| **Login Window** | Animated gradient background, password masking, role support |
| **Dashboard** | 4 stats cards, welcome message, quick actions, recent activity |
| **Patient List** | Sortable table with search, add/delete with confirmation |
| **Patient Form** | All fields (name, IC, phone, email, faculty, program, gender, age, blood type, allergies, emergency contact) |
| **Appointment List** | Calendar date picker, department/status filters, appointment cards |
| **Appointment Form** | Patient ID auto-verify, department-linked doctor selection, time slots |
| **Walk-in Queue** | Visual queue cards with emergency badge, call-next/add-walk-in |
| **Reports** | Statistics cards, bar chart (Cairo) showing department distribution |
| **Search** | Multi-criteria (ID, name, date), results in list |
| **Settings (Admin)** | User management table, backup/restore buttons, audit log viewer |

---

## Default Data Files

All stored in `data/` directory:
- `patients.txt` — Patient records
- `appointments.txt` — Appointment records
- `users.txt` — User accounts (SHA-256 hashed passwords)
- `schedule.txt` — Doctor schedules
- `audit_log.txt` — Operation audit trail
- `backups/` — Timestamped backup folder

---

## Project Files

```
medical_system/
├── medical_system.c           # Backend + CLI interface
├── medical_system_gui.c       # GTK GUI implementation (all views)
├── medical_system_gui.h       # GUI function prototypes
├── backend.h                  # Backend struct/extern declarations
├── main.c                     # GTK entry point
├── Makefile                   # Build configuration
├── SPEC.md                    # This file
├── README.md                  # Quick start guide
├── resources/css/
│   └── hospital_theme.css     # Professional GTK theme
├── data/                      # Persistent storage
│   ├── patients.txt
│   ├── appointments.txt
│   ├── users.txt
│   ├── schedule.txt
│   └── audit_log.txt
└── backups/                   # Auto-generated backups
```
