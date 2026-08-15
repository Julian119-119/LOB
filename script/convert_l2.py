import pandas as pd

sample_data = pd.read_csv(
    "sample_data/raw/deribit_incremental_book_L2_2020-04-01_BTC-PERPETUAL.csv", nrows=5000
)

sample_data.drop(columns=["exchange", "symbol", "local_timestamp"], inplace=True)
sample_data.rename(columns={"amount": "volume"}, inplace=True)
sample_data = sample_data[["timestamp", "side", "price", "volume", "is_snapshot"]]
sample_data["is_snapshot"] = sample_data["is_snapshot"].astype(str).str.lower()

sample_data.to_csv("sample_data/processed/convertion_l2_data.csv", index=False)
