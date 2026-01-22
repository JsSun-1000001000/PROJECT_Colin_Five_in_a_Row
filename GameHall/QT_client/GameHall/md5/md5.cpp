
#include "md5.h"

//using namespace std;


  
/* Constants for MD5Transform routine. */  
#define S11 7  
#define S12 12  
#define S13 17  
#define S14 22  
#define S21 5  
#define S22 9  
#define S23 14  
#define S24 20  
#define S31 4  
#define S32 11  
#define S33 16  
#define S34 23  
#define S41 6  
#define S42 10  
#define S43 15  
#define S44 21  
  
 #define UINT4 unsigned int
/* F, G, H and I are basic MD5 functions.
*/
/**
 * @brief MD5的4个基本非线性函数
 *
 * F、G、H、I分别用于4轮变换，输入3个32位参数，输出1个32位值。
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))  
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))  
#define H(x, y, z) ((x) ^ (y) ^ (z))  
#define I(x, y, z) ((y) ^ ((x) | (~z)))  
  
/* ROTATE_LEFT rotates x left n bits. 
*/
/**
 * @brief 循环左移操作
 *
 * @param x 待移位的值
 * @param n 移位位数（0-31）
 * @return 左移n位后的值（溢出位补到右侧）
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))  
  
/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4. 
Rotation is separate from addition to prevent recomputation. 
*/

/**
 * @brief 轮1的变换宏
 *
 * @param a,b,c,d 状态变量
 * @param x 当前32位字
 * @param s 移位值
 * @param ac 常数
 * 更新状态变量a：a = (a + F(b,c,d) + x + ac) 循环左移s位 + b
 */
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

/**
 * @brief 轮2的变换宏（逻辑同FF，使用G函数）
 */
#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

/**
 * @brief 轮3的变换宏（逻辑同FF，使用H函数）
 */
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

/**
 * @brief 轮4的变换宏（逻辑同FF，使用I函数）
 */
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

// 静态成员初始化
const Byte MD5::PADDING[64] = { 0x80 };     ///< 填充的起始字节为0x80，其余为0
const char MD5::HEX[16] = {  
    '0', '1', '2', '3',  
    '4', '5', '6', '7',  
    '8', '9', 'a', 'b',  
    'c', 'd', 'e', 'f'  
};  
  
/* Default construct. */
/**
 * @brief 默认构造函数实现
 *
 * 调用reset()初始化状态。
 */
MD5::MD5() {  
    reset();  
}  

/* Construct a MD5 object with a input buffer. */
/**
 * @brief 构造函数（内存数据）实现
 *
 * @param input 输入数据指针
 * @param length 数据长度
 * 初始化后立即处理输入数据。
 */
MD5::MD5(const void *input, size_t length) {  
    reset();  
    update(input, length);  
}  
  
/* Construct a MD5 object with a string. */
/**
 * @brief 构造函数（字符串）实现
 *
 * @param str 输入字符串
 * 初始化后立即处理字符串。
 */
MD5::MD5(const string &str) {  
    reset();  
    update(str);  
}  
  
/* Construct a MD5 object with a file. */
/**
 * @brief 构造函数（文件流）实现
 *
 * @param in 输入文件流
 * 初始化后立即处理文件内容。
 */
MD5::MD5(ifstream &in) {  
    reset();  
    update(in);  
}  
  
/* Return the message-digest */
/**
 * @brief 获取哈希值实现
 *
 * @return 指向16字节哈希值的指针
 * 若未完成计算，先调用final()。
 */
const Byte* MD5::digest() {
    if (!_finished) {  
        _finished = true;  
        final();  
    }  
    return _digest;  
}  
  
/* Reset the calculate state */
/**
 * @brief 重置状态实现
 *
 * 恢复状态变量、计数器和缓冲区，标记计算未完成。
 */
void MD5::reset() {  
  
    _finished = false;  
    /* reset number of bits. */  /* 重置消息长度计数器（初始为0比特） */
    _count[0] = _count[1] = 0;  
    /* Load magic initialization constants. */  /* 初始化状态变量（A、B、C、D） */
    _state[0] = 0x67452301;  
    _state[1] = 0xefcdab89;  
    _state[2] = 0x98badcfe;  
    _state[3] = 0x10325476;  
}  
  
