# 已知技術債

> **文件性質：歷史紀錄。** 原本是 `doc/refactor_plan.md`（紅黑樹 unique_ptr 重構，已完成並刪除該文件）附錄整理的技術債清單；重構本身的過程細節已經不需要保留，只留這份清單供之後排優先序參考。

這些項目跟樹重構沒有直接關係，是調查背景時一併整理的，涉及「還有哪些問題」與「未來想接實盤資料」：

- **`place_order`/`cancel_order` 目前沒有任何成交回報機制**（`src/LOB_L3_type.cpp` 回傳 `void`，撮合只是直接改 `Order.volume`，沒有 `Trade` event）。這是接實盤前一定要補的基礎設施（測試、日誌、視覺化都需要），但需要另外設計 `Trade` struct 與回傳型別。
- **`LOB` 沒有明確處理 copy/move 語意**：`pool`（`unsynchronized_pool_resource`）被 `order_map` 和每個 `PriceLevel::order_queue` 綁定，若 `LOB` 被 copy 會產生指向錯誤 pool 的 dangling container，目前沒有測試覆蓋到，建議之後 `=delete` 或正確實作。
- **完全沒有網路/檔案 I/O/JSON/CSV 解析（L3 端）**、**完全 single-threaded**（`pmr::unsynchronized_pool_resource` 明確非 thread-safe）——這兩點是接實盤資料前必須從零設計的部分：需要一個 feed adapter 把外部格式轉成 `Order`/操作事件，以及決定 ingestion thread 跟撮合邏輯之間的同步模型（single-writer + queue 是常見做法）。
- 較小的技術債：sentinel value（`PRICE_NO_VALUE` 等）可考慮改成 `std::optional`、`price` 用 `double` 存在實盤情境下可能有精度問題（可考慮改成整數化的 tick 表示）。
