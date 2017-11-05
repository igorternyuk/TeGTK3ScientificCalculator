#include "mainwindow.h"
#include "parser.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>

const ButtonData MainWindow::buttons_data [] = {
    {"C",    1, 0 }, {"CE",    1, 1 }, {"(",     1, 2 }, {")",   1, 3 }, {"<",  1, 4 }, {"abs",     1, 5 },
    {"log8", 2, 0 }, {"log16", 2, 1 }, {"<=",    2, 2 }, {">=",  2, 3 }, {">",  2, 4 }, {"mod",     2, 5 },
    {"x^2",  3, 0 }, {"x^3",   3, 1 }, {"sqrt",  3, 2 }, {"cbrt",3, 3 }, {"==", 3, 4 }, {"sin",     3, 5 },
    {"ln",   4, 0 }, {"log2",  4, 1 }, {"log10", 4, 2 }, {"exp", 4, 3 }, {"!=", 4, 4 }, {"cos",     4, 5 },
    {"7",    5, 0 }, {"8",     5, 1 }, {"9",     5, 2 }, {"9",   5, 3 }, {"&",  5, 4 }, {"tg",      5, 5 },
    {"4",    6, 0 }, {"5",     6, 1 }, {"6",     6, 2 }, {"*",   6, 3 }, {"|",  6, 4 }, {"ctg",     6, 5 },
    {"1",    7, 0 }, {"2",     7, 1 }, {"3",     7, 2 }, {"-",   7, 3 }, {"!",  7, 4 }, {"sec",     7, 5 },
    {"0",    8, 0 }, {".",     8, 1 }, {"=",     8, 2 }, {"+",   8, 3 }, {"n!", 8, 4 }, {"cosec",   8, 5 },
};

MainWindow::MainWindow()
{
    m_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_resizable(GTK_WINDOW(m_window), false);
    gtk_window_set_title(GTK_WINDOW(m_window), "TeGTK3Calculator");
    m_inputEntry = gtk_entry_new();

    m_lbl_angle_units = gtk_label_new("Angle units:");
    //Combobox
    m_combo_angle_units = gtk_combo_box_text_new();
    const char *angle_units[] = {"radians", "degrees", "grads"};
    for (std::size_t i {0u}; i < G_N_ELEMENTS (angle_units); i++){
        gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(m_combo_angle_units), angle_units[i]);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combo_angle_units), 0);

    //Checkboxes
    m_checkbox_hyp = gtk_check_button_new_with_label("Hyp");
    m_checkbox_arc = gtk_check_button_new_with_label("Arc");
    m_lbl_prec = gtk_label_new("Precision:");
    m_prec_spin_btn = gtk_spin_button_new_with_range(1,15,1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_prec_spin_btn), 6);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_box_pack_start(GTK_BOX(box),m_lbl_prec, true, false, 0);
    gtk_box_pack_start(GTK_BOX(box),m_prec_spin_btn, true, false, 0);
    gtk_box_pack_start(GTK_BOX(box), m_lbl_angle_units, true, false, 0);
    gtk_box_pack_start(GTK_BOX(box), m_combo_angle_units, true, false, 0);
    gtk_box_pack_start(GTK_BOX(box), m_checkbox_hyp, true, false, 0);
    gtk_box_pack_start(GTK_BOX(box), m_checkbox_arc, true, false, 0);

    m_table = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(m_table), TRUE);    
    gtk_grid_set_column_spacing(GTK_GRID (m_table), 6);
    gtk_grid_set_row_spacing (GTK_GRID (m_table), 6);

    GtkWidget *mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(mainBox), m_inputEntry, false, false, 0);
    gtk_box_pack_start(GTK_BOX(mainBox), box, false, false, 0);
    gtk_box_pack_start(GTK_BOX(mainBox), m_table, false, false, 0);

    for(auto data: buttons_data){
        GtkWidget *button = gtk_button_new_with_label(data.text);
        gtk_widget_set_hexpand (button, TRUE);
        gtk_widget_set_halign (button, GTK_ALIGN_BASELINE);
        gtk_grid_attach (GTK_GRID (m_table), button, data.col, data.row, 1, 1);
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_btn_clicked), this);
    }

    gtk_container_add(GTK_CONTAINER(m_window), mainBox);
    g_signal_connect(G_OBJECT(m_window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect (G_OBJECT(m_window), "key-press-event",
                        G_CALLBACK (key_release_event), this);
}

void MainWindow::show()
{
    gtk_widget_show_all(m_window);
}

void MainWindow::showMessage(GtkMessageType type, const char* msg)
{
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    auto dialog = gtk_message_dialog_new (GTK_WINDOW(m_window),
                                     flags,
                                     type,
                                     GTK_BUTTONS_OK,
                                     msg);
    g_signal_connect_swapped (dialog, "response",
                              G_CALLBACK (gtk_widget_destroy),
                              dialog);
    gtk_dialog_run (GTK_DIALOG (dialog));
}

void MainWindow::calculateExpression()
{
    std::string input(gtk_entry_get_text(GTK_ENTRY(m_inputEntry)));
    std::string angle_units(getAngleUnits());
    if(input.empty()){
        showMessage(GTK_MESSAGE_ERROR, "Empty input!");
    }
    else {
        std::vector<std::pair<char, double>> vars;
        Parser p(input, vars, angle_units);
        try
        {
            auto result = p.calculateExpression();
            std::ostringstream oss;
            int prec = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_prec_spin_btn));
            oss.precision(prec);
            oss << std::fixed;
            oss << result;
            gtk_entry_set_text(GTK_ENTRY(m_inputEntry), oss.str().c_str());

        }
        catch (std::exception &ex)
        {
            showMessage(GTK_MESSAGE_ERROR, ex.what());
        }
    }
}

