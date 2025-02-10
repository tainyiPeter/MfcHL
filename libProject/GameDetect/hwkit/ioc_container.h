#pragma once
#include <unordered_map>
#include <memory>
#include <mutex>
#include <any>
#include <map>
#include <memory>
#include <functional>
#include <exception>

class ioc_container
{
public:
	ioc_container(void) {}

	virtual ~ioc_container(void)
	{
	}

	bool unregister_type(const std::string &key)
	{
		std::lock_guard<std::recursive_mutex> lk(object_mutex_);
		creator_map_.erase(key);
		objects_.erase(key);
		return true;
	}

	void clear()
	{
		std::lock_guard<std::recursive_mutex> lk(object_mutex_);
		creator_map_.clear();
		objects_.clear();
	}

	void clear_objects()
	{
		std::lock_guard<std::recursive_mutex> lk(object_mutex_);
		objects_.clear();
	}

	template <class T>
	void register_type(std::string key)
	{
		std::lock_guard<std::recursive_mutex> lk(object_mutex_);
		typedef T* I;
		std::function<I()> function = [=]() ->I  { return static_cast<I>(new T()); };
		register_type(key, function);
	}

	template <class T>
	void add_type(std::string key, std::shared_ptr<T> object)
	{
		std::lock_guard<std::recursive_mutex> lk(object_mutex_);
		objects_[key] = object;
	}

	template <class I, class T, typename... Ts>
	void register_type(std::string key)
	{
		std::lock_guard<std::recursive_mutex> lk(object_mutex_);

		std::function<I* (Ts...)> function = [](Ts... args) ->I *{ return static_cast<I *>(new T(std::forward<Ts>(args)...)); };
		register_type(key, function);
	}

	template<class T, class Sub>
	void register_type_nodepend(std::string key)
	{
		std::lock_guard<std::recursive_mutex> lk(object_mutex_);

		std::function<T* ()> function = []() { return new Sub(); };
		register_type(key, function);
	}

	template <class I>
	std::shared_ptr<I> resolve_shared(std::string key)
	{
		std::lock_guard<std::recursive_mutex> lk(object_mutex_);

		if (objects_.find(key) != objects_.end())
		{
			return std::any_cast<std::shared_ptr<I>>(objects_[key]);
		}
		else
		{
			objects_[key] = std::shared_ptr<I>(resolve<I>(key));
			return std::any_cast<std::shared_ptr<I>>(objects_[key]);
		}
	}

	template <class I, typename... Ts>
	std::shared_ptr<I> resolve_shared(std::string key, Ts... args)
	{
		std::lock_guard<std::recursive_mutex> lk(object_mutex_);
		if(objects_.find(key) != objects_.end())
		{
			return std::any_cast<std::shared_ptr<I>>(objects_[key]);
		}else
		{
			objects_[key] = std::shared_ptr<I>(resolve<I, Ts...>(key, std::forward<Ts>(args)...));
			return std::any_cast<std::shared_ptr<I>>(objects_[key]);
		}
	}

	template<class T, typename... Args>
	std::shared_ptr<T> resolve_new_object(std::string key, Args... args)
	{
		std::lock_guard<std::recursive_mutex> lk(object_mutex_);

		T* t = resolve<T>(key, args...);
		return std::shared_ptr<T>(t);
	}


	//this function will crash when the vantage starts and calls the hardware contract
	//template <class I, typename... Ts>
	//I * resolve_pointer(std::string key, Ts... args)
	//{
	//	if (objects_.find(key) != objects_.end())
	//	{
	//		return std::any_cast<std::shared_ptr<I>>(objects_[key]).get();
	//	}
	//	else
	//	{
	//		//can't use unique_ptr to assign
	//		objects_[key] = std::shared_ptr<I>(resolve<I, Ts...>(key, args...));
	//		return std::any_cast<std::shared_ptr<I>>(objects_[key]).get();
	//	}
	//}

private:
	template <class I>
	I* resolve(const std::string &key)
	{
		if (creator_map_.find(key) == creator_map_.end())
			return nullptr;

		const std::any resolver = creator_map_[key];
		auto function = std::any_cast<std::function<I* ()>>(resolver);

		return function();
	}
	template <class I, typename... Ts>
	I* resolve(const std::string &key, Ts... args)
	{
		if (creator_map_.find(key) == creator_map_.end())
			return nullptr;

		const std::any resolver = creator_map_[key];
		auto function = std::any_cast<std::function<I* (Ts...)>>(resolver);

		return function(std::forward<Ts>(args)...);
	}
	//template<typename I, typename T, typename... Ts>
	//struct construct
	//{
	//	I invoke(Ts... args) { return I(new T(args...)); }
	//};
	void register_type(const std::string &key, const std::any &constructor)
	{
		if (creator_map_.find(key) != creator_map_.end())
			return;
		creator_map_.insert(std::make_pair(key, constructor));
	}


	std::unordered_map<std::string, std::any> creator_map_;
	std::unordered_map<std::string, std::any> objects_;
	std::recursive_mutex object_mutex_;
};