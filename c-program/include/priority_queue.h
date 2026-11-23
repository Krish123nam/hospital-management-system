#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "patient.h"

#define MAX_PATIENTS 100

struct PriorityQueue {
    Patient patients[MAX_PATIENTS];
    int size;
};

void initializeQueue(PriorityQueue *pq);
int isEmpty(PriorityQueue *pq);
int isFull(PriorityQueue *pq);

int higherPriority(Patient a, Patient b);

void insertPatient(PriorityQueue *pq, Patient p);

void heapifyUp(PriorityQueue *pq, int index);
void heapifyDown(PriorityQueue *pq, int index);

// Remove the highest-priority patient
int removeHighestPriority(PriorityQueue *pq, Patient *removed);

#endif
