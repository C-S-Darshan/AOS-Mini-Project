# Fixed Priority Scheduling React Frontend

This React app visualizes the real-time fixed priority scheduling demo from your custom Linux kernel module.

## How it works
- The backend server (Node.js) reads `/proc/fixed_sched` and exposes it as a REST API.
- The React frontend fetches this data every second and displays it in a live, color-coded table.

## Usage
1. Start the backend server:
   ```bash
   cd ../backend
   npm install express cors
   node server.js
   ```
2. Start the React frontend:
   ```bash
   cd ../frontend
   npm install
   npm start
   ```
3. Open [http://localhost:3000](http://localhost:3000) in your browser.

You will see a live dashboard of your real-time fixed priority scheduling kernel module!
