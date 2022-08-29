#include <cstring>
#include <string>
#include <malloc.h>
#include <iostream>
/*
 * 1. Отсутствие интерфейса для взаимодействия с переменными(опционально).
 * 2. Задание размера 'магическими числами', при необходимости достаточно вынести число как константу, например  int size = 1024;
 * 3. Использование сырых указателей, вместо доступных с С++11 - smart-pointers, отсюда и недостаток в виде ручного выделения памяти.
 * (П.3. Пример исправлен для версии с сырыми поинтерами, по необходимости могу предоставить вариант с умными указателями/контейнерами)
 * 4. Отсутствие деструктора/метода освобождения памяти(небезопасно, так как потребуется явно вызывать и можно забыть, плюс лучше соблюдать RAII).
 * 5. Опять таки возможно использование контейнеров из STL(более удобно/безопасно), не требуется ручное выделение памяти, все будет происходить автоматически
 * и через интерфейс allocator_traits.
 * 6. Возможен неприятный исход - например конструктор бросит std::bad_alloc, возникнет утечка памяти.
 * Стандарт арантирует что память из под членов класса будет очищена деструктором(при эксепшене), этого легко добиться соблюдая RAII.
 * С сырыми поинтерами нужно больше следить за очисткой в случае возникновения исключения.
 */

//1.
class SomeClass
{
public:
    //Копи-конструктор
    SomeClass(const SomeClass& _other)
    {
        deepCopy(_other);
    }
    void deepCopy(const SomeClass& _other)
    {
        delete[] x1; //дополнительная мера безопасности в случае использования данного метода в ином месте нежели конструктор.
        delete[] x2;

        try {
#ifdef _WIN32
            std::size_t sizeArr = _msize(_other.x1);
#elif defined __linux__
            std::size_t sizeArr = malloc_usable_size(_other.x1);
#endif
            x1 = new char[sizeArr];
            for (int i{0}; i < sizeArr; ++i) {
                x1[i] = _other.x1[i];
            }
        }
        catch (...)
        {
            delete[] x1;
            x1 = nullptr;
        }

        try {
#ifdef _WIN32
            std::size_t sizeArr = _msize(_other.x2);
#elif defined __linux__
            std::size_t sizeArr = malloc_usable_size(_other.x2);
#endif
            x2 = new char[sizeArr];
            for (int i{0}; i < sizeArr; ++i) {
                x2[i] = _other.x2[i];
            }
        }
        catch (...)
        {
            delete[] x2;
            x2 = nullptr;
        }
    }
    //Оператор присваивания
    SomeClass& operator=(const SomeClass& _other)
    {
        if (this != std::addressof(_other)) //Для случая SomeClass sC; sC = sC;
        {
            deepCopy(_other);
        }
        return *this;
    }
    SomeClass()
    {
        int sizeArr = 1024;
        try {
            x1 = new char[sizeArr];
            x2 = new char[sizeArr * sizeArr];
        }
        catch (...)
        {
            delete[] x1;
            delete[] x2;
            x1 = nullptr;
            x2 = nullptr;
        }
    }
    ~SomeClass()
    {
        delete[] x1;
        delete[] x2;
    }

private:
    char *x1 = nullptr;
    char *x2 = nullptr;
};




int main() {
    /*
     * 2. Т.к. Контейнеры являются потокобезопасными читать данные мы можем без каких либо последствий из
     * n - ого числа потоков.
     * Для записи возможно использовать обёртку в виде atomic(для большого размера контейнеров ультразатратно)
     * Использование мьютексов и обёрток, в частности lock_guard/scoped_lock(c++17),
     * возможно использование condition_variable для наличия уведомления об изменение общих данных, ну или семафоры(c++20).
     * Использование барьеров для разделения на несколько этапов, в данном случае этап1 - чтение, этап2 - запись.
     *
     */
    return 0;
}