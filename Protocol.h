#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstdint>

// Типы сообщений (ваша отличная наработка)
enum class MessageType : uint32_t {
    AuthRequest = 1,
    TextMessage = 3,
    ImageMessage = 4
};

// Заголовок (всегда 8 байт)
#pragma pack(push, 1)
struct PacketHeader {
    MessageType type;  // Тип сообщения
    uint32_t dataSize; // Размер данных, которые идут следом (в байтах)
};

// Структура для передачи простого текста (из ТЗ Архитектора)
struct TextMessagePacket {
    PacketHeader header;
    // Данные размером dataSize следуют в потоке TCP сразу за заголовком.
    // В коде C++ мы читаем их в std::string или массив символов.
};
#pragma pack(pop)

#endif // PROTOCOL_H
