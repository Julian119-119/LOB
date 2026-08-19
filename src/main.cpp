#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <optional>

#include "LOB_L2_type.hpp"

struct CLIArgs {
  std::string input_path, output_path, mode, query;
  std::optional<uint64_t> stop_at;
  size_t tar_k = 0;
  double tar_price = 0.0;
};

void print_usage() {
  std::cerr << "usage: main --input <path> --mode <streaming|batch> [--output "
               "<path>] [--stop-at <ts>] ...\n";
}

/* 收集參數 */
CLIArgs parse_args(int argc, char* argv[]) {
  CLIArgs args;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "-i" || arg == "--input") {
      if (i + 1 < argc)
        args.input_path = argv[++i];
      else {
        std::cerr
            << "error: --input expects a valid path for the target file\n";
        print_usage();
        std::exit(1);
      }
    } else if (arg == "-o" || arg == "--output") {
      if (i + 1 < argc)
        args.output_path = argv[++i];
      else {
        std::cerr
            << "error: --output expects a valid path for the output file\n";
        print_usage();
        std::exit(1);
      }
    } else if (arg == "--mode" || arg == "-m") {
      if (i + 1 < argc)
        args.mode = argv[++i];
      else {
        std::cerr << "error: --mode expects a valid mode\n";
        print_usage();
        std::exit(1);
      }
    } else if (arg == "--query") {
      if (i + 1 < argc)
        args.query = argv[++i];
      else {
        std::cerr << "error: --query expects a valid query\n";
        print_usage();
        std::exit(1);
      }
      if (args.query == "volume-at-bid-price" ||
          args.query == "volume-at-ask-price") {
        size_t pos;
        try {
          if (i + 1 < argc)
            args.tar_price = std::stod(argv[++i], &pos);
          else {
            std::cerr << "error: --volume-at-price expects a non-negative "
                         "decimal number\n";
            print_usage();
            std::exit(1);
          }
        } catch (const std::invalid_argument&) {
          std::cerr << "error: --volume-at-price expects a non-negative "
                       "decimal number, got: "
                    << argv[i] << '\n';
          std::exit(1);
        } catch (const std::out_of_range&) {
          std::cerr << "error: --volume-at-price value too large: " << argv[i]
                    << '\n';
          std::exit(1);
        }
        if (args.tar_price < 0 || pos != std::string(argv[i]).size()) {
          std::cerr << "error: --volume-at-price expects a non-negative "
                       "decimal number, got: "
                    << argv[i] << '\n';
          std::exit(1);
        }
      }
      if (args.query == "bid-top-k" || args.query == "ask-top-k") {
        try {
          if (i + 1 < argc)
            args.tar_k = std::stoull(argv[++i]);
          else {
            std::cerr << "error: --top-k expects a non-negative integer\n";
            print_usage();
            std::exit(1);
          }
        } catch (const std::invalid_argument&) {
          std::cerr << "error: --top-k expects a non-negative integer, got: "
                    << argv[i] << '\n';
          std::exit(1);
        } catch (const std::out_of_range&) {
          std::cerr << "error: --top-k value too large: " << argv[i] << '\n';
          std::exit(1);
        }
      }
    } else if (arg == "--stop-at") {
      try {
        if (i + 1 < argc)
          args.stop_at = std::stoull(argv[++i]);
        else {
          std::cerr << "error: --stop-at expects a non-negative integer\n";
          print_usage();
          std::exit(1);
        }
      } catch (const std::invalid_argument&) {
        std::cerr << "error: --stop-at expects a non-negative integer, got: "
                  << argv[i] << '\n';
        std::exit(1);
      } catch (const std::out_of_range&) {
        std::cerr << "error: --stop-at value too large: " << argv[i] << '\n';
        std::exit(1);
      }
    } else {
      std::cerr << "invalid parameter: " << arg << '\n';
      std::exit(1);
    }
  }

  return args;
}

/* 判斷參數是否合法 */
void valid_args(const CLIArgs& args) {
  // 檢查 path
  if (args.input_path.empty()) {
    std::cerr << "error: missing required argument: --input\n";
    print_usage();
    std::exit(1);
  }
  if (args.output_path.empty()) {
    std::cerr << "error: missing required argument: --output\n";
    print_usage();
    std::exit(1);
  }
  // 檢查 mode
  if (args.mode.empty()) {
    std::cerr << "error: missing required argument: --mode\n";
    print_usage();
    std::exit(1);
  }
  if (args.mode != "streaming" && args.mode != "batch") {
    std::cerr << "error: invalid mode\n";
    print_usage();
    std::exit(1);
  }
  // 檢查 query
  if (args.query.empty()) {
    std::cerr << "error: missing required argument: --query\n";
    print_usage();
    std::exit(1);
  }
  if (args.query != "mid-price" && args.query != "best-bid" &&
      args.query != "best-ask" && args.query != "spread" &&
      args.query != "volume-at-bid-price" && args.query != "volume-at-ask-price" &&
      args.query != "bid-top-k" && args.query != "ask-top-k") {
    std::cerr << "error: invalid query: " << args.query << '\n';
    std::cerr
        << "valid query: mid-price, best-bid, best-ask, spread, "
           "volume-at-bid-price, volume-at-ask-price, bid-top-k, ask-top-k\n";
    print_usage();
    std::exit(1);
  }
  // 檢查 mode 與 query 的搭配是否合法
  if (args.mode == "streaming" &&
      (args.query == "bid-top-k" || args.query == "ask-top-k")) {
    std::cerr << "error: can't use bid-top-k and ask-top-k in streaming mode\n";
    print_usage();
    std::exit(1);
  }
}

