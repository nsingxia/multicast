cmake_minimum_required( VERSION 2.8)

set(CXX_FLAGS
    -g                               # 保存调试信息
    -Wall                            # 编译后显示所有的警告
    -m64                             # 64位程序
    -Wextra                          # 打印额外的警告信息
    #-Werror                         # 警告变错误
    -Wno-unused-parameter            # 忽略未使用变量的警告
    -Wno-strict-aliasing             # 忽略严格别名规则的警告
    -O0                              # 不开启优化
    -ggdb3                           # ggdb3
    -D_DEBUG_                        # 设置D_DEBUG_
    -std=c++11                       # 使用c++11标准
    -pthread                         # phread线程
    )

# 将编译选项CXX_FLAGS设置到CMAKE_CXX_FLAGS中
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(ROOT  ${CMAKE_CURRENT_LIST_DIR})

include_directories(${ROOT})

# suppress warnings

set(MAIN
  ${ROOT}/main.cpp
)
add_executable(multicast ${MAIN})
target_link_libraries(multicast dl)
# target_link_libraries(main pthread dl)