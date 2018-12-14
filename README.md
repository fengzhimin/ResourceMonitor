### 准备工作:
#### manually compile kernel
- 第一步：将内核源码(3.16.40)减压到/usr/src目录下
- 第二步：根据kernel-3-16-40目录下的ReadMe指导来修改内核源码
- 第三步：如果第一次编译源码可以跳过该步骤
	* sudo rm -fr /boot/vmlinuz-3.16.40
	* sudo rm -fr /boot/Systemp.map-3.16.40
	* sudo rm -fr /boot/initrd.img-3.16.40
- 第四步：编译安装源码
	* 第一次编译源码的时候，需要将原有内核源码的.config文件（**/usr/src/linux-headers\-\*\-generic/.config**）拷贝到新内核源码目录下，这样就不用重新配置内核源码了。
	* cd /usr/src/linux-3.16.40
	* sudo make -j8 && sudo make modules_install && sudo make install
- 第五步：重启电脑
	* sudo reboot

#### automatic compile kernel
- sudo ./autoInstall.sh **Path: linux-3.16.40.tar.gz**
- Example: sudo ./autoInstall /home/User/linux-3.16.40.tar.gz

### ResourceMonitor服务程序源码编译安装(在内核层监控软件运行冲突信息)
- cd ResourceMonitor/Server
- make && sudo make install

### ResourceMonitor服务程序卸载(在内核层监控软件运行冲突信息)
- cd ResourceMonitor/Server
- sudo make uninstall

### ResourceMonitor客户程序源码编译安装(通知用户冲突软件信息)
- cd ResourceMonitor/Client
- make && sudo make install

### ResourceMonitor客户程序使用
- sudo ResourceMonitor启动用户层监控程序，该程序用于查看当有软件发生冲突后可以通过ResourceMonitor来查看冲突信息
