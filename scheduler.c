#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 10

// 프로세스 구조체 선언
typedef struct
{
    int pid;
    int t_arrival;
    int t_burst;
    int prty; // priority (우선순위 1: highest)
    int t_wait;
    int t_turnaround;
    int t_complete;
    int t_remaining;
} Process;

// 전역변수 선언
Process ready[MAX]; // 프로세스 10개를 담을 main 배열
int p_total = 0;    // 실제로 생성된 프로세스 개수
int quantum = 0;    // RR에서 사용할 time quantum 기준

// 주요 함수 선언
void fcfs();        // 1. First Come First Serve
void sjf_np();      // 2. Shortest Job First (Non-preemptive)
void sjf_p();       // 3. Shortest Job First (Preemptive)
void priority_np(); // 4. Priority (Non-preemptive)
void priority_p();  // 5. Priority (Preemptive)
void rr();          // 6. Round Robin
void compare_all(); // 7. 알고리즘 별 성능 비교

void show_gantt(int g_pid[], int g_time[], int g_index);
// g_pid: pid 출력용 배열 | g_time: 시간 표시용 배열 | g_index: 간트차트 내 구분 단위
void show_results();
// Average Waiting & Turnaround Time 출력

int main()
{
    int choice;

    while (1)
    {
        printf("\n=== OS Scheduling Algorithm Simulator ===\n");
        printf("1. Create Random Processes\n");
        printf("2. Create Manual Processes\n");
        printf("3. Exit\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printf("How many processes do you want to create? ");
            scanf("%d", &p_total);

            if (p_total <= 0 || p_total > MAX)
            {
                printf("Invalid number of processes.\n");
                p_total = 0;
                break;
            }

            for (int i = 0; i < p_total; i++)
            {
                ready[i].pid = i + 1;
                ready[i].t_arrival = rand() % 11;     // 0-10
                ready[i].t_burst = rand() % 10 + 1;   // 1-10
                ready[i].prty = rand() % p_total + 1; // 1-n
                ready[i].t_remaining = ready[i].t_burst;
                ready[i].t_wait = 0;
                ready[i].t_turnaround = 0;
                ready[i].t_complete = 0;
            }

            printf("\nRandomly generated processes:\n");
            printf("\n+-----+----------+----------+----------+\n");
            printf("| PID | Arrival  | Burst    | Priority |\n");
            printf("|     | Time     | Time     |          |\n");
            printf("+-----+----------+----------+----------+\n");

            for (int i = 0; i < p_total; i++)
            {
                printf("| %3d | %8d | %8d | %8d |\n", ready[i].pid, ready[i].t_arrival, ready[i].t_burst, ready[i].prty);
            }
            printf("+-----+----------+----------+----------+\n");
            break;
        case 2:
            printf("How many processes do you want to create? ");
            scanf("%d", &p_total);

            if (p_total <= 0 || p_total > MAX)
            {
                printf("Invalid number of processes.\n");
                p_total = 0;
                break;
            }

            for (int i = 0; i < p_total; i++)
            {
                ready[i].pid = i + 1;

                printf("\nProcess %d:\n", i + 1);
                printf("Enter arrival time: ");
                scanf("%d", &ready[i].t_arrival);
                printf("Enter burst time: ");
                scanf("%d", &ready[i].t_burst);
                printf("Enter priority: ");
                scanf("%d", &ready[i].prty);

                ready[i].t_remaining = ready[i].t_burst;
                ready[i].t_wait = 0;
                ready[i].t_turnaround = 0;
                ready[i].t_complete = 0;
            }

            printf("\nManually created processes:\n");
            printf("\n+-----+----------+----------+----------+\n");
            printf("| PID | Arrival  | Burst    | Priority |\n");
            printf("|     | Time     | Time     |          |\n");
            printf("+-----+----------+----------+----------+\n");

            for (int i = 0; i < p_total; i++)
            {
                printf("| %3d | %8d | %8d | %8d |\n", ready[i].pid, ready[i].t_arrival, ready[i].t_burst, ready[i].prty);
            }
            printf("+-----+----------+----------+----------+\n");
            break;
        case 3:
            printf("Exiting program...\n");
            exit(0);
        default:
            printf("Invalid choice. Please try again.\n");
        }

        int choice = 0;

        while (1)
        {
            printf("\n=== Scheduling Algorithms ===\n");
            printf("1. FCFS (First Come First Serve)\n");
            printf("2. SJF Non-Preemptive (Shortest Job First)\n");
            printf("3. SJF Preemptive (Shortest Remaining Time First)\n");
            printf("4. Priority Non-Preemptive\n");
            printf("5. Priority Preemptive\n");
            printf("6. Round Robin\n");
            printf("7. Compare All Algorithms\n");
            printf("8. Exit\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);

            switch (choice)
            {
            case 1:
                fcfs();
                break;
            case 2:
                sjf_np();
                break;
            case 3:
                sjf_p();
                break;
            case 4:
                priority_np();
                break;
            case 5:
                priority_p();
                break;
            case 6:
                printf("Enter time quantum: ");
                scanf("%d", &quantum); // RR을 위한 time quantum 값을 입력받고 전역변수에 할당
                rr();
                break;
            case 7:
                printf("Enter time quantum: ");
                scanf("%d", &quantum); // RR을 위한 time quantum 값을 입력받고 전역변수에 할당
                compare_all();
                break;
            case 8:
                printf("Exiting program...\n");
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
            }
        }
    }
}

void fcfs()
{
    printf("\n=== FCFS Scheduling ===\n");

    // 전체 프로세스 정렬을 위해 배열 복사
    Process sorted_ready[MAX];
    for (int i = 0; i < p_total; i++)
    {
        sorted_ready[i] = ready[i];
        sorted_ready[i].t_remaining = ready[i].t_burst; // 남은 시간 초기화
    }

    // 도착 시간 기준으로 정렬 (버블 정렬)
    for (int i = 0; i < p_total - 1; i++)
    {
        for (int j = 0; j < p_total - i - 1; j++)
        {
            if (sorted_ready[j].t_arrival > sorted_ready[j + 1].t_arrival)
            {
                Process temp = sorted_ready[j];
                sorted_ready[j] = sorted_ready[j + 1];
                sorted_ready[j + 1] = temp;
            }
        }
    }

    // 간트 차트용 배열
    int g_pid[50];   // pid-stamp 저장용
    int g_time[50];  // timestamp 저장용
    int g_index = 0; // 간트 차트 내 인덱스 초기화
    g_time[0] = 0;   // 첫번째 timestamp는 0으로 고정

    int p_completed = 0; // 작업 완료된 프로세스 개수
    int t_now = 0;

    while (p_completed < p_total)
    {
        int p_selected = -1; // -1: 선택된 프로세스가 없는 경우

        // 현재 시간까지 도착한 프로세스들 중에서 가장 먼저 도착한 프로세스 찾기
        for (int i = 0; i < p_total; i++)
        {
            // 아직 완료되지 않았고, 도착 시간이 현재 시간 이하인 프로세스들 중에서
            if (sorted_ready[i].t_remaining > 0 && sorted_ready[i].t_arrival <= t_now)
            {
                p_selected = i; // FCFS이므로 첫 번째로 찾은 프로세스가 가장 먼저 도착한 프로세스
                break;
            }
        }

        // 만약 현재 시간에 실행할 수 있는 프로세스가 없다면 (-1) IDLE로 처리
        if (p_selected == -1)
        {
            // 가장 빨리 도착하는 미완료 프로세스의 도착 시간 찾기
            int t_next_arrival = 999; // 임의 숫자 (999)
            for (int i = 0; i < p_total; i++)
            {
                if (sorted_ready[i].t_remaining > 0 && sorted_ready[i].t_arrival < t_next_arrival)
                {
                    t_next_arrival = sorted_ready[i].t_arrival;
                }
            }

            // IDLE을 간트 차트에 추가
            g_pid[g_index] = 0;                   // pid = 0은 IDLE을 의미
            g_time[g_index + 1] = t_next_arrival; // 다음 프로세스로 이동
            g_index++;

            t_now = t_next_arrival;
            continue; // 다시 프로세스 선택부터 시작 (while문 반복)
        }

        // 선택된 프로세스가 있으므로 실행 (Non-Preemptive이므로 끝까지 실행)
        g_pid[g_index] = sorted_ready[p_selected].pid;
        g_time[g_index + 1] = t_now + sorted_ready[p_selected].t_burst;
        g_index++;

        // 완료 시간, 대기 시간, 총 처리 시간을 계산 (총 처리 시간 = 완료 시간 - 도착 시간 & 대기 시간 = 총 처리 시간 - 실행 시간)
        t_now += sorted_ready[p_selected].t_burst;
        sorted_ready[p_selected].t_complete = t_now;
        sorted_ready[p_selected].t_turnaround = sorted_ready[p_selected].t_complete - sorted_ready[p_selected].t_arrival;
        sorted_ready[p_selected].t_wait = sorted_ready[p_selected].t_turnaround - sorted_ready[p_selected].t_burst;
        sorted_ready[p_selected].t_remaining = 0; // 프로세스 완료
        p_completed++;
    }

    // 결과를 기존 processes 배열에 복사 (기존 순서 유지 목적)
    for (int i = 0; i < p_total; i++)
    {
        for (int j = 0; j < p_total; j++)
        {
            if (ready[i].pid == sorted_ready[j].pid)
            {
                ready[i].t_complete = sorted_ready[j].t_complete;
                ready[i].t_turnaround = sorted_ready[j].t_turnaround;
                ready[i].t_wait = sorted_ready[j].t_wait;
                ready[i].t_remaining = 0; // 모든 프로세스 완료
                break;
            }
        }
    }

    // 간트 차트와 결과 출력
    show_gantt(g_pid, g_time, g_index);
    show_results();
}

void sjf_np()
{
    printf("\n=== SJF Non-Preemptive Scheduling ===\n");

    Process sorted_ready[MAX];
    for (int i = 0; i < p_total; i++)
    {
        sorted_ready[i] = ready[i];
        sorted_ready[i].t_remaining = ready[i].t_burst;
    }

    for (int i = 0; i < p_total - 1; i++)
    {
        for (int j = 0; j < p_total - i - 1; j++)
        {
            if (sorted_ready[j].t_arrival > sorted_ready[j + 1].t_arrival)
            {
                Process temp = sorted_ready[j];
                sorted_ready[j] = sorted_ready[j + 1];
                sorted_ready[j + 1] = temp;
            }
        }
    }

    int g_pid[50];
    int g_time[50];
    int g_index = 0;
    g_time[0] = 0;

    int p_completed = 0;
    int t_now = 0;

    while (p_completed < p_total)
    {
        int p_selected = -1;
        int t_shortest = 999; // 임의 숫자 (999)

        // 현재 시간까지 도착한 프로세스들 중에서 가장 짧은 burst time을 가진 프로세스 찾기
        for (int i = 0; i < p_total; i++)
        {
            // 아직 완료되지 않았고, 도착 시간이 현재 시간 이하인 (도착한) 프로세스들 중에서
            if (sorted_ready[i].t_remaining > 0 && sorted_ready[i].t_arrival <= t_now)
            {
                if (sorted_ready[i].t_burst < t_shortest)
                {
                    t_shortest = sorted_ready[i].t_burst;
                    p_selected = i; // 가장 짧은 burst를 가진 프로세스 선택
                }
            }
        }

        // IDLE: 만약 현재 시간에 실행할 수 있는 프로세스가 없다면 -1
        if (p_selected == -1)
        {
            // 가장 빨리 도착하는 미완료 프로세스의 도착 시간 찾고 시간 이동
            int t_next_arrival = 999;
            for (int i = 0; i < p_total; i++)
            {
                if (sorted_ready[i].t_remaining > 0 && sorted_ready[i].t_arrival < t_next_arrival)
                {
                    t_next_arrival = sorted_ready[i].t_arrival;
                }
            }

            // IDLE을 간트 차트에 추가
            g_pid[g_index] = 0; // 0은 IDLE을 의미
            g_time[g_index + 1] = t_next_arrival;
            g_index++;

            t_now = t_next_arrival;
            continue; // 다시 프로세스 선택부터 시작
        }

        // 선택된 프로세스를 실행 (Non-Preemptive이므로 끝까지 실행)
        g_pid[g_index] = sorted_ready[p_selected].pid;
        g_time[g_index + 1] = t_now + sorted_ready[p_selected].t_burst;
        g_index++;

        // 프로세스 완료 시간 및 기타 시간 계산
        t_now += sorted_ready[p_selected].t_burst;
        sorted_ready[p_selected].t_complete = t_now;
        sorted_ready[p_selected].t_turnaround = sorted_ready[p_selected].t_complete - sorted_ready[p_selected].t_arrival;
        sorted_ready[p_selected].t_wait = sorted_ready[p_selected].t_turnaround - sorted_ready[p_selected].t_burst;
        sorted_ready[p_selected].t_remaining = 0;
        p_completed++;
    }

    // 결과를 기존 processes 배열에 복사 (기존 순서 유지 목적)
    for (int i = 0; i < p_total; i++)
    {
        for (int j = 0; j < p_total; j++)
        {
            if (ready[i].pid == sorted_ready[j].pid)
            {
                ready[i].t_complete = sorted_ready[j].t_complete;
                ready[i].t_turnaround = sorted_ready[j].t_turnaround;
                ready[i].t_wait = sorted_ready[j].t_wait;
                ready[i].t_remaining = 0; // 모든 프로세스 완료
                break;
            }
        }
    }

    // 간트 차트와 결과 출력
    show_gantt(g_pid, g_time, g_index);
    show_results();
}

void sjf_p()
{
    printf("\n=== SJF Preemptive Scheduling ===\n");

    Process temp_ready[MAX];
    for (int i = 0; i < p_total; i++)
    {
        temp_ready[i] = ready[i];
        temp_ready[i].t_remaining = ready[i].t_burst;
    }

    // 간트 차트용 배열 (preemptive는 더 많은 context switch 가능)
    int g_pid[100];
    int g_time[100];
    int g_index = 0;
    g_time[0] = 0;

    int p_completed = 0;
    int t_now = 0;
    int g_prev = -1; // 이전에 실행된 프로세스 (간트 차트용)

    // 시간 단위로 스케줄링 실행 (t_now++으로 진행)
    while (p_completed < p_total)
    {
        int p_selected = -1;
        int t_shortest = 999;

        // 현재 시간에 도착한 프로세스들 중에서 가장 짧은 remaining time 찾기
        for (int i = 0; i < p_total; i++)
        {
            // 아직 완료되지 않았고, 도착 시간이 현재 시간 이하인 프로세스
            if (temp_ready[i].t_remaining > 0 && temp_ready[i].t_arrival <= t_now)
            {
                if (temp_ready[i].t_remaining < t_shortest)
                {
                    t_shortest = temp_ready[i].t_remaining;
                    p_selected = i;
                }
            }
        }

        // 실행할 프로세스가 없으면 IDLE
        if (p_selected == -1)
        {
            // 간트 차트에 IDLE 추가 (이전 프로세스와 다르면)
            if (g_prev != 0) // 0 = IDLE
            {
                g_pid[g_index] = 0;
                g_time[g_index + 1] = t_now + 1;
                g_index++;
                g_prev = 0;
            }
            else
            {
                // 이미 IDLE이면 시간만 연장
                g_time[g_index] = t_now + 1;
            }
            t_now++;
            continue;
        }

        // 프로세스 변경되었으면 간트 차트에 추가
        if (g_prev != temp_ready[p_selected].pid)
        {
            g_pid[g_index] = temp_ready[p_selected].pid;
            g_time[g_index + 1] = t_now + 1;
            g_index++;
            g_prev = temp_ready[p_selected].pid;
        }
        else
        {
            // 같은 프로세스가 계속 실행되면 시간만 연장
            g_time[g_index] = t_now + 1;
        }

        // 선택된 프로세스 1만큼 실행
        temp_ready[p_selected].t_remaining--;
        t_now++;

        // 프로세스가 완료되었는지 확인, 완료된 프로세스라면 추가 시간 정보 계산해서 저장
        if (temp_ready[p_selected].t_remaining == 0)
        {
            temp_ready[p_selected].t_complete = t_now;
            temp_ready[p_selected].t_turnaround = temp_ready[p_selected].t_complete - temp_ready[p_selected].t_arrival;
            temp_ready[p_selected].t_wait = temp_ready[p_selected].t_turnaround - temp_ready[p_selected].t_burst;
            p_completed++;
        }
    }

    // 결과를 기존 processes 배열에 복사
    for (int i = 0; i < p_total; i++)
    {
        for (int j = 0; j < p_total; j++)
        {
            if (ready[i].pid == temp_ready[j].pid)
            {
                ready[i].t_complete = temp_ready[j].t_complete;
                ready[i].t_turnaround = temp_ready[j].t_turnaround;
                ready[i].t_wait = temp_ready[j].t_wait;
                ready[i].t_remaining = 0; // 모든 프로세스 완료
                break;
            }
        }
    }

    // 간트 차트와 결과 출력
    show_gantt(g_pid, g_time, g_index);
    show_results();
}

void priority_np()
{
    printf("\n=== Priority Non-Preemptive Scheduling ===\n");

    Process sorted_ready[MAX];
    for (int i = 0; i < p_total; i++)
    {
        sorted_ready[i] = ready[i];
        sorted_ready[i].t_remaining = ready[i].t_burst;
    }

    // 도착 시간으로 정렬 (버블 정렬)
    for (int i = 0; i < p_total - 1; i++)
    {
        for (int j = 0; j < p_total - i - 1; j++)
        {
            if (sorted_ready[j].t_arrival > sorted_ready[j + 1].t_arrival)
            {
                Process temp = sorted_ready[j];
                sorted_ready[j] = sorted_ready[j + 1];
                sorted_ready[j + 1] = temp;
            }
        }
    }

    int g_pid[50];
    int g_time[50];
    int g_index = 0;
    g_time[0] = 0;

    int p_completed = 0;
    int t_now = 0;

    // 전반적인 로직은 SJF와 동일 - 다만 기준만 remaining time에서 priority로 변경
    while (p_completed < p_total)
    {
        int p_selected = -1;
        int top_prty = 999; // 가장 낮은 우선순위로 초기화 (숫자가 클수록 우선순위 낮음)

        // 현재 시간까지 도착한 프로세스들 중에서 가장 높은 우선순위를 가진 프로세스 찾기
        for (int i = 0; i < p_total; i++)
        {
            // 아직 완료되지 않았고, 도착 시간이 현재 시간 이하인 프로세스들 중에서
            if (sorted_ready[i].t_remaining > 0 && sorted_ready[i].t_arrival <= t_now)
            {
                // 우선순위가 더 높으면 (숫자가 더 작으면) 선택
                if (sorted_ready[i].prty < top_prty)
                {
                    top_prty = sorted_ready[i].prty;
                    p_selected = i;
                }
            }
        }

        // 만약 현재 시간에 실행할 수 있는 프로세스가 없다면 (-1) IDLE로 처리
        if (p_selected == -1)
        {
            // 가장 빨리 도착하는 미완료 프로세스의 도착 시간 찾기
            int t_next_arrival = 999;
            for (int i = 0; i < p_total; i++)
            {
                if (sorted_ready[i].t_remaining > 0 && sorted_ready[i].t_arrival < t_next_arrival)
                {
                    t_next_arrival = sorted_ready[i].t_arrival;
                }
            }

            // IDLE 시간을 간트 차트에 추가
            g_pid[g_index] = 0; // 0은 IDLE을 의미
            g_time[g_index + 1] = t_next_arrival;
            g_index++;

            t_now = t_next_arrival;
            continue; // 다시 프로세스 선택부터 시작
        }

        // 선택된 프로세스를 실행 (Non-Preemptive이므로 끝까지 실행)
        g_pid[g_index] = sorted_ready[p_selected].pid;
        g_time[g_index + 1] = t_now + sorted_ready[p_selected].t_burst;
        g_index++;

        // 프로세스 완료 시간 및 기타 시간 계산
        t_now += sorted_ready[p_selected].t_burst;
        sorted_ready[p_selected].t_complete = t_now;
        sorted_ready[p_selected].t_turnaround = sorted_ready[p_selected].t_complete - sorted_ready[p_selected].t_arrival;
        sorted_ready[p_selected].t_wait = sorted_ready[p_selected].t_turnaround - sorted_ready[p_selected].t_burst;
        sorted_ready[p_selected].t_remaining = 0;
        p_completed++;
    }

    // 결과를 기존 processes 배열에 복사 (기존 순서 유지)
    for (int i = 0; i < p_total; i++)
    {
        for (int j = 0; j < p_total; j++)
        {
            if (ready[i].pid == sorted_ready[j].pid)
            {
                ready[i].t_complete = sorted_ready[j].t_complete;
                ready[i].t_turnaround = sorted_ready[j].t_turnaround;
                ready[i].t_wait = sorted_ready[j].t_wait;
                ready[i].t_remaining = 0;
                break;
            }
        }
    }

    show_gantt(g_pid, g_time, g_index);
    show_results();
}

void priority_p()
{
    printf("\n=== Priority Preemptive Scheduling ===\n");

    // 버블 정렬할 필요없이 line 686을 통해 프로세스 선택하는 방식
    Process temp_ready[MAX];
    for (int i = 0; i < p_total; i++)
    {
        temp_ready[i] = ready[i];
        temp_ready[i].t_remaining = ready[i].t_burst;
    }

    int g_pid[100];
    int g_time[100];
    int g_index = 0;
    g_time[0] = 0;

    int p_completed = 0;
    int t_now = 0;
    int g_prev = -1; // 이전에 실행된 프로세스 (간트 차트용)

    // 시간 단위로 스케줄링 실행 (Preemptive기 때문)
    while (p_completed < p_total)
    {
        int p_selected = -1;
        int top_prty = 999; // 가장 낮은 우선순위로 초기화 (숫자가 클수록 우선순위 낮음)

        // 현재 시간에 도착한 프로세스들 중에서 가장 높은 우선순위 찾기
        for (int i = 0; i < p_total; i++)
        {
            // 아직 완료되지 않았고, 도착 시간이 현재 시간 이하인 프로세스
            if (temp_ready[i].t_remaining > 0 && temp_ready[i].t_arrival <= t_now)
            {
                // 우선순위가 더 높으면 (숫자가 더 작으면) 선택
                if (temp_ready[i].prty < top_prty)
                {
                    top_prty = temp_ready[i].prty;
                    p_selected = i;
                }
            }
        }

        // 실행할 프로세스가 없으면 (-1) IDLE
        if (p_selected == -1)
        {
            // 간트 차트에 IDLE 추가 (이전 프로세스와 다르면)
            if (g_prev != 0) // 0 = IDLE
            {
                g_pid[g_index] = 0;
                g_time[g_index + 1] = t_now + 1; // 1만큼 진행
                g_index++;
                g_prev = 0;
            }
            else
            {
                // 이미 IDLE이면 시간만 연장
                g_time[g_index] = t_now + 1; // 1만큼 진행
            }
            t_now++;
            continue;
        }

        // 프로세스 변경되었으면 간트 차트에 추가
        if (g_prev != temp_ready[p_selected].pid)
        {
            g_pid[g_index] = temp_ready[p_selected].pid;
            g_time[g_index + 1] = t_now + 1;
            g_index++;
            g_prev = temp_ready[p_selected].pid;
        }
        else
        {
            // 같은 프로세스가 계속 실행되면 시간만 연장
            g_time[g_index] = t_now + 1;
        }

        // 선택된 프로세스 1시간 실행
        temp_ready[p_selected].t_remaining--;
        t_now++;

        // 프로세스가 완료되었는지 확인 & 부가 정보 기입
        if (temp_ready[p_selected].t_remaining == 0)
        {
            temp_ready[p_selected].t_complete = t_now;
            temp_ready[p_selected].t_turnaround = temp_ready[p_selected].t_complete - temp_ready[p_selected].t_arrival;
            temp_ready[p_selected].t_wait = temp_ready[p_selected].t_turnaround - temp_ready[p_selected].t_burst;
            p_completed++;
        }
    }

    // 결과를 기존 processes 배열에 복사
    for (int i = 0; i < p_total; i++)
    {
        for (int j = 0; j < p_total; j++)
        {
            if (ready[i].pid == temp_ready[j].pid)
            {
                ready[i].t_complete = temp_ready[j].t_complete;
                ready[i].t_turnaround = temp_ready[j].t_turnaround;
                ready[i].t_wait = temp_ready[j].t_wait;
                ready[i].t_remaining = 0;
                break;
            }
        }
    }

    show_gantt(g_pid, g_time, g_index);
    show_results();
}

void rr()
{
    printf("\n=== Round Robin Scheduling ===\n");

    Process sorted_ready[MAX];
    for (int i = 0; i < p_total; i++)
    {
        sorted_ready[i] = ready[i];
        sorted_ready[i].t_remaining = ready[i].t_burst;
    }

    // 도착 시간으로 정렬 (버블 정렬)
    for (int i = 0; i < p_total - 1; i++)
    {
        for (int j = 0; j < p_total - i - 1; j++)
        {
            if (sorted_ready[j].t_arrival > sorted_ready[j + 1].t_arrival)
            {
                Process temp = sorted_ready[j];
                sorted_ready[j] = sorted_ready[j + 1];
                sorted_ready[j + 1] = temp;
            }
        }
    }

    int g_pid[100];
    int g_time[100];
    int g_index = 0;
    g_time[0] = 0; // 간트 차트는 항상 0에서 시작

    int p_completed = 0;
    int t_now = 0;
    int g_prev = -1;    // 이전에 실행된 프로세스 (간트 차트용)
    int checkpoint = 0; // 현재 확인할 프로세스 인덱스 (Round Robin용)

    // 모든 프로세스가 완료될 때까지 반복
    while (p_completed < p_total)
    {
        int p_selected = -1; // 실행할 프로세스 선택
        int p_checked = 0;   // 확인한 프로세스 개수

        // checkpoint부터 시작해서 Round Robin 방식으로 프로세스 찾기
        while (p_checked < p_total) // 전체 프로세스 배열 한바퀴 도는 loop
        {
            int i = (checkpoint + p_checked) % p_total; // 인덱스 순환을 위한 로직

            // 프로세스가 아직 완료되지 않았고, 현재 시간에 도착했는지 확인
            if (sorted_ready[i].t_remaining > 0 && sorted_ready[i].t_arrival <= t_now)
            {
                p_selected = i;                 // 실행할 프로세스 선택
                checkpoint = (i + 1) % p_total; // 다음에 확인할 인덱스 저장
                break;                          // 실행할 프로세스를 선정하였으면 while loop 탈출
            }
            p_checked++;
        }

        // 만약 실행할 프로세스가 없다면 IDLE 상태
        if (p_selected == -1)
        {
            // 가장 빨리 도착하는 미완료 프로세스의 도착 시간 찾기
            int t_next_arrival = 999;
            for (int i = 0; i < p_total; i++)
            {
                if (sorted_ready[i].t_remaining > 0 && sorted_ready[i].t_arrival < t_next_arrival)
                {
                    t_next_arrival = sorted_ready[i].t_arrival;
                }
            }

            // IDLE 시간을 간트 차트에 추가
            if (g_prev != 0) // 0 = IDLE
            {
                g_pid[g_index] = 0;
                g_time[g_index + 1] = t_next_arrival;
                g_index++;
                g_prev = 0;
            }
            else
            {
                // 이미 IDLE이면 시간만 연장
                g_time[g_index] = t_next_arrival;
            }

            t_now = t_next_arrival;
            continue; // 다시 프로세스 선택부터 시작
        }

        // 선택된 프로세스를 quantum만큼 (또는 남은 시간만큼) 실행 - 둘 중 min값만큼 burst
        int execution_time = (sorted_ready[p_selected].t_remaining < quantum) ? sorted_ready[p_selected].t_remaining : quantum;

        // RR에서는 항상 간트 차트에 추가 (같은 프로세스여도 quantum마다 분리)
        g_pid[g_index] = sorted_ready[p_selected].pid;
        g_time[g_index + 1] = t_now + execution_time;
        g_index++;
        g_prev = sorted_ready[p_selected].pid;

        // 프로세스 burst
        sorted_ready[p_selected].t_remaining -= execution_time;
        t_now += execution_time;

        // 프로세스가 완료되었는지 확인 & 완료 시 추가정보 기입
        if (sorted_ready[p_selected].t_remaining == 0)
        {
            sorted_ready[p_selected].t_complete = t_now;
            sorted_ready[p_selected].t_turnaround = sorted_ready[p_selected].t_complete - sorted_ready[p_selected].t_arrival;
            sorted_ready[p_selected].t_wait = sorted_ready[p_selected].t_turnaround - sorted_ready[p_selected].t_burst;
            p_completed++;
        }
    }

    // 결과를 기존 processes 배열에 복사 (기존 순서 유지)
    for (int i = 0; i < p_total; i++)
    {
        for (int j = 0; j < p_total; j++)
        {
            if (ready[i].pid == sorted_ready[j].pid)
            {
                ready[i].t_complete = sorted_ready[j].t_complete;
                ready[i].t_turnaround = sorted_ready[j].t_turnaround;
                ready[i].t_wait = sorted_ready[j].t_wait;
                ready[i].t_remaining = 0;
                break;
            }
        }
    }

    show_gantt(g_pid, g_time, g_index);
    show_results();
}

void compare_all()
{
    printf("\n=== Comparing All Algorithms ===\n");

    // 원본 프로세스 데이터 백업
    Process original_ready[MAX];
    for (int i = 0; i < p_total; i++)
    {
        original_ready[i] = ready[i];
    }

    // 결과 저장용 배열
    float avg_wait[6], avg_turnaround[6];
    char algo_list[6][30] = {"FCFS", "SJF Non-Preemptive", "SJF Preemptive", "Priority Non-Preemptive", "Priority Preemptive", "Round Robin"};

    // 1. FCFS 실행
    printf("\nCalculating FCFS...\n");
    fcfs();
    avg_wait[0] = 0;
    avg_turnaround[0] = 0;
    for (int i = 0; i < p_total; i++)
    {
        avg_wait[0] += ready[i].t_wait;
        avg_turnaround[0] += ready[i].t_turnaround;
    }
    avg_wait[0] /= p_total;
    avg_turnaround[0] /= p_total;

    // 원본 데이터 복원
    for (int i = 0; i < p_total; i++)
    {
        ready[i] = original_ready[i];
    }

    sleep(1);

    // 2. SJF Non-Preemptive 실행
    printf("\nCalculating SJF Non-Preemptive...\n");
    sjf_np();
    avg_wait[1] = 0;
    avg_turnaround[1] = 0;
    for (int i = 0; i < p_total; i++)
    {
        avg_wait[1] += ready[i].t_wait;
        avg_turnaround[1] += ready[i].t_turnaround;
    }
    avg_wait[1] /= p_total;
    avg_turnaround[1] /= p_total;

    // 원본 데이터 복원
    for (int i = 0; i < p_total; i++)
    {
        ready[i] = original_ready[i];
    }

    sleep(1);

    // 3. SJF Preemptive 실행
    printf("\nCalculating SJF Preemptive...\n");
    sjf_p();
    avg_wait[2] = 0;
    avg_turnaround[2] = 0;
    for (int i = 0; i < p_total; i++)
    {
        avg_wait[2] += ready[i].t_wait;
        avg_turnaround[2] += ready[i].t_turnaround;
    }
    avg_wait[2] /= p_total;
    avg_turnaround[2] /= p_total;

    // 원본 데이터 복원
    for (int i = 0; i < p_total; i++)
    {
        ready[i] = original_ready[i];
    }

    sleep(1);

    // 4. Priority Non-Preemptive 실행
    printf("\nCalculating Priority Non-Preemptive...\n");
    priority_np();
    avg_wait[3] = 0;
    avg_turnaround[3] = 0;
    for (int i = 0; i < p_total; i++)
    {
        avg_wait[3] += ready[i].t_wait;
        avg_turnaround[3] += ready[i].t_turnaround;
    }
    avg_wait[3] /= p_total;
    avg_turnaround[3] /= p_total;

    // 원본 데이터 복원
    for (int i = 0; i < p_total; i++)
    {
        ready[i] = original_ready[i];
    }

    sleep(1);

    // 5. Priority Preemptive 실행
    printf("\nCalculating Priority Preemptive...\n");
    priority_p();
    avg_wait[4] = 0;
    avg_turnaround[4] = 0;
    for (int i = 0; i < p_total; i++)
    {
        avg_wait[4] += ready[i].t_wait;
        avg_turnaround[4] += ready[i].t_turnaround;
    }
    avg_wait[4] /= p_total;
    avg_turnaround[4] /= p_total;

    // 원본 데이터 복원
    for (int i = 0; i < p_total; i++)
    {
        ready[i] = original_ready[i];
    }

    sleep(1);

    // 6. Round Robin 실행 (전역변수 사용)
    printf("\nCalculating Round Robin (quantum=%d)...\n", quantum);
    rr();
    avg_wait[5] = 0;
    avg_turnaround[5] = 0;
    for (int i = 0; i < p_total; i++)
    {
        avg_wait[5] += ready[i].t_wait;
        avg_turnaround[5] += ready[i].t_turnaround;
    }
    avg_wait[5] /= p_total;
    avg_turnaround[5] /= p_total;

    // 원본 데이터 복원
    for (int i = 0; i < p_total; i++)
    {
        ready[i] = original_ready[i];
    }

    sleep(1);

    // 결과 테이블 출력
    printf("\n=== Algorithm Comparison Results ===\n");
    printf("+---------------------------+------------------+---------------------+\n");
    printf("| Algorithm                 | Avg Waiting Time | Avg Turnaround Time |\n");
    printf("+---------------------------+------------------+---------------------+\n");

    for (int i = 0; i < 6; i++)
    {
        printf("| %-25s | %16.2f | %19.2f |\n", algo_list[i], avg_wait[i], avg_turnaround[i]);
    }
    printf("+---------------------------+------------------+---------------------+\n");

    // 최고 성능 찾기
    int best_wait = 0, best_turnaround = 0;
    for (int i = 1; i < 6; i++)
    {
        if (avg_wait[i] < avg_wait[best_wait])
            best_wait = i;
        if (avg_turnaround[i] < avg_turnaround[best_turnaround])
            best_turnaround = i;
    }

    printf("\nBest Average Waiting Time: %s (%.2f)", algo_list[best_wait], avg_wait[best_wait]);
    printf("\nBest Average Turnaround Time: %s (%.2f)\n", algo_list[best_turnaround], avg_turnaround[best_turnaround]);
}

void show_gantt(int g_pid[], int g_time[], int g_index)
{
    printf("\nGantt Chart:\n");

    // Top border
    for (int i = 0; i < g_index; i++)
    {
        printf("+--------");
    }
    printf("+\n");

    // Process IDs (0은 IDLE로 표시)
    for (int i = 0; i < g_index; i++)
    {
        if (g_pid[i] == 0)
        {
            printf("|  IDLE  ");
        }
        else
        {
            printf("|   P%d   ", g_pid[i]);
        }
    }
    printf("|\n");

    // Bottom border
    for (int i = 0; i < g_index; i++)
    {
        printf("+--------");
    }
    printf("+\n");

    // Time line
    for (int i = 0; i <= g_index; i++)
    {
        printf("%-9d", g_time[i]);
    }
    printf("\n");
}

void show_results()
{
    float avg_wait = 0, avg_turnaround = 0;

    printf("\n+-----+----------+------------+--------------+\n");
    printf("| PID | Waiting  | Turnaround | Completion   |\n");
    printf("|     | Time     | Time       | Time         |\n");
    printf("+-----+----------+------------+--------------+\n");

    for (int i = 0; i < p_total; i++)
    {
        printf("| %3d | %8d | %10d | %12d |\n", ready[i].pid, ready[i].t_wait, ready[i].t_turnaround, ready[i].t_complete);
        avg_wait += ready[i].t_wait;
        avg_turnaround += ready[i].t_turnaround;
    }
    printf("+-----+----------+------------+--------------+\n");

    avg_wait /= p_total;
    avg_turnaround /= p_total;

    printf("Average Waiting Time: %.2f\n", avg_wait);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround);
}