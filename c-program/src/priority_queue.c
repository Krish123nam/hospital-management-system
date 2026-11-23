/*
 * ========================================================
 * Project: Hospital Management System
 * Author : Krishnam Kothari
 * Purpose: Academic Submission & Portfolio
 * Module : Binary Max Heap Priority Queue Implementation
 * ========================================================
 */

#include <stdio.h>
#include "../include/priority_queue.h"

/* ---- Initialize ---- */

void initializeQueue(PriorityQueue *pq) {
    pq->size = 0;
}

/* ---- Boundary Checks ---- */

int isEmpty(PriorityQueue *pq) {
    return pq->size == 0;
}

int isFull(PriorityQueue *pq) {
    return pq->size == MAX_PATIENTS;
}

/* ---- Priority Comparison ---- */

int higherPriority(Patient a, Patient b) {

    if (a.priority != b.priority) {
        return a.priority > b.priority;
    }

    return a.arrivalOrder < b.arrivalOrder;
}

/* ---- Swap Helper ---- */

static void swap(Patient *a, Patient *b) {
    Patient temp = *a;
    *a = *b;
    *b = temp;
}

/* ---- Heapify Up ---- */

void heapifyUp(PriorityQueue *pq, int index) {

    while (index > 0) {

        int parent = (index - 1) / 2;

        if (higherPriority(
                pq->patients[index],
                pq->patients[parent])) {

            swap(&pq->patients[index],
                 &pq->patients[parent]);

            index = parent;

        } else {
            break;
        }
    }
}

/* ---- Heapify Down ---- */

void heapifyDown(PriorityQueue *pq, int index) {

    while (1) {

        int left    = 2 * index + 1;
        int right   = 2 * index + 2;
        int highest = index;

        if (left < pq->size &&
            higherPriority(
                pq->patients[left],
                pq->patients[highest])) {

            highest = left;
        }

        if (right < pq->size &&
            higherPriority(
                pq->patients[right],
                pq->patients[highest])) {

            highest = right;
        }

        if (highest == index) {
            break;
        }

        swap(&pq->patients[index],
             &pq->patients[highest]);

        index = highest;
    }
}

/* ---- Insert Patient ---- */

void insertPatient(PriorityQueue *pq, Patient p) {

    if (isFull(pq)) {
        printf("\nPriority Queue is full!\n");
        return;
    }

    pq->patients[pq->size] = p;

    heapifyUp(pq, pq->size);

    pq->size++;

    printf("\nPatient inserted successfully!\n");
}

/* ---- Remove Highest Priority Patient ---- */

int removeHighestPriority(PriorityQueue *pq, Patient *removed) {

    // Check whether queue is empty
    if (isEmpty(pq)) {
        return 0;
    }

    // Store the root patient
    *removed = pq->patients[0];

    // Replace root with the last patient
    pq->patients[0] = pq->patients[pq->size - 1];

    // Reduce queue size
    pq->size--;

    // Restore heap property
    if (!isEmpty(pq)) {
        heapifyDown(pq, 0);
    }

    return 1;
}
