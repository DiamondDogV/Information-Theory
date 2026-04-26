#include "rabincryptosystem.h"
#include <cmath>

bool RabinCryptoSystem :: isValid(long long x){

    if (x % 4 != 3) return false;
    if (x < 2) return false;
    if (x == 2 || x == 3) return true;
    if (x % 2 == 0 || x % 3 == 0) return false;
    for (long long i = 5; i * i <= x; i += 6) {
        if (x % i == 0 || x % (i + 2) == 0)
            return false;
    }

    return true;
}

RabinKeys RabinCryptoSystem::generateKeys(long long p, long long q) {
    return {p * q, p, q};
}

long long RabinCryptoSystem::encrypt(long long m, long long n, long long b) {
    // чтобы результат (m * (m + B)) не вызвал переполнения
    unsigned __int128 message = static_cast<unsigned __int128>(m);
    unsigned __int128 mod = static_cast<unsigned __int128>(n);
    unsigned __int128 b_val = static_cast<unsigned __int128>(b);

    // c = m * (m + b) mod n
    unsigned __int128 result = (message * (message + b_val)) % mod;

    return static_cast<long long>(result);
}

long long RabinCryptoSystem::power(long long base, long long exp, long long mod) {
    long long res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1) res = (__int128(res) * base) % mod;
        base = (__int128(base) * base) % mod;
        exp /= 2;
    }
    return res;
}

long long RabinCryptoSystem::extendedGCD(long long a, long long b, long long &x, long long &y) {
    if (a == 0) {
        x = 0; y = 1;
        return b;
    }
    long long x1, y1;
    long long gcd = extendedGCD(b % a, a, x1, y1);
    x = y1 - (b / a) * x1;
    y = x1;
    return gcd;
}

QVector<long long> RabinCryptoSystem::decrypt(long long c, long long p, long long q, long long b) {
    long long n = p * q;

    // 1. D = (4c + B^2) mod n
    // позволяет свести уравнение m^2 + Bm - c = 0 к (2m + B)^2 = D
    unsigned __int128 D = (4 * (unsigned __int128)c + (unsigned __int128)b * b) % n;

    // 2. Извлекаем квадратные корни из D по отдельным модулям p и q

    long long mp = power(D, (p + 1) / 4, p);
    long long mq = power(D, (q + 1) / 4, q);

    // 3. ПРИМЕНЕНИЕ КИТАЙСКОЙ ТЕОРЕМЫ ОБ ОСТАТКАХ
    // Ищем x такое, что:
    // x ≡ mp (mod p) ИЛИ x ≡ -mp (mod p)
    // x ≡ mq (mod q) ИЛИ x ≡ -mq (mod q)

    long long yp, yq;
    extendedGCD(p, q, yp, yq);
    // yp*p + yq*q = 1

    // Вычисляем 4 комбинации корней x = (2m + B)
    // r1 = (yp*p*mq + yq*q*mp) mod n
    // r2 = (yp*p*mq - yq*q*mp) mod n ... и их отрицания
    long long d[4];
    d[0] = (long long)((yp * p * mq + yq * q * mp) % n);
    d[1] = (long long)(n - d[0]);
    d[2] = (long long)((yp * p * mq - yq * q * mp) % n);
    d[3] = (long long)(n - d[2]);

    // 4. Переход от x к искомому m
    // Т.к. x = 2m + B, то m = (x - B) * 2^-1 mod n
    QVector<long long> results;
    for (int i = 0; i < 4; ++i) {
        long long mi;
        long long numerator = (d[i] - b);

        if (numerator % 2 == 0) {
            mi = (numerator / 2 + n) % n;
        } else {
            mi = ((numerator + n) / 2) % n;
        }
        results.append(mi);
    }

    return results;
}
