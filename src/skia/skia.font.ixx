//
// Created by love-yuri on 2026/1/12.
//
module;
#include <include/core/SkFontMgr.h>
#if defined(_WIN32)
#include "include/ports/SkTypeface_win.h"
#else
#include "include/ports/SkFontScanner_FreeType.h"
#include "include/ports/SkFontMgr_fontconfig.h"
#include <include/core/SkTypeface.h>
#endif

export module skia.font;

import yuri_log;

#if defined(_WIN32)
sk_sp<SkFontMgr> fontMgr = SkFontMgr_New_DirectWrite();
#else
sk_sp<SkFontMgr> fontMgr = SkFontMgr_New_FontConfig(nullptr, SkFontScanner_Make_FreeType());
#endif

export namespace skia::font {

#if defined(_WIN32)
constexpr auto default_font_path = R"(E:\love-yuri\journal-kmp\composeApp\src\commonMain\composeResources\font\MapleMono-NF-CN-Medium.ttf)";
#else
constexpr auto default_font_path = "/usr/share/fonts/TTF/FiraCode-Medium.ttf";
#endif

/**
 * 从文件中加载字体
 * @param path 字体文件路径
 * @return sk_sp<SkTypeface>
 */
sk_sp<SkTypeface> load_from_file(const std::string_view path) {
  auto typeface = fontMgr->makeFromFile(path.data(), 0);
  if (!typeface) {
    yuri::error("typeface is null! 字体： {} 加载失败!", path);
    return nullptr;
  }
  return typeface;
}

}



