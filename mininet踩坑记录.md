# mininet踩坑记录

[toc]

## 实验2

### 可能出现的问题

1. 拓扑出现环是ping不通的，需要启用生成树协议

![image-20210705102654495](C:\Users\zhoumeng\AppData\Roaming\Typora\typora-user-images\image-20210705102654495.png)

网络上的stp解决方案大都不能用，因为老师给的mininet版本不支持，例如：

[FAQ · mininet/mininet Wiki (github.com)](https://github.com/mininet/mininet/wiki/FAQ#ethernet-loops)

![image-20210705103208911](C:\Users\zhoumeng\AppData\Roaming\Typora\typora-user-images\image-20210705103208911.png)

[mininet - How to enable brctl stp using python script? - Stack Overflow](https://stackoverflow.com/questions/30888845/how-to-enable-brctl-stp-using-python-script)

![image-20210705103241494](C:\Users\zhoumeng\AppData\Roaming\Typora\typora-user-images\image-20210705103241494.png)

[SDN实验（一）：Mininet的安装问题与Fat-Tree的构建 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/360386839)

![image-20210705103733793](C:\Users\zhoumeng\AppData\Roaming\Typora\typora-user-images\image-20210705103733793.png)

### 解决办法

**下载最新版[mininet]([Releases · mininet/mininet (github.com)](https://github.com/mininet/mininet/releases/)), 自定义topo  + pox**

如果用老师给的pyretic虚拟机，也能进行下面1、2、3步，但是没有第4步的命令，所以查看生成树可能有点困难。你如果找到了别的方法查看生成树可以忽略。

1. 首先清理环境

   1. `sudo mn -c`
   2. `sudo killall ovs-controller`
2. 启动pox控制器[Using POX components to create a software defined networking application | Open-Source Routing and Network Simulation (brianlinkletter.com)](https://www.brianlinkletter.com/2015/09/using-pox-components-to-create-a-software-defined-networking-application/)



`sudo ~/pox/pox.py forwarding.l2_learning openflow.spanning_tree --no-flood --hold-down openflow.discovery host_tracker openflow.of_01 --port=6633`

如果没启动成功，可以换个端口试试，比如6653，当然这个换了下面启动的拓扑也要跟着换

3. 自定义topo，手敲代码或者miniedit图形编辑（推荐miniedit，方便截图，还不容易出错）
   1. 手敲代码启动方式(ppt上有讲解)`sudo mn --custom ./yourtopo.py --topo mytopo --controller=remote,port=6633`
   2. miniedit启动方式：`sudo python2 ~/mininet/examples/miniedit.py`，<u>用python3会出现建立好的topo无法保存的情况</u>，搭建好网络, 不需要手动设置ip，controller设置成remote模式，启动就ok了。可以保存成mn文件，这个是可视化文件，下次可以直接打开，也可以保存成python文件，不需要可视化的情况下，可以直接建立拓扑。

如果miniedit使用过程中出现==PuTTY X11 proxy: Authorisation not recognised==，输入`sudo cp /home/mininet/.Xauthority /root/.Xauthority`

miniedit或者wireshark如果出现打不开的情况，请下载vcxsrv软件，并启动。老师给的Xming可能会出现奇怪的报错。



4. 查看生成树(带**NO_FLOOD**的表示逻辑链路断开)

用`links`查看各端口连接情况，用`dpctl dump-ports-desc`查看哪些逻辑链路被禁用了。

![image-20210705225620479](C:\Users\zhoumeng\Desktop\Homework\network\homework\project2\image-20210705225620479.png)

## 实验3

直接使用csv文件中的mac地址是不行的，需要将读出来的文本mac地址转换成pyretic认识的MAC类

可以使用`from pyretic.core.network import MAC`

然后再使用**match(srcmac=MAC('00:00:00:00:00:01'))**这样的命令

