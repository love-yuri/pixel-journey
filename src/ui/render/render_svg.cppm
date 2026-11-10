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

/**
 * 获取当前可执行文件所在目录。
 * @return 可执行文件目录，获取失败时为空路径
 */
std::filesystem::path executable_directory();

/**
 * 解析 SVG 资源路径，依次尝试原始路径和相对于可执行文件的路径。
 * @param path 原始资源路径
 * @return 解析后的资源路径，未找到时返回原始路径
 */
std::string resolve_svg_path(std::string_view path);

std::filesystem::path executable_directory() {
#ifdef _WIN32
  std::wstring buffer(MAX_PATH, L'\0');
  const auto size = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
  if (size == 0) {
    return {};
  }
  buffer.resize(size);
  return std::filesystem::path(buffer).parent_path();
#else
  std::array<char, 4096> buffer{};
  const auto size = readlink("/proc/self/exe", buffer.data(), buffer.size() - 1);
  if (size <= 0) {
    return {};
  }
  return std::filesystem::path(
    std::string_view(buffer.data(), static_cast<std::size_t>(size))
  ).parent_path();
#endif
}

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

/**
 * 加载 SVG 文档，并配置字体、外部资源和文本整形。
 * @param path SVG 资源路径
 * @return 加载的 SVG 文档，读取或解析失败时为空指针
 */
[[nodiscard]] sk_sp<SkSVGDOM> load_svg(std::string_view path);

/** 将 SVG 光栅化为图片缓存，并在指定区域内按原始尺寸对齐绘制的节点。 */
class RenderSvg final : public RenderNode {
  std::string file_path;                      // SVG 资源路径
  sk_sp<SkSVGDOM> svg_dom = nullptr;          // 光栅化前使用的 SVG 文档
  sk_sp<SkImage> cache_ = nullptr;            // CPU 光栅化生成的图片缓存
  SkSize size_ = SkSize::MakeEmpty();         // SVG 原始尺寸
  SkPoint offset_ = { .fX = 0.f, .fY = 0.f }; // 图片绘制原点
  Alignment alignment_{};                     // 内容对齐方式

  /** 按 SVG 原始尺寸生成 CPU 图片缓存。 */
  void rasterize();

public:
  /**
   * 加载 SVG 并生成绘制缓存。
   * @param path SVG 资源路径
   */
  explicit RenderSvg(std::string_view path);

  /**
   * 绘制已有图片缓存，不重新加载或光栅化 SVG。
   * @param canvas 目标画布
   */
  void draw(SkCanvas *canvas) override;

  /**
   * 更新绘制区域，并按原始尺寸和对齐方式计算绘制原点。
   * @param rect 新的绘制区域
   */
  void update(SkRect rect) override;

  /**
   * 获取 SVG 原始宽度。
   * @return 原始宽度
   */
  [[nodiscard]] float width() const {
    return size_.width();
  }

  /**
   * 获取 SVG 原始高度。
   * @return 原始高度
   */
  [[nodiscard]] float height() const {
    return size_.height();
  }

  /**
   * 设置内容对齐方式，并更新绘制原点。
   * @param alignment 水平与垂直对齐方式
   */
  void set_alignment(Alignment alignment);

  /**
   * 直接设置图片绘制原点，下次更新区域或对齐方式时重新计算。
   * @param x 绘制原点的 x 坐标
   * @param y 绘制原点的 y 坐标
   */
  void move(float x, float y);
};

RenderSvg::RenderSvg(const std::string_view path) : file_path(path), svg_dom(load_svg(path)) {
  if (svg_dom) {
    size_ = svg_dom->containerSize();
    rasterize();
    svg_dom.reset();
  }
  update(self_box);
}

sk_sp<SkSVGDOM> load_svg(const std::string_view path) {
  const auto svg_path = resolve_svg_path(path);
  SkFILEStream stream{svg_path.c_str()};
  if (!stream.isValid()) {
    yuri::error("加载SVG失败，文件不存在或无法打开: {}", svg_path);
    return nullptr;
  }

  constexpr auto kDecodeType = resources::ImageDecodeStrategy::kLazyDecode; // 图片解码策略
  const auto file_res = resources::FileResourceProvider::Make(SkString{svg_path}, kDecodeType);
  auto data_res =
    resources::DataURIResourceProviderProxy::Make(file_res, kDecodeType, font::defaultFontMgr);

  auto svg = SkSVGDOM::Builder()
    .setFontManager(font::defaultFontMgr)
    .setResourceProvider(std::move(data_res))
    .setTextShapingFactory(shapers::BestAvailable())
    .make(stream);
  if (!svg) {
    yuri::error("解析SVG失败: {}", svg_path);
  }
  return svg;
}

void RenderSvg::set_alignment(const Alignment alignment) {
  if (alignment_ == alignment) {
    return;
  }
  alignment_ = alignment;
  update(self_box);
}

void RenderSvg::move(const float x, const float y) {
  offset_.set(x, y);
}

void RenderSvg::update(const SkRect rect) {
  self_box = rect;
  offset_ = alignment_.calc_aligned_position(self_box, SkRect::MakeSize(size_));
}

void RenderSvg::rasterize() {
  const int width = static_cast<int>(std::ceil(size_.width()));
  const int height = static_cast<int>(std::ceil(size_.height()));
  if (width <= 0 || height <= 0) {
    return;
  }

  // 使用 CPU 光栅化，避免后台线程与主线程争用 GPU context。
  const auto surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(width, height));
  if (!surface) {
    return;
  }

  SkCanvas *canvas = surface->getCanvas();
  canvas->clear(skia_colors::transparent);
  svg_dom->render(canvas);
  cache_ = surface->makeImageSnapshot();
}

void RenderSvg::draw(SkCanvas *canvas) {
  if (!visible || !cache_) {
    return;
  }
  canvas->drawImage(cache_, offset_.x(), offset_.y(), SkSamplingOptions{}, nullptr);
}

} // namespace ui::render
