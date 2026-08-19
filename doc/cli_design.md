# main.cpp 命令列參數設計

## 背景

`main.cpp` 支援兩種輸出情境：

1. **streaming**：邊呼叫 `load_CSV_snapshot` 邊透過 `on_batch_applied` callback 輸出。
2. **batch**：讀完整個檔案之後，才用 `L2_LOB` 的查詢方法（`get_top_k_info`、`get_best_bid_price` 等）一次性輸出結果。

`--mode` 決定「何時」輸出（每讀一筆就輸出 vs 讀完整批才輸出一次），`--query` 決定「輸出什麼」，大部分 query 兩種 mode 都能用，只有 `bid-top-k`/`ask-top-k` 限定只能在 batch 下用（見下方說明）。

兩種模式都可以使用 `load_CSV_snapshot` 的 `stop_at_timestamp` 參數（提前終止讀檔）。

## 參數設計

### 核心參數（兩種 mode 共用）

| Flag                    | 必要性 | 意義                                |
| ----------------------- | --- | --------------------------------- |
| `-i`, `--input <path>`  | 必要  | 輸入 CSV 路徑                         |
| `-o`, `--output <path>` | 必要  | 輸出 CSV 路徑                         |
| `-m`, `--mode <mode>`   | 必要  | `streaming` 或 `batch`             |
| `--query <query>`       | 必要  | 見下方「query 種類」——兩種 mode 都要指定       |
| `--stop-at <timestamp>` | 選用  | 兩種 mode 共用，到 `<timestamp>` 時停止（含） |

### query 種類

| query 值               | 適用 mode         | 額外參數（緊接在 query 值後面）                               | 對應查詢                                             |
| --------------------- | --------------- | ------------------------------------------------- | ------------------------------------------------ |
| `mid-price`           | streaming、batch | 無                                                 | `get_mid_price()`                                |
| `best-bid`            | streaming、batch | 無                                                 | `get_best_bid_price()` / `get_best_bid_volume()` |
| `best-ask`            | streaming、batch | 無                                                 | `get_best_ask_price()` / `get_best_ask_volume()` |
| `spread`              | streaming、batch | 無                                                 | `get_spread()`                                   |
| `volume-at-bid-price` | streaming、batch | `<price>`（例：`--query volume-at-bid-price 6420.5`） | `get_volume_at_price(price, Side::BUY)`          |
| `volume-at-ask-price` | streaming、batch | `<price>`                                         | `get_volume_at_price(price, Side::SELL)`         |
| `bid-top-k`           | 僅 batch         | `<k>`（例：`--query bid-top-k 10`）                   | `get_top_k_info(k, Side::BUY)`                   |
| `ask-top-k`           | 僅 batch         | `<k>`                                             | `get_top_k_info(k, Side::SELL)`                  |


## 輸出欄位設計

streaming 模式每一列都對應「這筆資料被套用後當下的狀態」，`timestamp` 語意清楚（逐行都在變，是真正的時間序列），保留。

**batch 模式的輸出暫時不放 `timestamp`。** 因為當前不確定用到 timestamp 的機會高或不高，且新增 timestamp 的代價較高（需要紀錄下來這個 PriceLevel 的最後改動時間）

| query                 | streaming 欄位                  | batch 欄位            |
| --------------------- | ----------------------------- | ------------------- |
| `mid-price`           | `timestamp,mid-price`         | `mid-price`         |
| `best-bid`            | `timestamp,best-bid,volume`   | `best-bid,volume`   |
| `best-ask`            | `timestamp,best-ask,volume`   | `best-ask,volume`   |
| `spread`              | `timestamp,spread`            | `spread`            |
| `volume-at-bid-price` | `timestamp,side,price,volume` | `side,price,volume` |
| `volume-at-ask-price` | `timestamp,side,price,volume` | `side,price,volume` |
| `bid-top-k`           | 不適用                           | `side,price,volume` |
| `ask-top-k`           | 不適用                           | `side,price,volume` |

## 暫不處理（先求簡單能動）

- `--query` 目前設計成**一次只能選一個**，輸出的 CSV 只對應單一查詢結果。之後如果需要一次輸出多個查詢（例如 `best-bid`、`best-ask`、`spread` 同時要），可以擴充成逗號分隔的清單（`--query best-bid,best-ask,spread`），但這會讓輸出欄位變成動態組裝，複雜度高不少——等單一 query 版本穩定、其他部分都設計得差不多後再評估。
- `bid-top-k`/`ask-top-k` 限定只能在 `--mode batch` 下使用：每次查詢會吐出 `k` 筆結果，如果放進 streaming 模式（每讀一筆 row 就觸發一次 callback），代表每一筆輸入都要展開成 `k` 列輸出，資料量會膨脹得很快。之後如果真的需要 streaming 模式下的 top-k，再另外設計。
