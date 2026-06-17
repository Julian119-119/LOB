# Limit Order Book

## Intro

本專案為以 c++ 實做的 Limit Order Book (LOB)。

side project 最初為 DSOOP 中 Red black tree 作業延伸而成。在此基礎之上，將底層 RBT 與 BST 修改成了泛型並且增加了訂單管理、撮合與取消以及資料查詢等 LOB 的核心功能。

此 side project 的長期目標為能夠承接實盤的 point in time 數據，並作為未來量化交易的學習與研究工具。

---
## Project Highlights

- Generic Binary Search Tree
- Generic Red-Black Tree
- Price-Time Priority Matching Engine
- O(1) Order Lookup via Hash Table

---
## Features
### 資料查詢 time complexity

| Function           | Complexity |
| ------------------ | ---------- |
| Best Bid / Ask     | O(1)       |
| Volume At Price    | O(log N)   |
| Top-K Price Levels | O(K)       |
| Order Lookup       | O(1)       |

---
## 目前功能

### 訂單管理

- 限價單下單
- 訂單取消
- 撮合訂單
- 買賣盤管理

### 支援的 Time In Force

- GTC
- IOC
- FOK

### 資料查詢

- Best Bid Price  
- Best Ask Price  
- Best Bid Volume  
- Best Ask Volume  
- Spread 
- Mid Price  
- Top-K Price Levels  
- Volume At Price
---
## Build & Run

本專案目前主要於 Linux 環境下開發與測試，環境如下：

- OS: MX Linux 23.6 (基於 Debian 12 延伸而成)
- Compiler: g++ 12.2.0
- Standard: c++ 17

### 相依需求

- c++ 17 以上
- GNU make

### 編譯測試程式

```bash
make test_program
```

### 執行測試

```bash
make test
```

### 編譯 Benchmark

```bash
make benchmark_program
```

### 執行 Benchmark

```bash
make run_benchmark
```

### 清除物件與執行檔

```bash
make clean
```

---
## Roadmap

### 短期目標

- 補充更多 Boundary Test
- 增加 Randomized Test
- Benchmark 擴充
- 改善文件與註解

### 長期目標

- 支援輸入 Point-in-Time 的 data
- Order Book 狀態的視覺化
- 增加更多分析工具
- 更多商品的支援

---
## Document 

詳細設計請參閱

```
docs/
├─ architecture.md
└─ benchmark.md
```