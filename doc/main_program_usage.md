# main_program 使用說明

> 此為**使用手冊**。 設計決策與取捨記錄在 `doc/cli_design.md` 之中。

`main_program`（由 `src/main.cpp` build 出來）讀取 L2 market data CSV，依照指定的 query 與 mode 輸出結果。

## 用法

```bash
./out/main_program -i <input CSV> -o <output CSV> -m <batch|streaming> --query <query> [額外參數] [--stop-at <timestamp>]
```

| Flag                     | 必要性 | 說明                                                            |
| ------------------------ | --- | ------------------------------------------------------------- |
| `-i`, `--input <path>`   | 必要  | 輸入 CSV 路徑                                                      |
| `-o`, `--output <path>`  | 必要  | 輸出 CSV 路徑                                                      |
| `-m`, `--mode <mode>`    | 必要  | `batch`（讀完整個檔案再一次輸出結果）或 `streaming`（邊讀邊輸出，每套用一筆資料就輸出當下狀態）       |
| `--query <query>`        | 必要  | 見下方「query 一覽」，部分 query 後面要接額外參數                                 |
| `--stop-at <timestamp>`  | 選用  | 讀到這個 timestamp（含，微秒）就停止，兩種 mode 都可用                             |

## query 一覽

| query 值                 | 適用 mode      | 額外參數              | 回傳內容        |
| ----------------------- | ------------ | ----------------- | ----------- |
| `mid-price`             | batch、streaming | 無                  | 中間價         |
| `best-bid`              | batch、streaming | 無                  | 最佳買價與其量     |
| `best-ask`              | batch、streaming | 無                  | 最佳賣價與其量     |
| `spread`                | batch、streaming | 無                  | 買賣價差        |
| `volume-at-bid-price`   | batch、streaming | `<price>`          | 該買價的總量      |
| `volume-at-ask-price`   | batch、streaming | `<price>`          | 該賣價的總量      |
| `bid-top-k`             | 僅 batch       | `<k>`              | 買方前 k 檔的價與量 |
| `ask-top-k`             | 僅 batch       | `<k>`              | 賣方前 k 檔的價與量 |

`bid-top-k`/`ask-top-k` 目前只能在 `batch` 模式下使用，streaming 模式不支援。

## 輸出欄位

| query                   | streaming 輸出欄位              | batch 輸出欄位          |
| ----------------------- | ----------------------------- | -------------------- |
| `mid-price`             | `timestamp,mid-price`         | `mid-price`           |
| `best-bid`              | `timestamp,best-bid,volume`   | `best-bid,volume`     |
| `best-ask`              | `timestamp,best-ask,volume`   | `best-ask,volume`     |
| `spread`                | `timestamp,spread`            | `spread`              |
| `volume-at-bid-price`   | `timestamp,side,price,volume` | `side,price,volume`   |
| `volume-at-ask-price`   | `timestamp,side,price,volume` | `side,price,volume`   |
| `bid-top-k`             | 不適用                            | `side,price,volume`   |
| `ask-top-k`             | 不適用                            | `side,price,volume`   |

這份輸出可以直接餵給 `script/make_chart.py`（見 `doc/chart_cli_design.md`）。

## 範例

**1. batch 模式查詢中間價**

```bash
./out/main_program -i sample_data/processed/convertion_l2_data.csv -o /tmp/mid_price.csv -m batch --query mid-price
```

**2. streaming 模式查詢 spread，並在特定時間點停止**

```bash
./out/main_program -i sample_data/processed/convertion_l2_data.csv -o /tmp/spread.csv -m streaming --query spread --stop-at 1585699200000000
```

**3. 查詢固定價位的量**

```bash
./out/main_program -i sample_data/processed/convertion_l2_data.csv -o /tmp/vol.csv -m streaming --query volume-at-bid-price 6420.5
```

**4. batch 模式查詢買方前 10 檔**

```bash
./out/main_program -i sample_data/processed/convertion_l2_data.csv -o /tmp/top10.csv -m batch --query bid-top-k 10
```

## 目前限制

- `bid-top-k`/`ask-top-k` 只能在 batch 模式使用（見上方說明）。
- `--query` 一次只能指定一個 query，無法同時輸出多個查詢結果。

## 設計細節

上述限制背後的取捨、輸出欄位順序的契約（新增 query 時要遵守的規則），記錄在 `doc/cli_design.md`，這裡不重複。
