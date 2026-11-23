/*
 * ============================================
 * Hospital Management System
 * Author: Krishnam Kothari
 * ============================================
 */

let heap = [];
let nextID = 1001;
let arrivalCounter = 0;
let treatmentHistory = [];

// LocalStorage persistence keys
const STORAGE_HEAP_KEY = 'hms_patient_heap';
const STORAGE_HIST_KEY = 'hms_treatment_history';
const STORAGE_NEXT_ID_KEY = 'hms_next_id';
const STORAGE_ARRIVAL_KEY = 'hms_arrival_counter';

function saveState() {
    try {
        localStorage.setItem(STORAGE_HEAP_KEY, JSON.stringify(heap));
        localStorage.setItem(STORAGE_HIST_KEY, JSON.stringify(treatmentHistory));
        localStorage.setItem(STORAGE_NEXT_ID_KEY, nextID.toString());
        localStorage.setItem(STORAGE_ARRIVAL_KEY, arrivalCounter.toString());
    } catch (e) {
        console.error('Storage save error:', e);
    }
}

function loadState() {
    try {
        const savedHeap = localStorage.getItem(STORAGE_HEAP_KEY);
        const savedHist = localStorage.getItem(STORAGE_HIST_KEY);
        const savedNextID = localStorage.getItem(STORAGE_NEXT_ID_KEY);
        const savedArrival = localStorage.getItem(STORAGE_ARRIVAL_KEY);

        if (savedHeap) heap = JSON.parse(savedHeap);
        if (savedHist) treatmentHistory = JSON.parse(savedHist);
        if (savedNextID) nextID = parseInt(savedNextID, 10);
        if (savedArrival) arrivalCounter = parseInt(savedArrival, 10);

        // Prepopulate with sample patients on very first run if empty
        if (!savedHeap && heap.length === 0) {
            insertPatient("Priya", 24, "Accident", 4);
            insertPatient("Neha", 32, "Emergency", 4);
            insertPatient("Amit", 24, "Infection", 2);
            insertPatient("Rahul", 21, "Fever", 1);
            saveState();
        }
    } catch (e) {
        console.error('Storage load error:', e);
    }
}

function higherPriority(a, b) {
    if (a.priority !== b.priority) {
        return a.priority > b.priority;
    }
    return a.arrivalOrder < b.arrivalOrder;
}

function swap(i, j) {
    let temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
}

function heapifyUp(index) {
    while (index > 0) {
        let parent = Math.floor((index - 1) / 2);
        if (higherPriority(heap[index], heap[parent])) {
            swap(index, parent);
            index = parent;
        } else {
            break;
        }
    }
}

function heapifyDown(index) {
    while (true) {
        let left  = 2 * index + 1;
        let right = 2 * index + 2;
        let best  = index;

        if (left < heap.length && higherPriority(heap[left], heap[best])) {
            best = left;
        }
        if (right < heap.length && higherPriority(heap[right], heap[best])) {
            best = right;
        }
        if (best === index) break;

        swap(index, best);
        index = best;
    }
}

function insertPatient(name, age, disease, priority) {
    let patient = {
        id: nextID++,
        name: name,
        age: age,
        disease: disease,
        priority: priority,
        arrivalOrder: ++arrivalCounter
    };

    heap.push(patient);
    heapifyUp(heap.length - 1);
    saveState();
    return patient;
}

function extractMax() {
    if (heap.length === 0) return null;

    let top = heap[0];
    heap[0] = heap[heap.length - 1];
    heap.pop();

    if (heap.length > 0) {
        heapifyDown(0);
    }

    saveState();
    return top;
}

function deleteById(id) {
    let index = -1;
    for (let i = 0; i < heap.length; i++) {
        if (heap[i].id === id) {
            index = i;
            break;
        }
    }
    if (index === -1) return false;

    heap[index] = heap[heap.length - 1];
    heap.pop();

    if (index < heap.length) {
        if (index > 0 && higherPriority(heap[index], heap[Math.floor((index - 1) / 2)])) {
            heapifyUp(index);
        } else {
            heapifyDown(index);
        }
    }

    saveState();
    return true;
}

function findById(id) {
    for (let i = 0; i < heap.length; i++) {
        if (heap[i].id === id) return heap[i];
    }
    return null;
}

function getSortedPatients() {
    let tempHeap = [...heap];
    let sorted = [];
    let tempLen = tempHeap.length;

    while (tempLen > 0) {
        sorted.push(tempHeap[0]);
        tempHeap[0] = tempHeap[tempLen - 1];
        tempLen--;

        let idx = 0;
        while (true) {
            let left  = 2 * idx + 1;
            let right = 2 * idx + 2;
            let best  = idx;

            if (left < tempLen && higherPriority(tempHeap[left], tempHeap[best])) {
                best = left;
            }
            if (right < tempLen && higherPriority(tempHeap[right], tempHeap[best])) {
                best = right;
            }
            if (best === idx) break;

            let tmp = tempHeap[idx];
            tempHeap[idx] = tempHeap[best];
            tempHeap[best] = tmp;
            idx = best;
        }
    }

    return sorted;
}

const priorityLabels = {
    4: 'Critical',
    3: 'High',
    2: 'Medium',
    1: 'Low'
};

function priorityBadge(p) {
    return `<span class="priority-badge priority-${p}">${p} - ${priorityLabels[p]}</span>`;
}

function showToast(message, type) {
    let toast = document.getElementById('toast');
    toast.textContent = message;
    toast.className = 'toast ' + type + ' show';
    setTimeout(() => {
        toast.className = 'toast';
    }, 3000);
}

