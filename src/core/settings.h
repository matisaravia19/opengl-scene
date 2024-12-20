#pragma once

#include <vector>

class SettingsObserver;

// Plain class implementing Observer pattern.
class Settings {
  std::vector<SettingsObserver*> subscribers;

  struct SetingsData {
    bool full_screen = false;
  } settingsData;

public:
  void subscribe(SettingsObserver* observer);
  void unsubscribe(SettingsObserver* observer);
  void notify();

  SetingsData& getSettingsData() { return settingsData; }

  void setFullscreen(bool fs);

  static Settings *ActiveSettings;
};

// Interface implemented by oberver subscribers.
class SettingsObserver {
public:
  virtual ~SettingsObserver() = default;
  virtual void notify(Settings* settings) = 0;
};