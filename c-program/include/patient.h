#ifndef PATIENT_H
#define PATIENT_H

typedef struct {
    int id;
    char name[50];
    int age;
    char disease[50];
    int priority;
    int arrivalOrder;
} Patient;

// Forward declaration
typedef struct PriorityQueue PriorityQueue;

// Patient management functions
void registerPatient(PriorityQueue *pq);
void searchPatient(PriorityQueue *pq);
void deletePatient(PriorityQueue *pq);
void displayPatients(PriorityQueue *pq);
void treatNextPatient(PriorityQueue *pq);
void updatePatientCounters(int maxID, int maxArrival);

#endif
