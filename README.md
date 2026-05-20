# Limit Order Book (LOB)

## 專案概述

本專案的核心學習目標，在於將《資料結構》課程中所學的樹狀結構（紅黑樹，Red-Black Tree），實際落地並應用於量化交易領域的「限價簿 (Limit Order Book, LOB)」。

*(註：本專案目前正處於底層資料結構的建構與指標邏輯除錯階段。待核心撮合邏輯與節點同步測試完成後，將持續更新系統)*

---
## Roadmap & TODO

### Phase 1: 核心資料結構

- [x] **底層紅黑樹重構 (RBT/BST)** 
	- [x] 導入 Template 泛型設計。 
	- [x] 引入可自訂的比較規則。 
	- [x] 優化插入邏輯，使其成為單次走訪
- [ ] **定義單筆訂單 `Order` 結構**
  - [ ] 包含 `order_id`, `price`, `volume` 等關鍵資訊（保留 price 以利 O(1) 刪單時能反查 PriceLevel）。
- [ ] **定義價格檔位 `PriceLevel` 結構**
  - [ ] 包含 `price`, `total_volume`。
  - [ ] 使用 `std::list<Order>` 實作 FIFO 排隊隊伍。
- [ ] **泛型比較邏輯**
  - [ ] 撰寫客製化 `Compare`，確保底層 RBT 僅根據 `PriceLevel.price` 進行比較。
- [ ] **實作委託簿 (LOB)**
  - [ ] 買方樹 (`BuyTree`) 與 賣方樹 (`SellTree`)。
  - [ ] 實作全域視角的 Order Map。
  - [ ] 實作 O(1) 刪單邏輯。

### Phase 2: 效能優化 (Future Work)

- [ ] 將 `std::list` 替換為 Array-backed Memory Pool，消除頻繁的 `new`/`delete`。
- [ ] 提升系統的 Cache Locality，將延遲降低。

---

## 開發日誌 (Changelog)

* **[2026-05-18] Initial Commit**
  * 建立專案基礎架構與 GitHub 儲存庫。
  * 將 Binary search tree 更改為使用 templete，以達成泛型的使用
* [2026-05-19] compelete RBT & BST
	* 正式完成 binary search tree 與 red black tree