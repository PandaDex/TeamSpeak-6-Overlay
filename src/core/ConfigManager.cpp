#include "ConfigManager.h"
#include "Constants.h"
#include "Logger.h"
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>

// Static members
ConfigManager* ConfigManager::m_instance = nullptr;
QMutex ConfigManager::m_mutex;

ConfigManager::ConfigManager()
    : QObject(nullptr)
{
    QString configPath;

    QFileInfo dbInfo(Constants::DB_NAME);

    if (dbInfo.isAbsolute()) {
        configPath = dbInfo.absolutePath() + "/" + dbInfo.baseName() + ".cfg";
    } else if (dbInfo.path() != ".") {
        configPath = dbInfo.path() + "/" + dbInfo.baseName() + ".cfg";
    } else {
        configPath = dbInfo.baseName() + ".cfg";
    }

    m_settings = new QSettings(configPath, QSettings::IniFormat, this);

    LOG_INFO(QString("Config file initialized at: %1").arg(configPath));
}

ConfigManager::~ConfigManager()
{
    if (m_settings) {
        m_settings->sync();
        delete m_settings;
        m_settings = nullptr;
    }
}

ConfigManager* ConfigManager::instance()
{
    if (!m_instance) {
        QMutexLocker locker(&m_mutex);
        if (!m_instance) {
            m_instance = new ConfigManager();
        }
    }
    return m_instance;
}

void ConfigManager::initialize()
{
    ConfigManager* inst = instance();

    QFileInfo fileInfo(inst->m_settings->fileName());
    QDir dir = fileInfo.absoluteDir();

    if (!dir.exists()) {
        if (dir.mkpath(".")) {
            LOG_INFO("Created config directory: " + dir.absolutePath());
        } else {
            LOG_ERROR("Failed to create config directory: " + dir.absolutePath());
            return;
        }
    }

    inst->m_settings->sync();

    if (inst->m_settings->status() != QSettings::NoError) {
        LOG_ERROR("Failed to initialize config file");
        return;
    }

    LOG_INFO("Config file probably ok");
}

QString ConfigManager::get(const QString &key)
{
    ConfigManager* inst = instance();
    QMutexLocker locker(&m_mutex);

    if (!inst->m_settings) {
        LOG_ERROR("Config file not initialized for get operation");
        return QString();
    }

    if (!inst->m_settings->contains(key)) {
        //LOG_WARNING("No value found for key: " + key);
        return QString();
    }

    QString value = inst->m_settings->value(key).toString();
    return value;
}

bool ConfigManager::set(const QString &key, const QString &value)
{
    ConfigManager* inst = instance();
    QMutexLocker locker(&m_mutex);

    if (!inst->m_settings) {
        LOG_ERROR("Config file not initialized for set operation");
        return false;
    }

    inst->m_settings->setValue(key, value);
    inst->m_settings->sync();

    if (inst->m_settings->status() != QSettings::NoError) {
        LOG_ERROR("Failed to write config value for key: " + key);
        return false;
    }

    locker.unlock();
    emit inst->configChanged(key, value);

    return true;
}
