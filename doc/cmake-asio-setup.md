## ASIO Header-Only를 사용한 CMake C++ 환경 구축 단계

### **1단계: 프로젝트 구조 설정**

```
<code_block_to_apply_changes_from>
```

### **2단계: ASIO 라이브러리 다운로드**

**옵션 1: Git Submodule 사용 (권장)**
```bash
git submodule add https://github.com/chriskohlhoff/asio.git lib/asio
git submodule update --init --recursive
```

**옵션 2: 직접 다운로드**
- [ASIO 공식 사이트](https://think-async.com/Asio/)에서 최신 버전 다운로드
- `lib/asio/` 디렉토리에 압축 해제

### **3단계: CMakeLists.txt 작성**

```cmake
cmake_minimum_required(VERSION 3.16)
project(cube_asio)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ASIO 헤더 경로 설정
set(ASIO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/asio/asio/include)

# ASIO standalone 정의
add_definitions(-DASIO_STANDALONE)
add_definitions(-DASIO_NO_DEPRECATED)

# Windows 특정 설정
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)  # Windows 7 이상
    add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

# 실행 파일 생성
add_executable(cube_asio src/main.cpp)

# 헤더 파일 경로 포함
target_include_directories(cube_asio PRIVATE 
    ${ASIO_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Windows에서 필요한 라이브러리 링크
if(WIN32)
    target_link_libraries(cube_asio ws2_32 wsock32)
endif()

# Linux/macOS에서 필요한 라이브러리 링크
if(UNIX)
    target_link_libraries(cube_asio pthread)
endif()
```

### **4단계: 기본 소스 파일 생성**

**src/main.cpp**
```cpp
#include <asio.hpp>
#include <iostream>
#include <string>

using asio::ip::tcp;

int main() {
    try {
        asio::io_context io_context;
        
        // 간단한 TCP 서버 예제
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
        
        std::cout << "Server listening on port 8080..." << std::endl;
        
        for (;;) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            
            std::string message = "Hello from ASIO server!\n";
            asio::error_code ignored_error;
            asio::write(socket, asio::buffer(message), ignored_error);
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    return 0;
}
```

### **5단계: 빌드 실행**

```bash
# 빌드 디렉토리 생성
mkdir build
cd build

# CMake 구성
cmake ..

# 빌드 실행
cmake --build .

# 실행 (Windows)
./Debug/cube_asio.exe

# 실행 (Linux/macOS)
./cube_asio
```

### **6단계: 추가 설정 (선택사항)**

**컴파일러 경고 설정:**
```cmake
if(MSVC)
    target_compile_options(cube_asio PRIVATE /W4)
else()
    target_compile_options(cube_asio PRIVATE -Wall -Wextra -Wpedantic)
endif()
```

**디버그/릴리즈 모드 설정:**
```cmake
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()
```

이제 프로젝트 구조를 실제로 생성해드릴까요?