function updateDashboard() {
    let counts = { 1: 0, 2: 0, 3: 0, 4: 0 };
    for (let p of heap) {
        counts[p.priority]++;
    }

    document.getElementById('totalPatients').textContent = heap.length;
    document.getElementById('criticalCount').textContent = counts[4];
    document.getElementById('highCount').textContent     = counts[3];
    document.getElementById('mediumCount').textContent   = counts[2];
    document.getElementById('lowCount').textContent      = counts[1];
}

function updateNextPatient() {
    let card = document.getElementById('nextPatientCard');
    let info = document.getElementById('nextPatientInfo');

    if (heap.length === 0) {
        card.style.display = 'none';
    } else {
        card.style.display = 'flex';
        let p = heap[0];
        info.innerHTML = `ID: ${p.id} &mdash; ${escapeHtml(p.name)} &mdash; ${escapeHtml(p.disease)} &mdash; ${priorityBadge(p.priority)}`;
    }
}

function renderQueue() {
    let tbody = document.getElementById('patientTableBody');

    if (heap.length === 0) {
        tbody.innerHTML = '<tr><td colspan="7" class="empty-msg">No patients in the queue</td></tr>';
        updateDashboard();
        updateNextPatient();
        return;
    }

    let sorted = getSortedPatients();
    let html = '';

    for (let i = 0; i < sorted.length; i++) {
        let p = sorted[i];
        html += `<tr>
            <td>${i + 1}</td>
            <td>${p.id}</td>
            <td>${escapeHtml(p.name)}</td>
            <td>${p.age}</td>
            <td>${escapeHtml(p.disease)}</td>
            <td>${priorityBadge(p.priority)}</td>
            <td><button class="btn btn-delete" onclick="deletePatientAction(${p.id})">Delete</button></td>
        </tr>`;
    }

    tbody.innerHTML = html;
    updateDashboard();
    updateNextPatient();
}

function renderHistory() {
    let tbody = document.getElementById('historyTableBody');

    if (treatmentHistory.length === 0) {
        tbody.innerHTML = '<tr><td colspan="6" class="empty-msg">No patients treated yet</td></tr>';
        return;
    }

    let html = '';
    for (let i = treatmentHistory.length - 1; i >= 0; i--) {
        let entry = treatmentHistory[i];
        let p = entry.patient;
        html += `<tr>
            <td>${treatmentHistory.length - i}</td>
            <td>${p.id}</td>
            <td>${escapeHtml(p.name)}</td>
            <td>${escapeHtml(p.disease)}</td>
            <td>${priorityBadge(p.priority)}</td>
            <td>${entry.treatedAt}</td>
        </tr>`;
    }

    tbody.innerHTML = html;
}

document.getElementById('registerForm').addEventListener('submit', function(e) {
    e.preventDefault();

    let name     = document.getElementById('patientName').value.trim();
    let age      = parseInt(document.getElementById('patientAge').value);
    let disease  = document.getElementById('patientDisease').value.trim();
    let priority = parseInt(document.getElementById('patientPriority').value);

    if (!name || !age || !disease || !priority) {
        showToast('Please fill in all fields!', 'error');
        return;
    }

    if (age < 1 || age > 120) {
        showToast('Age must be between 1 and 120!', 'error');
        return;
    }

    let patient = insertPatient(name, age, disease, priority);
    renderQueue();

    showToast(`Patient ${patient.name} registered! ID: ${patient.id}`, 'success');
    this.reset();
});

function treatPatient() {
    if (heap.length === 0) {
        showToast('No patients waiting for treatment!', 'error');
        return;
    }

    let patient = extractMax();

    let now = new Date();
    let timeStr = now.toLocaleTimeString('en-IN', { hour: '2-digit', minute: '2-digit' });
    let dateStr = now.toLocaleDateString('en-IN');

    treatmentHistory.push({
        patient: patient,
        treatedAt: dateStr + ' ' + timeStr
    });

    saveState();
    renderQueue();
    renderHistory();
    showToast(`Treating: ${patient.name} (ID: ${patient.id}) - ${priorityLabels[patient.priority]}`, 'info');
}

function searchPatient() {
    let idInput = document.getElementById('searchId');
    let id = parseInt(idInput.value);
    let resultDiv = document.getElementById('searchResult');

    if (!id) {
        showToast('Enter a valid Patient ID!', 'error');
        return;
    }

    let patient = findById(id);

    if (patient) {
        resultDiv.innerHTML = `
            <div class="search-found">
                <strong>Patient Found!</strong><br>
                <strong>ID:</strong> ${patient.id}<br>
                <strong>Name:</strong> ${escapeHtml(patient.name)}<br>
                <strong>Age:</strong> ${patient.age}<br>
                <strong>Condition:</strong> ${escapeHtml(patient.disease)}<br>
                <strong>Priority:</strong> ${priorityBadge(patient.priority)}
            </div>`;
    } else {
        resultDiv.innerHTML = `
            <div class="search-not-found">
                Patient with ID ${id} not found in the queue.
            </div>`;
    }
}

function deletePatientAction(id) {
    let success = deleteById(id);

    if (success) {
        renderQueue();
        showToast(`Patient ID ${id} removed from queue.`, 'info');
    } else {
        showToast(`Patient ID ${id} not found!`, 'error');
    }

    document.getElementById('searchResult').innerHTML = '';
}

function escapeHtml(str) {
    if (!str) return '';
    return str.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
}

// Initial load
loadState();
renderQueue();
renderHistory();
