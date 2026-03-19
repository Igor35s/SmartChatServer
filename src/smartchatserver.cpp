#include "smartchatserver.h"

#include <debug.h>

#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(SmartChatServerFactory, "smartchatserver.json", registerPlugin<SmartChatServer>(); )

SmartChatServer::SmartChatServer(QObject* parent, const KPluginMetaData& metaData, const QVariantList& args)
    : KDevelop::IPlugin(QStringLiteral("smartchatserver"), parent, metaData)
{
    Q_UNUSED(args);

    qCDebug(PLUGIN_SMARTCHATSERVER) << "Hello world, my plugin is loaded!";
}

// needed for QObject class created from K_PLUGIN_FACTORY_WITH_JSON
#include "smartchatserver.moc"
#include "moc_smartchatserver.cpp"
