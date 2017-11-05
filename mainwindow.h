#pragma once
#include <gtk/gtk.h>
#include <string>

typedef struct {

    const char *text;    /* --- Label display on button --- */
    int         row;     /* --- Row to place the button --- */
    int         col;      /* --- Column to place the button --- */

} ButtonData;

class MainWindow
{
public:
    explicit MainWindow();
    void show();
private:
    const static ButtonData buttons_data [];
    GtkWidget *m_window;
    GtkWidget *m_lbl_prec;
    GtkWidget *m_prec_spin_btn;
    GtkWidget *m_combo_angle_units;
    GtkWidget *m_lbl_angle_units;
    GtkWidget *m_checkbox_hyp;
    GtkWidget *m_checkbox_arc;
    GtkWidget *m_inputEntry;
    GtkWidget *m_table;
    void showMessage(GtkMessageType type, const char *msg);
    void calculateExpression();
    std::string getAngleUnits();
    friend void on_btn_clicked(GtkWidget*button, MainWindow *data);
    friend gboolean key_release_event(GtkWidget*, GdkEventKey *event, MainWindow *data);
};

void on_btn_clicked(GtkWidget*button, MainWindow *data);
gboolean key_release_event(GtkWidget *, GdkEventKey *event, MainWindow *data);


