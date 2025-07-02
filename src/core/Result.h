#ifndef RESULT_H
#define RESULT_H

#include <QString>

template<typename T = void>
class Result {
public:
    static Result success() {
        static_assert(std::is_void_v<T>, "Use success(value) for non-void types");
        return Result(true);
    }
    
    static Result success(const T& value) {
        static_assert(!std::is_void_v<T>, "Use success() for void types");
        return Result(true, value);
    }

    static Result error(const QString& message) {
        return Result(false, T{}, message);
    }

    bool isSuccess() const { return m_success; }
    bool isError() const { return !m_success; }

    template<typename U = T>
    typename std::enable_if<!std::is_void_v<U>, const U&>::type
    value() const {
        Q_ASSERT(m_success);
        return m_value;
    }

    const QString& errorMessage() const { return m_errorMessage; }
    
private:
    explicit Result(bool success, const T& value = T{}, const QString& error = QString())
        : m_success(success), m_value(value), m_errorMessage(error) {}
    
    bool m_success;
    T m_value;
    QString m_errorMessage;
};

template<>
class Result<void> {
public:
    static Result success() {
        return Result(true);
    }
    
    static Result error(const QString& message) {
        return Result(false, message);
    }
    
    bool isSuccess() const { return m_success; }
    bool isError() const { return !m_success; }
    
    const QString& errorMessage() const { return m_errorMessage; }
    
private:
    explicit Result(bool success, const QString& error = QString())
        : m_success(success), m_errorMessage(error) {}
    
    bool m_success;
    QString m_errorMessage;
};

#endif // RESULT_H