/* make_writer：依照 --query 建造需要的 writer */
std::function<void(const L2_LOB& this_book, const Rows_data&)> make_writer(
    const CLIArgs& args, std::fstream& fout, bool& is_first) {
  if (args.query == "mid-price") {
    return [&fout, &is_first](const L2_LOB& this_book,
                              const Rows_data& curr_row) {
      if (is_first) {
        is_first = false;
        fout << "timestamp,mid-price\n";
      }
      fout << curr_row.timestamp << ',' << this_book.get_mid_price() << '\n';
    };
  } else if (args.query == "best-bid") {
    return
        [&fout, &is_first](const L2_LOB& this_book, const Rows_data& curr_row) {
          if (is_first) {
            is_first = false;
            fout << "timestamp,best-bid,volume\n";
          }
          fout << curr_row.timestamp << ',' << this_book.get_best_bid_price()
               << ',' << this_book.get_best_bid_volume() << '\n';
        };
  } else if (args.query == "best-ask") {
    return
        [&fout, &is_first](const L2_LOB& this_book, const Rows_data& curr_row) {
          if (is_first) {
            is_first = false;
            fout << "timestamp,best-ask,volume\n";
          }
          fout << curr_row.timestamp << ',' << this_book.get_best_ask_price()
               << ',' << this_book.get_best_ask_volume() << '\n';
        };
  } else if (args.query == "spread") {
    return
        [&fout, &is_first](const L2_LOB& this_book, const Rows_data& curr_row) {
          if (is_first) {
            is_first = false;
            fout << "timestamp,spread\n";
          }
          fout << curr_row.timestamp << ',' << this_book.get_spread() << '\n';
        };
  } else if (args.query == "volume-at-bid-price") {
    return [&fout, &is_first, &args](const L2_LOB& this_book,
                                     const Rows_data& curr_row) {
      if (is_first) {
        is_first = false;
        fout << "timestamp,side,price,volume\n";
      }
      fout << curr_row.timestamp << ",bid," << args.tar_price << ','
           << this_book.get_volume_at_price(args.tar_price, Side::BUY) << '\n';
    };
  } else if (args.query == "volume-at-ask-price") {
    return [&fout, &is_first, &args](const L2_LOB& this_book,
                                     const Rows_data& curr_row) {
      if (is_first) {
        is_first = false;
        fout << "timestamp,side,price,volume\n";
      }
      fout << curr_row.timestamp << ",ask," << args.tar_price << ','
           << this_book.get_volume_at_price(args.tar_price, Side::SELL) << '\n';
    };
  } else {
    std::cerr << "make writer wrong\n";
    std::exit(1);
  }
}

void run_batch_output(const L2_LOB& this_book, const CLIArgs& args,
                      std::fstream& fout, bool& is_first) {
  if (args.query == "mid-price") {
    if (is_first) {
      is_first = false;
      fout << "mid-price\n";
    }
    fout << this_book.get_mid_price() << '\n';
  } else if (args.query == "best-bid") {
    if (is_first) {
      is_first = false;
      fout << "best-bid,volume\n";
    }
    fout << this_book.get_best_bid_price() << ','
         << this_book.get_best_bid_volume() << '\n';
  } else if (args.query == "best-ask") {
    if (is_first) {
      is_first = false;
      fout << "best-ask,volume\n";
    }
    fout << this_book.get_best_ask_price() << ','
         << this_book.get_best_ask_volume() << '\n';
  } else if (args.query == "spread") {
    if (is_first) {
      is_first = false;
      fout << "spread\n";
    }
    fout << this_book.get_spread() << '\n';
  } else if (args.query == "volume-at-bid-price") {
    if (is_first) {
      is_first = false;
      fout << "side,price,volume\n";
    }
    fout << "bid," << args.tar_price << ','
         << this_book.get_volume_at_price(args.tar_price, Side::BUY) << '\n';
  } else if (args.query == "volume-at-ask-price") {
    if (is_first) {
      is_first = false;
      fout << "side,price,volume\n";
    }
    fout << "ask," << args.tar_price << ','
         << this_book.get_volume_at_price(args.tar_price, Side::SELL) << '\n';
  } else if (args.query == "bid-top-k") {
    if (is_first) {
      is_first = false;
      fout << "side,price,volume\n";
    }
    for (const PriceLevelInfo& pli :
         this_book.get_top_k_info(args.tar_k, Side::BUY)) {
      fout << "bid," << pli.price << ',' << pli.volume << '\n';
    }
  } else if (args.query == "ask-top-k") {
    if (is_first) {
      is_first = false;
      fout << "side,price,volume\n";
    }
    for (const PriceLevelInfo& pli :
         this_book.get_top_k_info(args.tar_k, Side::SELL)) {
      fout << "ask," << pli.price << ',' << pli.volume << '\n';
    }
  } else {
    std::cerr << "run-batch-output wrong\n";
    std::exit(1);
  }
}

int main(int argc, char* argv[]) {
  L2_LOB book;
  CLIArgs args = parse_args(argc, argv);  // 收集參數
  valid_args(args);                       // 驗證參數
  std::fstream fout;
  bool is_first = true;

  /* 依據參數執行 L2_LOB 的 function */
  fout.open(args.output_path, std::ios::out);
  if (!fout.is_open()) {
    std::cerr << "invalid output path: " << args.output_path << '\n';
    std::exit(1);
  }

  try {
    if (args.mode == "streaming") {
      auto CSV_writer = make_writer(args, fout, is_first);
      book.load_CSV_snapshot(args.input_path, CSV_writer, args.stop_at);
    } else if (args.mode == "batch") {
      book.load_CSV_snapshot(args.input_path, nullptr, args.stop_at);
      run_batch_output(book, args, fout, is_first);
    }
  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << '\n';
    std::exit(1);
  }

  return 0;
}