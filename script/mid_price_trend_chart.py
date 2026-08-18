import pandas as pd
import matplotlib.pyplot as plt
import matplotx

sample_data = pd.read_csv("output/test_output_csv")

sample_data["timestamp"] = pd.to_datetime(sample_data["timestamp"], unit="us")

plot_data = sample_data.set_index("timestamp")["mid-price"].resample("1s").last()
plt.rcParams["font.size"] = 10
with plt.style.context(matplotx.styles.pitaya_smoothie["light"]):
    plt.figure(figsize=(19, 10))
    plt.plot(plot_data.index, plot_data.values)
    plt.title("mid-price trend chart")
    plt.xlabel("time")
    plt.ylabel("price")
    plt.show()
