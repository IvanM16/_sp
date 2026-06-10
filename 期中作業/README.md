# 期中作業：Linux 系統程式設計 — Mini Shell (myshell)

## 目錄

1. [專案簡介](#專案簡介)
2. [系統程式核心概念](#系統程式核心概念)
3. [功能說明](#功能說明)
4. [程式架構](#程式架構)
5. [原始碼逐段說明](#原始碼逐段說明)
6. [編譯與執行](#編譯與執行)
7. [測試範例](#測試範例)
8. [檔案結構](#檔案結構)

---

## 專案簡介

**myshell** 是一個以 C 語言實作的簡易命令列解譯器 (shell)，涵蓋本課程上半學期所學的
核心系統程式概念：**行程管理**、**檔案 I/O**、**檔案描述子重導向**、**管道通訊**
以及**訊號處理**。

本專案透過實作一個可互動使用的 shell，具體展示如何將 `fork`、`execvp`、`open`、
`close`、`read`、`write`、`dup2`、`pipe` 及 `signal` 等系統呼叫整合運用，
建構出一個實際可用的系統程式。

---

## 系統程式核心概念

本專案涵蓋的系統程式概念與對應的系統呼叫如下：

### 1. 行程管理 (Process Management)

| 系統呼叫 | 用途 |
|----------|------|
| `fork()` | 複製目前行程，建立子行程。父行程回傳子行程 PID，子行程回傳 0 |
| `execvp()` | 載入並執行外部程式，取代子行程的記憶體映像 |
| `waitpid()` | 父行程等待子行程結束，回收其資源（避免 zombie process） |
| `exit()` | 結束目前行程並回傳狀態碼 |

**執行流程：**
```
使用者輸入命令 → 剖析 → fork() → 子行程 execvp() → 父行程 waitpid()
```

### 2. 檔案描述子與 I/O 重導向 (File Descriptors & Redirection)

| 系統呼叫 | 用途 |
|----------|------|
| `open()` | 開啟檔案，回傳檔案描述子 (fd)。支援 O_RDONLY / O_WRONLY / O_CREAT / O_TRUNC / O_APPEND |
| `close()` | 關閉檔案描述子，釋放系統資源 |
| `dup2(oldfd, newfd)` | 將 newfd 指向 oldfd 指向的檔案，實現 I/O 重導向 |

**標準檔案描述子：**

| fd | 巨集 | 用途 | 重導向符號 |
|----|------|------|-----------|
| 0 | `STDIN_FILENO` | 標準輸入 | `<` |
| 1 | `STDOUT_FILENO` | 標準輸出 | `>` `>>` |
| 2 | `STDERR_FILENO` | 標準錯誤 | `2>` |

**重導向實作原理：**
```c
// 輸出重導向 (ls > output.txt)
int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
dup2(fd, STDOUT_FILENO);  // fd 1 指向 output.txt
close(fd);
// 之後寫入 stdout 的資料都會進到 output.txt
```

### 3. 管道 (Pipe) — 行程間通訊 (IPC)

| 系統呼叫 | 用途 |
|----------|------|
| `pipe(int fd[2])` | 建立單向通訊管道。fd[0] 為讀取端，fd[1] 為寫入端 |

**管道的運作原理 (以 `ls | wc -l` 為例)：**
```
父行程
  ├─ pipe() 建立管道
  ├─ fork() → 子行程 A: dup2(pipe[1], stdout) → execvp("ls")    (寫入端)
  ├─ fork() → 子行程 B: dup2(pipe[0], stdin)  → execvp("wc -l") (讀取端)
  ├─ close(pipe[0]), close(pipe[1])
  └─ waitpid() × 2
```

**關鍵點：** 父行程必須關閉管道兩端，否則讀取端永遠不會收到 EOF。

### 4. 訊號處理 (Signal Handling)

| 系統呼叫 | 用途 |
|----------|------|
| `signal(SIGINT, handler)` | 註冊 SIGINT (Ctrl+C) 的處理函式 |
| `signal(SIGCHLD, handler)` | 註冊子行程狀態改變的處理函式 |
| `waitpid(-1, NULL, WNOHANG)` | 非阻塞回收已結束的子行程 (zombie reaping) |

**設計考量：**
- **SIGINT (Ctrl+C)：** 在 shell 主程式中忽略，讓前景子行程以預設行為終止。避免 shell 自己被中斷。
- **SIGCHLD：** 用於背景行程的 zombie reaping。當背景行程結束時，核心發送 SIGCHLD，
  處理函式以 `WNOHANG` 回收所有已結束的子行程。

### 5. 背景執行 (Background Execution)

當命令以 `&` 結尾時，shell 不回等待子行程結束，立即顯示提示字元接受下一個命令。
子行程結束後由 SIGCHLD 處理函式回收。

```c
if (background) {
    printf("[%d] %s\n", pid, args[0]);  // 顯示背景行程 PID
    // 不呼叫 waitpid，直接回傳
}
```

---

## 功能說明

### 內建命令 (Built-in Commands)

| 命令 | 說明 | 範例 |
|------|------|------|
| `cd [dir]` | 切換工作目錄（無參數時切換到 $HOME） | `cd /tmp` |
| `pwd` | 顯示目前工作目錄 | `pwd` |
| `exit` | 離開 shell | `exit` |
| `help` | 顯示說明訊息 | `help` |

### I/O 重導向

| 符號 | 說明 | 範例 |
|------|------|------|
| `< file` | 標準輸入從檔案讀取 | `wc -l < /etc/passwd` |
| `> file` | 標準輸出寫入檔案（覆蓋） | `ls -la > listing.txt` |
| `>> file` | 標準輸出附加到檔案 | `echo "log" >> app.log` |

### 管道 (Pipe)

| 符號 | 說明 | 範例 |
|------|------|------|
| `\|` | 將左側命令的 stdout 連接至右側命令的 stdin | `ls \| grep .c \| wc -l` |

### 背景執行

| 符號 | 說明 | 範例 |
|------|------|------|
| `&` | 在背景執行命令，不等待結束 | `sleep 10 &` |

---

## 程式架構

```
myshell.c
│
├── main()                    主迴圈：顯示提示字元、讀取輸入、呼叫執行
│
├── parse_line()              剖析輸入行：分詞 → args[]，偵測 & 符號
│
├── execute_command()         命令分派器
│   ├── run_builtin()         處理內建命令 (cd, pwd, exit, help)
│   ├── execute_pipe()        處理管線命令 (|)
│   └── execute_single()      處理單一命令（含重導向）
│
├── split_pipe()              將管線命令拆分為左右兩個 args[]
│
├── has_pipe()                檢查命令是否包含管線符號
│
├── setup_redirections()      設定檔案重導向 (<, >, >>)
│   └── open() + dup2() + close()
│
├── sigchld_handler()         SIGCHLD 處理常式 — 回收 zombie process
│
└── sigint_handler()          SIGINT 處理常式 — 防止 shell 被 Ctrl+C 中斷
```

---

## 原始碼逐段說明

### 1. 常數定義與全域變數

```c
#define MAX_INPUT  1024    // 輸入命令最大長度
#define MAX_ARGS   128     // 命令參數最大數量
#define MAX_PIPE   16      // (保留) 管線節點數上限
#define PROMPT     "myshell> "

volatile sig_atomic_t child_exited = 0;  // 訊號安全旗標
```

### 2. 訊號處理函式

```c
void sigchld_handler(int sig) {
    child_exited = 1;
    while (waitpid(-1, NULL, WNOHANG) > 0);  // 回收所有 zombie
}

void sigint_handler(int sig) {
    write(STDOUT_FILENO, "\n", 1);  // 只換行，不中斷 shell
}
```

**設計要點：** SIGINT 處理函式中使用 `write()` 而非 `printf()`，因為 `write()` 是
async-signal-safe 函式，可在訊號處理常式中安全呼叫。

### 3. 輸入剖析 (parse_line)

```c
int parse_line(char *line, char *args[], int *background) {
    // 使用 strtok() 將輸入行切割為 tokens
    // 若遇到 "&" token，標記為背景執行
    // args[] 最後設為 NULL (execvp 要求)
}
```

**設計要點：** `strtok()` 會修改原始字串，因此 `line` 傳入後其內容會被 `\0` 切割。
`args` 陣列的最後一個元素必須是 `NULL`，這是 execvp 的規定。

### 4. 內建命令處理 (run_builtin)

```c
int run_builtin(char *args[]) {
    // cd:  使用 chdir() 系統呼叫切換目錄
    //      若無參數則切換至 $HOME
    // pwd: 使用 getcwd() 取得目前目錄
    // exit: 結束 shell 行程
    // help: 顯示說明
}
```

**設計要點：** 內建命令必須在 shell 行程內執行（不能 fork），例如 `cd` 若在子行程中執行
只會改變子行程的目錄，對 shell 本身無效。

### 5. 重導向設定 (setup_redirections)

```c
int setup_redirections(char *args[]) {
    // 掃描 args，找到 <, >, >>
    // 使用 open() 開啟目標檔案
    // 使用 dup2() 重導向 stdin/stdout
    // 使用 close() 關閉原始 fd
    // 將重導向符號與檔名從 args 中移除
}
```

**設計要點：** 重導向必須在 execvp 之前設定，因為 execvp 會保留檔案描述子表。
重導向符號和檔名從 args 中移除後，剩下的 args 就是實際傳給 execvp 的命令參數。

### 6. 單一命令執行 (execute_single)

```c
int execute_single(char *args[], int background) {
    pid_t pid = fork();
    if (pid == 0) {
        // 子行程：
        signal(SIGINT, SIG_DFL);  // 恢復預設行為，讓 Ctrl+C 可中斷
        setup_redirections(args); // 設定重導向
        execvp(args[0], args);   // 執行命令
        exit(127);               // execvp 失敗才到這裡
    }
    // 父行程：
    if (!background)
        waitpid(pid, &status, 0); // 等待前景行程結束
    else
        printf("[%d] %s\n", pid, args[0]); // 顯示背景行程 PID
}
```

### 7. 管線執行 (execute_pipe)

```c
int execute_pipe(char *args[]) {
    split_pipe(args, left_args, right_args);  // 拆分命令
    pipe(pipe_fd);                             // 建立管道

    // Fork 左側命令：stdout → pipe write end
    left_pid = fork();
    if (left_pid == 0) {
        close(pipe_fd[0]);                    // 關閉讀取端
        dup2(pipe_fd[1], STDOUT_FILENO);      // stdout 重導向至管道
        close(pipe_fd[1]);
        execvp(left_args[0], left_args);
    }

    // Fork 右側命令：stdin ← pipe read end
    right_pid = fork();
    if (right_pid == 0) {
        close(pipe_fd[1]);                    // 關閉寫入端
        dup2(pipe_fd[0], STDIN_FILENO);       // stdin 重導向自管道
        close(pipe_fd[0]);
        execvp(right_args[0], right_args);
    }

    // 父行程：關閉管道兩端，等待兩個子行程
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    waitpid(left_pid, NULL, 0);
    waitpid(right_pid, NULL, 0);
}
```

**為什麼父行程必須關閉 pipe fds？**
- 若父行程不關閉寫入端，右側命令的 stdin 永遠讀不到 EOF（因為還有行程持有寫入端）
- 同理，必須關閉讀取端以釋放資源

### 8. 主迴圈 (main)

```c
int main() {
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchld_handler);
    // 顯示歡迎訊息
    while (1) {
        printf(PROMPT);
        fgets(input, MAX_INPUT, stdin);
        parse_line(input, args, &background);
        execute_command(args, background);
    }
}
```

---

## 編譯與執行

### 環境需求

- Linux 作業系統
- GCC 編譯器
- GNU Make (選用)

### 編譯

```bash
cd 期中作業

# 使用 make
make

# 或手動編譯
gcc -Wall -Wextra -g -o myshell myshell.c
```

### 執行

```bash
# 互動模式
./myshell

# 執行測試
make test
```

### 清理

```bash
make clean
```

---

## 測試範例

```bash
$ ./myshell
╔══════════════════════════════════════╗
║          Welcome to myshell          ║
║  Built-in: cd pwd exit help         ║
║  Redirect: <  >  >>                  ║
║  Pipe:    |                          ║
║  BG:      &                          ║
╚══════════════════════════════════════╝

myshell> help
Built-in commands: cd, pwd, exit, help
Features: I/O redirection (<, >, >>), pipe (|), background (&)

myshell> pwd
/mnt/c/Users/ivanm/OneDrive/Documents/_sp/期中作業

myshell> ls -la
total 24
drwxrwxrwx 1 root root  4096 Jun 10 18:32 .
drwxrwxrwx 1 root root  4096 Jun 10 18:26 ..
-rwxrwxrwx 1 root root   934 Jun 10 18:32 Makefile
-rwxrwxrwx 1 root root   482 Jun 10 18:26 README.md
-rwxrwxrwx 1 root root  6545 Jun 10 18:32 myshell.c

myshell> echo "Hello from myshell" > /tmp/test.txt
myshell> cat /tmp/test.txt
Hello from myshell

myshell> cat < /tmp/test.txt
Hello from myshell

myshell> ls -la | grep .c
-rwxrwxrwx 1 root root 6545 Jun 10 18:32 myshell.c

myshell> sleep 5 &
[42] sleep
myshell> (可立即輸入下一個命令)

myshell> exit
myshell: goodbye
```

---

## 檔案結構

```
期中作業/
├── myshell.c      主程式（約 250 行）
├── Makefile       編譯腳本
└── README.md      本文件
```

---

## 使用到的系統呼叫總覽

| 系統呼叫 | 行號 (約) | 用途分類 |
|----------|-----------|----------|
| `fork()` | 139, 161, 173 | 行程管理 |
| `execvp()` | 143, 167, 178 | 程式載入與執行 |
| `waitpid()` | 147, 183 | 行程同步 |
| `exit()` | 145 | 行程結束 |
| `open()` | 84, 93, 103 | 檔案 I/O |
| `close()` | 87, 170, 181 | 檔案 I/O |
| `dup2()` | 86, 165, 175 | I/O 重導向 |
| `pipe()` | 158 | 行程間通訊 |
| `write()` | 22 | 訊號安全輸出 |
| `signal()` | 200 | 訊號處理 |
| `chdir()` | 120 | 目錄操作 |
| `getcwd()` | 126 | 目錄操作 |

---

## 設計決策與取捨

1. **為何不支援多層 pipe (`ls | grep c | wc -l`)？**  
   目前僅支援單層 pipe 以維持程式碼清晰。多層 pipe 可透過遞迴或迴圈處理，是合理的延伸方向。

2. **為何使用 `strtok()` 而非自行實作 parser？**  
   `strtok()` 是標準 C 函式庫提供的分詞工具，簡單可靠。缺點是會修改原始字串且不支援引號處理。

3. **背景行程的 zombie 如何處理？**  
   透過 SIGCHLD 訊號處理函式，以 `waitpid(-1, NULL, WNOHANG)` 非阻塞回收所有已結束的子行程。

4. **為何訊號處理函式使用 `write()` 而非 `printf()`？**  
   `write()` 是 async-signal-safe 函式，可在訊號處理常式中安全呼叫。`printf()` 等標準 I/O
   函式使用了全域緩衝區與鎖，在訊號處理常式中呼叫可能導致死結或未定義行為。

---

## 參考資料

- *Advanced Programming in the UNIX Environment* (W. Richard Stevens)
- *The Linux Programming Interface* (Michael Kerrisk)
- Linux man pages: `man 2 fork`, `man 3 exec`, `man 2 pipe`, `man 2 dup2`, `man 2 signal`, `man 2 open`
