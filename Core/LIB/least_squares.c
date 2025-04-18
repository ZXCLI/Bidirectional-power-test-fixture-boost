/* linear_least_squares.c */
#include "least_squares.h"
#include <math.h>

LS_Status Linear_LeastSquares_Fit(const float *x, const float *y, uint16_t n, LinearFitResult *result)
{
    if(n < 2) return ERR_TOO_FEW_POINTS;

    float sum_x = 0.0f, sum_y = 0.0f;
    float sum_xx = 0.0f, sum_xy = 0.0f;

    // 单次遍历计算累加值
    for(uint16_t i=0; i<n; i++){
        sum_x += x[i];
        sum_y += y[i];
        sum_xx += x[i] * x[i];
        sum_xy += x[i] * y[i];
    }

    // 计算斜率分母
    float denom = n * sum_xx - sum_x * sum_x;
    if(fabsf(denom) < 1e-7f) return ERR_SINGULAR_DATA;

    // 计算斜率和截距
    result->slope = (n * sum_xy - sum_x * sum_y) / denom;
    result->intercept = (sum_y - result->slope * sum_x) / n;

    // 计算R²
    float y_mean = sum_y / n;
    float ss_tot = 0.0f, ss_res = 0.0f;
    for(uint16_t i=0; i<n; i++){
        float y_pred = result->slope * x[i] + result->intercept;
        ss_tot += (y[i] - y_mean) * (y[i] - y_mean);
        ss_res += (y[i] - y_pred) * (y[i] - y_pred);
    }
    result->r_squared = 1.0f - ss_res / ss_tot;

    return LS_OK;
}
