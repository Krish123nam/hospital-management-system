#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "priority_queue.h"

// Load patient records
int loadPatients(PriorityQueue *pq);

// Save patient records
int savePatients(PriorityQueue *pq);

#endif
