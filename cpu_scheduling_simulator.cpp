#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <iomanip>

using namespace std;

struct Process {
    int id, arrival, burst, waiting, turnaround, remaining, completion;
};

// Function to print Gantt Chart
void printGanttChart(vector<int> &timeline) {
    cout << "\nGantt Chart:\n";
    for (int p : timeline) {
        cout << "P" << p << " | ";
    }
    cout << "\n";
}

// Function to display process details
void displayResults(vector<Process> &processes) {
    double total_waiting = 0, total_turnaround = 0;
    cout << "\nProcess\tWaiting\tTurnaround\n";
    for (auto &p : processes) {
        cout << "P" << p.id << "\t" << p.waiting << "\t" << p.turnaround << "\n";
        total_waiting += p.waiting;
        total_turnaround += p.turnaround;
    }
    cout << fixed << setprecision(2);
    cout << "Average Waiting Time: " << total_waiting / processes.size() << "\n";
    cout << "Average Turnaround Time: " << total_turnaround / processes.size() << "\n";
}

// FCFS Scheduling
void fcfs(vector<Process> &processes) {
    sort(processes.begin(), processes.end(), [](Process a, Process b) {
        return a.arrival < b.arrival;
    });
    
    int time = 0;
    vector<int> timeline;
    for (auto &p : processes) {
        if (time < p.arrival) time = p.arrival;
        p.waiting = time - p.arrival;
        time += p.burst;
        p.turnaround = p.waiting + p.burst;
        timeline.push_back(p.id);
    }
    printGanttChart(timeline);
    displayResults(processes);
}

// SJF Scheduling (Non-Preemptive)
void sjf(vector<Process> &processes) {
    sort(processes.begin(), processes.end(), [](Process a, Process b) {
        return a.arrival < b.arrival;
    });

    int time = 0, completed = 0;
    vector<int> timeline;
    vector<bool> isCompleted(processes.size(), false);

    while (completed < processes.size()) {
        int min_index = -1;
        int min_burst = 1e9;

        for (int i = 0; i < processes.size(); i++) {
            if (!isCompleted[i] && processes[i].arrival <= time && processes[i].burst < min_burst) {
                min_burst = processes[i].burst;
                min_index = i;
            }
        }

        if (min_index == -1) {
            time++;
            continue;
        }

        timeline.push_back(processes[min_index].id);
        processes[min_index].waiting = time - processes[min_index].arrival;
        time += processes[min_index].burst;
        processes[min_index].turnaround = processes[min_index].waiting + processes[min_index].burst;
        isCompleted[min_index] = true;
        completed++;
    }
    printGanttChart(timeline);
    displayResults(processes);
}

// SRT Scheduling (Preemptive)
void srt(vector<Process> &processes) {
    int time = 0, completed = 0;
    vector<int> timeline;

    while (completed < processes.size()) {
        int min_index = -1;
        int min_remaining = 1e9;

        for (int i = 0; i < processes.size(); i++) {
            if (processes[i].arrival <= time && processes[i].remaining > 0 && processes[i].remaining < min_remaining) {
                min_remaining = processes[i].remaining;
                min_index = i;
            }
        }

        if (min_index == -1) {
            time++;
            continue;
        }

        timeline.push_back(processes[min_index].id);
        processes[min_index].remaining--;
        time++;

        if (processes[min_index].remaining == 0) {
            completed++;
            processes[min_index].completion = time;
            processes[min_index].turnaround = processes[min_index].completion - processes[min_index].arrival;
            processes[min_index].waiting = processes[min_index].turnaround - processes[min_index].burst;
        }
    }
    printGanttChart(timeline);
    displayResults(processes);
}

// Round Robin Scheduling
void round_robin(vector<Process> &processes, int quantum) {
    queue<int> q;
    for (int i = 0; i < processes.size(); i++) {
        if (processes[i].arrival == 0) q.push(i);
    }

    int time = 0, completed = 0;
    vector<int> timeline;

    while (completed < processes.size()) {
        if (q.empty()) {
            time++;
            for (int i = 0; i < processes.size(); i++) {
                if (processes[i].arrival == time) q.push(i);
            }
            continue;
        }

        int i = q.front(); q.pop();
        timeline.push_back(processes[i].id);
        int exec_time = min(quantum, processes[i].remaining);
        time += exec_time;
        processes[i].remaining -= exec_time;

        for (int j = 0; j < processes.size(); j++) {
            if (j != i && processes[j].arrival <= time && processes[j].remaining > 0) {
                q.push(j);
            }
        }

        if (processes[i].remaining == 0) {
            completed++;
            processes[i].turnaround = time - processes[i].arrival;
            processes[i].waiting = processes[i].turnaround - processes[i].burst;
        } else {
            q.push(i);
        }
    }
    printGanttChart(timeline);
    displayResults(processes);
}

int main() {
    while (true) {
        int n, choice, quantum;
        cout << "\nEnter number of processes: ";
        cin >> n;
        if (n <= 0) {
            cout << "Invalid input. Number of processes must be positive.\n";
            continue;
        }

        vector<Process> processes(n);
        for (int i = 0; i < n; i++) {
            processes[i].id = i + 1;
            cout << "Enter arrival and burst time for P" << i + 1 << ": ";
            cin >> processes[i].arrival >> processes[i].burst;
            if (processes[i].arrival < 0 || processes[i].burst <= 0) {
                cout << "Invalid input. Arrival and burst time must be non-negative and burst time must be greater than 0.\n";
                i--;
                continue;
            }
            processes[i].remaining = processes[i].burst;
        }

        cout << "1.FCFS 2.SJF 3.SRT 4.RR 5.Exit: ";
        cin >> choice;
        if (choice == 5) break;

        switch (choice) {
            case 1: fcfs(processes); break;
            case 2: sjf(processes); break;
            case 3: srt(processes); break;
            case 4:
                cout << "Enter time quantum: ";
                cin >> quantum;
                if (quantum <= 0) {
                    cout << "Invalid quantum value. It must be positive.\n";
                    continue;
                }
                round_robin(processes, quantum);
                break;
            default: cout << "Invalid choice! Try again.\n";
        }
    }
    return 0;
}
