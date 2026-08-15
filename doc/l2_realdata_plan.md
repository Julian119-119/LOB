# L2 實盤資料大方向規劃

## 背景

LOB 目前的撮合引擎是 **L3（order-level）** 模型：每一筆委託都是獨立的 `Order`（有 `order_id`），同一 price level 內用 FIFO `order_queue` 排隊，撮合邏輯（`order_matching`）會實際消耗、拆分訂單。

真正容易取得的真實市場資料，絕大多數是 L2，而非 L3：

- **L1**：只有目前最佳買賣價（best bid/ask）與最新成交價。
- **L2**：每個 price level 的**總量**（例如前 10 檔），看不到裡面是哪幾筆訂單組成、也看不到訂單間的先後順序。交易所的 WebSocket depth feed（snapshot + diff）多半是這個層級，取得成本低。
- **L3**：逐筆委託的新增/取消/成交都個別可見，才能重建出跟真實交易所一致的隊列順序。這類資料稀少且通常昂貴（例如 LOBSTER，從 NASDAQ ITCH 重建）。

L2 資料沒有 order-level 細節，無法餵給現有的 FIFO 撮合邏輯。因此實作上需要的是重現某一個快照當下的 LOB 狀態。

## 已確認的方向

1. **保留現有 L3 `LOB`，不動它的撮合邏輯。** L2 是另一個獨立的使用情境，不是 `LOB` 的一個模式開關。
2. **L2 另外寫一個檔案/類別**，重用同一個 `RedBlackTree<T,Compare>` 容器（重構後已是通用的扁平樹 + iterator），但 value type 換成更輕量的：只有 `price` + `total_volume`，沒有 `order_queue`。
3. **查詢類 API（best bid/ask、spread、top-k）先在 L2/L3 各自實作，不提前抽出共用層。** 等兩邊都寫得差不多、有具體重複時，再考慮抽出共用的查詢。
4. **L2 的寫入操作為 upsert/remove：**
   - **Snapshot**：一次拿到目前所有 price level 的完整清單。
   - **Delta**：後續持續送「這個 price 的量變成多少」的單筆異動，量歸零就移除該 price level。屬於單點操作，概念上接近現有 `insert_emplace`/`remove`，只是觸發來源從「我方下單」變成「交易所回報」。
5. **資料前處理用 Python**：
   - L2 原始資料通常是巢狀 JSON，還要處理 snapshot/diff 對齊、sequence number 缺口偵測，這類需要維護狀態的邏輯用 Python（`json`/`pandas`）處理比較自然。
6. **先做批次讀檔，不做即時串流。** Python 抓資料、整理成固定格式的檔案（例如欄位為 `timestamp, price, volume, is_snapshot` 的 CSV），C++ 端離線讀檔重播（適合回測情境）。等批次流程跑順了，再考慮加上 socket/pipe 的即時串流模式（Python 即時餵、C++ 端監聽），不必一開始就處理連線時序、斷線重連等複雜度。

## 建議的實作順序

1. 用 Python 抓一小段真實 L2 資料（一次 snapshot + 一小段 delta stream）
2. 根據抓到的真實資料，定下 Python/C++ 之間的交換格式（初版：固定欄位 CSV）。
3. 設計 C++ 端批次讀檔的介面（讀檔 → 重建/更新 L2 樹狀結構）。
4. 設計 L2 底層樹的 value type 與寫入 API（`apply_snapshot`、`apply_delta` 之類）。
5. 批次讀檔流程穩定後，再評估是否要加上即時串流（socket/pipe）模式。

## 暫不處理的項目

以下維持既有技術債清單（見 `doc/refactor_plan.md` 附錄），與這次 L2 規劃無關，之後有需要再另外排優先序：

- 沒有 `Trade` 事件/回報機制。
- 沒有執行緒安全考量。
- Sentinel value（`PRICE_NO_VALUE` 等）尚未改成 `std::optional`。
- `price` 用 `double` 儲存，未來若要串接高精度場景可能需要改成整數化的 tick 表示。
