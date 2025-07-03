import React, { useEffect, useState } from 'react';
import './App.css';

function App() {
  const [tasks, setTasks] = useState([]);
  const [headers, setHeaders] = useState([]);

  useEffect(() => {
    const fetchData = () => {
      fetch('http://localhost:5000/api/fixed_sched')
        .then(res => res.json())
        .then(data => {
          setHeaders(data.headers);
          setTasks(data.tasks);
        })
        .catch(() => {
          setTasks([]);
        });
    };
    fetchData();
    const interval = setInterval(fetchData, 1000);
    return () => clearInterval(interval);
  }, []);

  return (
    <div className="App">
      <h1>Real-Time Fixed Priority Scheduling Demo</h1>
      <table>
        <thead>
          <tr>
            {headers.map(h => <th key={h}>{h}</th>)}
          </tr>
        </thead>
        <tbody>
          {tasks.map(task => (
            <tr key={task.TaskID} className={task.Status === 'Running' ? 'running' : 'stopped'}>
              <td>{task.TaskID}</td>
              <td>{task.Priority}</td>
              <td>{task.Status}</td>
              <td>{task.LastRun}</td>
            </tr>
          ))}
        </tbody>
      </table>
      <div className="legend">
        <span className="running">● Running</span>
        <span className="stopped">● Stopped</span>
      </div>
    </div>
  );
}

export default App;
