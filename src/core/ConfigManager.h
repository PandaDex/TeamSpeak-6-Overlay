#ifndef ConfigManager_H
#define ConfigManager_H
#include <QString>
#include <QObject>
#include <QSettings>
#include <QMutex>
#include <QMutexLocker>
class ConfigManager : public QObject
{
    Q_OBJECT
public:
    static void initialize();
    static QString get(const QString &key);
    static bool set(const QString &key, const QString &value);
    static ConfigManager* instance();
signals:
    void configChanged(const QString& key, const QString& value);
private:
    ConfigManager();
    ~ConfigManager();
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    static ConfigManager* m_instance;
    static QMutex m_mutex;
    QSettings* m_settings;
};
#endif // ConfigManager_H
