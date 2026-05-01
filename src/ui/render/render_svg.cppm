export module ui.render:svg;

import :base;
import skia.resource;
import skia.api;
import std;

using namespace skia;

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
  SkFILEStream fs{path.data()};
  constexpr auto decodeType = resources::ImageDecodeStrategy::kLazyDecode;
  const auto fileRes = resources::FileResourceProvider::Make(SkString{path}, decodeType);
  auto dataRes =
    resources::DataURIResourceProviderProxy::Make(fileRes, decodeType, font::defaultFontMgr);

  return SkSVGDOM::Builder()
    .setFontManager(font::defaultFontMgr)
    .setResourceProvider(std::move(dataRes))
    .setTextShapingFactory(shapers::BestAvailable())
    .make(fs);
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
