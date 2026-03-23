#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <cstdint>

// Типы сообщений
enum class MessageType : uint32_t {
    AuthRequest = 1,
    TextMessage = 3,
    ImageMessage = 4
};

// Заголовок (всегда 8 байт)
#pragma pack(push, 1)
struct PacketHeader {
    MessageType type;
    uint32_t dataSize;
};
#pragma pack(pop)
#endif