/* Updating the context with a input buffer. */
/**
 * @brief 追加内存数据实现（对外接口）
 *
 * @param input 数据指针
 * @param length 数据长度
 * 转发到底层字节处理接口。
 */
void MD5::update(const void *input, size_t length) {  
    update((const Byte*)input, length);
}  
  
/* Updating the context with a string. */
/**
 * @brief 追加字符串实现
 *
 * @param str 输入字符串
 * 转换为字节数组后处理。
 */
void MD5::update(const string &str) {  
    update((const Byte*)str.c_str(), str.length());
}  
  
/* Updating the context with a file. */
/**
 * @brief 追加文件内容实现
 *
 * @param in 输入文件流
 * 按缓冲区大小读取文件并处理。
 */
void MD5::update(ifstream &in) {  
  
    if (!in)  
        return;  
  
    std::streamsize length;  
    char buffer[BUFFER_SIZE];  
    while (!in.eof()) {  
        in.read(buffer, BUFFER_SIZE);  
        length = in.gcount();  //返回最近一次无格式输入操作实际提取的字符数
        if (length > 0)  
            update(buffer, length);  
    }  
    in.close();  
}  
  
/* MD5 block update operation. Continues an MD5 message-digest 
operation, processing another message block, and updating the 
context. 
*/
/**
 * @brief 内部字节数据处理实现
 *
 * @param input 字节数组
 * @param length 数据长度
 * 累积数据到缓冲区，满64字节时调用transform()处理。
 */
void MD5::update(const Byte *input, size_t length) {
  
    ulong i, index, partLen;  
  
    _finished = false;  // 追加数据后需重新计算
  
    /* Compute number of bytes mod 64 */  /* 计算当前缓冲区中已有的字节数（模64） */
    index = (ulong)((_count[0] >> 3) & 0x3f);  ///< _count[0]是低32位比特数，右移3得字节数
  
    /* update number of bits */  /* 更新消息总长度（比特） */
    if((_count[0] += ((ulong)length << 3)) < ((ulong)length << 3))  ///< 低32位溢出
        _count[1]++;                                                ///< 高32位+1
    _count[1] += ((ulong)length >> 29);                             ///< 剩余比特计入高32位（length <<3的高3位）
  
    partLen = 64 - index;  ///< 缓冲区剩余空间
  
    /* transform as many times as possible. */  /* 若输入数据足够填满缓冲区，先处理完整块 */
    if(length >= partLen) {  
  
        memcpy(&_buffer[index], input, partLen);  // 填满缓冲区
        transform(_buffer);  // 处理该块

        /* 处理剩余的完整块（每64字节一个） */
        for (i = partLen; i + 63 < length; i += 64)  
            transform(&input[i]);  
        index = 0;      // 重置缓冲区索引
  
    } else {  
        i = 0;          // 数据不足填满缓冲区，仅累积
    }  
  
    /* Buffer remaining input */   /* 缓存剩余数据到缓冲区 */
    memcpy(&_buffer[index], &input[i], length-i);  
}  
  
/* MD5 finalization. Ends an MD5 message-_digest operation, writing the 
the message _digest and zeroizing the context. 
*/
/**
 * @brief 完成计算实现
 *
 * 对剩余数据填充，处理最后一个块，生成最终哈希值。
 */
void MD5::final() {  
  
    Byte bits[8];
    ulong oldState[4];  
    ulong oldCount[2];  
    ulong index, padLen;  
  
    /* Save current state and count. */  /* 保存当前状态和计数器（允许后续继续update） */
    memcpy(oldState, _state, 16);
    memcpy(oldCount, _count, 8);  
  
    /* Save number of bits */  /* 将64位长度（比特）编码为字节数组（小端序） */
    encode(_count, bits, 8);  
  
    /* Pad out to 56 mod 64. */  /* 计算填充长度：补到56字节（448比特），若已超过则补到120字节（960比特） */
    index = (ulong)((_count[0] >> 3) & 0x3f);  
    padLen = (index < 56) ? (56 - index) : (120 - index);  
    update(PADDING, padLen);  
  
    /* Append length (before padding) */  /* 追加原始消息长度（64位） */
    update(bits, 8);  
  
    /* Store state in digest */  /* 将最终状态编码为16字节哈希值 */
    encode(_state, _digest, 16);  
  
    /* Restore current state and count. */  /* 恢复状态和计数器（支持重置后继续使用） */
    memcpy(_state, oldState, 16);  
    memcpy(_count, oldCount, 8);  
}  
  
