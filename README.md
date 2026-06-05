# Zuma-Game-C-EasyX

基于 C++ 与 EasyX 图形库的祖玛游戏实现（南京大学数据结构与算法课程大作业）

## 项目概述

基于经典祖玛游戏的简化版实现，使用 C++ 和 EasyX 图形库开发，重点演示数据结构与算法在游戏开发中的实际应用。

## 功能特性

### 核心玩法
- 彩色球链沿阿基米德螺旋由外向内排列
- 鼠标控制从窗口中央发射彩球
- 三连消判定与连锁消除（`EliminateRuns`）
- 发射点位于窗口正中央，支持 360° 瞄准

### 游戏机制
- 灰白色虚线螺旋辅助线，提示轨道方向
- 双缓冲渲染，消除画面闪烁
- 窗口自适应缩放：螺旋和球随窗口大小等比例缩放
- 支持拖动边框和最大化/还原

### 技术实现
- 单向链表实现动态球链存储
- EasyX 图形界面实时渲染
- 数值积分弧长算法精确控制球间距
- Windows API 窗口样式控制

## 项目结构

```
Zuma-Game-C-EasyX/
├── .gitignore
├── .vscode/tasks.json      # VS Code 构建任务
├── README.md
├── include/
│   ├── ball.h              # 球结构体定义
│   └── LinkList.h          # 链表接口声明
├── src/
│   ├── main.cpp            # 游戏主逻辑
│   └── LinkList.cpp        # 链表实现（CRUD + EliminateRuns）
└── build/                  # 构建输出目录
```

## 安装指南

### 环境要求
- Windows 系统
- MinGW-w64 (g++)
- EasyX 图形库 (MinGW 移植版)

### 配置步骤
1. 安装 [EasyX 图形库](https://easyx.cn)
2. 安装 MinGW-w64 并配置 PATH
3. 克隆本仓库：
   ```bash
   git clone https://github.com/JesseLee-CN/Zuma-Game-C-EasyX.git
   ```
4. 使用 VS Code 打开项目，按 `Ctrl+Shift+B` 构建
5. 运行 `build/ZumaGame.exe`

### 手动编译
```bash
g++ -g -Wall -Wextra -Iinclude src/main.cpp src/LinkList.cpp \
    -o build/ZumaGame.exe -leasyx -lgdi32
```

## 操作说明

| 操作 | 功能 |
|---|---|
| 鼠标移动 | 瞄准（显示白色瞄准线） |
| 鼠标左键按下 + 松开 | 向瞄准方向发射彩球 |
| 鼠标右键 | 退出游戏 |
| 拖动窗口边框 | 自由缩放窗口 |
| 最大化按钮 | 全屏模式 |

## 游戏规则

- 发射彩球击中球链中的球后，彩球插入该位置
- 若相邻同色球达 3 个及以上，全部消除
- 消除后若新相邻同色球达 3 个及以上，连锁消除
- 彩球飞出窗口边界后自动重置
