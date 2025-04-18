/* linear_least_squares.h */
#ifndef __LINEAR_LEAST_SQUARES_H
#define __LINEAR_LEAST_SQUARES_H

#include <stdint.h>

typedef enum {
    LS_OK            = 0,
    ERR_TOO_FEW_POINTS = -1,  // 需要至少2个数据点
    ERR_SINGULAR_DATA  = -2   // 数据无方差（垂直直线）
} LS_Status;

// 一阶拟合结果结构体
typedef struct {
    float slope;     // 斜率 (a)
    float intercept; // 截距 (b)
    float r_squared; // 拟合优度R²
} LinearFitResult;

// 批量拟合函数
LS_Status Linear_LeastSquares_Fit(
    const float *x,
    const float *y,
    uint16_t num_points,
    LinearFitResult *result
);

#endif