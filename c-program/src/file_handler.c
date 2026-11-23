/*
 * ========================================================
 * Project: Hospital Management System
 * Author : Krishnam Kothari
 * Purpose: Academic Submission & Portfolio
 * Module : Safe Atomic File Persistence Operations
 * ========================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <windows.h>

#include "../include/file_handler.h"
#include "../include/patient.h"
#include "../include/priority_queue.h"

#define DATA_FILE "data/patients.txt"
#define TEMP_FILE "data/patients.tmp"

// Save all waiting patients
int savePatients(PriorityQueue *pq) {

    FILE *fp = fopen(TEMP_FILE, "w");

    if (fp == NULL) {
        printf("\nError creating temporary patient file!\n");
        return 0;
    }

    for (int i = 0; i < pq->size; i++) {

        Patient p = pq->patients[i];

        if (fprintf(fp, "%d|%s|%d|%s|%d|%d\n",
                    p.id, p.name, p.age,
                    p.disease, p.priority,
                    p.arrivalOrder) < 0) {

            fclose(fp);
            remove(TEMP_FILE);
            printf("\nError writing patient records!\n");
            return 0;
        }
    }

    if (fclose(fp) != 0) {
        remove(TEMP_FILE);
        printf("\nError closing patient file!\n");
        return 0;
    }

    // Replace the previous file with the new file atomically
    if (!MoveFileExA(TEMP_FILE, DATA_FILE,
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {

        remove(TEMP_FILE);
        printf("\nError updating patient records!\n");
        return 0;
    }

    return 1;
}

// Load saved patient records
int loadPatients(PriorityQueue *pq) {

    FILE *fp = fopen(DATA_FILE, "r");

    if (fp == NULL) {

        if (errno == ENOENT) {
            printf("\nNo previous patient records found.\n");
            return 0;
        }

        printf("\nError opening patient records!\n");
        return -1;
    }

    char line[256];
    int loaded = 0;
    int maxID = 1000;
    int maxArrival = 0;
    int lineNumber = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {

        lineNumber++;
        Patient p;
        char extra;

        // Detect lines that exceed the buffer
        if (strchr(line, '\n') == NULL && !feof(fp)) {
            int ch;
            while ((ch = fgetc(fp)) != '\n' && ch != EOF);
            printf("Skipping oversized record at line %d.\n", lineNumber);
            continue;
        }

        // Read and validate the record format
        int fields = sscanf(line, "%d|%49[^|]|%d|%49[^|]|%d|%d %c",
                            &p.id, p.name, &p.age,
                            p.disease, &p.priority,
                            &p.arrivalOrder, &extra);

        if (fields != 6 || p.id <= 0 ||
            p.age < 1 || p.age > 120 ||
            p.priority < 1 || p.priority > 4 ||
            p.arrivalOrder < 1 ||
            strlen(p.name) == 0 ||
            strlen(p.disease) == 0) {

            printf("Skipping invalid record at line %d.\n", lineNumber);
            continue;
        }

        if (isFull(pq)) {
            printf("Queue capacity reached. Remaining records skipped.\n");
            break;
        }

        // Prevent duplicate patient IDs
        int duplicate = 0;
        for (int i = 0; i < pq->size; i++) {
            if (pq->patients[i].id == p.id) {
                duplicate = 1;
                break;
            }
        }

        if (duplicate) {
            printf("Duplicate patient ID skipped: %d\n", p.id);
            continue;
        }

        // Insert record into the heap
        pq->patients[pq->size] = p;
        heapifyUp(pq, pq->size);
        pq->size++;

        if (p.id > maxID) {
            maxID = p.id;
        }

        if (p.arrivalOrder > maxArrival) {
            maxArrival = p.arrivalOrder;
        }

        loaded++;
    }

    if (ferror(fp)) {
        fclose(fp);
        printf("\nError reading patient records!\n");
        return -1;
    }

    fclose(fp);

    // Restore the ID and arrival counters
    updatePatientCounters(maxID, maxArrival);

    printf("\n%d patient record(s) loaded successfully.\n", loaded);

    return loaded;
}
