
import os

class File:
    def __init__(self):
        pass

    #文件写入函数
    def write_txt(self, wifi, psd, ip):
        self.fp = open('WiFi.txt','w')
        self.fp.seek(0)   #将文件指针移至文件开头
        self.fp.truncate()#从文件开头开始，截断后面所有的内容=清空文件内容
        txt = 'WiFi#' + str(wifi) + '*'
        self.fp.write(txt)
        txt = 'PSD#' + str(psd) + '*'
        self.fp.write(txt)
        txt = 'IP#' + str(ip)
        self.fp.write(txt)
        self.fp.close()

    def read_txt(self):
        try:
            self.fp = open('WiFi.txt','r')
            self.fp.seek(0)
            buf = self.fp.read()
            information = buf.split('*')
            return information
        except:
            return -1


if __name__ == '__main__':
    c = File()
    c.write_txt('FYI','freenove','192.168.1.1')
    c.read_txt()





