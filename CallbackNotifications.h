#pragma once
#include <cstdint>
#include <list>
#include <algorithm>

namespace notifications {

//- ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Callback is the generic template class that handle callback.
// It is used to store callback function istelf

template <typename, class... Args>
struct Callback {
    using func_t = std::function<void(Args ...)>;

    constexpr Callback(const func_t& func)
            : _func(func) {
    }
    constexpr Callback(func_t&& func)
            : _func(std::move(func)) {
    }

    constexpr void operator()(Args... args) const {
        _func(std::forward<Args>(args)...);
    }

private:
    func_t _func;
};

// Callbacks is the generic template class that handle callbacks.
// It is used to register/deregister callback function
// It gives an ID to the callback function. this id is then used to unregister the callback.

template<class TAG, class... Args>
class Callbacks
{
public:
    using type_t = TAG;
    using id_t = std::uintmax_t;
    using callback_t = Callback<void, Args...>;
    using func_t = typename callback_t::func_t;
    static constexpr id_t InvalidId{0};

public:
    static Callbacks<TAG, Args...> & instance()
    { static Callbacks<TAG, Args...> _; return _; }

public:
    constexpr bool empty() const
    {
        return _stored.empty();
    }

    id_t add( callback_t cb )
    {
        const auto result = getNextId();
        _stored.emplace_back(result, cb);
        return result;
    }

    id_t add( typename callback_t::func_t cb )
    {
        return add(callback_t{cb});
    }

    bool remove( id_t id)
    {
        auto found = std::find_if( _stored.begin(), _stored.end(), [id](const typename Stored::value_type & v)->bool
            { return v.first == id; }
        );

        if (found == _stored.end())
            return false;

        _stored.erase(found);
        return true;
    }

    void operator()(Args... args) const
    {
        for (auto & cb : _stored)
            cb.second(std::forward<Args>(args)...);
    }

protected:
    using Stored = std::list<std::pair<id_t, callback_t>>;
    Stored _stored;

private:
    constexpr Callbacks() {}
    Callbacks(const Callbacks & ) = delete;
    Callbacks(Callbacks && ) = delete;
    Callbacks & operator=(const Callbacks & ) = delete;

    static inline id_t getNextId() {
        static id_t _{0};
        return ++_;
    }
};

template<class Callbacks>
class TScopedListener
{
public:
    using func_t = typename Callbacks::func_t;
    using id_t = typename Callbacks::id_t;
    using callback_t = typename Callbacks::callback_t;
    using callbacks_t = Callbacks;

public:
    TScopedListener(func_t cb)
    :   _id{ callbacks_t::instance().add(cb) }
    {}

    ~TScopedListener()
    { callbacks_t::instance().remove(_id); }

private:
    id_t _id;
};

}
