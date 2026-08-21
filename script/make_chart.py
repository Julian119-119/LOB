import pandas as pd
import matplotlib.pyplot as plt
import matplotx
import argparse
import sys


def main():
    # 讀取 CLI 參數
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", required=True, help="input CSV path")
    parser.add_argument(
        "--output", help="ouput CSV path. If not specified, display directly"
    )

    parser.add_argument(
        "--chart-type", choices=["line", "step", "histogram"], default="line"
    )
    parser.add_argument("--field", choices=["price", "volume", "both"])

    args = parser.parse_args()

    # 讀取檔案
    try:
        input_data = pd.read_csv(args.input)
    except FileNotFoundError:
        print(f"error: input file not found: {args.input}", file=sys.stderr)
        sys.exit(1)

    # 判斷檔案的狀態
    if "timestamp" in input_data.columns:
        mode = "streaming"
    else:
        mode = "batch"

    if "best-bid" in input_data.columns or "best-ask" in input_data.columns:
        is_multi_variable = True
    else:
        is_multi_variable = False

    # 檢查 batch mode 是否有 --field 參數
    if mode == "batch" and args.field is not None:
        print(
            "error: there are --field parameter for the input file in batch mode",
            file=sys.stderr,
        )
        sys.exit(1)

    # 檢查是否有單一數值的 input-data 帶有 "--field"
    if not is_multi_variable and args.field is not None:
        print(
            "error: there are --field parameter for the input file with one variable",
            file=sys.stderr,
        )
        sys.exit(1)

    # 檢查 --field，如果是在 "best-bid" 與 "best-ask" 則一定要有 --field
    if mode == "streaming" and is_multi_variable and args.field is None:
        print(
            "error: lack the '--field' parameter for the input file with multivariable",
            file=sys.stderr,
        )
        sys.exit(1)

    if mode == "streaming":  # streaming mode 才有 timestamp
        # 單一數值的 input_data (mid-price, spread, volume-at-price 等)
        if args.field != "both":
            if not is_multi_variable:  # input_data 本來就只有一個變數的狀況
                value_col = input_data.columns[-1]

                # 檢查最後一個 column 是否為唯一的變動數值
                more_variable = [
                    c
                    for c in input_data.columns
                    if input_data[c].nunique() > input_data[value_col].nunique()
                    and c != "timestamp"
                ]
                if more_variable:
                    print(
                        f"error: {more_variable} vary more than last column {value_col}",
                        f"check input format",
                        file=sys.stderr,
                    )
                    sys.exit(1)
            else:  # 在 input_data 中指定了要用某一個變數的狀況
                if args.field == "price":
                    if "best-bid" in input_data.columns:
                        value_col = "best-bid"
                    else:
                        value_col = "best-ask"
                else:
                    value_col = "volume"
            # 將 timestamp 從微秒的 uin64_t 轉為一般的時間表示法
            input_data["timestamp"] = pd.to_datetime(input_data["timestamp"], unit="us")
            # 將 timestamp  resamp 為每秒中的最後數值
            plot_data = (
                input_data.set_index("timestamp")[value_col].resample("1s").last()
            )
            # 設定圖表
            plt.rcParams["font.size"] = 14
            with plt.style.context(matplotx.styles.pitaya_smoothie["light"]):
                plt.figure(figsize=(19, 10))
                plot_title = value_col

                if args.chart_type == "line":
                    plt.plot(plot_data.index, plot_data.values)
                    plot_title += " trend chart"
                    plt.xlabel("time")
                    plt.ylabel(value_col)
                elif args.chart_type == "step":
                    plt.step(plot_data.index, plot_data.values)
                    plot_title += " step chart"
                    plt.xlabel("time")
                    plt.ylabel(value_col)
                else:
                    plt.hist(plot_data.values, bins="auto")
                    plot_title += " histogram"
                    plt.xlabel(value_col)
                    plt.ylabel("count")
                plt.title(plot_title)

                # 輸出圖表
                if args.output is None:
                    plt.show()
                else:
                    plt.savefig(args.output)
        else:  # 要求使用多個變數的狀況
            if "best-ask" in input_data.columns:
                price_col = "best-ask"
            else:
                price_col = "best-bid"

            # 轉換 timestamp 為一般的時間表示法
            input_data["timestamp"] = pd.to_datetime(input_data["timestamp"], unit="us")
            plot_data = input_data.set_index("timestamp").resample("1s").last()

            # 設定圖表
            plt.rcParams["font.size"] = 14
            with plt.style.context(matplotx.styles.pitaya_smoothie["light"]):
                # 設定 subplot 為上下或左右擺放
                if args.chart_type != "histogram":
                    fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True, figsize=(19, 10))
                else:
                    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(19, 10))

                # 設定 subplot 的標題
                ax1_title = price_col
                ax2_title = "volume"
                if args.chart_type == "line":
                    ax1.plot(plot_data.index, plot_data[price_col])
                    ax2.plot(plot_data.index, plot_data["volume"])
                    ax1.set_xlabel("time")
                    ax2.set_xlabel("time")
                    ax2.set_ylabel("volume")
                    ax1.set_ylabel(price_col)
                    ax1_title += " trend chart"
                    ax2_title += " trend chart"
                elif args.chart_type == "step":
                    ax1.step(plot_data.index, plot_data[price_col])
                    ax2.step(plot_data.index, plot_data["volume"])
                    ax1.set_xlabel("time")
                    ax2.set_xlabel("time")
                    ax1.set_ylabel(price_col)
                    ax2.set_ylabel("volume")
                    ax1_title += " step chart"
                    ax2_title += " step chart"
                else:
                    ax1.hist(plot_data[price_col], bins="auto")
                    ax1.set_xlabel(price_col)
                    ax1.set_ylabel("count")
                    ax2.set_xlabel("volume")
                    ax2.set_ylabel("count")
                    ax2.hist(plot_data["volume"], bins="auto")
                    ax1_title += " histogram"
                    ax2_title += " histogram"
                ax1.set_title(ax1_title)
                ax2.set_title(ax2_title)

                # 輸出圖表
                if args.output is None:
                    plt.show()
                else:
                    plt.savefig(args.output)
    else:  # batch mode
        input_data.index = range(1, len(input_data) + 1)
        print(input_data)


if __name__ == "__main__":
    main()
