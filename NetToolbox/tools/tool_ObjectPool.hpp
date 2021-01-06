#ifndef __TOOL_OBJECT_POOL_HPP__
#define __TOOL_OBJECT_POOL_HPP__

#include <memory>
#include <vector>
#include <mutex>

template<typename T, bool _is_lock = true>
class tool_ObjectPool {
public:
	std::unique_ptr<T>&& get_object () {
		auto _get_object = [this] () -> std::unique_ptr<T>&& {
			size_t _size = m_pool.size ();
			if (_size > 0) {
				std::unique_ptr<T> &&_ret = std::move (m_pool [_size - 1]);
				m_pool.resize (_size - 1);
				return _ret;
			}
			return std::move (std::make_unique<T> ());
		};
		if constexpr (_is_lock) {
			std::unique_lock<std::mutex> _ul (m_mutex);
			_get_object ();
		} else {
			_get_object ();
		}
	}

	void free_object (std::unique_ptr<T> &&_ptr) {
		if constexpr (_is_lock) {
			std::unique_lock<std::mutex> _ul (m_mutex);
			m_pool.emplace_back (_ptr);
		} else {
			m_pool.emplace_back (_ptr);
		}
	}

	void clear_memory () {
		if constexpr (_is_lock) {
			std::unique_lock<std::mutex> _ul (m_mutex);
			std::vector<std::unique_ptr<T>> _pool;
			_pool.swap (m_pool);
		} else {
			std::vector<std::unique_ptr<T>> _pool;
			_pool.swap (m_pool);
		}
	}

	std::vector<std::unique_ptr<T>> m_pool;
	std::mutex m_mutex;
};

#endif //__TOOL_OBJECT_POOL_HPP__
