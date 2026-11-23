/*
 * ========================================================
 * Project: Hospital Management System
 * Author : Krishnam Kothari
 * Purpose: Academic Submission & Portfolio
 * Module : Main Console Application (CLI)
 * ========================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "../include/patient.h"
#include "../include/priority_queue.h"
#include "../include/file_handler.h"

// Read menu choice
int getChoice() {

    char buffer[100];
    char *end;
    long choice;

    while (1) {

        printf("\nEnter your choice: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            exit(EXIT_FAILURE);
        }

        choice = strtol(buffer, &end, 10);

        while (*end == ' ' || *end == '\t' ||
               *end == '\n' || *end == '\r') {
            end++;
        }

        if (end != buffer && *end == '\0' &&
            choice >= 1 && choice <= 8) {
            return (int)choice;
        }

        printf("Invalid choice! Enter 1 to 8.\n");
    }
}

int main() {

    PriorityQueue pq;

    // Initialize Priority Queue
    initializeQueue(&pq);

    // Load previously saved patients
    loadPatients(&pq);

    int choice;

    do {

        printf("\n====================================");
        printf("\n      HOSPITAL MANAGEMENT SYSTEM");
        printf("\n====================================");

        printf("\n1. Register New Patient");
        printf("\n2. Display All Patients");
        printf("\n3. Search Patient");
        printf("\n4. Delete Patient");
        printf("\n5. View Next Patient");
        printf("\n6. Count Waiting Patients");
        printf("\n7. Treat Highest-Priority Patient");
        printf("\n8. Exit");

        printf("\n====================================");

        choice = getChoice();

        switch (choice) {

            case 1:
                registerPatient(&pq);
                savePatients(&pq);
                break;

            case 2:
                displayPatients(&pq);
                break;

            case 3:
                searchPatient(&pq);
                break;

            case 4:
                deletePatient(&pq);
                savePatients(&pq);
                break;

            case 5:

                if (isEmpty(&pq)) {
                    printf("\nNo patients waiting!\n");
                }
                else {
                    printf("\nNext patient for treatment:\n");

                    printf("Patient ID: %d\n",
                           pq.patients[0].id);

                    printf("Name: %s\n",
                           pq.patients[0].name);

                    printf("Priority: %d\n",
                           pq.patients[0].priority);
                }

                printf("\nPress Enter to return to menu...");
                getchar();
                break;

            case 6:
                printf("\nTotal waiting patients: %d\n",
                       pq.size);
                printf("\nPress Enter to return to menu...");
                getchar();
                break;

            case 7:
                treatNextPatient(&pq);
                savePatients(&pq);
                break;

            case 8:
                printf("\nThank you for using the system!\n");
                break;
        }

    } while (choice != 8);

    return 0;
}
