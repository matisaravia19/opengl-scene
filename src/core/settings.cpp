#include "settings.h"

void Settings::subscribe(SettingsObserver* observer) {
    subscribers.push_back(observer);
}

void Settings::unsubscribe(SettingsObserver* observer) {
    std::erase(subscribers, observer);
}

void Settings::notify() {
    for (const auto observer : subscribers) {
        observer->notify(this);
    }
}