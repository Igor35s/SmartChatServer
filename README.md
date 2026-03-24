# 🚀 SmartChatServer (C++)

Мощный многопоточный сервер для мессенджера, написанный на **C++17**.

## 🛠 Технологии
*   **Сетевой движок:** [Boost.Asio](https://www.boost.org) (асинхронная обработка соединений).
*   **База данных:** [PostgreSQL](https://www.postgresql.org) (хранение пользователей и истории).
*   **Драйвер БД:** [libpqxx](https://pqxx.org) (C++ клиент для Postgres).
*   **Система сборки:** [CMake](https://cmake.org) + Ninja.
*   **ОС:** Fedora Linux.

## 📈 Текущий статус
- [x] Скелет сервера на Boost.Asio.
- [x] Протокол обмена данными (PacketHeader).
- [x] Обработка входящих TCP-соединений (порт 1234).
- [ ] Интеграция с PostgreSQL (в процессе).
- [ ] Авторизация пользователей.

## 🚀 Как запустить
1. Установите зависимости: `boost-devel`, `libpqxx-devel`, `postgresql-server`.
2. Соберите проект через CMake:
   ```bash
   mkdir build && cd build
   cmake .. -G Ninja
   ninja
   ./SmartChatServer
