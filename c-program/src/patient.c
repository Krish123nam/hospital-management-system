/*
 * ========================================================
 * Project: Hospital Management System
 * Author : Krishnam Kothari
 * Purpose: Academic Submission & Portfolio
 * Module : Patient Business Logic & Operations
 * ========================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/patient.h"
#include "../include/priority_queue.h"

// Automatic patient ID and arrival sequence
static int nextPatientID = 1001;
static int nextArrivalOrder = 1;

// Restore counters after loading saved records
void updatePatientCounters(int maxID, int maxArrival) {

    if (maxID >= nextPatientID) {
        nextPatientID = maxID + 1;
    }

    if (maxArrival >= nextArrivalOrder) {
        nextArrivalOrder = maxArrival + 1;
    }
}

// Read a non-empty text field
static void readText(const char *message,
                     char *output, int size) {
    char buffer[256];

    while (1) {
        printf("%s", message);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            exit(EXIT_FAILURE);
        }

        // Remove newline
        buffer[strcspn(buffer, "\n")] = '\0';

        // Handle long input
        if (strchr(buffer, '\n') == NULL &&
            strlen(buffer) == sizeof(buffer) - 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            printf("Input is too long. Try again.\n");
            continue;
        }

        if (strlen(buffer) == 0) {
            printf("Input cannot be empty.\n");
            continue;
        }

        if (strlen(buffer) >= (size_t)size) {
            printf("Input is too long. Try again.\n");
            continue;
        }

        strcpy(output, buffer);
        return;
    }
}

// Read and validate an integer
static int readNumber(const char *message,
                      int minimum, int maximum) {
    char buffer[100];
    char *end;
    long value;

    while (1) {
        printf("%s", message);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            exit(EXIT_FAILURE);
        }

        value = strtol(buffer, &end, 10);

        while (*end == ' ' || *end == '\t' ||
               *end == '\n' || *end == '\r') {
            end++;
        }

        if (end != buffer && *end == '\0' &&
            value >= minimum && value <= maximum) {
            return (int)value;
        }

        printf("Invalid input. Enter a number between %d and %d.\n",
               minimum, maximum);
    }
}

// Display one patient
static void printPatient(Patient p) {
    printf("\nPatient ID    : %d", p.id);
    printf("\nName          : %s", p.name);
    printf("\nAge           : %d", p.age);
    printf("\nCondition     : %s", p.disease);
    printf("\nPriority      : %d", p.priority);
    printf("\nArrival Order : %d\n", p.arrivalOrder);
}

// Register a new patient
void registerPatient(PriorityQueue *pq) {

    if (isFull(pq)) {
        printf("\nWaiting list is full!\n");
        return;
    }

    Patient p;

    printf("\n===== PATIENT REGISTRATION =====\n");

    readText("Enter patient name: ",
             p.name, sizeof(p.name));

    p.age = readNumber("Enter patient age: ", 1, 120);

    readText("Enter medical condition: ",
             p.disease, sizeof(p.disease));

    printf("\nPriority Levels\n");
    printf("1. Low\n");
    printf("2. Medium\n");
    printf("3. High\n");
    printf("4. Critical\n");

    p.priority = readNumber("Select priority (1-4): ", 1, 4);

    // Generate patient ID and arrival order
    p.id = nextPatientID++;
    p.arrivalOrder = nextArrivalOrder++;

    // Insert into Binary Max Heap
    insertPatient(pq, p);

    printf("Generated Patient ID: %d\n", p.id);
}

// Search patient using ID
void searchPatient(PriorityQueue *pq) {

    if (isEmpty(pq)) {
        printf("\nNo patients available!\n");
        printf("\nPress Enter to return to menu...");
        getchar();
        return;
    }

    int id = readNumber("Enter patient ID: ", 1, 1000000);

    for (int i = 0; i < pq->size; i++) {

        if (pq->patients[i].id == id) {
            printf("\nPatient found!\n");
            printPatient(pq->patients[i]);
            printf("\nPress Enter to return to menu...");
            getchar();
            return;
        }
    }

    printf("\nPatient not found!\n");
    printf("\nPress Enter to return to menu...");
    getchar();
}

// Delete patient using ID
void deletePatient(PriorityQueue *pq) {

    if (isEmpty(pq)) {
        printf("\nNo patients available to delete!\n");
        return;
    }

    int id = readNumber("Enter patient ID to delete: ",
                        1, 1000000);

    int index = -1;

    // Find patient
    for (int i = 0; i < pq->size; i++) {

        if (pq->patients[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("\nPatient not found!\n");
        return;
    }

    // Replace deleted patient with last element
    pq->patients[index] = pq->patients[pq->size - 1];

    pq->size--;

    // Restore heap property
    if (index < pq->size) {

        if (index > 0 &&
            higherPriority(
                pq->patients[index],
                pq->patients[(index - 1) / 2])) {

            heapifyUp(pq, index);
        }
        else {
            heapifyDown(pq, index);
        }
    }

    printf("\nPatient deleted successfully!\n");
    printf("\nPress Enter to return to menu...");
    getchar();
}

// Display patients in priority order
void displayPatients(PriorityQueue *pq) {

    if (isEmpty(pq)) {
        printf("\nNo patients in waiting list!\n");
        printf("\nPress Enter to return to menu...");
        getchar();
        return;
    }

    // Copy original queue
    PriorityQueue temp = *pq;

    printf("\n===== PATIENT WAITING LIST =====\n");

    int position = 1;

    while (!isEmpty(&temp)) {

        Patient p = temp.patients[0];

        printf("\nTreatment Position: %d", position++);

        printPatient(p);

        // Remove root from temporary queue
        temp.patients[0] = temp.patients[temp.size - 1];

        temp.size--;

        if (!isEmpty(&temp)) {
            heapifyDown(&temp, 0);
        }
    }

    printf("\n================================\n");
    printf("Total patients waiting: %d\n", pq->size);
    printf("\nPress Enter to return to menu...");
    getchar();
}

// Process the next patient
void treatNextPatient(PriorityQueue *pq) {

    Patient treated;

    printf("\n===== PATIENT TREATMENT =====\n");

    // Remove highest-priority patient
    if (!removeHighestPriority(pq, &treated)) {

        printf("\nNo patients waiting for treatment!\n");
        return;
    }

    printf("\nPatient selected for treatment:\n");

    printPatient(treated);

    printf("\nTreatment simulation completed.");
    printf("\nPatient ID %d removed from waiting queue.\n",
           treated.id);

    printf("\nRemaining patients: %d\n", pq->size);
    printf("\nPress Enter to return to menu...");
    getchar();
}
