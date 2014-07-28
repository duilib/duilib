#ifndef OBSERVER_IMPL_BASE_HPP
#define OBSERVER_IMPL_BASE_HPP

#include <map>

template <typename ReturnT, typename ParamT>
class ReceiverImplBase;

template <typename ReturnT, typename ParamT>
class ObserverImplBase
{
public:
	virtual void AddReceiver(ReceiverImplBase<ReturnT, ParamT>* receiver) = 0;
	virtual void RemoveReceiver(ReceiverImplBase<ReturnT, ParamT>* receiver) = 0;
	virtual ReturnT Broadcast(ParamT param) = 0;
	virtual ReturnT Notify(ParamT param) = 0;
};

template <typename ReturnT, typename ParamT>
class ReceiverImplBase
{
public:
	virtual void AddObserver(ObserverImplBase<ReturnT, ParamT>* observer) = 0;
	virtual void RemoveObserver() = 0;
	virtual ReturnT Receive(ParamT param) = 0;
	virtual ReturnT Respond(ParamT param, ObserverImplBase<ReturnT, ParamT>* observer) = 0;
};

template <typename ReturnT, typename ParamT>
class ReceiverImpl;

template <typename ReturnT, typename ParamT>
class ObserverImpl : public ObserverImplBase<ReturnT, ParamT>
{
public:
	ObserverImpl()
		: count_(0)
	{}

	virtual ~ObserverImpl()	{}

	virtual void AddReceiver(ReceiverImplBase<ReturnT, ParamT>* receiver)
	{
		if (receiver == NULL)
			return;

		receivers_[count_] = receiver;
		receiver->AddObserver(this);
		count_++;
	}

	virtual void RemoveReceiver(ReceiverImplBase<ReturnT, ParamT>* receiver)
	{
		if (receiver == NULL)
			return;

		ReceiversMap::iterator it = receivers_.begin();
		for (; it != receivers_.end(); ++it)
		{
			if (it->second == receiver)
			{
				receivers_.erase(it);
				break;
			}
		}
	}

	virtual ReturnT Broadcast(ParamT param)
	{
		ReceiversMap::iterator it = receivers_.begin();
		for (; it != receivers_.end(); ++it)
		{
			it->second->Receive(param);
		}

		return ReturnT();
	}

	virtual ReturnT Notify(ParamT param)
	{
		ReceiversMap::iterator it = receivers_.begin();
		for (; it != receivers_.end(); ++it)
		{
			it->second->Respond(param, this);
		}

		return ReturnT();
	}

protected:
	typedef std::map<int, ReceiverImplBase<ReturnT, ParamT>*> ReceiversMap;
	ReceiversMap receivers_;
	int count_;
};


template <typename ReturnT, typename ParamT>
class ReceiverImpl : public ReceiverImplBase<ReturnT, ParamT>
{
public:
	ReceiverImpl()
		: count_(0)
	{}

	virtual ~ReceiverImpl()	{}

	virtual void AddObserver(ObserverImplBase<ReturnT, ParamT>* observer)
	{
		observers_[count_] = observer;
		count_++;
	}

	virtual void RemoveObserver()
	{
		ObserversMap::iterator it = observers_.begin();
		for (; it != observers_.end(); ++it)
		{
			it->second->RemoveReceiver(this);
		}
	}

	virtual ReturnT Receive(ParamT param)
	{
		return ReturnT();
	}

	virtual ReturnT Respond(ParamT param, ObserverImplBase<ReturnT, ParamT>* observer)
	{
		return ReturnT();
	}

protected:
	typedef std::map<int, ObserverImplBase<ReturnT, ParamT>*> ObserversMap;
	ObserversMap observers_;
	int count_;
};

#endif // OBSERVER_IMPL_BASE_HPP