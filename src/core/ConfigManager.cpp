#include "ConfigManager.h"
#include "Constants.h"
#include "Logger.h"
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QCoreApplication>

// Static members
ConfigManager* ConfigManager::m_instance = nullptr;
QMutex ConfigManager::m_mutex;

ConfigManager::ConfigManager()
    : QObject(nullptr)
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    if (configDir.isEmpty()) {
        configDir = QCoreApplication::applicationDirPath();
    } else {
        configDir += "/" + Constants::CONFIG_DIR_NAME;
    }
    QDir().mkpath(configDir);
    const QString configPath = QDir(configDir).filePath(Constants::CONFIG_FILE_NAME);

    migrateLegacyConfig(configPath);

    m_settings = new QSettings(configPath, QSettings::IniFormat, this);

    LOG_INFO(QString("Config file initialized at: %1").arg(configPath));
}

void ConfigManager::migrateLegacyConfig(const QString &configPath)
{
    if (QFileInfo::exists(configPath)) return;

    // Older versions stored config.cfg next to the executable / in the working dir
    const QStringList legacyCandidates = {
        QDir(QCoreApplication::applicationDirPath()).filePath(Constants::CONFIG_FILE_NAME),
        QDir::current().filePath(Constants::CONFIG_FILE_NAME)
    };

    for (const QString &legacy : legacyCandidates) {
        if (QFileInfo::exists(legacy) && QFile::copy(legacy, configPath)) {
            LOG_INFO("Migrated legacy config from: " + legacy);
            return;
        }
    }
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
    QMutexLocker locker(&m_mutex);

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

    if (!fileInfo.exists() || fileInfo.isWritable()) {
        LOG_INFO("Config file ok: " + fileInfo.absoluteFilePath());
    } else {
        LOG_ERROR("Config file is not writable: " + fileInfo.absoluteFilePath());
    }
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