std::string MainWindow::getAngleUnits()
{
    std::string angle_units;
    int au = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combo_angle_units));
    switch(au){
    case 1:
        angle_units = "gradus";
        break;
    case 2:
        angle_units = "grad";
        break;
    case 0:
    default:
        angle_units = "radian";
        break;
    }
    return angle_units;
}

void on_btn_clicked(GtkWidget*button, MainWindow *data)
{
    std::string input(gtk_entry_get_text(GTK_ENTRY(data->m_inputEntry)));
    std::string button_text(gtk_button_get_label(GTK_BUTTON(button)));

    std::string angle_units = data->getAngleUnits();

    bool is_hyp_mode_active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->m_checkbox_hyp));
    bool is_arc_mode_active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->m_checkbox_arc));

    if(button_text == "="){
        data->calculateExpression();
    }
    else if(button_text == "CE")
    {
        int cursor_pos = gtk_editable_get_position( GTK_EDITABLE(data->m_inputEntry) );
        if(input.size() > 0 &&  cursor_pos > 0){
            auto left_part = input.substr(0, cursor_pos - 1);
            auto right_part = input.substr(cursor_pos, input.size());
            auto res_expr = left_part + right_part;
            gtk_entry_set_text(GTK_ENTRY(data->m_inputEntry), res_expr.c_str());
        }
        gtk_widget_grab_focus(data->m_inputEntry);
        gtk_editable_set_position( GTK_EDITABLE(data->m_inputEntry), cursor_pos - 1);
    }
    else if(button_text == "C")
    {
       gtk_entry_set_text(GTK_ENTRY(data->m_inputEntry), "");       
    }
    else if(button_text == "sin")
    {
        if(is_hyp_mode_active)
        {
            input += is_arc_mode_active ? "arcsh" : "sh";
        }
        else
        {
            input += is_arc_mode_active ? "arcsin": "sin";
        }
        gtk_entry_set_text(GTK_ENTRY(data->m_inputEntry), input.c_str());
    }
    else if(button_text == "cos")
    {
        if(is_hyp_mode_active)
        {
            input += is_arc_mode_active ? "arcch" : "ch";
        }
        else
        {
            input += is_arc_mode_active ? "arccos": "cos";
        }
        gtk_entry_set_text(GTK_ENTRY(data->m_inputEntry), input.c_str());

    }
    else if(button_text == "tg")
    {
        if(is_hyp_mode_active)
        {
            input += is_arc_mode_active ? "arcth" : "th";
        }
        else
        {
            input += is_arc_mode_active ? "arctg": "tg";
        }
        gtk_entry_set_text(GTK_ENTRY(data->m_inputEntry), input.c_str());

    } else if(button_text == "ctg")
    {
        if(is_hyp_mode_active)
        {
            input += is_arc_mode_active ? "arcth" : "arccth";
        }
        else
        {
            input += is_arc_mode_active ? "arcctg": "ctg";
        }
        gtk_entry_set_text(GTK_ENTRY(data->m_inputEntry), input.c_str());

    }
    else if(button_text == "sec")
    {
        if(is_hyp_mode_active)
        {
            input += is_arc_mode_active ? "arcsech" : "sech";
        }
        else
        {
            input += is_arc_mode_active ? "arcsecans": "secans";
        }
        gtk_entry_set_text(GTK_ENTRY(data->m_inputEntry), input.c_str());

    }
    else if(button_text == "cosec")
    {
        if(is_hyp_mode_active)
        {
            input += is_arc_mode_active ? "arccsech" : "csech";
        }
        else
        {
            input += is_arc_mode_active ? "arccsecans": "csecans";
        }
        gtk_entry_set_text(GTK_ENTRY(data->m_inputEntry), input.c_str());

    }
    else if(button_text == "exp")
    {
        input += "_exp";
        gtk_entry_set_text(GTK_ENTRY(data->m_inputEntry), input.c_str());
    }
    else if(button_text == "n!")
    {
        input += "factorial";
        gtk_entry_set_text(GTK_ENTRY(data->m_inputEntry), input.c_str());
    }
    else if(button_text == "x^2")
    {
        input += "sqr";
        gtk_entry_set_text(GTK_ENTRY(data->m_inputEntry), input.c_str());
    }
    else if(button_text == "x^3")
    {
        input += "cube";
        gtk_entry_set_text(GTK_ENTRY(data->m_inputEntry), input.c_str());
    }
    else
    {
        input += button_text;
        gtk_entry_set_text(GTK_ENTRY(data->m_inputEntry), input.c_str());
    }
    //gtk_widget_grab_focus(data->m_inputEntry);
}

gboolean key_release_event(GtkWidget*, GdkEventKey *event, MainWindow *data)
{
    if (event->keyval == GDK_KEY_Return){
        std::cout << "Enter was pressed" << std::endl;
        data->calculateExpression();
        return TRUE;
    }
    return FALSE;
}
