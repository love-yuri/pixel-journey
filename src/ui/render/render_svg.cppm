//
// Created by yuri on 2026/2/8.
//

export module ui.render:svg;

import :base;
import skia.resource;
import ui.layout;
import yuri_log;
import std;

using namespace skia;
using namespace ui::layout;

export namespace ui::render {

class RenderSvg : public RenderNode {
public:
  /**
   * 从文件中加载svg，使用默认文字管理
   * @param path svg路径
   * @return svg dom
   */
  static sk_sp<SkSVGDOM> loadSvg(std::string_view path);

  explicit RenderSvg(std::string_view path);
  void render(SkCanvas *canvas) override;

  // svg宽度
  [[nodiscard]] float width() const;

  // svg高度
  [[nodiscard]] float height() const;

private:
  std::string file_path;             // svg 路径
  sk_sp<SkSVGDOM> svg_dom = nullptr; // svg dom
};

RenderSvg::RenderSvg(const std::string_view path) :file_path(path), svg_dom(loadSvg(path)) {
}

sk_sp<SkSVGDOM> RenderSvg::loadSvg(std::string_view path) {
  SkFILEStream fs{ path.data() };
  constexpr auto decodeType = resources::ImageDecodeStrategy::kLazyDecode;
  const auto fileResProvider = resources::FileResourceProvider::Make(SkString{ path }, decodeType);
  auto dataResProvider = resources::DataURIResourceProviderProxy::Make(fileResProvider, decodeType, font::defaultFontMgr);
  auto svgDom = SkSVGDOM::Builder()
      .setFontManager(font::defaultFontMgr)
      .setResourceProvider(std::move(dataResProvider))
      .setTextShapingFactory(shapers::BestAvailable())
      .make(fs);

  return std::move(svgDom);
}

void RenderSvg::render(SkCanvas *canvas) {
  canvas->save();
  canvas->translate(290, 80);
  svg_dom->render(canvas);
  canvas->restore();
}

float RenderSvg::width() const {
  return svg_dom->containerSize().width();
}

float RenderSvg::height() const {
  return svg_dom->containerSize().height();
}

} // namespace ui::render