/*
 * ========================================================
 * Project: Hospital Management System
 * Author : Krishnam Kothari
 * Purpose: Academic Submission & Portfolio
 * Module : Embedded C Web Server & REST API
 * ========================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../third_party/mongoose.h"
#include "../include/patient.h"
#include "../include/priority_queue.h"
#include "../include/file_handler.h"

static PriorityQueue pq;
static int nextWebID = 1001;
static int nextWebArrival = 1;

// Helper to escape JSON strings
static void print_json_string(char *dest, size_t max, const char *src) {
    size_t d = 0;
    for (size_t s = 0; src[s] != '\0' && d + 2 < max; s++) {
        if (src[s] == '"' || src[s] == '\\') {
            dest[d++] = '\\';
            dest[d++] = src[s];
        } else if (src[s] == '\n') {
            dest[d++] = '\\';
            dest[d++] = 'n';
        } else if (src[s] == '\r') {
            // skip
        } else {
            dest[d++] = src[s];
        }
    }
    dest[d] = '\0';
}

// Generate JSON array of patients in sorted treatment order
static void get_patients_json(char *buf, size_t max) {
    PriorityQueue temp = pq;
    size_t offset = 0;

    offset += snprintf(buf + offset, max - offset, "{\"patients\":[");

    int first = 1;
    while (!isEmpty(&temp)) {
        Patient p;
        if (!removeHighestPriority(&temp, &p)) break;

        char escapedName[100];
        char escapedDisease[100];
        print_json_string(escapedName, sizeof(escapedName), p.name);
        print_json_string(escapedDisease, sizeof(escapedDisease), p.disease);

        if (!first) {
            offset += snprintf(buf + offset, max - offset, ",");
        }
        first = 0;

        offset += snprintf(buf + offset, max - offset,
            "{\"id\":%d,\"name\":\"%s\",\"age\":%d,\"disease\":\"%s\",\"priority\":%d,\"arrivalOrder\":%d}",
            p.id, escapedName, p.age, escapedDisease, p.priority, p.arrivalOrder);

        if (offset >= max - 100) break;
    }

    snprintf(buf + offset, max - offset, "]}");
}

static void eventHandler(struct mg_connection *c, int event, void *eventData) {
    if (event == MG_EV_HTTP_MSG) {
        struct mg_http_message *req = (struct mg_http_message *) eventData;

        // 1. Connection check
        if (mg_match(req->uri, mg_str("/api/hello"), NULL)) {
            mg_http_reply(c, 200, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\n",
                          "{\"status\":\"connected\",\"backend\":\"C\"}\n");
        }
        // 2. GET /api/patients (List waiting patients in priority order)
        else if (mg_match(req->uri, mg_str("/api/patients"), NULL) && mg_strcmp(req->method, mg_str("GET")) == 0) {
            static char jsonBuf[32768];
            get_patients_json(jsonBuf, sizeof(jsonBuf));
            mg_http_reply(c, 200, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\n",
                          "%s", jsonBuf);
        }
        // 3. POST /api/patients (Register new patient)
        else if (mg_match(req->uri, mg_str("/api/patients"), NULL) && mg_strcmp(req->method, mg_str("POST")) == 0) {
            if (isFull(&pq)) {
                mg_http_reply(c, 400, "Content-Type: application/json\r\n",
                              "{\"error\":\"Waiting list is full!\"}");
                return;
            }

            char *nameStr = mg_json_get_str(req->body, "$.name");
            char *diseaseStr = mg_json_get_str(req->body, "$.disease");
            double age = 0;
            double priority = 0;

            mg_json_get_num(req->body, "$.age", &age);
            mg_json_get_num(req->body, "$.priority", &priority);

            if (!nameStr || !diseaseStr || strlen(nameStr) == 0 || strlen(diseaseStr) == 0 ||
                age < 1 || age > 120 || priority < 1 || priority > 4) {
                if (nameStr) free(nameStr);
                if (diseaseStr) free(diseaseStr);
                mg_http_reply(c, 400, "Content-Type: application/json\r\n",
                              "{\"error\":\"Invalid patient data!\"}");
                return;
            }

            char name[50] = {0};
            char disease[50] = {0};
            strncpy(name, nameStr, sizeof(name) - 1);
            strncpy(disease, diseaseStr, sizeof(disease) - 1);
            free(nameStr);
            free(diseaseStr);

            // Sync counters with loaded queue
            for (int i = 0; i < pq.size; i++) {
                if (pq.patients[i].id >= nextWebID) nextWebID = pq.patients[i].id + 1;
                if (pq.patients[i].arrivalOrder >= nextWebArrival) nextWebArrival = pq.patients[i].arrivalOrder + 1;
            }

            Patient p;
            p.id = nextWebID++;
            p.arrivalOrder = nextWebArrival++;
            strncpy(p.name, name, sizeof(p.name) - 1);
            p.age = (int)age;
            strncpy(p.disease, disease, sizeof(p.disease) - 1);
            p.priority = (int)priority;

            insertPatient(&pq, p);
            savePatients(&pq);

            char escapedName[100];
            char escapedDisease[100];
            print_json_string(escapedName, sizeof(escapedName), p.name);
            print_json_string(escapedDisease, sizeof(escapedDisease), p.disease);

            mg_http_reply(c, 201, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\n",
                          "{\"success\":true,\"patient\":{\"id\":%d,\"name\":\"%s\",\"age\":%d,\"disease\":\"%s\",\"priority\":%d,\"arrivalOrder\":%d}}",
                          p.id, escapedName, p.age, escapedDisease, p.priority, p.arrivalOrder);
        }
        // 4. POST /api/treat (Treat next patient)
        else if (mg_match(req->uri, mg_str("/api/treat"), NULL) && mg_strcmp(req->method, mg_str("POST")) == 0) {
            Patient treated;
            if (!removeHighestPriority(&pq, &treated)) {
                mg_http_reply(c, 400, "Content-Type: application/json\r\n",
                              "{\"success\":false,\"error\":\"No patients waiting for treatment!\"}");
                return;
            }

            savePatients(&pq);

            char escapedName[100];
            char escapedDisease[100];
            print_json_string(escapedName, sizeof(escapedName), treated.name);
            print_json_string(escapedDisease, sizeof(escapedDisease), treated.disease);

            mg_http_reply(c, 200, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\n",
                          "{\"success\":true,\"patient\":{\"id\":%d,\"name\":\"%s\",\"age\":%d,\"disease\":\"%s\",\"priority\":%d}}",
                          treated.id, escapedName, treated.age, escapedDisease, treated.priority);
        }
        // 5. DELETE /api/patients?id=... (Delete patient by ID)
        else if (mg_match(req->uri, mg_str("/api/patients"), NULL) && mg_strcmp(req->method, mg_str("DELETE")) == 0) {
            char idBuf[32];
            int id = 0;
            if (mg_http_get_var(&req->query, "id", idBuf, sizeof(idBuf)) > 0) {
                id = atoi(idBuf);
            }

            int index = -1;
            for (int i = 0; i < pq.size; i++) {
                if (pq.patients[i].id == id) {
                    index = i;
                    break;
                }
            }

            if (index == -1) {
                mg_http_reply(c, 404, "Content-Type: application/json\r\n",
                              "{\"success\":false,\"error\":\"Patient not found!\"}");
                return;
            }

            pq.patients[index] = pq.patients[pq.size - 1];
            pq.size--;

            if (index < pq.size) {
                if (index > 0 && higherPriority(pq.patients[index], pq.patients[(index - 1) / 2])) {
                    heapifyUp(&pq, index);
                } else {
                    heapifyDown(&pq, index);
                }
            }

            savePatients(&pq);

            mg_http_reply(c, 200, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\n",
                          "{\"success\":true}");
        }
        // 6. GET /api/search?id=... (Search patient by ID)
        else if (mg_match(req->uri, mg_str("/api/search"), NULL)) {
            char idBuf[32];
            int id = 0;
            if (mg_http_get_var(&req->query, "id", idBuf, sizeof(idBuf)) > 0) {
                id = atoi(idBuf);
            }

            int found = 0;
            Patient p;
            for (int i = 0; i < pq.size; i++) {
                if (pq.patients[i].id == id) {
                    p = pq.patients[i];
                    found = 1;
                    break;
                }
            }

            if (found) {
                char escapedName[100];
                char escapedDisease[100];
                print_json_string(escapedName, sizeof(escapedName), p.name);
                print_json_string(escapedDisease, sizeof(escapedDisease), p.disease);

                mg_http_reply(c, 200, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\n",
                              "{\"found\":true,\"patient\":{\"id\":%d,\"name\":\"%s\",\"age\":%d,\"disease\":\"%s\",\"priority\":%d}}",
                              p.id, escapedName, p.age, escapedDisease, p.priority);
            } else {
                mg_http_reply(c, 200, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\n",
                              "{\"found\":false}");
            }
        }
        // 7. Static file serving (HTML, CSS, JS)
        else {
            struct mg_http_serve_opts options = {
                .root_dir = "web",
                .fs = &mg_fs_posix
            };
            mg_http_serve_dir(c, req, &options);
        }
    }
}

int main(void) {
    // Initialize Priority Queue
    initializeQueue(&pq);

    // Load saved patient records from data/patients.txt
    loadPatients(&pq);

    // Sync counters with loaded queue
    for (int i = 0; i < pq.size; i++) {
        if (pq.patients[i].id >= nextWebID) nextWebID = pq.patients[i].id + 1;
        if (pq.patients[i].arrivalOrder >= nextWebArrival) nextWebArrival = pq.patients[i].arrivalOrder + 1;
    }

    struct mg_mgr manager;
    mg_mgr_init(&manager);

    // Start local C web server
    if (mg_http_listen(&manager, "http://127.0.0.1:8085", eventHandler, NULL) == NULL) {
        printf("Unable to start web server on port 8085!\n");
        mg_mgr_free(&manager);
        return 1;
    }

    printf("\n======================================================\n");
    printf("  HOSPITAL MANAGEMENT C WEB SERVER STARTED!\n");
    printf("  Website URL : http://127.0.0.1:8085\n");
    printf("  Backend     : C Binary Max Heap Priority Queue\n");
    printf("  Data File   : data/patients.txt\n");
    printf("  Press Ctrl+C in this terminal to stop the server.\n");
    printf("======================================================\n\n");

    for (;;) {
        mg_mgr_poll(&manager, 1000);
    }

    mg_mgr_free(&manager);
    return 0;
}
