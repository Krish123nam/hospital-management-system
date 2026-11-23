# Hospital Management System 🏥

A complete, modular Hospital Emergency Department Management System featuring both an **Interactive Console Application** and a **Web Application powered directly by a native C HTTP backend**.

---

## 👤 Author & Project Information

- **Developer:** **Krishnam Kothari**
- **Project Title:** Hospital Management System
- **Language / Core Tech:** C (C11 Standard), HTML5, CSS3, JavaScript (Fetch API)
- **Data Structure:** Priority Queue (Binary Max Heap)
- **Networking:** Mongoose Embedded C HTTP Server
- **Platform:** Windows (GCC / MinGW-w64)
- **Purpose:** Academic Assessment Submission & Portfolio Project
- **Year:** 2026

---

## 🌟 Key Highlights

- **Single Core Engine in C:** Both the console CLI and the website share the exact same C data structures, heap algorithms, and file persistence logic.
- **Native C Backend Server:** Web application powered by an embedded C HTTP server using Mongoose. No Python, Node.js, or external frameworks needed.
- **Priority-Based Triage:** Patients are prioritized based on medical urgency (Critical > High > Medium > Low) with a FIFO tiebreak for equal priorities.
- **Data Persistence:** Records are permanently saved and loaded via `data/patients.txt` with atomic write safety.
- **RESTful Endpoints:**
  - `GET /api/hello` &mdash; Healthcheck and status
  - `GET /api/patients` &mdash; Waiting list in treatment order
  - `POST /api/patients` &mdash; Register new patient with validation
  - `POST /api/treat` &mdash; Treat highest-priority patient
  - `DELETE /api/patients?id=...` &mdash; Remove patient by ID
  - `GET /api/search?id=...` &mdash; Search patient by ID

---

## 📂 Project Architecture

```
c-program/
├── data/
│   └── patients.txt        # Persistent patient records
├── docs/                   # Documentation & diagrams
├── include/
│   ├── file_handler.h      # Persistence interface
│   ├── patient.h           # Patient structure & operations
│   └── priority_queue.h    # Queue interface
├── src/
│   ├── file_handler.c      # Safe atomic file save/load
│   ├── main.c              # Console application entry point
│   ├── patient.c           # Patient business logic & validation
│   ├── priority_queue.c    # Queue implementation & rebalancing
│   └── web_server.c        # C HTTP Server & REST API
├── tests/
│   └── test_cases.md       # Comprehensive test matrix
├── third_party/
│   ├── mongoose.c          # Mongoose embedded C HTTP server
│   └── mongoose.h
├── web/
│   ├── app.js              # Frontend API client
│   ├── index.html          # Dashboard & management UI
│   └── style.css           # Modern medical-themed UI
├── hospital_app.exe        # Compiled console app
└── web_server.exe          # Compiled C web server
```

---

## 🚀 How to Run

### Option 1: Run the Web Application (Website + C Backend)

1. Open a terminal in the `c-program` directory:
   ```powershell
   cd C:\Users\USER\.gemini\antigravity\scratch\hospital-management-system\c-program
   ```
2. Compile the C Web Server (if needed):
   ```powershell
   gcc -std=c11 -Iinclude -Ithird_party src/web_server.c src/patient.c src/priority_queue.c src/file_handler.c third_party/mongoose.c -lws2_32 -o web_server.exe
   ```
3. Run the C Web Server:
   ```powershell
   .\web_server.exe
   ```
4. Open your browser and navigate to:
   ```
   http://127.0.0.1:8085
   ```

### Option 2: Run the Console Application (CLI)

1. Open a terminal in the `c-program` directory:
   ```powershell
   cd C:\Users\USER\.gemini\antigravity\scratch\hospital-management-system\c-program
   ```
2. Compile the Console Application (if needed):
   ```powershell
   gcc -Wall -Wextra -std=c11 -Iinclude src/main.c src/patient.c src/priority_queue.c src/file_handler.c -o hospital_app.exe
   ```
3. Run the executable:
   ```powershell
   .\hospital_app.exe
   ```

---

## ⚙️ Algorithm & Complexity Analysis

| Operation | Time Complexity | Explanation |
|---|---|---|
| Register Patient | $\mathcal{O}(\log n)$ | Heapify Up |
| Treat Next Patient | $\mathcal{O}(\log n)$ | Extract root and Heapify Down |
| Search by ID | $\mathcal{O}(n)$ | Linear scan |
| Delete by ID | $\mathcal{O}(n)$ | Linear search + Heapify Up/Down |
| Display All Patients | $\mathcal{O}(n \log n)$ | Cloned heap successive extractions |
| View Next Patient | $\mathcal{O}(1)$ | Read root element |
| Count Patients | $\mathcal{O}(1)$ | Size variable access |

**Space Complexity:** $\mathcal{O}(n)$ storage for patient records.

---

## 🧪 Testing Checklist

All 18 comprehensive tests documented in `tests/test_cases.md` have been fully executed and verified:
- Automatic patient ID generation & continuous numbering across restarts.
- Priority levels: Critical (4) > High (3) > Medium (2) > Low (1).
- FIFO arrival ordering tiebreak for patients with the same priority.
- Permanent file storage with atomic replacement safety (`MoveFileExA`).
- Cross-platform HTTP REST API tested and validated.

---

## 📄 License & Academic Note

Developed by **Krishnam Kothari** for educational and portfolio presentation purposes. This simulated healthcare triage application is designed to demonstrate computer science data structures and systems programming in C.
