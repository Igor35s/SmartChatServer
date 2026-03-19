set(smartchatserver_SRCS
    smartchatserver.cpp
)

ecm_qt_declare_logging_category(smartchatserver_SRCS
    HEADER debug.h
    IDENTIFIER PLUGIN_SMARTCHATSERVER
    CATEGORY_NAME "kdevelop.plugins.smartchatserver"
    DESCRIPTION "KDevelop plugin: SmartChatServer"
    EXPORT SMARTCHATSERVER
)

kdevplatform_add_plugin(smartchatserver
    SOURCES ${smartchatserver_SRCS}
)

target_link_libraries(smartchatserver
    KDev::Interfaces
)
