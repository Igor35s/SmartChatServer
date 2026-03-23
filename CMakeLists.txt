cmake_minimum_required(VERSION 3.10)
project(SmartChatServer)

set(CMAKE_CXX_STANDARD 17)

# Ищем Boost (теперь он его НАЙДЕТ в системе Fedora)
find_package(Boost REQUIRED COMPONENTS system thread)

add_executable(SmartChatServer src/smartchatserver.cpp Protocol.h)

# Подключаем пути автоматически
target_include_directories(SmartChatServer PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Линкуем (склеиваем) библиотеки
target_link_libraries(SmartChatServer PRIVATE
    Boost::system
    Boost::thread
    pqxx
    pq
    pthread
)
