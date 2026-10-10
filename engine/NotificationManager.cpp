#include "NotificationManager.h"
#include "GameObject.h"
#include <algorithm>

namespace CMPUT350 {

    void NotificationManager::Register(std::weak_ptr<GameObject> object, const std::string& key)
    {
        if (!object.expired())
        {
            auto temp = object.lock();
            mListeners[key].push_back(temp);
        }
        else
        {
            auto toDelete = std::find(mListeners[key].begin(), mListeners[key].end(), object);
            int intToDelete = std::distance(mListeners[key].begin(), toDelete);
            mListeners[key].at(intToDelete) = mListeners[key].back();
            mListeners[key].pop_back();
        }
    }
    
    void NotificationManager::Unregister(std::weak_ptr<GameObject> object, const std::string& key)
    {
        auto toDelete = std::find(mListeners[key].begin(), mListeners[key].end(), object);
        int intToDelete = std::distance(mListeners[key].begin(), toDelete);
        mListeners[key].at(intToDelete) = mListeners[key].back();
        mListeners[key].pop_back();
    }

    void NotificationManager::Notify(const std::string& message)
    {
        for (auto object : mListeners[message])
        {
            if (!object.expired())
            {
                std::shared_ptr<CMPUT350::GameObject> temp = object.lock();
                temp->ReceiveNotification(message);
            }
            else
            {
                auto toDelete = std::find(mListeners[message].begin(), mListeners[message].end(), object);
                int intToDelete = std::distance(mListeners[message].begin(), toDelete);
                mListeners[message].at(intToDelete) = mListeners[message].back();
                mListeners[message].pop_back();
            }
        }
    }

}   // namespace CMPUT350