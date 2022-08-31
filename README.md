### 操作系统实验

-----

包括以下四个实验：

- **copy**
    - 分别在windows和linux下完成一个目录复制命令mycp，包括目录下的文件和子目录。复制后，不仅权限一致，而且时间属性也一致。linux下要求支持软链接。
    - 实验报告：[copy实验报告](/copy/README.md)
<br>

- **time**
    - 在windows和linux下分别实现mytime命令，通过命令行参数接受要运行的程序，创建一个独立的进程来运行该程序，并记录程序运行的时间。
    - 实验报告：[time实验报告](/time/README.md)
<br>

- **memory-monitor**
    - 设计一个内存监视器，能实时地显示windows系统中内存的使用情况，包括系统地址空间的布局，物理内存的使用情况；能实时显示某个进程的虚拟地址空间布局和工作集信息等。
    - 实验报告：[memory-monitor实验报告](/memory-monitor/README.md)
<br>

- **consumer-and-producer**
    - 分别在windows和linux下模拟实现下列内容：
        - 一个大小为3的缓冲区，初始为空
        - 2个生产者
            - 随机等待一段时间，往缓冲区添加数据，
            - 若缓冲区已满，等待消费者取走数据后再添加
            - 重复6次
        - 3个消费者
            - 随机等待一段时间，从缓冲区读取数据
            - 若缓冲区为空，等待生产者添加数据后再读取
            - 重复4次

    - 实验报告：[consumer-and-producer实验报告](/consumer-and-producer/README.md)