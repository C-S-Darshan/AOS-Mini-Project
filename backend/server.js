// Simple Node.js backend to serve /proc/fixed_sched to React frontend
const express = require('express');
const fs = require('fs');
const cors = require('cors');
const app = express();
const PORT = 5000;

app.use(cors());

app.get('/api/fixed_sched', (req, res) => {
    fs.readFile('/proc/fixed_sched', 'utf8', (err, data) => {
        if (err) {
            return res.status(500).json({ error: 'Could not read /proc/fixed_sched' });
        }
        // Parse the data into JSON for easier frontend use
        const lines = data.trim().split('\n');
        const headers = lines[0].split(/\s+/);
        const tasks = lines.slice(1).map(line => {
            const [TaskID, Priority, Status, LastRun] = line.split(/\s+/);
            return { TaskID, Priority, Status, LastRun };
        });
        res.json({ headers, tasks });
    });
});

app.listen(PORT, () => {
    console.log(`Backend server running on http://localhost:${PORT}`);
});
