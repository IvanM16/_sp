# HW5: Linux 多執行緒程式設計

## 目錄

1. [基礎概念](#基礎概念)
2. [程式一：銀行存提款模擬 (bank.c)](#程式一銀行存提款模擬-bankc)
3. [程式二：生產者消費者問題 (producer_consumer.c)](#程式二生產者消費者問題-producer_consumerc)
4. [程式三：哲學家用餐問題 (dining_philosophers.c)](#程式三哲學家用餐問題-dining_philosophersc)
5. [編譯與執行](#編譯與執行)

---

## 基礎概念

### 執行緒 (Thread)

執行緒是作業系統能夠進行排程的最小執行單位。一個行程 (process) 可以包含多個執行緒，
這些執行緒**共享相同的記憶體空間**（程式碼、全域變數、堆積記憶體），但各自擁有
獨立的暫存器、堆疊 (stack) 和程式計數器。

**多執行緒的優點：**
- 提升程式平行處理能力
- 充分利用多核心 CPU
- 適合 I/O 密集與計算密集並存的場景
- 執行緒間通訊成本低（共享記憶體）

**多執行緒的挑戰：**
- 競爭條件 (race condition)
- 死結 (deadlock)
- 資料同步問題
- 除錯困難

### 競爭條件 (Race Condition)

當多個執行緒同時讀寫共享資源，而程式執行的結果**取決於執行緒的執行順序**時，
就稱為競爭條件。這是一種非確定性的錯誤，難以重現與除錯。

**範例：**兩個執行緒同時對 `balance` 進行 `+= 100` 操作：

```
執行緒 A: 讀取 balance (0) → 計算 0+100=100 → 寫入 balance (100)
執行緒 B: 讀取 balance (0) → 計算 0+100=100 → 寫入 balance (100)
結果: balance = 100 (應該為 200！)
```

因為執行緒 B 在 A 寫入前就讀取了舊值，導致 A 的寫入被覆蓋。

### 互斥鎖 (Mutex)

Mutex (Mutual Exclusion) 是一種同步機制，用於**保護共享資源**，確保同一時間
只有一個執行緒能夠存取該資源。

**基本操作：**
- `pthread_mutex_lock()`：鎖定 mutex，若已被鎖定則阻塞等待
- `pthread_mutex_unlock()`：解鎖 mutex，讓其他等待的執行緒可以鎖定

**臨界區間 (Critical Section)：**被 mutex 保護的程式碼區段，同一時間只有一個執行緒能執行。

```c
pthread_mutex_lock(&lock);
// --- 臨界區間開始 ---
balance += 100;  // 只有一個執行緒能執行此處
// --- 臨界區間結束 ---
pthread_mutex_unlock(&lock);
```

### 死結 (Deadlock)

當兩個或多個執行緒互相等待對方釋放資源，導致所有執行緒**永久阻塞**，即為死結。

**死結的四個必要條件 (Coffman Conditions)：**
1. **互斥 (Mutual Exclusion)：**資源一次只能被一個執行緒持有
2. **持有並等待 (Hold and Wait)：**持有資源的同時等待其他資源
3. **不可搶奪 (No Preemption)：**資源不能被強制釋放
4. **循環等待 (Circular Wait)：**存在執行緒間的循環依賴

**預防死結的策略：**
- 打破循環等待：所有執行緒按照固定順序取得資源
- 使用 `trylock` 並在失敗時釋放已持有的資源
- 設定超時機制
- 減少鎖的粒度

### 條件變數 (Condition Variable)

條件變數用於執行緒間的**事件通知**，讓執行緒在特定條件滿足前進入等待狀態。
通常與 mutex 搭配使用：

- `pthread_cond_wait()`：釋放 mutex 並等待條件信號
- `pthread_cond_signal()`：喚醒一個等待中的執行緒
- `pthread_cond_broadcast()`：喚醒所有等待中的執行緒

---

## 程式一：銀行存提款模擬 (bank.c)

### 問題描述

模擬銀行帳戶的存款與提款操作。使用 4 個執行緒，每個執行緒執行 25,000 次
存款+提款（共 100,000 次操作）。每次存款 100 元後立即提款 100 元，
因此最終餘額應為 0。

### 實作方式

- 建立一個共享的 `Account` 結構，包含 `balance`、`mutex` 和 `use_mutex` 旗標
- 執行兩次模擬：**不使用 mutex**（展示競爭條件）和**使用 mutex**（正確同步）
- 每個執行緒在迴圈中執行 `balance += 100` 和 `balance -= 100`

### 核心程式碼

```c
void *do_transactions(void *arg) {
    for (int i = 0; i < OPS_PER_THREAD; i++) {
        if (account.use_mutex) pthread_mutex_lock(&account.lock);
        // --- 臨界區間 ---
        account.balance += 100;
        account.balance -= 100;
        // --- 臨界區間 ---
        if (account.use_mutex) pthread_mutex_unlock(&account.lock);
    }
    return NULL;
}
```

### 預期輸出

- **不使用 mutex：**最終餘額通常不為 0，展示了競爭條件導致的資料錯誤
- **使用 mutex：**最終餘額為 0，確保執行緒安全

---

## 程式二：生產者消費者問題 (producer_consumer.c)

### 問題描述

經典的同步問題：**生產者**產生資料放入有限大小的緩衝區，**消費者**從緩衝區取出資料。
生產者不能在緩衝區滿時放置資料，消費者不能在緩衝區空時取出資料。

### 實作方式

- 使用**環形緩衝區** (circular buffer)，大小為 10
- 2 個生產者執行緒，2 個消費者執行緒，共生產 20 個項目
- 使用 **mutex** 保護緩衝區的存取
- 使用兩個**條件變數**：
  - `not_full`：當緩衝區滿時，生產者等待
  - `not_empty`：當緩衝區空時，消費者等待
- 隨機的 `usleep()` 模擬生產與消費的處理時間

### 核心程式碼

```c
// 生產者
pthread_mutex_lock(&buf.lock);
while (buf.count == BUFFER_SIZE)           // 緩衝區滿，等待
    pthread_cond_wait(&buf.not_full, &buf.lock);
buf.buffer[buf.in] = item;                  // 放入資料
buf.in = (buf.in + 1) % BUFFER_SIZE;
buf.count++;
pthread_cond_signal(&buf.not_empty);        // 通知消費者
pthread_mutex_unlock(&buf.lock);

// 消費者
pthread_mutex_lock(&buf.lock);
while (buf.count == 0)                     // 緩衝區空，等待
    pthread_cond_wait(&buf.not_empty, &buf.lock);
int item = buf.buffer[buf.out];             // 取出資料
buf.out = (buf.out + 1) % BUFFER_SIZE;
buf.count--;
pthread_cond_signal(&buf.not_full);         // 通知生產者
pthread_mutex_unlock(&buf.lock);
```

### 預期輸出

- 生產者與消費者交替工作
- 緩衝區佔用量在 0 到 10 之間變化
- 最終所有生產的項目都被消費（`items_produced == items_consumed == 20`）

---

## 程式三：哲學家用餐問題 (dining_philosophers.c)

### 問題描述

五位哲學家圍坐圓桌，每人之間有一根筷子。哲學家交替進行**思考**和**用餐**。
用餐需要同時拿到左右兩根筷子。問題在於：若所有哲學家同時拿起左邊的筷子，
則會形成循環等待而產生死結。

### 實作方式

- 5 個哲學家執行緒，5 根筷子（每根筷子是一個 mutex）
- 每位哲學家吃 3 餐
- **死結預防策略：**最後一位哲學家（編號 4）先拿右邊筷子再拿左邊，打破循環等待

### 核心程式碼（死結預防）

```c
if (id == NUM_PHILOSOPHERS - 1) {
    // 最後一位哲學家：先右後左（打破循環等待）
    pthread_mutex_lock(&chopsticks[right]);
    pthread_mutex_lock(&chopsticks[left]);
} else {
    // 其他哲學家：先左後右
    pthread_mutex_lock(&chopsticks[left]);
    pthread_mutex_lock(&chopsticks[right]);
}
eat(id);
pthread_mutex_unlock(&chopsticks[left]);
pthread_mutex_unlock(&chopsticks[right]);
```

### 其他避免死結的方法

1. **限制同時用餐人數**：最多允許 4 位哲學家同時嘗試拿筷子
2. **奇偶編號策略**：奇數先拿左、偶數先拿右
3. **使用 trylock**：嘗試鎖定，失敗則釋放已持有的筷子並重試

### 預期輸出

- 哲學家交替思考和用餐
- 不會發生死結，所有哲學家最終都能吃完
- 輸出訊息顯示每根筷子的取得與釋放過程

---

## 編譯與執行

### 環境需求

- Linux 作業系統
- GCC 編譯器
- POSIX Thread Library (pthread)

### 編譯指令

```bash
# 銀行存提款模擬
gcc -o bank bank.c -lpthread

# 生產者消費者問題
gcc -o producer_consumer producer_consumer.c -lpthread

# 哲學家用餐問題
gcc -o dining_philosophers dining_philosophers.c -lpthread
```

### 執行

```bash
./bank
./producer_consumer
./dining_philosophers
```

### 一次編譯所有程式

```bash
gcc -o bank bank.c -lpthread && \
gcc -o producer_consumer producer_consumer.c -lpthread && \
gcc -o dining_philosophers dining_philosophers.c -lpthread
```

---

## 參考資料

- *Operating System Concepts* (Silberschatz, Galvin, Gagne)
- POSIX Threads Programming (Blaise Barney, LLNL)
- Linux 系統程式設計

---

> **作業要求：**HW5 - 多執行緒程式設計與同步問題  
> **檔案結構：** `bank.c` | `producer_consumer.c` | `dining_philosophers.c` | `README.md`
