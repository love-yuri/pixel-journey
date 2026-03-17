export module ui.render:svg;

import :base;
import skia.resource;
import skia.api;
import vulkan.context;
import ui.layout;
import yuri_log;
import std;

using namespace skia;
using namespace ui::layout;

export namespace ui::render {

class RenderSvg : public RenderNode {
public:
  /**
   * 从文件中加载svg
   * @param path svg路径
   * @return svg Dom树
   */
  static sk_sp<SkSVGDOM> loadSvg(std::string_view path);

  explicit RenderSvg(std::string_view path);
  void render(SkCanvas *canvas) override;

  // svg 宽度
  [[nodiscard]] float width() const {
    return size_.width();
  }

  // svg 高度
  [[nodiscard]] float height() const {
    return size_.height();
  }

  /**
   * 移动svg到指定
   */
  void move(float x, float y);

private:
  /**
   * 将 SVG 一次性栅格化为离屏 SkImage，后续每帧 blit
   */
  void rasterize();

  std::string file_path;              // 文件路径
  sk_sp<SkSVGDOM> svg_dom = nullptr;  // svg dom
  sk_sp<SkImage> cache_ = nullptr;    // 栅格化缓存
  SkSize size_ = SkSize::MakeEmpty(); // svg尺寸
  SkPoint offset_ = {0.f, 0.f};    // 抽出为成员，方便后续调整
};

RenderSvg::RenderSvg(const std::string_view path) : file_path(path), svg_dom(loadSvg(path)) {
  if (svg_dom) {
    size_ = svg_dom->containerSize();
    rasterize();
    svg_dom.reset();
  }
}

sk_sp<SkSVGDOM> RenderSvg::loadSvg(std::string_view path) {
  SkFILEStream fs{ path.data() };
  constexpr auto decodeType = resources::ImageDecodeStrategy::kLazyDecode;
  const auto fileRes = resources::FileResourceProvider::Make(SkString{path}, decodeType);
  auto dataRes = resources::DataURIResourceProviderProxy::Make(fileRes, decodeType, font::defaultFontMgr);

  return SkSVGDOM::Builder()
    .setFontManager(font::defaultFontMgr)
    .setResourceProvider(std::move(dataRes))
    .setTextShapingFactory(shapers::BestAvailable())
    .make(fs);
}

void RenderSvg::move(const float x, const float y) {
  offset_.set(x, y);
}

void RenderSvg::rasterize() {
  const int w = static_cast<int>(std::ceil(size_.width()));
  const int h = static_cast<int>(std::ceil(size_.height()));
  if (w <= 0 || h <= 0) {
    return;
  }

  auto surface = SkSurfaces::RenderTarget(
    vulkan_context->skia_direct_context.get(),
    gpu::Budgeted::kNo,
    SkImageInfo::MakeN32Premul(w, h)
  );

  if (!surface) {
    return;
  }

  SkCanvas *offscreen = surface->getCanvas();
  offscreen->clear(skia_colors::transparent);
  svg_dom->render(offscreen);

  cache_ = surface->makeImageSnapshot();
}

void RenderSvg::render(SkCanvas *canvas) {
  if (!cache_) {
    return;
  }

  canvas->drawImage(cache_, offset_.x(), offset_.y(),
    SkSamplingOptions{}, nullptr
  );
}

} // namespace ui::render