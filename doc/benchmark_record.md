
# Benchmark 紀錄

> 此為**歷史紀錄。** 用來將 `benchmark_program`（`make run_benchmark`）的執行結果留存，不是使用手冊。「refactor」指 `doc/refactor_plan.md` 描述的紅黑樹 unique_ptr 重構。

## Before refactor

Total number of test data in each round: 10000000

| round | Total spending time | Average spending time |
| ----- | ------------------- | --------------------- |
| 一     | 5745ms              | 574ns                 |
| 二     | 6102ms              | 610ns                 |
| 三     | 5727ms              | 572ns                 |
| 四     | 5820ms              | 582ns                 |
| 五     | 5784ms              | 578ns                 |

median: 
- average spending time: 578ns
- total spending time: 5784ms

## After refactor

Total number of test data in each round: 10000000

| round | Total spending time | Average spending time |
| ----- | ------------------- | --------------------- |
| 一     | 5070ms              | 507ns                 |
| 二     | 5036ms              | 503ns                 |
| 三     | 5067ms              | 506ns                 |
| 四     | 4996ms              | 499ns                 |
| 五     | 5046ms              | 504ns                 |

median: 
- average spending time: 504ns
- total spending time: 5046ms