/* MD5 basic transformation. Transforms _state based on block. */
/**
 * @brief 处理512位块实现
 *
 * @param block 64字节数据块
 * 执行4轮变换，更新状态变量A、B、C、D。
 */
void MD5::transform(const Byte block[64]) {
  
    ulong a = _state[0], b = _state[1], c = _state[2], d = _state[3], x[16];  
  
    decode(block, x, 64);  
  
    /* Round 1 */  
    FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */  
    FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */  
    FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */  
    FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */  
    FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */  
    FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */  
    FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */  
    FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */  
    FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */  
    FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */  
    FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */  
    FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */  
    FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */  
    FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */  
    FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */  
    FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */  
  
    /* Round 2 */  
    GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */  
    GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */  
    GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */  
    GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */  
    GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */  
    GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */  
    GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */  
    GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */  
    GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */  
    GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */  
    GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */  
    GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */  
    GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */  
    GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */  
    GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */  
    GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */  
  
    /* Round 3 */  
    HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */  
    HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */  
    HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */  
    HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */  
    HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */  
    HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */  
    HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */  
    HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */  
    HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */  
    HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */  
    HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */  
    HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */  
    HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */  
    HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */  
    HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */  
    HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */  
  
    /* Round 4 */  
    II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */  
    II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */  
    II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */  
    II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */  
    II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */  
    II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */  
    II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */  
    II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */  
    II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */  
    II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */  
    II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */  
    II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */  
    II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */  
    II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */  
    II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */  
    II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */  
    /* 更新状态变量（累加本轮结果） */
    _state[0] += a;  
    _state[1] += b;  
    _state[2] += c;  
    _state[3] += d;  
}  
  
/* Encodes input (ulong) into output (md5byte). Assumes length is
a multiple of 4. 
*/
/**
 * @brief 长整数数组编码为字节数组实现
 *
 * @param input 32位长整数数组
 * @param output 输出字节数组
 * @param length 总长度（字节，4的倍数）
 * 按小端序编码（低字节在前）。
 */
void MD5::encode(const ulong *input, Byte *output, size_t length) {
  
    for(size_t i=0, j=0; j<length; i++, j+=4) {  
        output[j]= (Byte)(input[i] & 0xff);             ///< 低8位
        output[j+1] = (Byte)((input[i] >> 8) & 0xff);   ///< 次低8位
        output[j+2] = (Byte)((input[i] >> 16) & 0xff);  ///< 次高低8位
        output[j+3] = (Byte)((input[i] >> 24) & 0xff);  ///< 高8位
    }  
}  
  
/* Decodes input (md5byte) into output (ulong). Assumes length is
a multiple of 4. 
*/
/**
 * @brief 字节数组解码为长整数数组实现
 *
 * @param input 输入字节数组
 * @param output 32位长整数数组
 * @param length 总长度（字节，4的倍数）
 * 按小端序解码（低字节对应长整数低位）。
 */
void MD5::decode(const Byte *input, ulong *output, size_t length) {
  
    for(size_t i=0, j=0; j<length; i++, j+=4) {    
        output[i] = ((ulong)input[j]) | (((ulong)input[j+1]) << 8) |  
            (((ulong)input[j+2]) << 16) | (((ulong)input[j+3]) << 24);  
    }  
}  
  
/* Convert md5byte array to hex string. */
/**
 * @brief 字节数组转十六进制字符串实现
 *
 * @param input 输入字节数组
 * @param length 字节长度
 * @return 十六进制字符串（小写）
 */
string MD5::bytesToHexString(const Byte *input, size_t length) {
    string str;  
    str.reserve(length << 1);  
    for(size_t i = 0; i < length; i++) {  
        int t = input[i];  
        int a = t / 16;  
        int b = t % 16;  
        str.append(1, HEX[a]);  
        str.append(1, HEX[b]);  
    }  
    return str;  
}  
  
/* Convert digest to string value */
/**
 * @brief 哈希值转字符串实现
 *
 * @return 32位十六进制字符串
 */

string MD5::toString() {  
    return bytesToHexString(digest(), 16);  
}
