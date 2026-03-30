//
// Created by love-yuri on 2026/3/30.
//

export module core.utils:uuid;

import std;

namespace utils::uuid {

/**
 * 生成一个uuid
 * @return 36位uuid
 */
export [[nodiscard]] std::string generate() {
  // clang-format off
  thread_local std::mt19937_64 rng {
    std::random_device{}() ^
    static_cast<std::uint64_t>(
      std::chrono::high_resolution_clock::now().time_since_epoch().count()
    )
  };
  // clang-format on
  std::uniform_int_distribution<std::uint32_t> dist(0, 15);
  std::uniform_int_distribution<std::uint32_t> dist2(8, 11);

  static constexpr std::string_view hex = "0123456789abcdef";
  static constexpr std::string_view tmpl = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";

  std::string result;
  result.reserve(36);

  for (const char c : tmpl) {
    if (c == 'x')
      result += hex[dist(rng)];
    else if (c == 'y')
      result += hex[dist2(rng)];
    else
      result += c;
  }

  return result;
}

} // namespace utils::uuid
