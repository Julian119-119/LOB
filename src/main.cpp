#include <fstream>
#include <iostream>
#include <string>

#include "LOB_L2_type.hpp"

int main() {
  L2_LOB book;
  std::string output_csv = "output/test_output_csv";
  std::string input_csv = "sample_data/processed/convertion_l2_data.csv";

  std::fstream fout;
  fout.open(output_csv, std::ios::out);
  if (!fout.is_open()) {
    std::cerr << "failed to open: " << output_csv << '\n';
    return 1;
  }

  /* lambda */
  bool is_first = true;
  std::function<void(const L2_LOB& this_book, const Rows_data&)> CSV_writer =
      [&fout, &is_first](const L2_LOB& this_book, const Rows_data& curr_row) {
        if (is_first) {
          is_first = false;
          fout << "timestamp,mid-price\n";
        }
        fout << curr_row.timestamp << ',' << this_book.get_mid_price() << '\n';
      };

  book.load_CSV_snapshot(input_csv, CSV_writer);
}