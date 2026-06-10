# 期末總整理：系統程式設計 — 全學期作業回顧

> 本文件彙整 HW1 至期中作業的所有內容，包含各作業摘要、核心概念與檔案連結。

---

## 目錄

| 作業 | 主題 | 關鍵技術 |
|------|------|----------|
| [HW1](#hw1--custom-c-compiler--virtual-machine) | 自製 C 編譯器與虛擬機 | Lexer, Parser, IR, VM, Backpatching |
| [HW2](#hw2--modular-c-compiler--virtual-machine) | 模組化編譯器與虛擬機 | 多檔案架構、分離式編譯 |
| [HW3](#hw3--enhanced-modular-compiler--virtual-machine) | 強化版編譯器（EBNF + IR 參考手冊） | 運算子優先權、完整 IR Opcode |
| [HW4](#hw4--sp-book-system-programming-book) | SP-Book：系統程式電子書 | 互動式閱讀器、Demo、文字/HTML 匯出 |
| [HW5](#hw5--linux-多執行緒程式設計) | Linux 多執行緒程式設計 | Thread, Race Condition, Mutex, Deadlock, Condition Variable |
| [HW6](#hw6--行程與檔案相關程式) | 行程與檔案 I/O | fork, execvp, open/read/write/close, dup2, pipe |
| [期中作業](#期中作業--mini-shell-myshell) | Mini Shell (myshell) | 整合全部系統呼叫：行程、檔案、重導向、管道、訊號 |

---

## 技術學習路徑

```
HW1 ──→ HW2 ──→ HW3 ──→ HW4 ──→ HW5 ──→ HW6 ──→ 期中作業
 │        │        │        │        │        │         │
 編譯器   模組化   強化    電子書   多執行緒  行程/檔案  整合專案
 雛形     架構     EBNF    互動式   同步      I/O       Mini Shell
```

---

## HW1 — Custom C Compiler & Virtual Machine

> **檔案：** [`HW1/`](../HW1/) — 完整文件 [`README.MD`](../HW1/README.MD)

實作一個**單一檔案**的 C 編譯器與堆疊式虛擬機。

### 核心模組

| 模組 | 功能 |
|------|------|
| **Lexer** | 原始碼 → Token 序列、跳過註解 (`//` `/* */`) |
| **Parser** | 遞迴下降剖析、EBNF 文法規則 |
| **IR Generator** | 複雜運算式 → 三地址碼 / 四元式 |
| **VM** | 堆疊式虛擬機，逐行解釋執行 IR |

### 關鍵技術
- **Backpatching（回溯修補）：** 處理 `if` / `while` 的前向跳轉，先生成佔位地址 `?`，剖析完區塊後再修補正確位址
- **Call Stack / Stack Frame：** 支援函式呼叫、參數傳遞、遞迴，使用獨立的執行框架隔離變數

### 支援語法
```
func multiply(a, b) { return a * b; }
x = 5;
while (x > 0) { x = x - 1; }
```

---

## HW2 — Modular C Compiler & Virtual Machine

> **檔案：** [`HW2/`](../HW2/) — 完整文件 [`README.MD`](../HW2/README.MD)

將 HW1 重構為**多檔案模組化架構**。

### 檔案架構

| 檔案 | 職責 |
|------|------|
| `tiny_compiler.h` | 中央標頭檔：結構定義、函式原型 |
| `lexer.c` | 詞法分析器 |
| `parser.c` | 遞迴下降剖析器 |
| `codegen.c` | IR 程式碼產生器 |
| `vm.c` | 虛擬機 |
| `main.c` | 進入點、檔案 I/O、管線協調 |

### 改進重點
- 關注點分離 (Separation of Concerns)
- 模組化架構易於閱讀、除錯與擴充
- 編譯指令：`gcc main.c lexer.c parser.c codegen.c vm.c -o my_compiler`

---

## HW3 — Enhanced Modular Compiler & Virtual Machine

> **檔案：** [`HW3/`](../HW3/) — 完整文件 [`README.md`](../HW3/README.md)

基於 HW2 架構進行強化，加入**完整 EBNF 文法文件**與**IR Opcode 參考手冊**。

### 新增內容

- **完整 EBNF 文法定義**（程式、函式、敘述、運算式層級）
- **運算子優先權表**（`== < >` → `+ -` → `* /`）
- **完整 IR Opcode 參考手冊**（資料搬移、算術、比較、控制流程、函式）

### 編譯管線

```
原始碼 → Lexer → Parser → Codegen (IR) → VM 執行 → 輸出全域變數
```

### IR Opcode 分類

| 類別 | Opcode |
|------|--------|
| 資料搬移 | `LOAD_IMM`, `STORE` |
| 算術 | `ADD`, `SUB`, `MUL`, `DIV` |
| 比較 | `IS_EQ`, `IS_LT`, `IS_GT` |
| 控制流程 | `JMP`, `JMP_FALSE` |
| 函式 | `DEF_FUNC`, `END_FUNC`, `PUSH_ARG`, `CALL`, `BIND_PARAM`, `RETURN` |

---

## HW4 — SP-Book: System Programming Book

> **檔案：** [`HW4/`](../HW4/) — 完整文件 [`README.MD`](../HW4/README.MD)

一本**「本身即為程式」**的系統程式電子書。共 10 章、40 小節，內容以 C 資料結構儲存於程式中，
提供互動式 CLI 閱讀器及文字/HTML 匯出。

### 章節

| 章 | 標題 |
|----|------|
| 1 | What Is System Programming? |
| 2 | The C Language for Systems |
| 3 | Lexical Analysis |
| 4 | Recursive-Descent Parsing |
| 5 | Intermediate Representation (IR) |
| 6 | Backpatching and Forward References |
| 7 | Virtual Machine Architecture |
| 8 | Linkers and Loaders |
| 9 | Operating System Essentials |
| 10 | Bringing It All Together |

### 互動指令

| 指令 | 功能 |
|------|------|
| `1`-`10` | 閱讀指定章節 |
| `3.2` | 閱讀指定小節 |
| `t` | 顯示目錄 |
| `e` / `h` | 匯出文字 / HTML |
| `q` | 離開 |

### Demo 程式

| 檔案 | 示範概念 |
|------|----------|
| `demo_memory.c` | C 程式記憶體佈局 |
| `demo_stack.c` | 呼叫堆疊框架 |
| `demo_vm.c` | 迷你堆疊式 VM |
| `demo_lexer.c` | 獨立詞法分析器 |

---

## HW5 — Linux 多執行緒程式設計

> **檔案：** [`HW5/`](../HW5/) — 完整文件 [`README.md`](../HW5/README.md)

### 基礎概念

| 概念 | 說明 |
|------|------|
| **Thread** | OS 排程最小單位，同行程內共享記憶體 |
| **Race Condition** | 多執行緒同時讀寫共享資源，結果取決於執行順序 |
| **Mutex** | 互斥鎖，確保臨界區間一次只有一個執行緒進入 |
| **Deadlock** | 多執行緒互相等待對方釋放資源，永久阻塞 |
| **Condition Variable** | 條件變數，用於執行緒間事件通知 |

### 程式實作

| 程式 | 說明 | 關鍵技術 |
|------|------|----------|
| [`bank.c`](../HW5/bank.c) | 模擬 4 執行緒各 25,000 次存提款 | mutex 保護、race condition 展示 |
| [`producer_consumer.c`](../HW5/producer_consumer.c) | 環形緩衝區、2 生產者 2 消費者 | mutex + condition variable |
| [`dining_philosophers.c`](../HW5/dining_philosophers.c) | 5 位哲學家、死結預防 | mutex 順序打破循環等待 |

### 死結預防策略（哲學家用餐）
- 最後一位哲學家**先拿右筷再拿左筷**（打破循環等待）
- 其他哲學家先左後右

---

## HW6 — 行程與檔案相關程式

> **檔案：** [`HW6/`](../HW6/) — 完整文件 [`README.md`](../HW6/README.md)

### 核心概念

| 概念 | 系統呼叫 | 說明 |
|------|----------|------|
| **行程管理** | `fork()`, `execvp()`, `waitpid()` | 建立子行程、載入程式、等待結束 |
| **檔案 I/O** | `open()`, `close()`, `read()`, `write()` | 低階檔案操作，直接操作 fd |
| **檔案描述子** | fd 0/1/2 | stdin/stdout/stderr |
| **I/O 重導向** | `dup2(oldfd, newfd)` | 將 fd 重新指向，實現 `<` `>` `>>` |
| **管道** | `pipe(fd[2])` | 父子行程單向通訊 |

### 程式實作

| 程式 | 說明 |
|------|------|
| [`fork_exec.c`](../HW6/fork_exec.c) | fork 子行程 → execvp 執行 `ls -l` → 父行程 waitpid |
| [`file_copy.c`](../HW6/file_copy.c) | 低階 open/read/write/close 檔案複製 |
| [`redirect.c`](../HW6/redirect.c) | dup2 示範：stdout→檔案、stdin←pipe、stderr→檔案 |
| [`pipe_demo.c`](../HW6/pipe_demo.c) | 父寫入 pipe → 子 cat 讀出（父子 IPC） |

### 標準檔案描述子

| fd | 名稱 | 用途 | 重導向符號 |
|----|------|------|-----------|
| 0 | stdin | 鍵盤輸入 | `<` |
| 1 | stdout | 終端機輸出 | `>` `>>` |
| 2 | stderr | 終端機錯誤 | `2>` |

---

## 期中作業 — Mini Shell (myshell)

> **檔案：** [`期中作業/`](../期中作業/) — 完整文件 [`README.md`](../期中作業/README.md)

**整合全學期所學**的期末專案。實作一個可互動的命令列解譯器 (shell)。

### 功能清單

| 功能 | 實作方式 |
|------|----------|
| **內建命令** | `cd` (chdir), `pwd` (getcwd), `exit`, `help` |
| **外部命令** | fork + execvp 執行任意程式 |
| **I/O 重導向** | `<` (stdin), `>` (stdout 覆蓋), `>>` (stdout 附加)，使用 open + dup2 |
| **管道** | `\|` 連接兩個命令，使用 pipe + dup2 |
| **背景執行** | `&` 不等待子行程，SIGCHLD 回收 zombie |
| **訊號處理** | SIGINT (Ctrl+C 不中斷 shell), SIGCHLD (回收背景行程) |

### 檔案架構

```
期中作業/
├── myshell.c    主程式 (~250 行)
├── Makefile     編譯 / 測試 / 清理
└── README.md    完整技術文件
```

### 涵蓋的系統呼叫總覽

| 類別 | 系統呼叫 |
|------|----------|
| 行程管理 | `fork()`, `execvp()`, `waitpid()`, `exit()` |
| 檔案 I/O | `open()`, `close()` |
| I/O 重導向 | `dup2()` |
| 行程間通訊 | `pipe()` |
| 訊號處理 | `signal()` |
| 訊號安全輸出 | `write()` |
| 目錄操作 | `chdir()`, `getcwd()` |

### 程式架構

```
myshell.c
├── main()                   主迴圈
├── parse_line()             剖析輸入
├── execute_command()        命令分派
│   ├── run_builtin()        cd / pwd / exit / help
│   ├── execute_pipe()       管線 (|)
│   └── execute_single()     單一命令 + 重導向
├── split_pipe()             拆分管線
├── has_pipe()               檢查管線
├── setup_redirections()     < > >> 處理
├── sigchld_handler()        回收 zombie
└── sigint_handler()         防止 Ctrl+C 中斷
```

### 使用範例

```bash
$ ./myshell
myshell> pwd
myshell> ls -la | grep .c
myshell> echo "hello" > /tmp/test.txt
myshell> cat < /tmp/test.txt
myshell> sleep 5 &
[42] sleep
myshell> exit
```

---

## 全作業技術對照表

| 技術 | HW1 | HW2 | HW3 | HW4 | HW5 | HW6 | 期中 |
|------|:--:|:--:|:--:|:--:|:--:|:--:|:---:|
| Lexer / Tokenizer | ✓ | ✓ | ✓ | ✓ | — | — | — |
| Parser / EBNF | ✓ | ✓ | ✓ | — | — | — | — |
| IR / Code Generation | ✓ | ✓ | ✓ | — | — | — | — |
| Virtual Machine | ✓ | ✓ | ✓ | ✓ | — | — | — |
| Backpatching | ✓ | ✓ | ✓ | — | — | — | — |
| Stack Frame / Call Stack | ✓ | ✓ | ✓ | — | — | — | — |
| Multi-file Architecture | — | ✓ | ✓ | ✓ | — | ✓ | ✓ |
| fork / execvp | — | — | — | — | — | ✓ | ✓ |
| open / read / write / close | — | — | — | — | — | ✓ | ✓ |
| dup2 / Redirection | — | — | — | — | — | ✓ | ✓ |
| pipe / IPC | — | — | — | — | — | ✓ | ✓ |
| Thread / pthread | — | — | — | — | ✓ | — | — |
| Mutex / Lock | — | — | — | — | ✓ | — | — |
| Condition Variable | — | — | — | — | ✓ | — | — |
| Deadlock Prevention | — | — | — | — | ✓ | — | — |
| Signal Handling | — | — | — | — | — | — | ✓ |
| Makefile | — | — | — | — | — | — | ✓ |

---

## 編譯指令速查

```bash
# HW1
gcc h1.c -o h1

# HW2 / HW3
gcc main.c lexer.c parser.c codegen.c vm.c -o my_compiler

# HW4
gcc main.c book_content.c reader.c -o spbook
gcc demo_memory.c -o demo_memory
gcc demo_stack.c  -o demo_stack
gcc demo_vm.c     -o demo_vm
gcc demo_lexer.c  -o demo_lexer

# HW5
gcc -o bank bank.c -lpthread
gcc -o producer_consumer producer_consumer.c -lpthread
gcc -o dining_philosophers dining_philosophers.c -lpthread

# HW6
gcc -o fork_exec fork_exec.c
gcc -o file_copy file_copy.c
gcc -o redirect redirect.c
gcc -o pipe_demo pipe_demo.c

# 期中作業
gcc -Wall -Wextra -g -o myshell myshell.c
# 或
make
```

---

## 檔案結構總覽

```
_sp/
├── HW1/
│   ├── README.MD          # 編譯器雛形文件
│   └── h1.c               # 單檔案編譯器 + VM
│
├── HW2/
│   ├── README.MD          # 模組化架構文件
│   ├── tiny_compiler.h    # 中央標頭檔
│   ├── lexer.c            # 詞法分析器
│   ├── parser.c           # 語法剖析器
│   ├── codegen.c          # IR 產生器
│   ├── vm.c               # 虛擬機
│   └── main.c             # 進入點
│
├── HW3/
│   ├── README.md          # 強化版文件（EBNF + IR 手冊）
│   ├── tiny_compiler.h
│   ├── lexer.c
│   ├── parser.c
│   ├── codegen.c
│   ├── vm.c
│   └── main.c
│
├── HW4/
│   ├── README.MD          # SP-Book 文件
│   ├── spbook.h           # 標頭檔
│   ├── book_content.c     # 10 章 40 小節內容
│   ├── reader.c           # 互動閱讀器
│   ├── main.c             # 進入點
│   ├── demo_memory.c      # 記憶體佈局 Demo
│   ├── demo_stack.c       # 呼叫堆疊 Demo
│   ├── demo_vm.c          # 迷你 VM Demo
│   └── demo_lexer.c       # 獨立 Lexer Demo
│
├── HW5/
│   ├── README.md          # 多執行緒文件
│   ├── bank.c             # 銀行存提款模擬
│   ├── producer_consumer.c # 生產者消費者
│   └── dining_philosophers.c # 哲學家用餐
│
├── HW6/
│   ├── README.md          # 行程與檔案 I/O 文件
│   ├── fork_exec.c        # fork + execvp
│   ├── file_copy.c        # open/read/write/close
│   ├── redirect.c         # dup2 重導向
│   └── pipe_demo.c        # pipe 父子通訊
│
├── 期中作業/
│   ├── README.md          # Mini Shell 完整文件
│   ├── myshell.c          # Shell 主程式
│   └── Makefile           # 編譯腳本
│
└── 期末/
    └── README.md          # 本文件 — 全學期總整理
```

---

> **課程：** 系統程式設計  
> **範圍：** HW1 → 期中作業  
> **總計：** 7 個作業，30+ 個程式檔，涵蓋編譯器、VM、多執行緒、行程管理、檔案 I/O、Shell 實作
