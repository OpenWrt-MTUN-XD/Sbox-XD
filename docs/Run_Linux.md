## Linux 安装

### 方式一：AppImage（推荐）

下载 `Sbox-XD-Linux-x64.AppImage`，赋予执行权限后直接运行：

```shell
chmod +x Sbox-XD-Linux-x64.AppImage
./Sbox-XD-Linux-x64.AppImage
```

AppImage 自带 Qt 运行库，开箱即用，无需额外安装依赖。

### 方式二：便携包（tar.gz）

下载 `Sbox-XD-Linux64.tar.gz`，解压后运行目录中的 `sbox-xd`：

```shell
tar xzf Sbox-XD-Linux64.tar.gz
./sbox-xd/sbox-xd
```

### 其他发行版说明

**使用 Linux 系统相信您已具备基本的排错能力，
本项目不提供特定发行版/架构的支持，预编译文件不能满足您的需求时，请自行编译/适配。**

已知部分 Linux 发行版无法使用、非 x86_64 暂无适配，可以尝试自行编译。

## Linux 运行

便携包解压后，目录内包含：

- `sbox-xd`：主程序（GUI）
- `sboxxd_core`：sing-box 内核（RPC 模式）
- `sing-box`：sing-box 命令行内核（自定义核心）
- `updater`：自动更新程序
- `geoip.dat` / `geoip.db` / `geosite.dat` / `geosite.db`：geo 规则数据

Ubuntu 22.04 若提示缺少 xcb 依赖：`sudo apt install libxcb-xinerama0`
