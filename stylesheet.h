#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <QString>


struct StyleSheet
{
    StyleSheet(const QString &main_color="rgb(188, 212, 230)"
               , const QString &second_color="rgb(214, 226, 233)"
               , const QString &hover_color="rgb(153, 193, 222)"
               , const QString &text_color="black"
               , const QString &border_color="rgb(153, 193, 222)")
        : main_color_(main_color)
        , second_color_(second_color)
        , hover_color_(hover_color)
        , text_color_(text_color)
        , border_color_(border_color) {}

    [[nodiscard]] QString pushButton_styleSheet(int font_size=11) const;

    [[nodiscard]] QString toolButton_styleSheet(int font_size=11) const;

    [[nodiscard]] QString comboBox_styleSheet(int font_size=11) const;

    [[nodiscard]] QString menu_styleSheet(int font_size=11) const;

    [[nodiscard]] QString menuBar_styleSheet(int font_size=11) const;

    [[nodiscard]] QString table_styleSheet(int font_size=11) const;

    [[nodiscard]] QString lineEdit_styleSheet(int font_size=11) const;

    [[nodiscard]] QString label_styleSheet(int font_size=11) const;

    [[nodiscard]] QString window_styleSheet() const;

    [[nodiscard]] QString messageBox_styleSheet(int font_size=11) const;

    [[nodiscard]] QString listWidget_styleSheet(int font_size=11) const;

    [[nodiscard]] QString textEdit_styleSheet(int font_size=11) const;

private:
    QString main_color_;
    QString second_color_;
    QString hover_color_;
    QString text_color_;
    QString border_color_;
};

#endif // STYLESHEET_H
