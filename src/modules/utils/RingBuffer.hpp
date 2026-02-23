#pragma once
#include <cstddef>
#include <cstdint>
#include <utility>

/// @brief A fixed-size circular buffer that overwrites old elements when full.
template <typename T, typename Alloc = std::allocator<T>>
class RingBuffer {
public:
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using const_pointer = T const*;
    using const_reference = T const&;
    using allocator_type = Alloc;
    using allocator_traits = std::allocator_traits<Alloc>;
    using size_type = size_t;

    explicit constexpr RingBuffer(size_type n, Alloc const& alloc = Alloc()) noexcept(std::is_nothrow_constructible_v<Alloc>)
        : m_alloc(alloc),
          m_allocStart(allocator_traits::allocate(m_alloc, n)),
          m_allocEnd(m_allocStart + n),
          m_start(m_allocStart),
          m_end(m_allocStart),
          m_size(0),
          m_capacity(n) {}

    constexpr ~RingBuffer() noexcept(std::is_nothrow_destructible_v<T>) {
        if (m_allocStart) {
            this->destroy_all();
            allocator_traits::deallocate(m_alloc, m_allocStart, m_capacity);
        }
    }

    constexpr RingBuffer(RingBuffer const& other)
        : m_alloc(allocator_traits::select_on_container_copy_construction(other.m_alloc)),
          m_allocStart(allocator_traits::allocate(m_alloc, other.m_capacity)),
          m_allocEnd(m_allocStart + other.m_capacity),
          m_start(m_allocStart),
          m_end(m_allocStart),
          m_size(0),
          m_capacity(other.m_capacity)
    {
        for (size_type i = 0; i < other.m_size; ++i) {
            pointer p = other.m_start + i;
            if (p >= other.m_allocEnd) p -= other.m_capacity;
            this->push_back(*p);
        }
    }

    RingBuffer& operator=(RingBuffer const& other) {
        if (this != &other) {
            if (m_allocStart) {
                this->destroy_all();
                allocator_traits::deallocate(m_alloc, m_allocStart, m_capacity);
            }
            m_alloc = allocator_traits::select_on_container_copy_construction(other.m_alloc);
            m_allocStart = allocator_traits::allocate(m_alloc, other.m_capacity);
            m_allocEnd = m_allocStart + other.m_capacity;
            m_start = m_allocStart;
            m_end = m_allocStart;
            m_size = 0;
            m_capacity = other.m_capacity;

            for (size_type i = 0; i < other.m_size; ++i) {
                pointer p = other.m_start + i;
                if (p >= other.m_allocEnd) p -= other.m_capacity;
                this->push_back(*p);
            }
        }
        return *this;
    }

    constexpr RingBuffer(RingBuffer&& other) noexcept
        : m_alloc(std::move(other.m_alloc)),
          m_allocStart(std::exchange(other.m_allocStart, nullptr)),
          m_allocEnd(std::exchange(other.m_allocEnd, nullptr)),
          m_start(std::exchange(other.m_start, nullptr)),
          m_end(std::exchange(other.m_end, nullptr)),
          m_size(std::exchange(other.m_size, 0)),
          m_capacity(std::exchange(other.m_capacity, 0)) {}

    RingBuffer& operator=(RingBuffer&& other) noexcept {
        if (m_allocStart) {
            this->destroy_all();
            allocator_traits::deallocate(m_alloc, m_allocStart, m_capacity);
        }
        m_alloc = std::move(other.m_alloc);
        m_allocStart = std::exchange(other.m_allocStart, nullptr);
        m_allocEnd = std::exchange(other.m_allocEnd, nullptr);
        m_start = std::exchange(other.m_start, nullptr);
        m_end = std::exchange(other.m_end, nullptr);
        m_size = std::exchange(other.m_size, 0);
        m_capacity = std::exchange(other.m_capacity, 0);
        return *this;
    }

    template <typename... Args>
    constexpr void emplace_back(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>) {
        if (m_size == m_capacity) {
            std::destroy_at(m_end);
        }

        std::construct_at(m_end, std::forward<Args>(args)...);
        if (++m_end == m_allocEnd) {
            m_end = m_allocStart;
        }

        if (m_size == m_capacity) {
            if (++m_start == m_allocEnd) {
                m_start = m_allocStart;
            }
        } else {
            ++m_size;
        }
    }

    constexpr void push_back(T const& value) noexcept(std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_destructible_v<T>) {
        this->emplace_back(value);
    }

