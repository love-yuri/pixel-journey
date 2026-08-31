//
// Created by love-yuri on 2026/4/27.
//

export module yuri.skia.resource:path;

import yuri.skia.api;

export namespace skia::path {

/**
 * 创建心形路径
 * @param cx 中心x坐标
 * @param cy 中心y坐标
 * @param size 尺寸（宽度）
 * @return 心形SkPath
 */
SkPath makeHeartPath(const float cx, const float cy, const float size) {
  const float w = size;
  const float h = size;
  const float left = cx - w * 0.5f;
  const float top = cy - h * 0.5f;
  SkPathBuilder builder;
  builder.moveTo(SkPoint::Make(cx, top + h));
  builder.cubicTo(SkPoint::Make(cx - w * 0.22f, top + h * 0.79f),
                  SkPoint::Make(left, top + h * 0.61f),
                  SkPoint::Make(left, top + h * 0.39f));
  builder.cubicTo(SkPoint::Make(left, top + h * 0.18f),
                  SkPoint::Make(left + w * 0.14f, top),
                  SkPoint::Make(left + w * 0.32f, top));
  builder.cubicTo(SkPoint::Make(left + w * 0.41f, top),
                  SkPoint::Make(cx, top + h * 0.06f),
                  SkPoint::Make(cx, top + h * 0.14f));
  builder.cubicTo(SkPoint::Make(cx, top + h * 0.06f),
                  SkPoint::Make(left + w * 0.59f, top),
                  SkPoint::Make(left + w * 0.68f, top));
  builder.cubicTo(SkPoint::Make(left + w * 0.86f, top),
                  SkPoint::Make(left + w, top + h * 0.18f),
                  SkPoint::Make(left + w, top + h * 0.39f));
  builder.cubicTo(SkPoint::Make(left + w, top + h * 0.61f),
                  SkPoint::Make(cx + w * 0.22f, top + h * 0.79f),
                  SkPoint::Make(cx, top + h));
  builder.close();
  return builder.detach();
}

}
