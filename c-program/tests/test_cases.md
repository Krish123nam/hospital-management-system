# Hospital Management System - Complete Test Matrix
**Author:** Krishnam Kothari  
**Date:** 2026  
**Purpose:** Academic Submission Verification

| Test ID | Test Scenario | Inputs | Expected Output | Status |
|---|---|---|---|---|
| TC-01 | Register Patient | Name: Rahul, Age: 21, Condition: Fever, Priority: 1 | Generated Patient ID: 1001, Queue size: 1 | Passed |
| TC-02 | Register High Priority | Name: Priya, Age: 24, Condition: Accident, Priority: 4 | Generated Patient ID: 1002, Root of Heap | Passed |
| TC-03 | Equal Priority Tiebreak | Name: Neha, Age: 25, Condition: Emergency, Priority: 4 | Generated Patient ID: 1004, Priya (Arrival 2) treated before Neha (Arrival 4) | Passed |
| TC-04 | Display Queue | Option 2 | Priya -> Neha -> Amit -> Rahul | Passed |
| TC-05 | Search Patient | Option 3, ID: 1002 | Patient found! ID 1002, Name: Priya | Passed |
| TC-06 | Delete Patient | Option 4, ID: 1003 (Amit) | Patient deleted successfully! Amit removed from heap | Passed |
| TC-07 | View Next Patient | Option 5 | Next patient for treatment: ID 1002, Priya | Passed |
| TC-08 | Count Patients | Option 6 | Total waiting patients: 3 | Passed |
| TC-09 | Treat Highest Priority | Option 7 | Patient 1002 (Priya) selected & removed; Heap rebalanced | Passed |
| TC-10 | Post-Treatment Queue | Option 2 | Neha -> Amit -> Rahul | Passed |
| TC-11 | Successive Treatment | Option 7 | Patient 1004 (Neha) selected & removed | Passed |
| TC-12 | Empty Queue Safety | Option 7 on empty queue | "No patients waiting for treatment!" safely handled | Passed |
| TC-13 | Persistence - Save on Register | Register Rahul, Priya, Amit | Records written to `data/patients.txt` | Passed |
| TC-14 | Persistence - Load on Startup | Restart program | "3 patient record(s) loaded successfully." | Passed |
| TC-15 | Persistence - ID Continuity | Register Neha after restart | Auto-assigned ID 1004 (no conflict with loaded IDs) | Passed |
| TC-16 | Persistence - Save on Deletion | Delete Amit (1003), restart | 3 records loaded; Amit remains deleted | Passed |
| TC-17 | Persistence - Save on Treatment | Treat Priya (1002), restart | 2 records loaded (Neha and Rahul); Priya remains treated | Passed |
| TC-18 | Atomic File Safety | File write via `patients.tmp` | Atomic replacement using `MoveFileExA` ensures crash resistance | Passed |
