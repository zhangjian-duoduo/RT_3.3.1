# sensor库编译环境

## 编译方法
1. 添加编译器路径到 PATH 环境变量:
    - 对于 Linux SDK:

            cd $SDK_DIR/board_support/toolchain

    - 对于 RTOS SDK:
      
            cd $SDK_DIR/docs_tools/software/pc
            tar xf 编译器压缩包

        - 可能是 arm-fullhanv2-eabi-b3.tgz (V3.x.x 以后的版本)
        - 可能是 gcc-arm-fullhan-eabi-6-2017-q2-update.tar.bz2 (V2.x.x 以后的版本)
        - 可能是 arm-2013.11-24-arm-none-eabi-i686-pc-linux-gnu.tar.bz2 (V1.3.1 及以前的版本)

    - 执行:

            export PATH=$(pwd)/上一步解压出的文件夹名/bin:$PATH

2. 运行 ./setup_env.sh 生成 personal.make
3. 运行 build.sh 进行编译:

        ./build.sh

4. 编译好的sensor库放在 _output 文件夹中
