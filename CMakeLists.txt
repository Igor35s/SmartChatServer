cmake_minimum_required(VERSION 3.10)
project(SmartChatServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Ищем Boost
find_package(Boost REQUIRED COMPONENTS system)

# 2. Ищем библиотеку системных потоков (для std::thread)
find_package(Threads REQUIRED)

# 3. Ищем PostgreSQL и libpqxx (через PkgConfig для Fedora)
find_package(PostgreSQL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

add_executable(SmartChatServer src/smartchatserver.cpp Protocol.h)

# 4. Подключаем пути
target_include_directories(SmartChatServer PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${PostgreSQL_INCLUDE_DIRS}
    ${PQXX_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 5. Линкуем библиотеки
target_link_libraries(SmartChatServer PRIVATE
    Boost::system
    Threads::Threads
    ${PostgreSQL_LIBRARIES}
    ${PQXX_LIBRARIES}
)
