## cmake启用gcov
```cmake
set(CMAKE_C_FLAGS "-std=c99")
message("LCOV is enabled: ${ENABLE_COVERAGE}")
if (ENABLE_COVERAGE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
endif ()
```

编译时添加编译宏
```bash
-DENABLE_COVERAGE=ON
```

## 安装LCOV
### 方法一
> 下载并编译安装
```sudo
wget http://ncu.dl.sourceforge.net/project/ltp/Coverage Analysis/LCOV-1.12/lcov-1.12.tar.gz
tar -zxvf lcov-1.12.tar.gz 
make
make install
```

安装相关依赖
```bash
# Can't locate JSON/PP.pm in @INC (@INC contains:
sudo yum install perl-CPAN
sudo perl -MCPAN -e 'install JSON'

# Can't locate IO/Uncompress/Gunzip.pm in @INC (@INC contains:
sudo yum install perl-IO-Compress.noarch

# Can't locat Digest/MD5.pm in @INC(@INC contains:
sudo yum -y install perl-Digest-MD5
```

### 方法二
> 下载rpm包本地安装
> 
> https://github.com/linux-test-project/lcov/releases/tag/v1.15
```bash
wget https://github.com/linux-test-project/lcov/releases/download/v1.15/lcov-1.15-1.noarch.rpm

sudo yum localinstall lcov-1.15-1.noarch.rpm
```
## 执行生成覆盖率
```bash
# 1. 生成result.info

lcov -c -d ./ -o result.info

# 2. 排除文件

lcov -r result.info  */gtest/*  **/include/** -o result.info

# 3. 生成html文件

mkdir lcov
genhtml -o lcov result.info
```