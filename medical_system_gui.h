#ifndef MEDICAL_SYSTEM_GUI_H
#define MEDICAL_SYSTEM_GUI_H

#include <gtk/gtk.h>
#include "backend.h"

#define APP_NAME "PKU UTHM Medical System"
#define WINDOW_WIDTH 1350
#define WINDOW_HEIGHT 850
#define SIDEBAR_WIDTH 220
#define LOGIN_WIDTH 420
#define LOGIN_HEIGHT 580

extern GtkWidget *main_window;
extern GtkWidget *content_stack;
extern GtkWidget *sidebar_box;
extern GtkWidget *status_bar;
extern GtkWidget *login_window;
extern GtkWidget *login_spinner;
extern GtkWidget *login_error_label;
extern GtkWidget *username_entry;
extern GtkWidget *password_entry;
extern char current_username[50];

void setup_css(void);
void show_notification(const char *message, const char *type);
void show_toast(const char *msg);
void show_confirmation(const char *title, const char *msg, GCallback cb, gpointer data);
void switch_to_view(const char *name);
void update_status(const char *msg);

GtkWidget* create_card(const char *title);
GtkWidget* form_row(const char *label, GtkWidget *widget);
GtkWidget* badge(const char *text, const char *type);

void create_login_window(void);
void on_main_window_closed(void);
void create_main_window(void);
void do_logout(void);
void confirm_delete_patient(void);
void on_patient_save(GtkButton *btn, gpointer data);
void on_appointment_save(GtkButton *btn, gpointer data);
void on_patient_id_changed(GtkEntry *e, gpointer data);
void on_dept_changed(GtkComboBox *dept, gpointer data);
void on_search_execute(GtkButton *btn, gpointer data);
void on_queue_next(GtkButton *btn, gpointer data);
void on_queue_add(GtkButton *btn, gpointer data);
void on_calendar_changed(GtkCalendar *cal, gpointer data);
void on_appointment_book(GtkButton *btn, gpointer data);
void on_manage_users(GtkButton *btn, gpointer data);
void on_backup(GtkButton *btn, gpointer data);
void on_restore(GtkButton *btn, gpointer data);
void do_restore(void);
gboolean draw_chart(GtkWidget *w, cairo_t *cr, gpointer data);
void populate_patient_treeview(GtkWidget *tv);
void populate_appointment_list(GtkWidget *list, const char *date);
void update_queue_display(void);
void update_dashboard_stats(void);

#endif
