#pragma once
#include <iostream>
#include <filesystem>
class Utils {
public:
  static std::pair<uint32_t, uint32_t> parse_read_pair(const std::filesystem::path& dot_plot) {
    /**
      * Parse the read pair from the file name. The parsed should suffice
      * the special file name format used in this study. The format is
      * should be in from of [A-Za-z0-9]+_[0-9]+_[0-9]+\.[A-Za-z0-9]
      *
      * @param dot_plot the to-be-parsed file name of dot plot.
      * @return the read pair infered from the file name.
      */
    auto file_name = std::string(dot_plot.stem());
    auto dlim1_pos = file_name.find('_');
    auto deducted = file_name;
    deducted = deducted.substr(dlim1_pos + 1);
    // std::cout << deducted << std::endl;
    auto dlim2_pos = deducted.find('_');
    auto r1 = std::stoul(deducted.substr(0, dlim2_pos));
    // std::cout << r1 << std::endl;
    deducted = deducted.substr(dlim2_pos + 1);
    auto r2 = std::stoul(deducted);
    // std::cout << r2 << std::endl;
    return {r1, r2};
  }
};
