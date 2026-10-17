#include "NotificationManager.h"
#include "GameObject.h"
#include <algorithm>

namespace CMPUT350 {
    static bool SameObject(const std::weak_ptr<GameObject>& a,
                           const std::weak_ptr<GameObject>& b) {
                            return !a.owner_before(b) && !b.owner_before(a);
    }


    void NotificationManager::Register(std::weak_ptr<GameObject> object, const std::string& key)
    {
        if (object.expired())
        {
            return; // nothing to register
        }

        auto& list = mListeners[key];
        
        // Don't add the same object twice
        for (const auto& existing : list) {
            if (SameObject(existing, object)) {
                return;
            }
        }
        list.push_back(object);
    }
    
    void NotificationManager::Unregister(std::weak_ptr<GameObject> object, const std::string& key)
    {
        auto it = mListeners.find(key);
        if (it == mListeners.end()) {
            return; // no one registered for this key
        }

        auto& list = it->second;
        auto toDelete = std::find_if(list.begin(), list.end(),
        [&](const std::weak_ptr<GameObject>& w) { 
            return SameObject(w, object); 
        });

        // Only remove if actually found
        if (toDelete != list.end()) {
            *toDelete = list.back(); // swap with last removal
            list.pop_back();
        }
    }

    void NotificationManager::Notify(const std::string& message)
    {
        auto it = mListeners.find(message);
        if (it == mListeners.end()) {
            return;
        }

        // Iterate over a copy, so a listener can safely Register/Unregister
        // from inside ReceiveNotifiction without breaking this loop
        std::vector<std::weak_ptr<GameObject>> listeners = it->second;
        for (auto& w : listeners) {
            // lock() returns a nullptr if expired
            if (auto obj = w.lock()) {
                obj->ReceiveNotification(message);
            }
        
        }

        // clean out dead listeners in one pass, after the loop is done
        auto& list = mListeners[message];
        list.erase(std::remove_if(list.begin(), list.end(), 
                                    [](const std::weak_ptr<GameObject>& w) {
                                        return w.expired();
                                    }),
                                list.end());


    }

}   // namespace CMPUT350