# U BOOT

### 简介

本文件夹下的内容用于制作 **U 启动**

### 目标

实现以下启动流程：

1. 把 OS 的 `kernel` 拷贝至U盘的 `boot` 文件夹下，并重命名 `kernel 文件` 为 `kernel`
2. 重启电脑，选择从U盘引导
3. 电脑最后将把U盘中的 `kernel` 加载至内存并运行。如此，可从真实电脑上运行自写的 OS

### 制作过程

0. 以 `MBR` 格式对U盘进行分区，并创建一个文件系统为 `FAT32` 的分区

1. 编译本文件下所有代码，即在本文件夹下执行 `make all`

2. 把编译得到的 `mbr` `dbr` `bootloader` 拷贝至U盘指定扇区

   ```shell
   # U_DRIVE_FILE 为U盘的设备文件，PARTITION_DRIVE_FILE 为分区的设备文件
   U_DRIVE_FILE=/dev/disk2
   PARTITION_DRIVE_FILE=/dev/disk2s1
   # 拷贝 MBR
   sudo dd if=mbr of=${U_DRIVE_FILE} bs=512 count=1
   # 拷贝 DBR
   sudo dd if=dbr of=${PARTITION_DRIVE_FILE} bs=512 count=1
   # 拷贝 bootloader
   sudo dd if=bootloader of=${PARTITION_DRIVE_FILE} bs=512 oseek=2 count=2
   ```

3. 在上述新建的分区中，创建 `boot` 目录，并把本项目编译得到的 `kernel.bin` 拷贝至此目录下，并重命名为 `kernel`

4. over!

### 原理

请 [参考](https://blog.csdn.net/kiss_xiaojie/article/details/115465005)
