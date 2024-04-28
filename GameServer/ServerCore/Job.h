#pragma once
#include <functional>

/*---------
	Job
----------*/

using CallbackType = std::function<void()>;

class Job
{
public:
	Job(CallbackType&& callback) : _callback(std::move(callback))
	{
	}

	template<typename T, typename Ret, typename... Args>
	Job(shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		_callback = [owner, memFunc, args...]()
		{
			(owner.get()->*memFunc)(args...);
		};
	}
	// ĸ�ĸ� �ϰ� ���͸� ����� � �ƴϸ� �׳� Ŭ�������� ��� �ְ� shared_ptr�� 
	// ����ִ¼��� ������������ ������ �ֱ�� ������ ����Ŭ�� �Ͼ���ʰ�(�޸𸮸�)�� �����ؾ��Ѵ�.
	
	void Execute()
	{
		_callback();
	}

private:
	CallbackType _callback;
};

