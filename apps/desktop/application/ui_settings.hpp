#pragma once

#include <QFont>
#include <QSettings>
#include <memory>

class QApplication;
class QTranslator;

namespace cppproject::desktop {

class UiSettings final {
public:
    enum class Language { english, simplified_chinese };

    explicit UiSettings(QApplication& application);
    ~UiSettings();

    UiSettings(const UiSettings&) = delete;
    UiSettings& operator=(const UiSettings&) = delete;

    void apply_saved_preferences();
    void set_language(Language language);
    [[nodiscard]] Language language() const;
    [[nodiscard]] QFont application_font(int point_size) const;

private:
    void load_bundled_fonts();
    [[nodiscard]] static Language system_language();

    QApplication& application_;
    QSettings settings_;
    std::unique_ptr<QTranslator> translator_;
    QString font_family_;
};

} // namespace cppproject::desktop
