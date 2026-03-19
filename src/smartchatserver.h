#ifndef SMARTCHATSERVER_H
#define SMARTCHATSERVER_H

#include <interfaces/iplugin.h>

class SmartChatServer : public KDevelop::IPlugin
{
    Q_OBJECT

public:
    // KPluginFactory-based plugin wants constructor with this signature
    SmartChatServer(QObject* parent, const KPluginMetaData& metaData, const QVariantList& args);
};

#endif // SMARTCHATSERVER_H
