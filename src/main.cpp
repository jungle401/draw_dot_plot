// STL
#include <iostream>
#include <fstream>
#include <filesystem>
#include <utility>

// thrid_party
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <fmt/format.h>

// Custom
#include <resizer.hpp>
#include <config.hpp>
#include <utils.hpp>

int main(int argc, char** argv) {
  if (argc != 3 && argc != 8) { std::cout << "[ERROR] arg num error\n"; return -1; }
  if (argc == 3) {
    auto dp_dir = std::filesystem::path(argv[1]);
    auto out_dir = std::filesystem::path(argv[2]);
    // ====== single thread ======
    // for (const auto& dir_entry : std::filesystem::directory_iterator{dp_dir}) {
    //   auto rp = Utils::parse_read_pair(dir_entry.path());
    //   auto dp = DotPlot::get_inst_from_dir(dp_dir, rp.first, rp.second);
    //   Resizer resizer;
    //   resizer.run(out_dir, dp);
    // }
    // ====== single thread ======
    // ====== multi-thread ======
    auto dir_entry_paths = std::vector<std::filesystem::path>();
    for (const auto& dir_entry : std::filesystem::directory_iterator{dp_dir}) {
      dir_entry_paths.push_back(dir_entry.path());
    }
    #pragma omp parallel for
    for (int i = 0; i < dir_entry_paths.size(); i++) {
      auto rp = Utils::parse_read_pair(dir_entry_paths[i]);
      auto dp = DotPlot::get_inst_from_dir(dp_dir, rp.first, rp.second);
      Resizer resizer;
      resizer.run(out_dir, dp);
    }
    // ====== multi-thread ======
  }
  if (argc == 8) {
    auto dp_dir = std::filesystem::path(argv[1]);
    auto out_dir = std::filesystem::path(argv[2]);
    auto r1 = std::stoul(argv[3]);
    auto r2 = std::stoul(argv[4]);
    auto left = std::stoi(argv[5]);
    auto up = std::stoi(argv[6]);
    auto square_width = std::stoi(argv[7]);
    auto dp = DotPlot::get_inst_from_dir(dp_dir, r1, r2);
    Resizer resizer;
    resizer.run(out_dir, dp, square_width, {left, up});
  }
  return 0;
}
