module;
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
export module yuri.ui.render:svg;

import :base;
import yuri.skia.resource;
import yuri.skia.api;
import yuri.log;
import std;

using namespace skia;

namespace {

// 获取当前可执行文件所在目录
std::filesystem::path executable_directory() {
#ifdef _WIN32
  std::wstring buffer(MAX_PATH, L'\0');
  const auto size = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
  if (size == 0) return {};
  buffer.resize(size);
  return std::filesystem::path(buffer).parent_path();
#else
  std::array<char, 4096> buffer{};
  const auto size = readlink("/proc/self/exe", buffer.data(), buffer.size() - 1);
  if (size <= 0) return {};
  return std::filesystem::path(std::string_view(buffer.data(), static_cast<std::size_t>(size))).parent_path();
#endif
}

// 解析SVG资源路径
std::string resolve_svg_path(const std::string_view path) {
  const auto original_path = std::filesystem::path(path);
  if (std::filesystem::exists(original_path)) {
    return original_path.string();
  }

  const auto exe_dir = executable_directory();
  if (exe_dir.empty()) {
    return std::string(path);
  }

  const auto exe_relative_path = exe_dir / original_path;
  if (std::filesystem::exists(exe_relative_path)) {
    return exe_relative_path.string();
  }

  return std::string(path);
}

} // namespace

export namespace ui::render {

class RenderSvg : public RenderNode {
public:
  using RenderNode::update;

  static sk_sp<SkSVGDOM> loadSvg(std::string_view path);

  explicit RenderSvg(std::string_view path);
  void render(SkCanvas *canvas) override;
  void update() override;

  [[nodiscard]] float width() const {
    return size_.width();
  }

  [[nodiscard]] float height() const {
    return size_.height();
  }

  void move(float x, float y);

private:
  void rasterize();

  std::string file_path{};
  sk_sp<SkSVGDOM> svg_dom = nullptr;
  sk_sp<SkImage> cache_ = nullptr;
  SkSize size_ = SkSize::MakeEmpty();
  SkPoint offset_ = {0.f, 0.f};
};

RenderSvg::RenderSvg(const std::string_view path) : file_path(path), svg_dom(loadSvg(path)) {
  if (svg_dom) {
    size_ = svg_dom->containerSize();
    rasterize();
    svg_dom.reset();
  }
}

sk_sp<SkSVGDOM> RenderSvg::loadSvg(std::string_view path) {
  const auto svg_path = resolve_svg_path(path);
  SkFILEStream fs{svg_path.c_str()};
  if (!fs.isValid()) {
    yuri::error("加载SVG失败，文件不存在或无法打开: {}", svg_path);
    return nullptr;
  }

  constexpr auto decodeType = resources::ImageDecodeStrategy::kLazyDecode;
  const auto fileRes = resources::FileResourceProvider::Make(SkString{svg_path}, decodeType);
  auto dataRes =
    resources::DataURIResourceProviderProxy::Make(fileRes, decodeType, font::defaultFontMgr);

  auto svg = SkSVGDOM::Builder()
    .setFontManager(font::defaultFontMgr)
    .setResourceProvider(std::move(dataRes))
    .setTextShapingFactory(shapers::BestAvailable())
    .make(fs);
  if (!svg) {
    yuri::error("解析SVG失败: {}", svg_path);
  }
  return svg;
}

void RenderSvg::move(const float x, const float y) {
  offset_.set(x, y);
}

void RenderSvg::update() {
  // SVG 内容边界: {0, 0, w, h}
  offset_ = calcAlignedPosition(SkRect::MakeSize(size_));
}

void RenderSvg::rasterize() {
  const int w = static_cast<int>(std::ceil(size_.width()));
  const int h = static_cast<int>(std::ceil(size_.height()));
  if (w <= 0 || h <= 0) return;

  // 使用 CPU 光栅化，避免后台线程与主线程争用 GPU context
  const auto surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(w, h));
  if (!surface) return;

  SkCanvas *offscreen = surface->getCanvas();
  offscreen->clear(skia_colors::transparent);
  svg_dom->render(offscreen);
  cache_ = surface->makeImageSnapshot();
}

void RenderSvg::render(SkCanvas *canvas) {
  if (!cache_) return;
  canvas->drawImage(cache_, offset_.x(), offset_.y(), SkSamplingOptions{}, nullptr);
}

} // namespace ui::render
