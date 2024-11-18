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

void Settings::setFullscreen(const bool fs) {
    const bool changed = settingsData.full_screen != fs;
    settingsData.full_screen = fs;
    if (changed) notify();
}