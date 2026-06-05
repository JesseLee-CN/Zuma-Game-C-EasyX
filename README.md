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

### 难度系统
- 初始难度选择界面，五种模式可选
- **INFINITE**：初始 10 球，无限发射次数
- **EASY**：初始 10 球，100 次发射机会
- **COMMON**：初始 20 球，80 次发射机会
- **HARD**：初始 30 球，50 次发射机会
- **CUSTOMIZED**：自定义初始球数（5–50）和发射次数（10–200 或无限）

### 计分系统
- 单轮消除得分：$N^2 \times 10 \times (1 + (k-1) \times 0.5)$
  - $N$ 为本轮消除球数（≥3），$k$ 为连锁轮次
  - 一次性消除越多球得分越高，分次消除总分较低
- 右上角实时显示总分和剩余发射次数
- 消除时弹出浮动 "+分数" 动画
- 详细规则见 [`docs/rules.md`](docs/rules.md)

### 结算系统
- 全部球消除（CLEAR）：淡出动画 + 分数飞入屏幕中央 + 绿色 "CLEAR!" 结算画面
- 发射耗尽（GAME OVER）：同样淡出动画，按剩余球数 $×20$ 扣分，红色 "GAME OVER" 结算画面
- 结算画面提供 **TRY AGAIN**（返回难度选择）和 **EXIT**（退出游戏）按钮

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
- 五状态游戏状态机（MENU / CUSTOMIZE / PLAYING / CLEARING / SETTLEMENT）

## 项目结构

```
Zuma-Game-C-EasyX/
├── .gitignore
├── .vscode/tasks.json      # VS Code 构建任务
├── README.md
├── docs/
│   └── rules.md            # 计分规则文档
├── include/
│   ├── ball.h              # 球结构体定义
│   └── LinkList.h          # 链表接口声明
├── src/
│   ├── main.cpp            # 游戏主逻辑（状态机、渲染、输入）
│   └── LinkList.cpp        # 链表实现（CRUD + EliminateRuns + 计分）
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
| 鼠标移动 | 瞄准（显示白色瞄准线）/ 菜单按钮悬停 |
| 鼠标左键点击 | 菜单选择 / 发射彩球 / 按钮交互 |
| 鼠标右键 | 任意界面退出游戏 |
| 拖动窗口边框 | 自由缩放窗口 |
| 最大化按钮 | 全屏模式 |

## 游戏规则

- 在难度选择界面选择模式（或进入自定义页面），开始游戏
- 发射彩球击中球链中的球后，彩球插入该位置
- 若相邻同色球达 3 个及以上，全部消除并计分
- 消除后若新相邻同色球达 3 个及以上，连锁消除（得分递增）
- 彩球飞出窗口边界后自动重置，消耗一次发射机会（INFINITE 模式除外）
- 全部球消除则获胜，发射次数耗尽且有剩余球则失败