    constexpr void push_back(T&& value) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_destructible_v<T>) {
        this->emplace_back(std::move(value));
    }

    constexpr void pop_front() noexcept(std::is_nothrow_destructible_v<T>) {
        if (m_size == 0) return;
        std::destroy_at(m_start);
        if (++m_start == m_allocEnd) {
            m_start = m_allocStart;
        }
        --m_size;
    }

    constexpr void pop_back() noexcept(std::is_nothrow_destructible_v<T>) {
        if (m_size == 0) return;
        if (m_end == m_allocStart) m_end = m_allocEnd;
        --m_end;
        std::destroy_at(m_end);
        --m_size;
    }

    constexpr void clear() noexcept(std::is_nothrow_destructible_v<T>) {
        this->destroy_all();
        m_start = m_allocStart;
        m_end = m_allocStart;
        m_size = 0;
    }

    constexpr void resize(size_type new_capacity) {
        pointer new_alloc = allocator_traits::allocate(m_alloc, new_capacity);

        size_type new_size = std::min(m_size, new_capacity);
        size_type offset = m_size - new_size;

        for (size_type i = 0; i < new_size; ++i) {
            pointer src = m_start + offset + i;
            if (src >= m_allocEnd) src -= m_capacity;
            std::construct_at(new_alloc + i, std::move(*src));
        }

        destroy_all();
        if (m_allocStart) {
            allocator_traits::deallocate(m_alloc, m_allocStart, m_capacity);
        }

        m_allocStart = new_alloc;
        m_allocEnd = new_alloc + new_capacity;
        m_start = new_alloc;
        m_end = new_alloc + new_size;
        m_size = new_size;
        m_capacity = new_capacity;
    }

    constexpr reference front() noexcept { return *m_start; }
    constexpr const_reference front() const noexcept { return *m_start; }
    constexpr reference back() noexcept { return *(m_end == m_allocStart ? m_allocEnd - 1 : m_end - 1); }
    constexpr const_reference back() const noexcept { return *(m_end == m_allocStart ? m_allocEnd - 1 : m_end - 1); }

    constexpr size_type size() const noexcept { return m_size; }
    constexpr size_type capacity() const noexcept { return m_capacity; }
    constexpr bool empty() const noexcept { return m_size == 0; }
    constexpr bool full() const noexcept { return m_size == m_capacity; }

    template <bool IsConst>
    struct iterator_impl {
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = std::conditional_t<IsConst, T const*, T*>;
        using reference         = std::conditional_t<IsConst, T const&, T&>;
        using buffer_ptr        = std::conditional_t<IsConst, RingBuffer const*, RingBuffer*>;

    private:
        buffer_ptr m_rb = nullptr;
        pointer m_ptr = nullptr;
        size_type m_index = 0;

        friend class RingBuffer;
        constexpr iterator_impl(buffer_ptr rb, pointer ptr, size_type index) noexcept
            : m_rb(rb), m_ptr(ptr), m_index(index) {}

    public:
        constexpr iterator_impl() = default;

        constexpr reference operator*() const noexcept { return *m_ptr; }
        constexpr pointer operator->() const noexcept { return m_ptr; }

        constexpr iterator_impl& operator++() noexcept {
            ++m_index;
            if (++m_ptr == m_rb->m_allocEnd)
                m_ptr = m_rb->m_allocStart;
            return *this;
        }

        constexpr iterator_impl operator++(int) noexcept {
            iterator_impl temp = *this;
            ++(*this);
            return temp;
        }

        constexpr bool operator==(iterator_impl const& other) const noexcept {
            return m_index == other.m_index;
        }

        constexpr bool operator!=(iterator_impl const& other) const noexcept {
            return !(*this == other);
        }
    };

    using iterator = iterator_impl<false>;
    using const_iterator = iterator_impl<true>;

    constexpr iterator begin() noexcept { return iterator(this, m_start, 0); }
    constexpr iterator end() noexcept { return iterator(this, m_end, m_size); }
    constexpr const_iterator begin() const noexcept { return const_iterator(this, m_start, 0); }
    constexpr const_iterator end() const noexcept { return const_iterator(this, m_end, m_size); }
    constexpr const_iterator cbegin() const noexcept { return begin(); }
    constexpr const_iterator cend() const noexcept { return end(); }

    constexpr reference operator[](size_type index) noexcept {
        pointer p = m_start + index;
        if (p >= m_allocEnd) p -= m_capacity;
        return *p;
    }

    constexpr const_reference operator[](size_type index) const noexcept {
        pointer p = m_start + index;
        if (p >= m_allocEnd) p -= m_capacity;
        return *p;
    }

private:
    constexpr void destroy_all() noexcept(std::is_nothrow_destructible_v<T>) {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            for (size_type i = 0; i < m_size; ++i) {
                pointer p = m_start + i;
                if (p >= m_allocEnd) p -= m_capacity;
                std::destroy_at(p);
            }
        }
    }

private:
    GEODE_NO_UNIQUE_ADDRESS Alloc m_alloc;
    pointer m_allocStart;
    pointer m_allocEnd;
    pointer m_start;
    pointer m_end;
    size_type m_size;
    size_type m_capacity;
};