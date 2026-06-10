# HW6: 行程與檔案相關程式

## 目錄

1. [核心概念](#核心概念)
   - [行程 (Process) 與 fork()](#行程-process-與-fork)
   - [execvp() 系列函式](#execvp-系列函式)
   - [檔案描述子 (File Descriptor)](#檔案描述子-file-descriptor)
   - [open / close / read / write](#open--close--read--write)
   - [dup2() 與 I/O 重導向](#dup2-與-io-重導向)
   - [管道 (Pipe)](#管道-pipe)
2. [程式一：fork + execvp (fork_exec.c)](#程式一fork--execvp-fork_execc)
3. [程式二：低階檔案複製 (file_copy.c)](#程式二低階檔案複製-file_copyc)
4. [程式三：I/O 重導向 (redirect.c)](#程式三io-重導向-redirectc)
5. [程式四：管道通訊 (pipe_demo.c)](#程式四管道通訊-pipe_democ)
6. [編譯與執行](#編譯與執行)

---

## 核心概念

### 行程 (Process) 與 fork()

**行程**是執行中的程式實體，擁有獨立的記憶體空間（程式碼、資料、堆疊、堆積）。
每個行程由一個獨一無二的 **PID (Process ID)** 識別。

**fork()** 是 Linux 系統中建立新行程的唯一方法：
- 呼叫 `fork()` 的行程稱為**父行程 (parent process)**
- `fork()` 回傳兩次：
  - 在父行程中回傳**子行程的 PID**（> 0）
  - 在子行程中回傳 **0**
  - 失敗時回傳 **-1**
- 子行程會**複製**父行程的記憶體空間（copy-on-write 機制）
- 父子行程共享開啟的檔案描述子

```c
pid_t pid = fork();
if (pid == 0) {
    // 子行程
} else if (pid > 0) {
    // 父行程
} else {
    // fork 失敗
}
```

### execvp() 系列函式

**exec 系列函式**用於載入並執行新的程式，**取代**當前行程的記憶體映像。
呼叫成功後不會回傳，呼叫失敗則回傳 -1。

exec 函式家族的命名規則：

| 後綴 | 意義 | 範例 |
|------|------|------|
| `l` (list) | 參數以 list 傳遞 | `execl("/bin/ls", "ls", "-l", NULL)` |
| `v` (vector) | 參數以 array 傳遞 | `execv("/bin/ls", args)` |
| `p` (path) | 使用 $PATH 搜尋 | `execvp("ls", args)` |
| `e` (environment) | 指定環境變數 | `execle("/bin/ls", "ls", NULL, envp)` |

**常見組合：fork + exec**
```c
pid_t pid = fork();
if (pid == 0) {
    execvp("ls", args);   // 子行程被替換為 ls
    perror("execvp");      // 只有失敗才會執行到這裡
    exit(1);
}
// 父行程繼續執行...
waitpid(pid, NULL, 0);
```

### 檔案描述子 (File Descriptor)

檔案描述子是作業系統用於追蹤開啟檔案的**非負整數**。每個行程都有自己的
檔案描述子表 (file descriptor table)。

**標準檔案描述子：**

| 名稱 | 數值 | 巨集 | 用途 |
|------|------|------|------|
| 標準輸入 (stdin) | `0` | `STDIN_FILENO` | 預設從鍵盤讀取 |
| 標準輸出 (stdout) | `1` | `STDOUT_FILENO` | 預設輸出到終端機 |
| 標準錯誤 (stderr) | `2` | `STDERR_FILENO` | 預設輸出錯誤到終端機 |

### open / close / read / write

這些是 POSIX 低階 I/O 系統呼叫，直接操作檔案描述子：

```c
// 開啟檔案
int fd = open("file.txt", O_RDONLY);            // 唯讀
int fd = open("out.txt", O_WRONLY | O_CREAT, 0644); // 唯寫，若不存在則建立

// 讀取
char buf[128];
ssize_t n = read(fd, buf, sizeof(buf));  // n = 實際讀取位元組數，0=EOF，-1=錯誤

// 寫入
ssize_t n = write(fd, buf, len);         // n = 實際寫入位元組數

// 關閉
close(fd);
```

**open() 常用旗標：**
- `O_RDONLY`：唯讀
- `O_WRONLY`：唯寫
- `O_RDWR`：讀寫
- `O_CREAT`：若檔案不存在則建立
- `O_TRUNC`：開啟時清空檔案內容
- `O_APPEND`：寫入時附加到檔尾

### dup2() 與 I/O 重導向

**dup2(oldfd, newfd)** 將 `newfd` 指向 `oldfd` 指向的檔案，實現 I/O 重導向。

典型用法（fork + exec 前重導向）：
```c
int fd = open("output.txt", O_WRONLY | O_CREAT, 0644);
dup2(fd, STDOUT_FILENO);  // stdout (1) 現在指向 output.txt
close(fd);                 // 關閉舊的描述子
execvp("ls", args);        // ls 的輸出會寫入 output.txt
```

Shell 中 `>` `>>` `<` `|` 等重導向符號底層就是透過 dup2() 實作的。

### 管道 (Pipe)

**pipe()** 建立一對檔案描述子，用於**父子行程間的通訊**：
- `pipe_fd[0]`：讀取端 (read end)
- `pipe_fd[1]`：寫入端 (write end)
- 資料單向流動：從寫入端流向讀取端

典型用法：父行程寫入管道、子行程從管道讀取（或相反），常搭配 dup2 實現類似
Shell pipe `|` 的功能。

---

## 程式一：fork + execvp (fork_exec.c)

### 功能說明

展示 `fork()` 建立子行程，子行程使用 `execvp()` 執行 `ls -l`，
父行程使用 `waitpid()` 等待子行程結束並取得結束狀態。

### 關鍵程式碼

```c
pid = fork();
if (pid == 0) {
    char *args[] = {"ls", "-l", NULL};
    execvp("ls", args);    // 子行程映像被 ls 取代
    perror("execvp failed");
    exit(1);
} else {
    int status;
    waitpid(pid, &status, 0);  // 父行程等待子行程結束
    printf("Child exited with status %d\n", WEXITSTATUS(status));
}
```

### 輸出說明

- 父行程先顯示自己的 PID
- fork 後顯示子行程 PID
- 子行程執行 `ls -l`，輸出目錄內容
- 父行程等待子行程結束後顯示退出狀態碼

---

## 程式二：低階檔案複製 (file_copy.c)

### 功能說明

使用 `open()` / `read()` / `write()` / `close()` 等低階系統呼叫實作檔案複製。
不使用標準 C 函式庫的 `fopen()` / `fread()` / `fwrite()`。

### 用法

```bash
./file_copy <source_file> <destination_file>
```

### 關鍵程式碼

```c
int src_fd = open(argv[1], O_RDONLY);
int dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);

char buf[4096];
ssize_t n;
while ((n = read(src_fd, buf, sizeof(buf))) > 0) {
    write(dst_fd, buf, n);
}

close(src_fd);
close(dst_fd);
```

### 輸出說明

- 顯示來源與目標檔案的檔案描述子編號
- 顯示複製的總位元組數
- 確認兩個檔案描述子都已關閉

---

## 程式三：I/O 重導向 (redirect.c)

### 功能說明

展示三個 dup2() 重導向情境：

1. **stdout 重導向**：將 `ls -la` 的輸出寫入 `redirect_out.txt` 檔案
2. **stdin 重導向**：透過 pipe 將資料傳入 `wc -l` 指令
3. **stderr 重導向**：將錯誤訊息寫入 `redirect_err.txt`，stdout 仍輸出到終端機

### 關鍵程式碼

```c
// 重導向 stdout (fd=1) 到檔案
int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
dup2(fd, STDOUT_FILENO);   // fd 1 現在指向 output.txt
close(fd);

// 重導向 stderr (fd=2) 到檔案
dup2(fd, STDERR_FILENO);
close(fd);
fprintf(stderr, "This goes to file!\n");  // 寫入檔案而非終端機
```

### 輸出說明

- `redirect_out.txt`：包含 `ls -la` 的輸出
- `redirect_err.txt`：包含原本要輸出到終端機的錯誤訊息
- 終端機顯示示範過程的說明文字

---

## 程式四：管道通訊 (pipe_demo.c)

### 功能說明

展示父子行程透過 pipe + dup2 進行通訊：
- 父行程建立管道後 fork
- 子行程將 stdin 重導向至管道讀取端，執行 `cat` 讀取並輸出
- 父行程透過管道寫入端傳送訊息給子行程

### 關鍵程式碼

```c
int pipe_fd[2];
pipe(pipe_fd);          // pipe_fd[0]=讀取端, pipe_fd[1]=寫入端

pid = fork();
if (pid == 0) {
    close(pipe_fd[1]);              // 子行程關閉寫入端
    dup2(pipe_fd[0], STDIN_FILENO); // stdin 指向管道讀取端
    close(pipe_fd[0]);
    execvp("cat", args);            // cat 從管道讀取資料
} else {
    close(pipe_fd[0]);              // 父行程關閉讀取端
    write(pipe_fd[1], "Hello!", 6); // 寫入管道
    close(pipe_fd[1]);              // 關閉寫入端，發送 EOF
    waitpid(pid, NULL, 0);
}
```

### 輸出說明

- 顯示管道的兩個檔案描述子編號
- 子行程透過 cat 輸出從父行程接收到的訊息
- 父行程確認子行程結束

---

## 編譯與執行

### 環境需求

- Linux 作業系統
- GCC 編譯器

### 一次編譯所有程式

```bash
gcc -o fork_exec fork_exec.c
gcc -o file_copy file_copy.c
gcc -o redirect redirect.c
gcc -o pipe_demo pipe_demo.c
```

### 執行範例

```bash
# fork + exec 示範
./fork_exec

# 低階檔案複製（先建立測試檔案）
echo "Hello, World!" > test.txt
./file_copy test.txt test_copy.txt
cat test_copy.txt

# I/O 重導向示範
./redirect
cat redirect_out.txt
cat redirect_err.txt

# 管道通訊示範
./pipe_demo
```

### 清理產出檔案

```bash
rm -f test.txt test_copy.txt redirect_out.txt redirect_err.txt
```

---

## 檔案描述子對照表

| fd | 名稱 | 預設目標 | dup2 可重導向至 |
|----|------|----------|-----------------|
| 0 | stdin | 鍵盤 | 檔案讀取端 / pipe |
| 1 | stdout | 終端機 | 檔案寫入端 |
| 2 | stderr | 終端機 | 檔案寫入端 |

---

## 參考資料

- *Advanced Programming in the UNIX Environment* (W. Richard Stevens)
- *The Linux Programming Interface* (Michael Kerrisk)
- Linux man pages: `man 2 fork`, `man 3 exec`, `man 2 open`, `man 2 pipe`, `man 2 dup2`
