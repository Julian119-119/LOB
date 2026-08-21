# main.cpp 命令列參數設計

> 此為**設計筆記**，用以記錄 CLI 背後的取捨與規則，不是操作說明。要查怎麼下指令、有哪些 flag/query，參見 `doc/main_program_usage.md`。

## 背景

`main.cpp` 支援兩種輸出情境：

1. **streaming**：邊呼叫 `load_CSV_snapshot` 邊透過 `on_batch_applied` callback 輸出。
2. **batch**：讀完整個檔案之後，才用 `L2_LOB` 的查詢方法（`get_top_k_info`、`get_best_bid_price` 等）一次性輸出結果。

`--mode` 決定「何時」輸出（每讀一筆就輸出 vs 讀完整批才輸出一次），`--query` 決定「輸出什麼」，大部分 query 兩種 mode 都能用，只有 `bid-top-k`/`ask-top-k` 限定只能在 batch 下用（見下方說明）。

兩種模式都可以使用 `load_CSV_snapshot` 的 `stop_at_timestamp` 參數（提前終止讀檔）。

## 參數設計的取捨

核心參數（`-i`/`-o`/`-m`/`--query`/`--stop-at`）與各 query 的適用 mode、額外參數，實際清單見 `doc/main_program_usage.md`；這裡只記錄背後的規則與限制成因，避免跟使用手冊重複維護同一份表格。

`--query` 目前設計成**一次只能選一個**，輸出的 CSV 只對應單一查詢結果。之後如果需要一次輸出多個查詢（例如 `best-bid`、`best-ask`、`spread` 同時要），可以擴充成逗號分隔的清單（`--query best-bid,best-ask,spread`），但這會讓輸出欄位變成動態組裝，複雜度高不少——等單一 query 版本穩定、其他部分都設計得差不多後再評估。

`bid-top-k`/`ask-top-k` 限定只能在 `--mode batch` 下使用：每次查詢會吐出 `k` 筆結果，如果放進 streaming 模式（每讀一筆 row 就觸發一次 callback），代表每一筆輸入都要展開成 `k` 列輸出，資料量會膨脹得很快。之後如果真的需要 streaming 模式下的 top-k，再另外設計。

## 輸出欄位設計的取捨

streaming 模式每一列都對應「這筆資料被套用後當下的狀態」，`timestamp` 語意清楚（逐行都在變，是真正的時間序列），保留。

**batch 模式的輸出暫時不放 `timestamp`。** 因為當前不確定用到 timestamp 的機會高或不高，且新增 timestamp 的代價較高（需要紀錄下來這個 PriceLevel 的最後改動時間）

各 query 實際的輸出欄位清單見 `doc/main_program_usage.md`。

新增 query 時的規則：**streaming mode 輸出的最後一欄，必須是這次查詢真正隨時間變動的數值**（其餘欄位可以是 `timestamp`，或是像 `volume-at-bid-price` 的 `side`/`price` 那種在單次查詢裡固定不變的識別欄）。`script/make_chart.py` 依賴這個順序，用 `columns[-1]` 自動判斷要畫哪一欄，不靠 query 名稱另外判斷。之後新增 streaming query 時，輸出欄位順序需遵守這個慣例，否則會讓 `make_chart.py` 的自動判斷邏輯失效。
