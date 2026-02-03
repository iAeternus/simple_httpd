#ifndef ERROR_H
#define ERROR_H

/**
 * @brief 设置错误，内部保存 message
 */
void err_set(const char* fmt, ...);

/**
 * @brief 获取最后的错误字符串
 */
const char* err_last();

/**
 * @brief 清除错误
 */
void err_clear();

/**
 * @brief 打印错误信息
 */
void err_print();

#endif // ERROR_H