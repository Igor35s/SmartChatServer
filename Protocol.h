#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstdint>

enum class MessageType : uint32_t {
    RegistrationRequest = 1,
    AuthRequest = 2,
    TextMessage = 3,
    SystemNotification = 10
};

#pragma pack(push, 1)
struct PacketHeader {
    MessageType type;   // 4 байта: Тип
    uint32_t sender_id; // 4 байта: Кто шлет (0 для новых)
    uint32_t target_id; // 4 байта: Кому (0 = всем, >0 = приват)
    uint32_t dataSize;  // 4 байта: Размер полезной нагрузки
}; // Итого: 16 байт
#pragma pack(pop)

#endif
