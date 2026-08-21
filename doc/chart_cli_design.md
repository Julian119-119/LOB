# make_chart.py 使用說明


`script/make_chart.py` 讀取 `main_program` 產生的 CSV，自動判斷該畫成圖還是印成 table，不需要額外告訴它輸入是哪種模式。

## 用法

```bash
python3 script/make_chart.py --input <CSV路徑> [--output <圖片路徑>] [--chart-type line|step|histogram]
```

| Flag                  | 必要性 | 說明                                                                      |
| --------------------- | --- | ----------------------------------------------------------------------- |
| `--input <path>`      | 必要  | 要讀的 CSV，通常就是 `main_program --output` 產生的那個檔案                            |
| `--output <path>`     | 選用  | 存成圖片檔（例如 `.png`）。不給的話會直接跳出視窗互動顯示                                        |
| `--chart-type <type>` | 選用  | `line`（預設）、`step`、`histogram` 三選一。只有輸入是「streaming + 單一數值」時才有效果，其他情況會被忽略 |

## 範例

**1. batch 輸出 → 印成 table**

```bash
./out/main_program -i sample_data/processed/convertion_l2_data.csv -o /tmp/best_bid.csv -m batch --query best-bid
python3 script/make_chart.py --input /tmp/best_bid.csv
```

不會開圖，直接在終端機印出整份資料的 table。

**2. streaming 單一數值 → 折線圖(預設)**

```bash
./out/main_program -i sample_data/processed/convertion_l2_data.csv -o /tmp/mid_price.csv -m streaming --query mid-price
python3 script/make_chart.py --input /tmp/mid_price.csv
```

跳出一個視窗，畫出 `mid-price` 隨時間變化的折線圖。

**3. 換成 step 圖**

```bash
python3 script/make_chart.py --input /tmp/mid_price.csv --chart-type step
```

**4. 換成分佈圖（histogram）**

```bash
python3 script/make_chart.py --input /tmp/mid_price.csv --chart-type histogram
```

x 軸是數值範圍，y 軸是次數——看的是這個值整段期間大概落在哪裡、有沒有離群值，不是時間趨勢。

**5. 不開視窗，直接存成圖片**

```bash
python3 script/make_chart.py --input /tmp/mid_price.csv --chart-type step --output /tmp/mid_price_step.png
```

**6. 固定價位的量隨時間變化**

```bash
./out/main_program -i sample_data/processed/convertion_l2_data.csv -o /tmp/vol.csv -m streaming --query volume-at-bid-price 6420.5
python3 script/make_chart.py --input /tmp/vol.csv --chart-type line
```

## 目前還不支援

1. 多個 input。當前只能輸入一個 input，無法做出多個 input 的多圖比較
2. best-ask 與 best-bid 的 streaming mode，目前還無法指定 price 與 volume 使用不同的圖表種類


## 設計細節

自動判斷輸入格式的邏輯（batch/streaming 怎麼分、要畫哪一欄）、以及對 `main_program` 輸出欄位順序的依賴，記錄在程式碼註解與 `doc/cli_design.md` 中。
