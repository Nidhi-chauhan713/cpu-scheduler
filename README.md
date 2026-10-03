# 🖥️ CPU Scheduler Simulator

![C++](https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=c%2B%2B)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey)

A multithreaded CPU scheduling simulator written in modern C++17. Each process is
modeled as an independent worker thread, coordinated by a central dispatcher — closely
mirroring how a real OS scheduler decides *which* process runs *when*.

Implements three classic scheduling algorithms:

- **FCFS** (First-Come, First-Served)
- **Round Robin** (configurable time quantum)
- **Priority Scheduling**

For each algorithm, the simulator prints:

- 📋 A timestamped **event log** (`STARTED` / `RESUMED` / `PREEMPTED` / `FINISHED` / `IDLE`)
- 📊 A visual **Gantt chart** of CPU execution
- 📈 A **performance metrics table** — completion, waiting, and turnaround time per
  process, plus averages

---

## 📸 Sample Output

```
Gantt Chart:
|           P1           |      P2      |        P3         |
0                        5              8                   12

Performance Metrics: FCFS
   PID   Arrival   Burst  Completion   Waiting  Turnaround
----------------------------------------------------------
     1         0       5           5         0           5
     2         1       3           8         4           7
     3         2       4          12         6          10
----------------------------------------------------------
Average Waiting Time:    3.33
Average Turnaround Time: 7.33
```

---

## 📁 Project Structure

```
cpu-scheduler-simulator/
├── include/
│   ├── event_logger.h
│   ├── fcfs_scheduler.h
│   ├── priority_scheduler.h
│   ├── process.h
│   ├── round_robin_scheduler.h
│   ├── scheduler.h
│   └── scheduler_engine.h
├── src/
│   ├── event_logger.cpp
│   ├── fcfs_scheduler.cpp
│   ├── main.cpp
│   ├── priority_scheduler.cpp
│   ├── round_robin_scheduler.cpp
│   └── scheduler_engine.cpp
└── README.md
```

---

## ⚙️ Requirements

A C++17-capable compiler with real `std::thread` / `std::mutex` support (GCC 9+
recommended).

> **Windows users:** use a modern MinGW-w64 build with **posix** threading. Older
> MinGW distributions (pre-2019, GCC < 8) do not enable `<thread>`/`<mutex>` and will
> fail to compile this project.

---

## 🚀 Build & Run

**Windows:**

```powershell
g++ -std=c++17 -pthread -I include src\event_logger.cpp src\fcfs_scheduler.cpp src\main.cpp src\priority_scheduler.cpp src\round_robin_scheduler.cpp src\scheduler_engine.cpp -o sim.exe
.\sim.exe
```

**Linux / macOS:**

```bash
g++ -std=c++17 -pthread -I include src/event_logger.cpp src/fcfs_scheduler.cpp src/main.cpp src/priority_scheduler.cpp src/round_robin_scheduler.cpp src/scheduler_engine.cpp -o sim
./sim
```

> `-pthread` is required — it enables the threading primitives used by
> `scheduler_engine.cpp`.

---

## 🧠 Design Notes

**Why real threads?** Each `Process` runs on its own worker thread inside
`SchedulerEngine::processWorker`. A central dispatcher loop
(`SchedulerEngine::run`) decides which process's thread is allowed to execute at
each simulated time tick, using one condition variable to wake the chosen worker
and a second to wait for that worker to report the tick complete. This mirrors the
real OS distinction between *which process is schedulable* (the `Scheduler`
interface) and *which process is actually running right now* (the engine's
dispatch loop) — while keeping each process's state (`remaining_time`,
`start_time`, etc.) naturally encapsulated in its own thread of control.

**Where the metrics come from:** `completion_time`, `waiting_time`, and
`turnaround_time` are computed once a process finishes its final tick, using the
standard formulas:

```
turnaround_time = completion_time - arrival_time
waiting_time    = turnaround_time - burst_time
```

**Adding a new scheduling algorithm:** implement the `Scheduler` interface
(`addProcess`, `selectNextProcess`, `empty` — see `scheduler.h`), then add a new
run block in `main.cpp` following the pattern used for `FCFSScheduler` /
`RoundRobinScheduler` / `PriorityScheduler`.

---

## 🔭 Known Limitations

- The process list is currently hardcoded in `main.cpp` — no file/CLI input yet.
- A process with `burst_time <= 0` is clamped to 1 tick in `SchedulerEngine::run`
  to avoid an edge case where it would never report completion.
