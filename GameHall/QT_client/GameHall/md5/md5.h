/**
 * @file md5.h
 * @brief md5 header file
 * @author jssun
 * @date 2026-01-03
 * @version 0.0.1
 */
#pragma once
#ifndef MD5_H
#define MD5_H
  
#include <string>
#include <fstream>

  
/* Type define */
typedef unsigned char Byte;     ///< 无符号字节类型
typedef unsigned long ulong;    ///< 无符号long整数类型
  
using std::string;  
using std::ifstream;  
  
/* MD5 declaration. */
/**
 * @brief MD5消息摘要算法类\n
 *
 * 实现MD5算法，支持对内存数据、字符串和文件计算128位哈希值，
 * 并提供哈希值的十六进制字符串表示。
 */
class MD5 {  
public:
    /**
     * @brief 默认构造函数
     *
     * 初始化MD5计算状态（状态变量、计数器等）。
     */
    MD5();

    /**
     * @brief 构造函数（接收内存数据）
     *
     * @param input 输入数据的指针
     * @param length 输入数据的长度（字节）
     * 初始化状态后，立即处理输入数据。
     */
    MD5(const void *input, size_t length);

    /**
     * @brief 构造函数（接收字符串）
     *
     * @param str 输入字符串
     * 初始化状态后，立即处理字符串数据。
     */
    MD5(const string &str);

    /**
     * @brief 构造函数（接收文件流）
     *
     * @param in 输入文件流（已打开）
     * 初始化状态后，立即处理文件内容。
     */
    MD5(ifstream &in);

    /**
     * @brief 追加内存数据到MD5计算
     *
     * @param input 待追加数据的指针
     * @param length 数据长度（字节）
     */
    void update(const void *input, size_t length);

    /**
     * @brief 追加字符串到MD5计算
     *
     * @param str 待追加的字符串
     */
    void update(const string &str);

    /**
     * @brief 追加文件内容到MD5计算
     *
     * @param in 待读取的文件流（已打开）
     */
    void update(ifstream &in);

    /**
     * @brief 获取最终的MD5哈希值（16字节）
     *
     * @return 指向16字节哈希值的指针（内部缓冲区）
     * @note 若计算未完成，会先调用final()完成计算。
     */
    const Byte* digest();

    /**
     * @brief 将哈希值转换为32位十六进制字符串
     *
     * @return 哈希值的十六进制表示（小写）
     */
    string toString();

    /**
     * @brief 重置MD5计算状态
     *
     * 恢复初始状态，可重新开始新的MD5计算。
     */
    void reset();  
private:

    /**
     * @brief 内部接口：追加字节数据到计算
     *
     * @param input 待追加的字节数组
     * @param length 数据长度（字节）
     * 处理输入数据，当缓冲区满64字节时调用transform()。
     */
    void update(const Byte *input, size_t length);

    /**
     * @brief 完成MD5计算
     *
     * 对剩余数据进行填充，处理最后一个块，生成最终哈希值。
     */
    void final();

    /**
     * @brief 处理单个512位（64字节）数据块
     *
     * @param block 64字节的数据块
     * 执行4轮变换，更新状态变量A、B、C、D。
     */
    void transform(const Byte block[64]);

    /**
     * @brief 将长整数数组编码为字节数组
     *
     * @param input 长整数数组（32位）
     * @param output 输出的字节数组
     * @param length 转换的总长度（字节，需为4的倍数）
     * 按小端序转换（低字节在前）。
     */
    void encode(const ulong *input, Byte *output, size_t length);

    /**
     * @brief 将字节数组解码为长整数数组
     *
     * @param input 输入的字节数组
     * @param output 输出的长整数数组（32位）
     * @param length 转换的总长度（字节，需为4的倍数）
     * 按小端序转换（低字节对应长整数低位）。
     */
    void decode(const Byte *input, ulong *output, size_t length);

    /**
     * @brief 将字节数组转换为十六进制字符串
     *
     * @param input 输入的字节数组
     * @param length 字节数组长度
     * @return 对应的十六进制字符串（小写）
     */
    string bytesToHexString(const Byte *input, size_t length);
  
    /* class uncopyable */
    /**< 禁止拷贝构造和赋值 */
    MD5(const MD5&);  
    MD5& operator=(const MD5&);  
private:  
    ulong _state[4];    /* state (ABCD) */          ///< 状态变量（A、B、C、D）
    ulong _count[2];    /* number of bits, modulo 2^64 (low-order word first) */  ///< 消息长度计数器（低32位、高32位，单位：比特）
    Byte _buffer[64];   /* input buffer */          ///< 输入数据缓冲区（最多64字节，凑齐512位块）
    Byte _digest[16];   /* message digest */        ///< 最终哈希值（16字节，128位）
    bool _finished;     /* calculate finished ? */  ///< 计算是否完成的标志
  
    static const Byte PADDING[64];  /* padding for calculate */ ///< 填充数据（首字节0x80，其余0x00）
    static const char HEX[16];                                  ///< 十六进制字符表（0-9, a-f）
    static const size_t BUFFER_SIZE = 1024;                     ///< 文件读取缓冲区大小（字节）
};  
  
#endif/*MD5_H*/
