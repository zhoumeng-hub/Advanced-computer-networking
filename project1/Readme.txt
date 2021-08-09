文档结构
.
├── Readme.txt
├── enhanced_pcap
│   ├── inter_cluster_aggr-12-0.pcap
│   ├── inter_cluster_core-14-0.pcap
│   ├── inter_cluster_ser-0-0.pcap
│   ├── inter_cluster_tor-8-0.pcap
│   ├── many2one_aggr-12-0.pcap
│   ├── many2one_core-14-0.pcap
│   ├── many2one_ser-0-0.pcap
│   └── many2one_tor-8-0.pcap
├── expriment01
│   ├── inter_cluster_aggr-12-0.pcap
│   ├── inter_cluster_core-14-0.pcap
│   ├── inter_cluster_dcn.cc
│   ├── inter_cluster_ser-0-0.pcap
│   └── inter_cluster_tor-8-0.pcap
├── expriment02
│   ├── many2one_aggr-12-0.pcap
│   ├── many2one_core-14-0.pcap
│   ├── many2one_dcn.cc
│   ├── many2one_ser-0-0.pcap
│   └── many2one_tor-8-0.pcap
├── expriment03
│   ├── ecmp_aggr-12-0.pcap
│   ├── ecmp_core0-16-0.pcap
│   ├── ecmp_core1-17-0.pcap
│   ├── ecmp_ser-0-0.pcap
│   ├── ecmp_tor-8-0.pcap
│   └── enable_ecmp_traffic.cc
└── reporter.pdf

-----------------------------------------------------------------------------------------------------------------------------------------------------------
说明：
1. expriment01~03分别存放了inter cluster traffic，many to one traffic 和 enable ECMP traffic三个实验的源代码和n1, t1, a1, c1(c2)节点的pcap文件
2. enhanced_pcap存放的是inter cluster traffic，many to one traffic两实验进行拥塞改进后的pcap文件
3. repoter.pdf记录了三个实验的实验流程以及结果分析
4. 模型建立流程大致按照如下步骤（因拓扑和TCP流不同而略有差异）
------4.1 初始化节点
------4.2 将节点加入对应的网段
------4.3 对各网段建立相应的信道（如CSMA, P2P)并分配网卡设备
------4.4 在各节点上安装协议栈
------4.5 在各网段分配IP地址
------4.6 安装应用程序，建立TCP连接
------4.7 设置Trace，开始模拟
-----------------------------------------------------------------------------------------------------------------------------------------------------------
