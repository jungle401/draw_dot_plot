#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <cassert>
#include <filesystem>

enum class EStrand {
  FORWARD,
  REVERSE
};

using Dot = std::pair<uint32_t, uint32_t>;
using Dots = std::vector<Dot>;

class DotPlot {
 public:
  DotPlot() {
  }
  /* DotPlot(const DotPlot& dot_plot) */ 
  /*   : r1(dot_plot.r1), r2(dot_plot.r2), e_strand(dot_plot.e_strand), len1(dot_plot.len1), len2(dot_plot.len2), dots(dot_plot.dots) { */
  /*   } */
  DotPlot(uint32_t r1_, uint32_t r2_, EStrand e_strand_, uint32_t len1_, uint32_t len2_)
    : r1(r1_), r2(r2_), e_strand(e_strand_), len1(len1_), len2(len2_) {}
  uint32_t r1;
  uint32_t r2;
  EStrand e_strand;
  uint32_t len1;
  uint32_t len2;
  Dots dots;

  static void get_from_dir_dps(const std::filesystem::path& dir, std::vector<DotPlot>& dps, EStrand e_strand) {
    // std::cout << "get dot plots from dir\t";
    // std::cout << dir << std::endl;
    for (auto const& dir_entry : std::filesystem::directory_iterator{dir}) 
    {
      auto file = dir_entry.path().filename();
      auto get_string_after_delim = [] (const std::string& str,const char& delim) {
        auto pos = str.find(delim);
        return str.substr(pos + 1);
      };
      auto get_num_before_delim = [] (const std::string& str, const char& delim) {
        auto pos = str.find(delim);
        return std::stoul(str.substr(0, pos));
      };
      auto trimed = get_string_after_delim(file, '_');
      auto r1id = get_num_before_delim(trimed, '_');
      trimed = get_string_after_delim(trimed, '_');
      auto r2id = get_num_before_delim(trimed, '_');
      auto fin = std::ifstream{dir_entry.path()};
      assert(fin.is_open());
      auto str = std::string();
      std::getline(fin, str, 'x');
      auto dot_plot = DotPlot();
      dot_plot.r1 = r1id;
      dot_plot.r2 = r2id;
      dot_plot.e_strand = e_strand;
      dot_plot.len1 = std::stoul(str);
      std::getline(fin, str);
      dot_plot.len2 = std::stoul(str);
      while (std::getline(fin, str, ',')) {
        auto dot = std::pair<uint32_t, uint32_t>();
        dot.first = std::stoul(str);
        std::getline(fin, str);
        dot.second = std::stoul(str);
        dot_plot.dots.push_back(dot);
      }
      dps.emplace_back(std::move(dot_plot));
    }
  }

  static void log_dot_plot(const DotPlot& dp) {
    std::cout << "dot_plot instance" << std::endl;
    printf("read pair [%d, %d]\n", dp.r1, dp.r2);
    printf("dp size[%d, %d]\n", dp.len1, dp.len2);
    /* std::cout << "strand" << dp.e_strand << '\n'; */
    std::cout << "dots: ";
    auto t = int(3);
    for (const auto& d : dp.dots) {
      std::cout << d.first << ',';
      std::cout << d.second << "  ";
      if (t-- < 0) {
        break;
      }
    }
    printf(" ... (%ld)\n", dp.dots.size());
    std::cout << " ------------------ \n";
  }

  static void log_dot_plot_list(const std::vector<DotPlot>& dot_plots, int amount) {
    if (amount > 10) {
      std::cout << "too much dot_plots logged, exit." << std::endl;
      exit(0);
    }
    for (const auto& dp : dot_plots) {
      if (amount-- == 0) { break; }
      log_dot_plot(dp);
    }
  }

  static void write_dot_plots(
      const std::vector<DotPlot>& dot_plots, 
      const std::filesystem::path& out_dir) {
    for (const auto& dp : dot_plots) {
      auto fout = std::ofstream{out_dir / ("DotPlot_" + std::to_string(dp.r1) +
          "_" + std::to_string(dp.r2) + "_" +
          (dp.e_strand == EStrand::FORWARD ? "fw" : "rc") + ".dpt")};
      assert(fout.is_open());
      fout << dp.len1 << 'x' << dp.len2 << '\n';
      for (const auto& dot : dp.dots) {
        fout << dot.first << ',';
        fout << dot.second << '\n';
      }
    }
  }

  static DotPlot& get_inst (std::vector<DotPlot>& dot_plots, uint32_t r1, uint32_t r2) {
    auto f = std::find_if(dot_plots.begin(), dot_plots.end(), [&r1, &r2] (DotPlot& dot_plot) {
        return dot_plot.r1 == r1 && dot_plot.r2 == r2;
        });
    if (f == dot_plots.end()) {
      std::cerr << "get dot_plot: instance not found, exit." << std::endl;
      exit(0);
    }
    return *f;
  }

  static DotPlot get_inst_from_dir (const std::filesystem::path& dir_dot_plots, const auto& r1, const auto& r2) {
      auto dot_plots = std::vector<DotPlot>();
      // std::cout << dir_dot_plots << std::endl;
      get_from_dir_dps(dir_dot_plots, dot_plots, EStrand::FORWARD);
      return get_inst(dot_plots, r1, r2);
  }
};
