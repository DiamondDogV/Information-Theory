#ifndef RABINCRYPTOSYSTEM_H
#define RABINCRYPTOSYSTEM_H

#include <QVector>
#include <QPair>

struct RabinKeys {
    long long n; // Открытый ключ
    long long p; // Секретный ключ 1
    long long q; // Секретный ключ 2
};

class RabinCryptoSystem {
public:
    // Генерирует ключи. p и q должны быть вида 4k + 3
    static RabinKeys generateKeys(long long p, long long q);
    //проверка на валидность
    static bool isValid(long long p);
    // Шифрование: c = m(m + b) mod n
    static long long encrypt(long long message, long long n, long long b);

    // Дешифрование: возвращает 4 возможных корня
    static QVector<long long> decrypt(long long ciphertext, long long p, long long q, long long b);

private:
    // Расширенный алгоритм Евклида для поиска коэффициентов x и y
    static long long extendedGCD(long long a, long long b, long long &x, long long &y);

    // Быстрое возведение в степень по модулю
    static long long power(long long base, long long exp, long long mod);
};

#endif // RABINCRYPTOSYSTEM_H
