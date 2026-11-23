/* ============================================
   Hospital Management System - Frontend App
   Communicates with the C HTTP Server Backend
   ============================================ */

let treatmentHistory = [];

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

// Fetch all patients from C backend
async function fetchPatients() {
    const statusElem = document.getElementById('serverStatus');
    try {
        const res = await fetch('/api/patients');
        if (!res.ok) throw new Error('C backend returned ' + res.status);
        const data = await res.json();
        
        statusElem.textContent = 'Connected';
        statusElem.style.color = '#38a169';

        renderQueue(data.patients);
        updateDashboard(data.patients);
    } catch (err) {
        console.error('Connection error:', err);
        statusElem.textContent = 'Offline';
        statusElem.style.color = '#e53e3e';
    }
}

// Render queue received from C
function renderQueue(patients) {
    const tbody = document.getElementById('patientTableBody');
    const nextCard = document.getElementById('nextPatientCard');
    const nextInfo = document.getElementById('nextPatientInfo');

    if (!patients || patients.length === 0) {
        tbody.innerHTML = '<tr><td colspan="7" class="empty-msg">No patients in the queue</td></tr>';
        nextCard.style.display = 'none';
        return;
    }

    // Next patient is the root (position 0 in sorted list)
    const next = patients[0];
    nextCard.style.display = 'flex';
    nextInfo.innerHTML = `ID: ${next.id} &mdash; ${next.name} &mdash; ${next.disease} &mdash; ${priorityBadge(next.priority)}`;

    let html = '';
    patients.forEach((p, index) => {
        html += `<tr>
            <td>${index + 1}</td>
            <td>${p.id}</td>
            <td>${escapeHtml(p.name)}</td>
            <td>${p.age}</td>
            <td>${escapeHtml(p.disease)}</td>
            <td>${priorityBadge(p.priority)}</td>
            <td><button class="btn btn-delete" onclick="deletePatient(${p.id})">Delete</button></td>
        </tr>`;
    });

    tbody.innerHTML = html;
}

// Update dashboard counters
function updateDashboard(patients) {
    patients = patients || [];
    let counts = { 1: 0, 2: 0, 3: 0, 4: 0 };
    patients.forEach(p => {
        if (counts[p.priority] !== undefined) counts[p.priority]++;
    });

    document.getElementById('totalPatients').textContent = patients.length;
    document.getElementById('criticalCount').textContent = counts[4];
    document.getElementById('highCount').textContent     = counts[3];
    document.getElementById('mediumCount').textContent   = counts[2];
    document.getElementById('lowCount').textContent      = counts[1];
}

// Register patient by sending POST to C backend
document.getElementById('registerForm').addEventListener('submit', async function(e) {
    e.preventDefault();

    const name     = document.getElementById('patientName').value.trim();
    const age      = parseInt(document.getElementById('patientAge').value);
    const disease  = document.getElementById('patientDisease').value.trim();
    const priority = parseInt(document.getElementById('patientPriority').value);

    if (!name || !age || !disease || !priority) {
        showToast('Please fill in all fields!', 'error');
        return;
    }

    try {
        const res = await fetch('/api/patients', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ name, age, disease, priority })
        });

        if (!res.ok) {
            const errData = await res.json();
            throw new Error(errData.error || 'Registration failed');
        }

        const result = await res.json();
        showToast(`Patient registered! ID: ${result.patient.id}`, 'success');
        this.reset();
        fetchPatients();
    } catch (err) {
        showToast(err.message, 'error');
    }
});

// Treat next patient by sending POST to C backend
async function treatNextPatient() {
    try {
        const res = await fetch('/api/treat', { method: 'POST' });
        const result = await res.json();

        if (!res.ok || !result.success) {
            showToast(result.error || 'No patients waiting!', 'error');
            return;
        }

        const p = result.patient;
        const now = new Date();
        const timeStr = now.toLocaleTimeString('en-IN', { hour: '2-digit', minute: '2-digit' });
        const dateStr = now.toLocaleDateString('en-IN');

        treatmentHistory.push({
            patient: p,
            treatedAt: dateStr + ' ' + timeStr
        });

        renderHistory();
        showToast(`Treated: ${p.name} (ID: ${p.id}) - ${priorityLabels[p.priority]}`, 'info');
        fetchPatients();
    } catch (err) {
        showToast('Treatment failed: ' + err.message, 'error');
    }
}

// Search patient by ID
async function searchPatient() {
    const id = parseInt(document.getElementById('searchId').value);
    const resultDiv = document.getElementById('searchResult');

    if (!id) {
        showToast('Please enter a valid Patient ID!', 'error');
        return;
    }

    try {
        const res = await fetch(`/api/search?id=${id}`);
        const data = await res.json();

        if (data.found) {
            const p = data.patient;
            resultDiv.innerHTML = `
                <div class="search-found">
                    <strong>Patient Found!</strong><br>
                    <strong>ID:</strong> ${p.id}<br>
                    <strong>Name:</strong> ${escapeHtml(p.name)}<br>
                    <strong>Age:</strong> ${p.age}<br>
                    <strong>Condition:</strong> ${escapeHtml(p.disease)}<br>
                    <strong>Priority:</strong> ${priorityBadge(p.priority)}
                </div>`;
        } else {
            resultDiv.innerHTML = `
                <div class="search-not-found">
                    Patient with ID ${id} not found in the waiting list.
                </div>`;
        }
    } catch (err) {
        showToast('Search failed: ' + err.message, 'error');
    }
}

// Delete patient from C backend
async function deletePatient(id) {
    if (!confirm(`Are you sure you want to delete patient ID ${id}?`)) return;

    try {
        const res = await fetch(`/api/patients?id=${id}`, { method: 'DELETE' });
        const data = await res.json();

        if (res.ok && data.success) {
            showToast(`Patient ID ${id} removed.`, 'info');
            fetchPatients();
        } else {
            showToast(data.error || 'Delete failed', 'error');
        }

        document.getElementById('searchResult').innerHTML = '';
    } catch (err) {
        showToast('Delete error: ' + err.message, 'error');
    }
}

// Render treatment history table
function renderHistory() {
    const tbody = document.getElementById('historyTableBody');
    if (treatmentHistory.length === 0) {
        tbody.innerHTML = '<tr><td colspan="6" class="empty-msg">No patients treated yet</td></tr>';
        return;
    }

    let html = '';
    for (let i = treatmentHistory.length - 1; i >= 0; i--) {
        const entry = treatmentHistory[i];
        const p = entry.patient;
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

function escapeHtml(str) {
    if (!str) return '';
    return str.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
}

// Initial fetch & poll every 5 seconds
window.addEventListener('DOMContentLoaded', () => {
    fetchPatients();
    setInterval(fetchPatients, 5000);
});
