#pragma once
#include <iostream>
#include <fstream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <filesystem>
#include <DotPlot.hpp>
#include <fmt/format.h>

class Resizer {
private:
  int res_img_len;
  int dpr1;
  int dpr2;
  int dot_plot_long_len;
  std::pair<int, int> left_up;
  int select_size;

  void config (
    int res_img_len,
    const DotPlot& dot_plot,
    std::pair<int, int> left_up
  ) {
    this->res_img_len = res_img_len;
    this->dpr1 = dot_plot.r1;
    this->dpr2 = dot_plot.r2;
    // this->dot_plot_long_len = (dot_plot.len1 > dot_plot.len2) ? dot_plot.len1 : dot_plot.len2;
    this->left_up = left_up;
    this->select_size = res_img_len;
  }

  void config (
    const DotPlot& dot_plot
  ) {
    this->res_img_len = 3000;
    this->dpr1 = dot_plot.r1;
    this->dpr2 = dot_plot.r2;
    this->dot_plot_long_len = (dot_plot.len1 > dot_plot.len2) ? dot_plot.len1 : dot_plot.len2;
    this->left_up = {0,0};
    // this->select_size = select_size;
    this->select_size = dot_plot_long_len;
    // std::cout << fmt::format("read {} len: {}, read {} len: {}\n", dpr1, dot_plot.len1, dpr2, dot_plot.len2);
  }

  std::pair<int, int> shrink(int coef1, int coef2) {
    int res1 = coef1 - left_up.first;
    res1 = res1 * res_img_len / select_size;
    int res2 = coef2 - left_up.second;
    res2 = res2 * res_img_len / select_size;
    return {res1, res2};
  }

  bool check_valid (int res_img_len, const DotPlot& dot_plot) {

    if (dot_plot_long_len < res_img_len) {
      std::cout << fmt::format("dot_plot_long_len[ {}] < res_img_len[ {}]\n", dot_plot_long_len, res_img_len);
      std::cout << "no need to resize image sample. ";
      std::cout << fmt::format("read pair: [{}, {}]", dot_plot.r1, dot_plot.r2);
      return false;
    }
    return true;
  }

  bool in_range(const Dot& dot) {
    if (dot.first <= left_up.first || dot.first >= left_up.first + select_size) {
      return false;
    }
    if (dot.second <= left_up.second || dot.second >= left_up.second + select_size) {
      return false;
    }
    return true;
  }

  inline void enlarge (const std::pair<int, int>& p, int magnitude, int x_st, int y_st, cv::Mat& binMat) {
    // if (p.first - x_st > magnitude && p.second - y_st > magnitude && res_img_len - p.first + x_st > magnitude && res_img_len - p.second + y_st) {
      for (int i = -magnitude + 1; i < magnitude; i++) {
        if (p.first - x_st + i < 0 || p.first - x_st + i >= res_img_len) {continue;}
        for (int j = -magnitude + 1; j < magnitude; j++) {
          if (p.second - y_st + j < 0 || p.second - y_st + j > res_img_len) { continue; }
          binMat.at<uint8_t>(p.first - x_st + i, p.second - y_st + j) = 255;
        }
      }
    // }
  };

  inline void raw_dot (const std::pair<int, int>& p, int x_st, int y_st, cv::Mat& binMat) {
    binMat.at<uint8_t>(p.first - x_st, p.second - y_st) = 255;
  }

  void draw(cv::Mat& binMat, DotPlot& dot_plot, int magnitude, bool elg) {
    size_t x_st = 0, y_st = 0;
    for (auto& p : dot_plot.dots) {
      if (!in_range(p)) {
        continue;
      }
      p = shrink(p.first, p.second);
      // std::cout << fmt::format("{}, {}", p.first, p.second) << std::endl;

      if (p.first > x_st && p.first < x_st + res_img_len - 1 && p.second > y_st && p.second < y_st + res_img_len + 1) {
        // auto no_enlarge = [&] () {
        //   binMat.at<uint8_t>(p.first - x_st, p.second - y_st) = 255;
        // };
        if (elg) {
          enlarge(p, magnitude, x_st, y_st, binMat);
        } else {
          raw_dot(p, x_st, y_st, binMat);
          // no_enlarge();
        }
      }
    }
    // boarder
    for (int x = 0; x < dot_plot.len1; x++) {
      auto p = std::pair<int, int>{x, dot_plot.len2};
      p = shrink(p.first, p.second);
      // if (p.first > x_st && p.first < x_st + res_img_len - 1 && p.second > y_st && p.second < y_st + res_img_len + 1) {
      enlarge(p, magnitude, x_st, y_st, binMat);
      // }
    }
    for (int y = 0; y < dot_plot.len2; y++) {
      auto p = std::pair<int, int>{dot_plot.len1, y};
      p = shrink(p.first, p.second);
      // if (p.first > x_st && p.first < x_st + res_img_len - 1 && p.second > y_st && p.second < y_st + res_img_len + 1) {
      enlarge(p, magnitude, x_st, y_st, binMat);
      // }
    }
  }

public:
  void run(
    const std::filesystem::path& out_dir,
    DotPlot& dot_plot
  ) {
    if (dot_plot.dots.size() < 10) {
      return;
    }
    config(dot_plot);
    // if (!check_valid(res_img_len, dot_plot)) { return; }
    auto out_file = out_dir / fmt::format("compressed_{}_{}.bmp", dot_plot.r1, dot_plot.r2);
    std::filesystem::create_directories(out_dir);
    cv::Mat binMat = cv::Mat::zeros(res_img_len, res_img_len, CV_8UC1);
    draw(binMat, dot_plot, 5, true);
    imwrite(out_file, binMat);
  }

  void run(
    const std::filesystem::path& out_dir,
    DotPlot& dot_plot,
    int res_img_len,
    const std::pair<int, int>& left_up
  ) {
    config(res_img_len, dot_plot, left_up);
    // if (!check_valid(res_img_len, dot_plot)) { return; }
    auto out_file = out_dir / fmt::format("compressed_{}_{}.bmp", dot_plot.r1, dot_plot.r2);
    cv::Mat binMat = cv::Mat::zeros(res_img_len, res_img_len, CV_8UC1);
    draw(binMat, dot_plot, 8, false);
    imwrite(out_file, binMat);
  }
};
