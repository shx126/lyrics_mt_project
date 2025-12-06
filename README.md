# 带多线程支持的歌词播放器（Lyrics Player with Multi-threading）
一个基于终端的歌词播放器，支持同步显示歌词，并采用多线程架构。

## 功能特性
- 支持 LRC 歌词文件的解析与时间排序
- 实时高亮显示当前播放的歌词行
- 完整 UTF-8 编码支持，可显示中、英、日、韩等多语言歌词
- 多线程架构设计：独立的定时线程（控制播放进度）与显示线程（更新终端界面）
- 使用 mplayer 在后台播放音频文件

## 项目结构
lyrics_mt_project/
├── bin/                # 可执行程序输出目录
│   └── lyrics_player   # 编译生成的可执行文件
├── build/              # 构建中间文件（对象文件、编译缓存等）
├── include/            # 头文件目录
│   ├── display.h       # 显示模块接口
│   ├── lrc_parser.h    # LRC 解析器接口
│   ├── player.h        # 播放器控制接口
│   ├── timer.h         # 定时器模块接口
│   └── utils.h         # 工具函数接口
├── src/                # 源代码目录
│   ├── display.c       # 终端显示逻辑（清屏、打印、高亮等）
│   ├── lrc_parser.c    # LRC 文件解析与排序
│   ├── main.c          # 主函数入口
│   ├── player.c        # 音频播放控制（调用 mplayer）
│   ├── timer.c         # 定时器线程，监控播放进度
│   └── utils.c         # 字符串处理、UTF-8 支持等工具函数
├── test_data/          # 测试数据目录
│   ├── sample.lrc      # 示例歌词文件
│   └── sample.mp3      # 示例音频文件
├── .gitignore          # Git 忽略规则配置
├── Makefile            # 编译脚本
└── README.md           # 项目说明文档

## 编译与运行
1. 安装依赖
```bash
sudo apt install mplayer
```

2. 编译项目 
```bash
make
```

3. 运行程序
```bash
./bin/lyrics_player test_data/sample.mp3 test_data/sample.lrc
```
> 参数说明：
    > 第一个参数：音频文件路径（如 .mp3）
    > 第二个参数：歌词文件路径（如 .lrc）
