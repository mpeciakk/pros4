#include <MM/kmalloc.hpp>
template <typename T, u32 initialCapacity = 8> class Vector {
private:
    u32 capacity = 8;
    u32 size = 0;

    T* data;

public:
    Vector() : capacity(initialCapacity), data(new T[capacity]) { }

    ~Vector() {
        clear();
    }

    T* begin() const {
        return data;
    }

    T* end() const {
        return data + size;
    }

    void append(T elem) {
        ensureCapacity(size + 1);
        data[size++] = elem;
    }

    T takeLast() {
        return data[--size];
    }

    T& operator[](u32 ind) const
    {
        return data[ind];
    }

    void clear() {
        delete[] data;
    }

private:
    // TODO: better performance
    void ensureCapacity(u32 desiredCapacity) {
        if (desiredCapacity <= capacity) {
            return;
        }

        T* temp = new T[desiredCapacity];

//        T* temp = (T*) kmalloc(sizeof(T) * desiredCapacity);

        for (int i = 0; i < capacity; i++) {
            temp[i] = data[i];
        }

        delete[] data;
        capacity = desiredCapacity;
        data = temp;
    }
};