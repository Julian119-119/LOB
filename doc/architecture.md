# 核心資料結構藍圖 (Core Data Structures)

## 1. 單筆訂單 (Order)

最小的單位，記錄每一筆委託的詳細資訊。
* `order_id`: 全域唯一的訂單識別碼，用於 O(1) 尋址。
* `volume`: 訂單數量（買或賣多少股/張）。
* `timestamp`: 記錄下單的絕對時間（供回測與稽核使用，搓合優先序由 queue 位置隱含決定）。
* `price`: 記錄該筆訂單的價格（保留此欄位以利 O(1) 刪單時能反查對應的 Price Level）。

## 2. 價格檔位 (Price Level)

實際存進紅黑樹 (RBT) 的節點結構，記錄單一價格檔位的深度資訊。
* `price`: 檔位價格，作為 RBT 排序與查找的唯一 Key 值。
* `total_volume`: 該檔位目前所有訂單數量的「總和 (總股數)」，提供 O(1) 的報價查詢。
* `order_queue`: 具備 FIFO 特性的 container (`list`)，儲存相同價格下的訂單，維持時間優先。

## 3. Limit Order Book

負責統籌搓合邏輯與全域視角。
* `BuyTree` (RBT): 儲存買方 Price Level，價格高者優先 (Max-Tree)。
* `SellTree` (RBT): 儲存賣方 Price Level，價格低者優先 (Min-Tree)。
* `OrderMap`: 訂單映射表 (`unordered_map`)，紀錄 `order_id` 與在 Queue 中記憶體位置 (Iterator) 的對應關係，確保隨機刪單為 O(1) 複雜度。