//
// Created by yuri on 2026/1/15.
//

export module utils.math;

export namespace math {

/**
 * 对a / b的结果做向上取整
 */
int ceil(const int a, const int b) {
  return (a + b - 1) / b;
}

}