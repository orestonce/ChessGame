#ifndef RONET_H
#define RONET_H

#include "RoNetTypes.h"

/**
 * RestOnce 的网络库 [接口参考 Kangle 的网络库 KnNet]
 * 使用说明 :
 *    1. 使用使用网络库之前必须成功调用一次 InitNetworkLib()
 *    2. 使用完毕后, 可以FreeNetworkLib 也可以等到程序结束时自动释放资源
 *    3. 目前网络库只支持被动链接, 所以监听端口不能为空
 *    4. 在对应事件发生后, 网络库会回调相应函数, 以便上层应用做对应处理
 *    5. 接收数据包, 是按照[4字节网络字节序的包大小信息| 包主体信息] 的格式来分包的, 回调
 *        函数收到的OnRecvData(uConnIndex, pBuffer, uBufferBytes)是完整数据包
 *    6. 发数据包, 未进行分包处理, 需要上层应用做打包处理
 *    7. 调用一次 RoNet::LogicTick () ，会回调[0, n) 次网络事件, 回调一次 AppLogic, 返回值为是否收到网络封包
 *    8. 一次RoNet::LogicTick() 占用时间大约为10ms
 */

namespace RoNet
{

/**
 * @brief InitNetworkLib 初始化网络库
 * @param xData 初始化数据
 * @return true -> 初始化成功, false -> 初始化失败
 */
bool InitNetworkLib(const SRoNetInitData& xData);

/**
 * @brief FreeNetworkLib 释放网络库
 */
void FreeNetworkLib();

/**
 * @brief Disconnect 断开某个链接
 * @param uConnIndex 链接在网络库里面的索引 [1, m_uMaxConnCount]
 */
void Disconnect(UINT uConnIndex);

/**
 * @brief SendData 发送数据到对应的socket
 * @param uConnIndex 链接索引
 * @param pBuffer 数据缓冲
 * @param uBufferBytes 数据字节数
 * @return true -> 成功复制待发送的数据到发送缓冲, flase -> 各种错误
 */
bool SendData(UINT uConnIndex, const char* pBuffer, UINT uBufferBytes);

/**
 * @brief LogicTick 一次逻辑处理, 有限的时间内等待各种网络事件的发生, 并在最后调用AppLogic
 * @return true -> 收到过网络数据包, false -> 没有收到新的数据包
 */
bool LogicTick();

}

#endif // RONET_H
