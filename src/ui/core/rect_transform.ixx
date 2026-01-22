//
// Created by yuri on 2026/1/18.
//

export module ui.core:rect_transform;

import skia.api;

using namespace skia;

export namespace ui::core {

class RectTransform {
protected:
  SkRect local_rect_ {0, 0, 0, 0 }; // 几何描述
  float width_ = 0;                   // 控件宽度
  float height_ = 0;                  // 控件高度

public:
  RectTransform() noexcept = default;
  virtual ~RectTransform() noexcept = default;

  /**
   * 获取控件宽度
   */
  [[nodiscard]] float width() const noexcept {
    return width_;
  }

  /**
   * 获取控件高度
   */
  [[nodiscard]] float height() const noexcept {
    return height_;
  }

  /**
   * 设置控件几何状态
   * @param rect 控件所占rect，基于父控件
   */
  inline virtual void setGeometry(const SkRect &rect) noexcept;

  /**
   * 设置控件几何状态
   * @param x 相对于父控件：x
   * @param y 相对于父控件: y
   * @param width 控件宽度
   * @param height 控件高度
   */
  inline virtual void setGeometry(float x, float y, float width, float height) noexcept;

  /**
   * 修改控件的宽高
   * @param width 控件宽度
   * @param height 控件高度
   */
  virtual void resize(float width, float height) noexcept;

  /**
   * 移动控件位置
   * @param x 新的坐标
   * @param y 新的坐标
   */
  inline void move(float x, float y) noexcept;

  /**
   * 移动控件位置
   * @param point 目标点
   */
  inline void move(const SkPoint& point) noexcept;

  /**
   * 重新计算
   */
  virtual void update() {}
};

inline void RectTransform::setGeometry(const SkRect &rect) noexcept {
  this->local_rect_ = rect;
  width_ = rect.width();
  height_ = rect.height();
}

inline void RectTransform::setGeometry(const float x, const float y, const float width, const float height) noexcept {
  width_ = width;
  height_ = height;
  local_rect_.setXYWH(x, y, width, height);
}

inline inline void RectTransform::resize(const float width, const float height) noexcept {
  width_ = width;
  height_ = height;
  local_rect_.setXYWH(local_rect_.x(), local_rect_.y(), width, height);
}

inline void RectTransform::move(const float x, const float y) noexcept {
  local_rect_.setXYWH(x, y, width_, height_);
}

inline void RectTransform::move(const SkPoint &point) noexcept {
  move(point.x(), point.y());
}

} // namespace ui::core