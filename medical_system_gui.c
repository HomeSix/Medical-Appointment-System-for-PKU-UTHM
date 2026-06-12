/*
 * medical_system_gui.c - GTK+3 GUI for PKU UTHM Medical System
 * Group 4 | BIK10903 Data Structure | Sem 2 2025/2026
 *
 * All backend data operations are in medical_system.c.
 * This file handles the GTK interface, calling backend functions.
 */

#include "medical_system_gui.h"
#include <math.h>

/* ==================== GLOBAL WIDGETS ==================== */
GtkWidget *main_window = NULL;
GtkWidget *content_stack = NULL;
GtkWidget *sidebar_box = NULL;
GtkWidget *status_bar = NULL;
GtkWidget *login_window = NULL;
GtkWidget *login_spinner = NULL;
GtkWidget *login_error_label = NULL;
GtkWidget *username_entry = NULL;
GtkWidget *password_entry = NULL;
char current_username[50] = "";
char current_role[20] = "";

/* ==================== CSS & THEME ==================== */

void setup_css(void) {
    GtkCssProvider *prov = gtk_css_provider_new();
    GError *err = NULL;
    gtk_css_provider_load_from_path(prov, "resources/css/hospital_theme.css", &err);
    if (err) {
        g_warning("CSS: %s", err->message);
        g_error_free(err);
        const gchar *fallback =
            "window{background:#f8f9fa}"
            "stack,scrolledwindow,.content-area{background:#f8f9fa}"
            "headerbar{background:#0066CC;color:white}"
            "button{border-radius:8px;padding:8px 18px;color:#343A40;background:#fff;border:2px solid #dee2e6}"
            "entry{border-radius:8px;border:2px solid #dee2e6;padding:8px}"
            "entry:focus{border-color:#0066CC}"
            "treeview row:selected{background:#0066CC;color:white}"
            "treeview header button{background:#0066CC;color:white;font-weight:bold}";
        gtk_css_provider_load_from_data(prov, fallback, -1, NULL);
    }
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(prov),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(prov);
}

/* ==================== UTILITIES ==================== */

GtkWidget* create_card(const char *title) {
    GtkWidget *f = gtk_frame_new(NULL);
    gtk_style_context_add_class(gtk_widget_get_style_context(f), "card");
    if (title) {
        GtkWidget *l = gtk_label_new(title);
        gtk_style_context_add_class(gtk_widget_get_style_context(l), "card-title");
        gtk_widget_set_margin_start(l, 12);
        gtk_widget_set_margin_top(l, 8);
        gtk_frame_set_label_widget(GTK_FRAME(f), l);
    }
    return f;
}

GtkWidget* form_row(const char *label, GtkWidget *widget) {
    GtkWidget *b = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    GtkWidget *l = gtk_label_new(label);
    gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(l), "dim-label");
    gtk_box_pack_start(GTK_BOX(b), l, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(b), widget, FALSE, FALSE, 0);
    return b;
}

GtkWidget* badge(const char *text, const char *type) {
    GtkWidget *l = gtk_label_new(text);
    gchar *c = g_strdup_printf("badge-%s", type);
    gtk_style_context_add_class(gtk_widget_get_style_context(l), c);
    g_free(c);
    return l;
}

void update_status(const char *msg) {
    if (status_bar) {
        guint id = gtk_statusbar_get_context_id(GTK_STATUSBAR(status_bar), "main");
        gtk_statusbar_push(GTK_STATUSBAR(status_bar), id, msg);
    }
}

void show_notification(const char *message, const char *type) {
    GtkMessageType mt = GTK_MESSAGE_INFO;
    if (strcmp(type, "error") == 0) mt = GTK_MESSAGE_ERROR;
    else if (strcmp(type, "warning") == 0) mt = GTK_MESSAGE_WARNING;
    else if (strcmp(type, "question") == 0) mt = GTK_MESSAGE_QUESTION;
    GtkWidget *d = gtk_message_dialog_new(
        main_window ? GTK_WINDOW(main_window) : GTK_WINDOW(login_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        mt, GTK_BUTTONS_OK, "%s", message);
    gtk_window_set_title(GTK_WINDOW(d), type);
    gtk_dialog_run(GTK_DIALOG(d));
    gtk_widget_destroy(d);
}

static void toast_destroyed(gpointer data, GObject *obj) {
    guint *tag = data;
    if (*tag) g_source_remove(*tag);
    *tag = 0;
    g_object_weak_unref(obj, toast_destroyed, data);
    g_free(data);
}

static gboolean toast_timeout(gpointer w) {
    if (GTK_IS_WIDGET(w)) gtk_widget_destroy(GTK_WIDGET(w));
    return G_SOURCE_REMOVE;
}

void show_toast(const char *msg) {
    if (!main_window) return;
    GtkWidget *w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_decorated(GTK_WINDOW(w), FALSE);
    gtk_window_set_resizable(GTK_WINDOW(w), FALSE);
    gtk_window_set_modal(GTK_WINDOW(w), FALSE);
    gtk_window_set_transient_for(GTK_WINDOW(w), GTK_WINDOW(main_window));
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(w), TRUE);
    gtk_window_set_type_hint(GTK_WINDOW(w), GDK_WINDOW_TYPE_HINT_NOTIFICATION);

    GtkWidget *l = gtk_label_new(msg);
    gtk_widget_set_margin_start(l, 24); gtk_widget_set_margin_end(l, 24);
    gtk_widget_set_margin_top(l, 12); gtk_widget_set_margin_bottom(l, 12);
    gtk_container_add(GTK_CONTAINER(w), l);

    GtkCssProvider *p = gtk_css_provider_new();
    gtk_css_provider_load_from_data(p,
        "window{background:#343A40;border-radius:8px;}"
        "label{color:white;font-weight:600;font-size:13px;}", -1, NULL);
    GtkStyleContext *ctx = gtk_widget_get_style_context(w);
    gtk_style_context_add_provider(ctx, GTK_STYLE_PROVIDER(p), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(p);

    gtk_window_set_default_size(GTK_WINDOW(w), -1, -1);
    gint mx, my, mw, mh;
    gtk_window_get_position(GTK_WINDOW(main_window), &mx, &my);
    gtk_window_get_size(GTK_WINDOW(main_window), &mw, &mh);
    gtk_window_move(GTK_WINDOW(w), mx + mw - 360, my + mh - 80);
    gtk_widget_show_all(w);

    guint *tag = g_new(guint, 1);
    *tag = g_timeout_add(2000, toast_timeout, w);
    g_object_weak_ref(G_OBJECT(w), toast_destroyed, tag);
}

void show_confirmation(const char *title, const char *msg, GCallback cb, gpointer data) {
    (void)data;
    GtkWidget *d = gtk_message_dialog_new(
        GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "%s", msg);
    gtk_window_set_title(GTK_WINDOW(d), title);
    if (gtk_dialog_run(GTK_DIALOG(d)) == GTK_RESPONSE_YES && cb) {
        void (*fn)(void) = (void (*)(void))cb;
        fn();
    }
    gtk_widget_destroy(d);
}

void switch_to_view(const char *name) {
    if (!content_stack) return;
    gtk_stack_set_visible_child_name(GTK_STACK(content_stack), name);
    GList *children = gtk_container_get_children(GTK_CONTAINER(sidebar_box));
    for (GList *l = children; l; l = l->next) {
        const char *vn = g_object_get_data(G_OBJECT(l->data), "view-name");
        if (vn && strcmp(vn, name) == 0)
            gtk_style_context_add_class(gtk_widget_get_style_context(l->data), "active");
        else
            gtk_style_context_remove_class(gtk_widget_get_style_context(l->data), "active");
    }
    g_list_free(children);
}

static GtkWidget* stat_card(const char *title, const char *icon, const char *color) {
    GtkWidget *c = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_style_context_add_class(gtk_widget_get_style_context(c), "stat-card");
    gtk_style_context_add_class(gtk_widget_get_style_context(c), color);
    GtkWidget *top = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *img = gtk_image_new_from_icon_name(icon, GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_box_pack_start(GTK_BOX(top), img, FALSE, FALSE, 0);
    GtkWidget *val = gtk_label_new("0");
    gtk_style_context_add_class(gtk_widget_get_style_context(val), "stat-number");
    gtk_box_pack_start(GTK_BOX(top), val, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(c), top, FALSE, FALSE, 0);
    g_object_set_data(G_OBJECT(c), "val", val);
    GtkWidget *tl = gtk_label_new(title);
    gtk_style_context_add_class(gtk_widget_get_style_context(tl), "stat-label");
    gtk_widget_set_halign(tl, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(c), tl, FALSE, FALSE, 0);
    return c;
}

static void set_stat(GtkWidget *card, int value) {
    GtkWidget *l = g_object_get_data(G_OBJECT(card), "val");
    if (l) { gchar *t = g_strdup_printf("%d", value); gtk_label_set_text(GTK_LABEL(l), t); g_free(t); }
}

/* ==================== LOGIN WINDOW ==================== */

static gboolean draw_bg(GtkWidget *w, cairo_t *cr, gpointer data) {
    (void)data;
    static float t = 0; t += 0.02f;
    int W = gtk_widget_get_allocated_width(w), H = gtk_widget_get_allocated_height(w);
    cairo_pattern_t *p = cairo_pattern_create_linear(0, 0, W, H);
    cairo_pattern_add_color_stop_rgb(p, 0, 0.0 + 0.04*sin(t), 0.4 + 0.04*cos(t*0.7), 0.8);
    cairo_pattern_add_color_stop_rgb(p, 1, 0.0, 0.64, 0.64 + 0.04*sin(t*0.5));
    cairo_set_source(cr, p);
    cairo_paint(cr);
    cairo_pattern_destroy(p);
    cairo_set_source_rgba(cr, 1, 1, 1, 0.04);
    cairo_set_line_width(cr, 2);
    float sz = 80 + 15*sin(t*0.3);
    cairo_move_to(cr, W*0.85 - sz/2, H*0.15);
    cairo_line_to(cr, W*0.85 + sz/2, H*0.15);
    cairo_move_to(cr, W*0.85, H*0.15 - sz/2);
    cairo_line_to(cr, W*0.85, H*0.15 + sz/2);
    cairo_stroke(cr);
    return FALSE;
}

static gboolean redraw_timer(gpointer w) {
    gtk_widget_queue_draw(GTK_WIDGET(w));
    return G_SOURCE_CONTINUE;
}

static void on_login(GtkButton *btn, gpointer data) {
    (void)btn; (void)data;
    const char *u = gtk_entry_get_text(GTK_ENTRY(username_entry));
    const char *p = gtk_entry_get_text(GTK_ENTRY(password_entry));
    if (strlen(u) == 0 || strlen(p) == 0) {
        gtk_label_set_markup(GTK_LABEL(login_error_label), "<span color='#DC3545' weight='bold'>Please enter username and password.</span>");
        gtk_widget_show(login_error_label); return;
    }
    gtk_spinner_start(GTK_SPINNER(login_spinner));
    gtk_widget_show(login_spinner);
    gtk_widget_set_sensitive(username_entry, FALSE);
    gtk_widget_set_sensitive(password_entry, FALSE);

    int ok = authenticate(u, p);
    if (ok) {
        User *usr = findUser(u);
        if (usr) {
            strncpy(current_username, usr->username, 49);
            strncpy(current_role, usr->role, 19);
            currentUser = usr;
            strcpy(currentUser->lastLogin, getCurrentTimestamp());
            appendAuditLog("LOGIN", u, "SUCCESS");
        }
        gtk_widget_hide(login_window);
        create_main_window();
        gtk_widget_show_all(main_window);
        update_status("System ready.");
    } else {
        gtk_spinner_stop(GTK_SPINNER(login_spinner));
        gtk_widget_hide(login_spinner);
        gtk_widget_set_sensitive(username_entry, TRUE);
        gtk_widget_set_sensitive(password_entry, TRUE);
        gtk_label_set_markup(GTK_LABEL(login_error_label), "<span color='#DC3545' weight='bold'>Invalid credentials. Try again.</span>");
        gtk_widget_show(login_error_label);
    }
}

void create_login_window(void) {
    login_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(login_window), "Login - PKU UTHM Medical System");
    gtk_window_set_default_size(GTK_WINDOW(login_window), LOGIN_WIDTH, LOGIN_HEIGHT);
    gtk_window_set_position(GTK_WINDOW(login_window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(login_window), FALSE);
    g_signal_connect(login_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *overlay = gtk_overlay_new();
    gtk_container_add(GTK_CONTAINER(login_window), overlay);

    GtkWidget *bg = gtk_drawing_area_new();
    g_signal_connect(bg, "draw", G_CALLBACK(draw_bg), NULL);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), bg);

    GtkWidget *center = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(center, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(center, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_start(center, 40);
    gtk_widget_set_margin_end(center, 40);
    gtk_widget_set_margin_top(center, 40);
    gtk_widget_set_margin_bottom(center, 40);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), center);

    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_widget_set_name(card, "login-card");
    gtk_container_set_border_width(GTK_CONTAINER(card), 40);

    GdkPixbuf *pb = gdk_pixbuf_new_from_file_at_scale("resources/logo.png", 80, 80, TRUE, NULL);
    GtkWidget *logo = gtk_image_new_from_pixbuf(pb);
    g_object_unref(pb);
    gtk_box_pack_start(GTK_BOX(card), logo, FALSE, FALSE, 0);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='xx-large' weight='bold' color='#0052A3'>PKU UTHM</span>");
    gtk_box_pack_start(GTK_BOX(card), title, FALSE, FALSE, 0);

    GtkWidget *sub = gtk_label_new("Medical Appointment System");
    gtk_style_context_add_class(gtk_widget_get_style_context(sub), "dim-label");
    gtk_box_pack_start(GTK_BOX(card), sub, FALSE, FALSE, 0);

    username_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Username");
    gtk_widget_set_size_request(username_entry, 280, -1);
    gtk_box_pack_start(GTK_BOX(card), username_entry, FALSE, FALSE, 0);

    password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(password_entry), 0x25CF);
    gtk_widget_set_size_request(password_entry, 280, -1);
    gtk_box_pack_start(GTK_BOX(card), password_entry, FALSE, FALSE, 0);
    g_signal_connect(password_entry, "activate", G_CALLBACK(on_login), NULL);

    GtkWidget *login_btn = gtk_button_new_with_label("Sign In");
    gtk_style_context_add_class(gtk_widget_get_style_context(login_btn), "primary");
    gtk_widget_set_size_request(login_btn, 280, 44);
    g_signal_connect(login_btn, "clicked", G_CALLBACK(on_login), NULL);
    gtk_box_pack_start(GTK_BOX(card), login_btn, FALSE, FALSE, 0);

    login_spinner = gtk_spinner_new();
    gtk_widget_set_size_request(login_spinner, 28, 28);
    gtk_box_pack_start(GTK_BOX(card), login_spinner, FALSE, FALSE, 0);

    login_error_label = gtk_label_new("");
    gtk_widget_set_no_show_all(login_error_label, TRUE);
    gtk_box_pack_start(GTK_BOX(card), login_error_label, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(center), card, FALSE, FALSE, 0);
    gtk_widget_show_all(login_window);
    gtk_widget_hide(login_spinner);
    g_timeout_add(50, redraw_timer, bg);
}

/* ==================== MAIN WINDOW ==================== */

static void on_logout(GtkButton *btn, gpointer data) {
    (void)btn; (void)data;
    show_confirmation("Confirm Logout", "Are you sure you want to logout?", G_CALLBACK(do_logout), NULL);
}

static int is_logging_out = 0;

void on_main_window_closed(void) {
    if (!is_logging_out) {
        main_window = NULL;
        gtk_main_quit();
    }
}

void do_logout(void) {
    is_logging_out = 1;
    if (currentUser) {
        appendAuditLog("LOGOUT", currentUser->username, "SUCCESS");
        currentUser = NULL;
    }
    saveAll();
    memset(current_username, 0, 50);
    memset(current_role, 0, 20);
    if (main_window) { gtk_widget_destroy(main_window); main_window = NULL; }
    is_logging_out = 0;
    create_login_window();
    gtk_widget_show_all(login_window);
    gtk_widget_hide(login_spinner);
}

/* ==================== SIDEBAR ==================== */
typedef struct { const char *label; const char *icon; const char *view; const char *roles; } NavItem;

static void on_sidebar_click(GtkButton *btn, gpointer data) {
    (void)data;
    const char *vn = g_object_get_data(G_OBJECT(btn), "view-name");
    if (!vn) return;
    switch_to_view(vn);
    GtkWidget *tv = g_object_get_data(G_OBJECT(content_stack), "patient-treeview");
    GtkWidget *al = g_object_get_data(G_OBJECT(content_stack), "appointments-list");
    if (strcmp(vn, "patients") == 0 && tv) populate_patient_treeview(tv);
    if (strcmp(vn, "appointments") == 0 && al) populate_appointment_list(al, NULL);
    if (strcmp(vn, "queue") == 0) update_queue_display();
    if (strcmp(vn, "dashboard") == 0) update_dashboard_stats();
}

/* forward declarations */
void populate_patient_treeview(GtkWidget *tv);
void populate_appointment_list(GtkWidget *list, const char *date);
void update_queue_display(void);
void update_dashboard_stats(void);

void create_sidebar(void) {
    sidebar_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_style_context_add_class(gtk_widget_get_style_context(sidebar_box), "sidebar");
    gtk_widget_set_size_request(sidebar_box, SIDEBAR_WIDTH, -1);

    GtkWidget *logo_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_widget_set_margin_top(logo_box, 16);
    gtk_widget_set_margin_bottom(logo_box, 8);
    GdkPixbuf *pb = gdk_pixbuf_new_from_file_at_scale("resources/logo.png", 48, 48, TRUE, NULL);
    GtkWidget *logo = gtk_image_new_from_pixbuf(pb);
    g_object_unref(pb);
    gtk_box_pack_start(GTK_BOX(logo_box), logo, FALSE, FALSE, 0);
    GtkWidget *sn = gtk_label_new("PKU UTHM");
    gtk_style_context_add_class(gtk_widget_get_style_context(sn), "card-title");
    gtk_box_pack_start(GTK_BOX(logo_box), sn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sidebar_box), logo_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sidebar_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 6);

    NavItem items[] = {
        {"Dashboard", "computer", "dashboard", "admin,doctor,staff"},
        {"Patients", "system-users", "patients", "admin,doctor,staff"},
        {"Appointments", "office-calendar", "appointments", "admin,doctor,staff"},
        {"Walk-in Queue", "list-add", "queue", "admin,doctor,staff"},
        {"Reports", "x-office-spreadsheet", "reports", "admin,doctor,staff"},
        {"Search", "system-search", "search", "admin,doctor,staff"},
        {"Settings", "preferences-system", "settings", "admin"},
        {NULL, NULL, NULL, NULL}
    };

    for (int i = 0; items[i].label; i++) {
        GtkWidget *btn = gtk_button_new();
        GtkWidget *hb = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        GtkWidget *ic = gtk_image_new_from_icon_name(items[i].icon, GTK_ICON_SIZE_MENU);
        gtk_box_pack_start(GTK_BOX(hb), ic, FALSE, FALSE, 0);
        GtkWidget *lb = gtk_label_new(items[i].label);
        gtk_widget_set_halign(lb, GTK_ALIGN_START);
        gtk_box_pack_start(GTK_BOX(hb), lb, TRUE, TRUE, 0);
        gtk_container_add(GTK_CONTAINER(btn), hb);
        g_object_set_data_full(G_OBJECT(btn), "view-name", g_strdup(items[i].view), g_free);
        g_object_set_data_full(G_OBJECT(btn), "roles", g_strdup(items[i].roles), g_free);
        g_signal_connect(btn, "clicked", G_CALLBACK(on_sidebar_click), NULL);
        g_object_set_data(G_OBJECT(btn), "allowed-roles", (gpointer)items[i].roles);
        gtk_box_pack_start(GTK_BOX(sidebar_box), btn, FALSE, FALSE, 1);
    }

    gtk_box_pack_end(GTK_BOX(sidebar_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 6);
    GtkWidget *lo = gtk_button_new_with_label("Logout");
    gtk_style_context_add_class(gtk_widget_get_style_context(lo), "danger");
    g_signal_connect(lo, "clicked", G_CALLBACK(on_logout), NULL);
    gtk_box_pack_end(GTK_BOX(sidebar_box), lo, FALSE, FALSE, 4);

    GList *ch = gtk_container_get_children(GTK_CONTAINER(sidebar_box));
    for (GList *l = ch; l; l = l->next) {
        const char *rl = g_object_get_data(G_OBJECT(l->data), "allowed-roles");
        if (rl && !strstr(rl, current_role))
            gtk_widget_set_visible(GTK_WIDGET(l->data), FALSE);
    }
    g_list_free(ch);
}

/* ==================== HEADER ==================== */
static void create_header(void) {
    GtkWidget *hb = gtk_header_bar_new();
    gtk_header_bar_set_title(GTK_HEADER_BAR(hb), APP_NAME);
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(hb), TRUE);
    gchar *ut = g_strdup_printf("%s | %s", current_username, current_role);
    GtkWidget *ul = gtk_label_new(ut);
    g_free(ut);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(hb), ul);
    GtkWidget *lo = gtk_button_new_from_icon_name("system-shutdown-symbolic", GTK_ICON_SIZE_BUTTON);
    gtk_widget_set_tooltip_text(lo, "Logout");
    g_signal_connect(lo, "clicked", G_CALLBACK(on_logout), NULL);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(hb), lo);
    gtk_window_set_titlebar(GTK_WINDOW(main_window), hb);
}

/* ==================== DASHBOARD VIEW ==================== */

static void on_quick_action_clicked(GtkButton *btn, gpointer data) {
    (void)data;
    const char *vn = g_object_get_data(G_OBJECT(btn), "qa-view");
    if (vn) {
        GtkWidget *tv = g_object_get_data(G_OBJECT(content_stack), "patient-treeview");
        GtkWidget *al = g_object_get_data(G_OBJECT(content_stack), "appointments-list");
        if (strcmp(vn, "patients") == 0 && tv) populate_patient_treeview(tv);
        if (strcmp(vn, "appointments") == 0 && al) populate_appointment_list(al, NULL);
        if (strcmp(vn, "queue") == 0) update_queue_display();
        if (strcmp(vn, "dashboard") == 0) update_dashboard_stats();
        switch_to_view(vn);
    }
}

GtkWidget* create_dashboard_view(void) {
    GtkWidget *sc = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sc), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 18);
    gtk_widget_set_margin_start(box, 24); gtk_widget_set_margin_end(box, 24);
    gtk_widget_set_margin_top(box, 24); gtk_widget_set_margin_bottom(box, 24);
    gtk_container_add(GTK_CONTAINER(sc), box);

    GtkWidget *welcome = gtk_label_new(NULL);
    gtk_widget_set_halign(welcome, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(box), welcome, FALSE, FALSE, 0);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 14);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 14);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_box_pack_start(GTK_BOX(box), grid, FALSE, FALSE, 0);

    GtkWidget *s1 = stat_card("Total Patients", "system-users", "blue");
    GtkWidget *s2 = stat_card("Today's Appointments", "office-calendar", "teal");
    GtkWidget *s3 = stat_card("Walk-in Queue", "list-add", "amber");
    GtkWidget *s4 = stat_card("Completed Today", "emblem-ok", "green");
    g_object_set_data(G_OBJECT(content_stack), "stat-total-patients", s1);
    g_object_set_data(G_OBJECT(content_stack), "stat-today-appts", s2);
    g_object_set_data(G_OBJECT(content_stack), "stat-queue-size", s3);
    g_object_set_data(G_OBJECT(content_stack), "stat-completed", s4);
    gtk_grid_attach(GTK_GRID(grid), s1, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), s2, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), s3, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), s4, 3, 0, 1, 1);

    GtkWidget *ac = create_card("Quick Actions");
    gtk_box_pack_start(GTK_BOX(box), ac, FALSE, FALSE, 0);
    GtkWidget *ab = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_add(GTK_CONTAINER(ac), ab);
    const char *qa[][2] = {{"New Patient","system-users"},{"Book Appointment","office-calendar"},{"Process Queue","media-playback-start"},{"Generate Report","x-office-spreadsheet"}};
    const char *qa_views[] = {"patient-form", "appointment-form", "queue", "reports"};
    for (int i = 0; i < 4; i++) {
        GtkWidget *b = gtk_button_new();
        GtkWidget *bb = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        gtk_box_pack_start(GTK_BOX(bb), gtk_image_new_from_icon_name(qa[i][1], GTK_ICON_SIZE_BUTTON), FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(bb), gtk_label_new(qa[i][0]), FALSE, FALSE, 0);
        gtk_container_add(GTK_CONTAINER(b), bb);
        gtk_style_context_add_class(gtk_widget_get_style_context(b), "primary");
        gtk_box_pack_start(GTK_BOX(ab), b, TRUE, TRUE, 0);
        g_object_set_data_full(G_OBJECT(b), "qa-view", g_strdup(qa_views[i]), g_free);
        g_signal_connect(b, "clicked", G_CALLBACK(on_quick_action_clicked), NULL);
    }

    GtkWidget *rc = create_card("Recent Activity");
    gtk_box_pack_start(GTK_BOX(box), rc, TRUE, TRUE, 0);
    GtkWidget *rl = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(rl), GTK_SELECTION_NONE);
    gtk_container_add(GTK_CONTAINER(rc), rl);
    g_object_set_data(G_OBJECT(content_stack), "activity-list", rl);

    return sc;
}

void update_dashboard_stats(void) {
    if (!content_stack) return;
    GtkWidget *w = g_object_get_data(G_OBJECT(content_stack), "welcome-label");
    if (!w) {
        GtkWidget *sc = gtk_stack_get_visible_child(GTK_STACK(content_stack));
        if (sc) w = g_object_get_data(G_OBJECT(sc), "welcome-label");
    }
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    char ds[50]; strftime(ds, 50, "%A, %d %B %Y", tm);
    gchar *mk = g_markup_printf_escaped("<span size='x-large' weight='bold'>Welcome, %s</span>\n<span size='small' foreground='%s'>%s</span>",
        current_username, "#6C757D", ds);
    GtkWidget *wl = g_object_get_data(G_OBJECT(content_stack), "welcome-label");
    if (!wl) {
        GtkWidget *vis = gtk_stack_get_visible_child(GTK_STACK(content_stack));
        if (vis) wl = g_object_get_data(G_OBJECT(vis), "welcome-label");
    }
    if (wl) gtk_label_set_markup(GTK_LABEL(wl), mk);
    g_free(mk);

    int tp = countPatients();
    int ta = 0, tc = 0;
    time_t t = time(NULL); struct tm *ltm = localtime(&t);
    char td[15]; snprintf(td, 15, "%02d/%02d/%04d", ltm->tm_mday, ltm->tm_mon+1, ltm->tm_year+1900);
    Appointment *cur = appointmentList;
    while (cur) {
        if (strcmp(cur->date, td) == 0) {
            ta++;
            if (strcmp(cur->status, "Completed") == 0) tc++;
        }
        cur = cur->next;
    }

    set_stat(g_object_get_data(G_OBJECT(content_stack), "stat-total-patients"), tp);
    set_stat(g_object_get_data(G_OBJECT(content_stack), "stat-today-appts"), ta);
    set_stat(g_object_get_data(G_OBJECT(content_stack), "stat-queue-size"), walkInQueue.size);
    set_stat(g_object_get_data(G_OBJECT(content_stack), "stat-completed"), tc);
}

/* ==================== PATIENT LIST VIEW ==================== */

void populate_patient_treeview(GtkWidget *tv) {
    if (!tv) return;
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tv)));
    if (!store) return;
    gtk_list_store_clear(store);
    Patient *cur = patientList;
    while (cur) {
        GtkTreeIter it; gtk_list_store_append(store, &it);
        gtk_list_store_set(store, &it, 0, cur->patientID, 1, cur->name, 2, cur->icNumber,
            3, cur->faculty, 4, cur->phone, 5, "Active", -1);
        cur = cur->next;
    }
}

static void on_patient_add(GtkButton *btn, gpointer data) {
    (void)btn; (void)data;
    switch_to_view("patient-form");
}

static char *pending_delete_id = NULL;

static void on_patient_delete_clicked(GtkButton *btn, gpointer data) {
    (void)btn; (void)data;
    GtkWidget *tv = g_object_get_data(G_OBJECT(content_stack), "patient-treeview");
    if (!tv) return;
    GtkTreeSelection *sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(tv));
    GtkTreeModel *model; GtkTreeIter it;
    if (!gtk_tree_selection_get_selected(sel, &model, &it)) {
        show_notification("Please select a patient to delete.", "warning"); return;
    }
    gchar *id; gtk_tree_model_get(model, &it, 0, &id, -1);
    gchar *nm; gtk_tree_model_get(model, &it, 1, &nm, -1);
    gchar *msg = g_strdup_printf("Delete patient %s (%s)?", nm, id);
    if (pending_delete_id) g_free(pending_delete_id);
    pending_delete_id = g_strdup(id);
    show_confirmation("Confirm Delete", msg, G_CALLBACK(confirm_delete_patient), NULL);
    g_free(msg); g_free(id); g_free(nm);
}

void confirm_delete_patient(void) {
    if (!pending_delete_id) return;
    Patient *p = findPatientByID(pending_delete_id);
    if (!p) { show_notification("Patient not found.", "error"); return; }
    Patient *cur = patientList, *prev = NULL;
    while (cur) {
        if (strcmp(cur->patientID, pending_delete_id) == 0) {
            if (prev) prev->next = cur->next; else patientList = cur->next;
            break;
        }
        prev = cur; cur = cur->next;
    }
    hashRemove(pending_delete_id);
    appendAuditLog("DELETE_PATIENT", pending_delete_id, "SUCCESS");
    savePatients();
    show_notification("Patient deleted.", "success");
    GtkWidget *tv = g_object_get_data(G_OBJECT(content_stack), "patient-treeview");
    if (tv) populate_patient_treeview(tv);
}

static gboolean on_patient_cell_clicked(GtkWidget *tv, GdkEventButton *ev, gpointer data) {
    (void)data;
    if (ev->button != 1 || ev->type != GDK_BUTTON_PRESS) return FALSE;
    GtkTreePath *path;
    GtkTreeViewColumn *col;
    gint cx, cy;
    if (!gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(tv), (gint)ev->x, (gint)ev->y, &path, &col, &cx, &cy))
        return FALSE;
    gint col_idx = -1;
    GList *cols = gtk_tree_view_get_columns(GTK_TREE_VIEW(tv));
    for (GList *l = cols; l; l = l->next) {
        col_idx++;
        if (l->data == (gpointer)col) break;
    }
    g_list_free(cols);
    if (col_idx < 0) { gtk_tree_path_free(path); return FALSE; }
    GtkTreeIter it;
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(tv));
    if (!gtk_tree_model_get_iter(model, &it, path)) { gtk_tree_path_free(path); return FALSE; }
    gchar *text;
    gtk_tree_model_get(model, &it, col_idx, &text, -1);
    if (text) {
        GtkClipboard *clip = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
        gtk_clipboard_set_text(clip, text, -1);
        gchar *msg = g_strdup_printf("Copied: %s", text);
        show_toast(msg);
        g_free(msg);
        g_free(text);
    }
    gtk_tree_path_free(path);
    return FALSE;
}

GtkWidget* create_patient_list_view(void) {
    GtkWidget *sc = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_widget_set_margin_start(box, 24); gtk_widget_set_margin_end(box, 24);
    gtk_widget_set_margin_top(box, 24); gtk_widget_set_margin_bottom(box, 24);
    gtk_container_add(GTK_CONTAINER(sc), box);

    GtkWidget *hb = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *tl = gtk_label_new("Patient Management");
    gtk_style_context_add_class(gtk_widget_get_style_context(tl), "card-title");
    gtk_box_pack_start(GTK_BOX(hb), tl, TRUE, TRUE, 0);

    GtkWidget *add = gtk_button_new_with_label("+ Add Patient");
    gtk_style_context_add_class(gtk_widget_get_style_context(add), "success");
    g_signal_connect(add, "clicked", G_CALLBACK(on_patient_add), NULL);
    gtk_box_pack_end(GTK_BOX(hb), add, FALSE, FALSE, 0);

    GtkWidget *del = gtk_button_new_with_label("Delete");
    gtk_style_context_add_class(gtk_widget_get_style_context(del), "danger");
    g_signal_connect(del, "clicked", G_CALLBACK(on_patient_delete_clicked), NULL);
    gtk_box_pack_end(GTK_BOX(hb), del, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), hb, FALSE, FALSE, 0);

    GtkListStore *store = gtk_list_store_new(6, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkWidget *tv = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(tv), GTK_TREE_VIEW_GRID_LINES_HORIZONTAL);
    const char *cols[] = {"Patient ID", "Name", "IC Number", "Faculty", "Phone", "Status"};
    for (int i = 0; i < 6; i++) {
        GtkCellRenderer *r = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *c = gtk_tree_view_column_new_with_attributes(cols[i], r, "text", i, NULL);
        gtk_tree_view_column_set_resizable(c, TRUE);
        gtk_tree_view_column_set_min_width(c, 90);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tv), c);
    }
    g_object_set_data(G_OBJECT(content_stack), "patient-treeview", tv);
    g_object_set_data(G_OBJECT(content_stack), "patient-store", store);
    g_signal_connect(tv, "button-press-event", G_CALLBACK(on_patient_cell_clicked), NULL);

    GtkWidget *tsc = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(tsc), tv);
    GtkWidget *card = create_card(NULL);
    gtk_container_add(GTK_CONTAINER(card), tsc);
    gtk_box_pack_start(GTK_BOX(box), card, TRUE, TRUE, 0);

    GtkTreeSelection *sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(tv));
    gtk_tree_selection_set_mode(sel, GTK_SELECTION_SINGLE);

    return sc;
}

/* ==================== PATIENT FORM VIEW ==================== */

GtkWidget* create_patient_form_view(void) {
    GtkWidget *sc = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_set_margin_start(box, 24); gtk_widget_set_margin_end(box, 24);
    gtk_widget_set_margin_top(box, 24); gtk_widget_set_margin_bottom(box, 24);
    gtk_container_add(GTK_CONTAINER(sc), box);

    GtkWidget *hb = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *bk = gtk_button_new_from_icon_name("go-previous", GTK_ICON_SIZE_BUTTON);
    g_signal_connect_swapped(bk, "clicked", G_CALLBACK(switch_to_view), "patients");
    gtk_box_pack_start(GTK_BOX(hb), bk, FALSE, FALSE, 0);
    GtkWidget *tl = gtk_label_new("New Patient Record");
    gtk_style_context_add_class(gtk_widget_get_style_context(tl), "card-title");
    gtk_box_pack_start(GTK_BOX(hb), tl, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), hb, FALSE, FALSE, 0);

    GtkWidget *card = create_card("Patient Information");
    gtk_box_pack_start(GTK_BOX(box), card, FALSE, FALSE, 0);
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 12);
    gtk_widget_set_margin_start(grid, 16); gtk_widget_set_margin_end(grid, 16);
    gtk_widget_set_margin_top(grid, 16); gtk_widget_set_margin_bottom(grid, 16);
    gtk_container_add(GTK_CONTAINER(card), grid);

    GtkWidget *id_e = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(id_e), "Auto-generated");
    gtk_widget_set_sensitive(id_e, FALSE);
    GtkWidget *nm_e = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(nm_e), "Full Name");
    GtkWidget *ic_e = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(ic_e), "000000-00-0000");
    GtkWidget *ph_e = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(ph_e), "01xxxxxxxxx");
    GtkWidget *em_e = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(em_e), "name@student.uthm.edu.my");
    GtkWidget *fc = gtk_combo_box_text_new();
    const char *facs[] = {"FSKTM","FPTP","FKAAB","FKEE","FKMP","FPTV","FKAAS","PPBU",NULL};
    for (int i = 0; facs[i]; i++) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(fc), facs[i]);
    GtkWidget *pr_e = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(pr_e), "e.g., BIT, BCE");
    GtkWidget *gc = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(gc), "Male");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(gc), "Female");
    GtkWidget *ag = gtk_spin_button_new_with_range(1, 120, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ag), 20);
    GtkWidget *bc = gtk_combo_box_text_new();
    const char *blood[] = {"A+","A-","B+","B-","AB+","AB-","O+","O-",NULL};
    for (int i = 0; blood[i]; i++) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bc), blood[i]);
    GtkWidget *al_e = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(al_e), "Allergies (or None)");
    GtkWidget *emc_e = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(emc_e), "Emergency contact");

    gtk_grid_attach(GTK_GRID(grid), form_row("Patient ID", id_e), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), form_row("Full Name *", nm_e), 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), form_row("IC Number *", ic_e), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), form_row("Phone *", ph_e), 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), form_row("Email *", em_e), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), form_row("Faculty *", fc), 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), form_row("Program *", pr_e), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), form_row("Gender *", gc), 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), form_row("Age *", ag), 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), form_row("Blood Type", bc), 1, 4, 1, 1);
    GtkWidget *al_r = form_row("Allergies", al_e);
    gtk_widget_set_hexpand(al_r, TRUE);
    gtk_grid_attach(GTK_GRID(grid), al_r, 0, 5, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), form_row("Emergency Contact", emc_e), 0, 6, 1, 1);

    g_object_set_data(G_OBJECT(content_stack), "form-id", id_e);
    g_object_set_data(G_OBJECT(content_stack), "form-name", nm_e);
    g_object_set_data(G_OBJECT(content_stack), "form-ic", ic_e);
    g_object_set_data(G_OBJECT(content_stack), "form-phone", ph_e);
    g_object_set_data(G_OBJECT(content_stack), "form-email", em_e);
    g_object_set_data(G_OBJECT(content_stack), "form-faculty", fc);
    g_object_set_data(G_OBJECT(content_stack), "form-program", pr_e);
    g_object_set_data(G_OBJECT(content_stack), "form-gender", gc);
    g_object_set_data(G_OBJECT(content_stack), "form-age", ag);
    g_object_set_data(G_OBJECT(content_stack), "form-blood", bc);
    g_object_set_data(G_OBJECT(content_stack), "form-allergies", al_e);
    g_object_set_data(G_OBJECT(content_stack), "form-emergency", emc_e);

    GtkWidget *abox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(abox, GTK_ALIGN_END);
    GtkWidget *cb = gtk_button_new_with_label("Cancel");
    gtk_style_context_add_class(gtk_widget_get_style_context(cb), "ghost");
    g_signal_connect_swapped(cb, "clicked", G_CALLBACK(switch_to_view), "patients");
    gtk_box_pack_start(GTK_BOX(abox), cb, FALSE, FALSE, 0);
    GtkWidget *sb = gtk_button_new_with_label("Save Patient");
    gtk_style_context_add_class(gtk_widget_get_style_context(sb), "success");
    g_signal_connect(sb, "clicked", G_CALLBACK(on_patient_save), NULL);
    gtk_box_pack_start(GTK_BOX(abox), sb, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), abox, FALSE, FALSE, 0);

    return sc;
}

void on_patient_save(GtkButton *btn, gpointer data) {
    (void)btn; (void)data;
    const char *nm = gtk_entry_get_text(GTK_ENTRY(g_object_get_data(G_OBJECT(content_stack), "form-name")));
    if (strlen(nm) == 0) { show_notification("Name is required.", "error"); return; }

    Patient *p = createPatientNode();
    if (!p) { show_notification("Memory error!", "error"); return; }
    generatePatientID(p->patientID);
    strncpy(p->name, nm, MAX_NAME-1);
    strncpy(p->icNumber, gtk_entry_get_text(GTK_ENTRY(g_object_get_data(G_OBJECT(content_stack), "form-ic"))), MAX_IC-1);
    strncpy(p->phone, gtk_entry_get_text(GTK_ENTRY(g_object_get_data(G_OBJECT(content_stack), "form-phone"))), MAX_PHONE-1);
    strncpy(p->email, gtk_entry_get_text(GTK_ENTRY(g_object_get_data(G_OBJECT(content_stack), "form-email"))), MAX_EMAIL-1);
    strncpy(p->faculty, gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(g_object_get_data(G_OBJECT(content_stack), "form-faculty"))), MAX_FACULTY-1);
    strncpy(p->program, gtk_entry_get_text(GTK_ENTRY(g_object_get_data(G_OBJECT(content_stack), "form-program"))), MAX_PROGRAM-1);
    const char *gen = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(g_object_get_data(G_OBJECT(content_stack), "form-gender")));
    if (gen) strncpy(p->gender, gen, MAX_GENDER-1);
    p->age = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(g_object_get_data(G_OBJECT(content_stack), "form-age")));
    const char *bt = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(g_object_get_data(G_OBJECT(content_stack), "form-blood")));
    if (bt) strncpy(p->bloodType, bt, MAX_BLOOD-1);
    strncpy(p->allergies, gtk_entry_get_text(GTK_ENTRY(g_object_get_data(G_OBJECT(content_stack), "form-allergies"))), MAX_ALLERGIES-1);
    if (strlen(p->allergies) == 0) strcpy(p->allergies, "None");
    strncpy(p->emergencyContact, gtk_entry_get_text(GTK_ENTRY(g_object_get_data(G_OBJECT(content_stack), "form-emergency"))), MAX_EMERGENCY-1);

    insertPatientToList(p);
    insertPatientToBST(&patientBST, p);
    hashInsert(p->patientID);
    appendAuditLog("INSERT_PATIENT", p->patientID, "SUCCESS");
    savePatients();

    show_notification("Patient saved successfully!", "success");
    GtkWidget *tv = g_object_get_data(G_OBJECT(content_stack), "patient-treeview");
    if (tv) populate_patient_treeview(tv);
    switch_to_view("patients");
}

/* ==================== APPOINTMENT LIST VIEW ==================== */

void populate_appointment_list(GtkWidget *list, const char *date) {
    if (!list) return;
    GList *ch = gtk_container_get_children(GTK_CONTAINER(list));
    g_list_free_full(ch, (GDestroyNotify)gtk_widget_destroy);

    time_t t = time(NULL); struct tm *ltm = localtime(&t);
    char td[15]; snprintf(td, 15, "%02d/%02d/%04d", ltm->tm_mday, ltm->tm_mon+1, ltm->tm_year+1900);
    if (!date) date = td;

    Appointment *cur = appointmentList;
    int found = 0;
    while (cur) {
        if (strcmp(cur->date, date) == 0) {
            found = 1;
            GtkWidget *row = gtk_list_box_row_new();
            GtkWidget *rb = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
            gtk_widget_set_margin_start(rb, 16); gtk_widget_set_margin_end(rb, 16);
            gtk_widget_set_margin_top(rb, 12); gtk_widget_set_margin_bottom(rb, 12);

            gchar *txt = g_strdup_printf("<b>%s</b>  %s  |  %s", cur->time, cur->patientID, cur->doctorName);
            GtkWidget *dl = gtk_label_new(NULL);
            gtk_label_set_markup(GTK_LABEL(dl), txt); g_free(txt);
            gtk_widget_set_halign(dl, GTK_ALIGN_START);
            gtk_box_pack_start(GTK_BOX(rb), dl, TRUE, TRUE, 0);

            const char *bt = "info";
            if (strcmp(cur->status, "Scheduled") == 0) bt = "warning";
            else if (strcmp(cur->status, "Completed") == 0) bt = "success";
            else if (strcmp(cur->status, "Cancelled") == 0) bt = "danger";
            GtkWidget *bg = badge(cur->status, bt);
            gtk_box_pack_end(GTK_BOX(rb), bg, FALSE, FALSE, 0);

            gtk_container_add(GTK_CONTAINER(row), rb);
            gtk_list_box_insert(GTK_LIST_BOX(list), row, -1);
        }
        cur = cur->next;
    }
    if (!found) {
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *lb = gtk_label_new("No appointments on this date.");
        gtk_widget_set_margin_start(lb, 16); gtk_widget_set_margin_end(lb, 16);
        gtk_widget_set_margin_top(lb, 20); gtk_widget_set_margin_bottom(lb, 20);
        gtk_container_add(GTK_CONTAINER(row), lb);
        gtk_list_box_insert(GTK_LIST_BOX(list), row, -1);
    }
    gtk_widget_show_all(list);
}

GtkWidget* create_appointment_list_view(void) {
    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);

    GtkWidget *left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_size_request(left, 300, -1);
    gtk_paned_pack1(GTK_PANED(paned), left, FALSE, FALSE);

    GtkWidget *cc = create_card("Select Date");
    gtk_box_pack_start(GTK_BOX(left), cc, FALSE, FALSE, 0);
    GtkWidget *cal = gtk_calendar_new();
    gtk_widget_set_margin_start(cal, 10); gtk_widget_set_margin_end(cal, 10);
    gtk_widget_set_margin_top(cal, 10); gtk_widget_set_margin_bottom(cal, 10);
    gtk_container_add(GTK_CONTAINER(cc), cal);

    GtkWidget *fc = create_card("Filters");
    gtk_box_pack_start(GTK_BOX(left), fc, TRUE, TRUE, 0);
    GtkWidget *fb = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_start(fb, 12); gtk_widget_set_margin_end(fb, 12);
    gtk_widget_set_margin_top(fb, 12); gtk_widget_set_margin_bottom(fb, 12);
    gtk_container_add(GTK_CONTAINER(fc), fb);
    const char *sts[] = {"Scheduled", "Completed", "Cancelled"};
    for (int i = 0; i < 3; i++) {
        GtkWidget *chk = gtk_check_button_new_with_label(sts[i]);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chk), TRUE);
        gtk_box_pack_start(GTK_BOX(fb), chk, FALSE, FALSE, 0);
    }

    GtkWidget *right = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_widget_set_margin_start(right, 20); gtk_widget_set_margin_end(right, 20);
    gtk_widget_set_margin_top(right, 20); gtk_widget_set_margin_bottom(right, 20);
    gtk_paned_pack2(GTK_PANED(paned), right, TRUE, FALSE);

    GtkWidget *hb = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *tl = gtk_label_new("Appointments");
    gtk_style_context_add_class(gtk_widget_get_style_context(tl), "card-title");
    gtk_box_pack_start(GTK_BOX(hb), tl, TRUE, TRUE, 0);
    GtkWidget *bk = gtk_button_new_with_label("+ Book Appointment");
    gtk_style_context_add_class(gtk_widget_get_style_context(bk), "primary");
    g_signal_connect(bk, "clicked", G_CALLBACK(on_appointment_book), NULL);
    gtk_box_pack_end(GTK_BOX(hb), bk, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(right), hb, FALSE, FALSE, 0);

    GtkWidget *list_sc = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *al = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(al), GTK_SELECTION_SINGLE);
    gtk_container_add(GTK_CONTAINER(list_sc), al);
    g_object_set_data(G_OBJECT(content_stack), "appointments-list", al);

    GtkWidget *card = create_card(NULL);
    gtk_container_add(GTK_CONTAINER(card), list_sc);
    gtk_box_pack_start(GTK_BOX(right), card, TRUE, TRUE, 0);

    g_signal_connect(cal, "day-selected", G_CALLBACK(on_calendar_changed), al);

    return paned;
}

void on_calendar_changed(GtkCalendar *cal, gpointer data) {
    guint y, m, d;
    gtk_calendar_get_date(cal, &y, &m, &d);
    m++;
    char date[15]; snprintf(date, 15, "%02u/%02u/%04u", d, m, y);
    populate_appointment_list(GTK_WIDGET(data), date);
}

void on_appointment_book(GtkButton *btn, gpointer data) {
    (void)btn; (void)data;
    switch_to_view("appointment-form");
}

/* ==================== APPOINTMENT FORM VIEW ==================== */

GtkWidget* create_appointment_form_view(void) {
    GtkWidget *sc = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_set_margin_start(box, 24); gtk_widget_set_margin_end(box, 24);
    gtk_widget_set_margin_top(box, 24); gtk_widget_set_margin_bottom(box, 24);
    gtk_container_add(GTK_CONTAINER(sc), box);

    GtkWidget *hb = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *bk = gtk_button_new_from_icon_name("go-previous", GTK_ICON_SIZE_BUTTON);
    g_signal_connect_swapped(bk, "clicked", G_CALLBACK(switch_to_view), "appointments");
    gtk_box_pack_start(GTK_BOX(hb), bk, FALSE, FALSE, 0);
    GtkWidget *tl = gtk_label_new("Book New Appointment");
    gtk_style_context_add_class(gtk_widget_get_style_context(tl), "card-title");
    gtk_box_pack_start(GTK_BOX(hb), tl, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), hb, FALSE, FALSE, 0);

    GtkWidget *pc = create_card("1. Select Patient");
    gtk_box_pack_start(GTK_BOX(box), pc, FALSE, FALSE, 0);
    GtkWidget *pb = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_start(pb, 16); gtk_widget_set_margin_end(pb, 16);
    gtk_widget_set_margin_top(pb, 16); gtk_widget_set_margin_bottom(pb, 16);
    gtk_container_add(GTK_CONTAINER(pc), pb);
    GtkWidget *ps = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ps), "Enter Patient ID...");
    gtk_widget_set_hexpand(ps, TRUE);
    gtk_box_pack_start(GTK_BOX(pb), ps, TRUE, TRUE, 0);
    GtkWidget *pv = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(pb), pv, FALSE, FALSE, 0);
    g_signal_connect(ps, "changed", G_CALLBACK(on_patient_id_changed), pv);

    GtkWidget *dc = create_card("2. Appointment Details");
    gtk_box_pack_start(GTK_BOX(box), dc, FALSE, FALSE, 0);
    GtkWidget *dg = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(dg), 20);
    gtk_grid_set_row_spacing(GTK_GRID(dg), 12);
    gtk_widget_set_margin_start(dg, 16); gtk_widget_set_margin_end(dg, 16);
    gtk_widget_set_margin_top(dg, 16); gtk_widget_set_margin_bottom(dg, 16);
    gtk_container_add(GTK_CONTAINER(dc), dg);

    GtkWidget *dept_c = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dept_c), "General");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dept_c), "Dental");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dept_c), "Mental Health");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dept_c), "Physiotherapy");

    GtkWidget *doc_c = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(doc_c), "Select Doctor");
    gtk_combo_box_set_active(GTK_COMBO_BOX(doc_c), 0);

    g_signal_connect(dept_c, "changed", G_CALLBACK(on_dept_changed), doc_c);

    GtkWidget *cal = gtk_calendar_new();
    GtkWidget *time_c = gtk_combo_box_text_new();
    const char *slots[] = {"08:00 AM","08:30 AM","09:00 AM","09:30 AM","10:00 AM","10:30 AM","11:00 AM","11:30 AM","02:00 PM","02:30 PM","03:00 PM","03:30 PM","04:00 PM","04:30 PM"};
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(time_c), "Select Time");
    for (int i = 0; i < 14; i++) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(time_c), slots[i]);

    GtkWidget *sym_t = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(sym_t), GTK_WRAP_WORD_CHAR);
    gtk_widget_set_size_request(sym_t, -1, 80);
    GtkWidget *sym_sc = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(sym_sc), sym_t);

    gtk_grid_attach(GTK_GRID(dg), form_row("Department *", dept_c), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(dg), form_row("Doctor *", doc_c), 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(dg), form_row("Date *", cal), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(dg), form_row("Time *", time_c), 1, 1, 1, 1);
    GtkWidget *sr = form_row("Symptoms / Reason", sym_sc);
    gtk_grid_attach(GTK_GRID(dg), sr, 0, 2, 2, 1);

    g_object_set_data(G_OBJECT(content_stack), "apt-patient-id", ps);
    g_object_set_data(G_OBJECT(content_stack), "apt-dept", dept_c);
    g_object_set_data(G_OBJECT(content_stack), "apt-doctor", doc_c);
    g_object_set_data(G_OBJECT(content_stack), "apt-date", cal);
    g_object_set_data(G_OBJECT(content_stack), "apt-time", time_c);
    g_object_set_data(G_OBJECT(content_stack), "apt-symptoms", sym_t);

    GtkWidget *abox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(abox, GTK_ALIGN_END);
    GtkWidget *cb = gtk_button_new_with_label("Cancel");
    gtk_style_context_add_class(gtk_widget_get_style_context(cb), "ghost");
    g_signal_connect_swapped(cb, "clicked", G_CALLBACK(switch_to_view), "appointments");
    gtk_box_pack_start(GTK_BOX(abox), cb, FALSE, FALSE, 0);
    GtkWidget *sb = gtk_button_new_with_label("Book Appointment");
    gtk_style_context_add_class(gtk_widget_get_style_context(sb), "primary");
    g_signal_connect(sb, "clicked", G_CALLBACK(on_appointment_save), NULL);
    gtk_box_pack_start(GTK_BOX(abox), sb, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), abox, FALSE, FALSE, 0);

    return sc;
}

void on_patient_id_changed(GtkEntry *e, gpointer data) {
    const char *id = gtk_entry_get_text(e);
    Patient *p = findPatientByID(id);
    if (p) {
        gchar *txt = g_strdup_printf("<span foreground='green'>✓ %s</span>", p->name);
        gtk_label_set_markup(GTK_LABEL(data), txt);
        g_free(txt);
    } else if (strlen(id) > 0) {
        gtk_label_set_markup(GTK_LABEL(data), "<span foreground='red'>Patient not found</span>");
    } else {
        gtk_label_set_text(GTK_LABEL(data), "");
    }
}

void on_dept_changed(GtkComboBox *dept, gpointer data) {
    GtkWidget *doc_c = GTK_WIDGET(data);
    GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(doc_c));
    if (model) gtk_list_store_clear(GTK_LIST_STORE(model));
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(doc_c), "Select Doctor");

    const char *dept_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(dept));
    if (!dept_text) { gtk_combo_box_set_active(GTK_COMBO_BOX(doc_c), 0); return; }

    Schedule *cur = scheduleList;
    while (cur) {
        if (strcmp(cur->department, dept_text) == 0)
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(doc_c), cur->doctorName);
        cur = cur->next;
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(doc_c), 0);
}

void on_appointment_save(GtkButton *btn, gpointer data) {
    (void)btn; (void)data;
    const char *pid = gtk_entry_get_text(GTK_ENTRY(g_object_get_data(G_OBJECT(content_stack), "apt-patient-id")));
    if (!findPatientByID(pid)) { show_notification("Invalid Patient ID.", "error"); return; }

    Appointment *a = createAppointmentNode();
    if (!a) return;
    generateAppointmentID(a->appointmentID);
    strcpy(a->status, "Scheduled");
    strcpy(a->diagnosis, "Pending");
    strcpy(a->prescription, "Pending");
    strcpy(a->patientID, pid);

    GtkWidget *cal = g_object_get_data(G_OBJECT(content_stack), "apt-date");
    guint y, m, d; gtk_calendar_get_date(GTK_CALENDAR(cal), &y, &m, &d); m++;
    snprintf(a->date, MAX_DATE, "%02u/%02u/%04u", d, m, y);

    const char *tm = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(g_object_get_data(G_OBJECT(content_stack), "apt-time")));
    if (!tm || strcmp(tm, "Select Time") == 0) { show_notification("Please select a time.", "error"); free(a); return; }
    strncpy(a->time, tm, MAX_TIME-1);

    const char *dept = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(g_object_get_data(G_OBJECT(content_stack), "apt-dept")));
    if (dept) strncpy(a->department, dept, MAX_DEPT-1);

    const char *doc = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(g_object_get_data(G_OBJECT(content_stack), "apt-doctor")));
    if (doc && strcmp(doc, "Select Doctor") != 0) strncpy(a->doctorName, doc, MAX_DOCTOR-1);

    GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_object_get_data(G_OBJECT(content_stack), "apt-symptoms")));
    GtkTextIter start, end; gtk_text_buffer_get_bounds(buf, &start, &end);
    char *sym = gtk_text_buffer_get_text(buf, &start, &end, FALSE);
    if (sym && strlen(sym) > 0) strncpy(a->symptoms, sym, MAX_SYMPTOMS-1);
    else strcpy(a->symptoms, "Not specified");
    g_free(sym);

    insertAppointmentToList(a);
    appendAuditLog("INSERT_APPOINTMENT", a->appointmentID, "SUCCESS");
    saveAppointments();
    show_notification("Appointment booked!", "success");
    GtkWidget *al = g_object_get_data(G_OBJECT(content_stack), "appointments-list");
    if (al) populate_appointment_list(al, a->date);
    switch_to_view("appointments");
}

/* ==================== QUEUE VIEW ==================== */

void update_queue_display(void) {
    GtkWidget *flow = g_object_get_data(G_OBJECT(content_stack), "queue-flow");
    if (!flow) return;
    GList *ch = gtk_container_get_children(GTK_CONTAINER(flow));
    g_list_free_full(ch, (GDestroyNotify)gtk_widget_destroy);

    QueueNode *cur = walkInQueue.front;
    int pos = 1;
    while (cur) {
        GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
        gtk_widget_set_margin_start(card, 8); gtk_widget_set_margin_end(card, 8);
        gtk_widget_set_margin_top(card, 8); gtk_widget_set_margin_bottom(card, 8);
        gtk_widget_set_size_request(card, 180, -1);
        gtk_style_context_add_class(gtk_widget_get_style_context(card), "card");

        if (cur->isEmergency) {
            GtkWidget *eb = badge("EMERGENCY", "danger");
            gtk_box_pack_start(GTK_BOX(card), eb, FALSE, FALSE, 0);
        }
        gchar *num = g_strdup_printf("#%d", pos++);
        GtkWidget *nl = gtk_label_new(num);
        gtk_style_context_add_class(gtk_widget_get_style_context(nl), "stat-number");
        gtk_box_pack_start(GTK_BOX(card), nl, FALSE, FALSE, 0);
        GtkWidget *nn = gtk_label_new(cur->name);
        gtk_box_pack_start(GTK_BOX(card), nn, FALSE, FALSE, 0);
        gchar *wt = g_strdup_printf("~%d min", pos * 10);
        GtkWidget *wl = gtk_label_new(wt);
        gtk_style_context_add_class(gtk_widget_get_style_context(wl), "dim-label");
        gtk_box_pack_start(GTK_BOX(card), wl, FALSE, FALSE, 0);
        g_free(wt); g_free(num);

        gtk_flow_box_insert(GTK_FLOW_BOX(flow), card, -1);
        cur = cur->next;
    }

    GtkWidget *badge_w = g_object_get_data(G_OBJECT(content_stack), "queue-count");
    if (badge_w) {
        gchar *t = g_strdup_printf("%d in queue", walkInQueue.size);
        gtk_label_set_text(GTK_LABEL(badge_w), t);
        g_free(t);
    }
    gtk_widget_show_all(flow);
}

GtkWidget* create_queue_view(void) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_style_context_add_class(gtk_widget_get_style_context(box), "content-area");
    gtk_widget_set_margin_start(box, 24); gtk_widget_set_margin_end(box, 24);
    gtk_widget_set_margin_top(box, 24); gtk_widget_set_margin_bottom(box, 24);

    GtkWidget *hb = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *tl = gtk_label_new("Walk-in Queue");
    gtk_style_context_add_class(gtk_widget_get_style_context(tl), "card-title");
    gtk_box_pack_start(GTK_BOX(hb), tl, TRUE, TRUE, 0);
    GtkWidget *cb = badge("0 in queue", "info");
    g_object_set_data(G_OBJECT(content_stack), "queue-count", cb);
    gtk_box_pack_start(GTK_BOX(hb), cb, FALSE, FALSE, 0);

    GtkWidget *nx = gtk_button_new_with_label("Call Next");
    gtk_style_context_add_class(gtk_widget_get_style_context(nx), "success");
    g_signal_connect(nx, "clicked", G_CALLBACK(on_queue_next), NULL);
    gtk_box_pack_end(GTK_BOX(hb), nx, FALSE, FALSE, 0);
    GtkWidget *ad = gtk_button_new_with_label("+ Add Walk-in");
    gtk_style_context_add_class(gtk_widget_get_style_context(ad), "primary");
    g_signal_connect(ad, "clicked", G_CALLBACK(on_queue_add), NULL);
    gtk_box_pack_end(GTK_BOX(hb), ad, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), hb, FALSE, FALSE, 0);

    GtkWidget *qc = create_card("Current Queue");
    gtk_box_pack_start(GTK_BOX(box), qc, TRUE, TRUE, 0);
    GtkWidget *qsc = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(qc), qsc);
    GtkWidget *flow = gtk_flow_box_new();
    gtk_flow_box_set_homogeneous(GTK_FLOW_BOX(flow), TRUE);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(flow), 4);
    gtk_container_add(GTK_CONTAINER(qsc), flow);
    g_object_set_data(G_OBJECT(content_stack), "queue-flow", flow);

    GtkWidget *sc = create_card("Now Serving");
    gtk_box_pack_start(GTK_BOX(box), sc, FALSE, FALSE, 0);
    GtkWidget *sl = gtk_label_new("No patient being served");
    gtk_style_context_add_class(gtk_widget_get_style_context(sl), "stat-number");
    gtk_widget_set_margin_top(sl, 16); gtk_widget_set_margin_bottom(sl, 16);
    gtk_container_add(GTK_CONTAINER(sc), sl);
    g_object_set_data(G_OBJECT(content_stack), "now-serving-label", sl);

    update_queue_display();
    return box;
}

static void do_enqueue(const char *pid, const char *name, int is_emergency) {
    QueueNode *qn = (QueueNode*)malloc(sizeof(QueueNode));
    if (!qn) return;
    strcpy(qn->patientID, pid);
    strcpy(qn->name, name);
    strcpy(qn->timeAdded, getCurrentTimestamp());
    qn->isEmergency = is_emergency;
    qn->next = NULL;
    if (qn->isEmergency) {
        QueueNode *cq = walkInQueue.front, *pv = NULL;
        while (cq && !cq->isEmergency) { pv = cq; cq = cq->next; }
        if (!pv) { qn->next = walkInQueue.front; walkInQueue.front = qn; }
        else { qn->next = cq; pv->next = qn; }
        if (!qn->next) walkInQueue.rear = qn;
    } else {
        if (!walkInQueue.rear) walkInQueue.front = walkInQueue.rear = qn;
        else { walkInQueue.rear->next = qn; walkInQueue.rear = qn; }
    }
    walkInQueue.size++;
    update_queue_display();
}

void on_queue_add(GtkButton *btn, gpointer data) {
    (void)btn; (void)data;
    GtkWidget *d = gtk_dialog_new_with_buttons("Add Walk-in Patient",
        GTK_WINDOW(main_window), GTK_DIALOG_MODAL, "Add", GTK_RESPONSE_ACCEPT, "Cancel", GTK_RESPONSE_REJECT, NULL);
    GtkWidget *c = gtk_dialog_get_content_area(GTK_DIALOG(d));
    GtkWidget *e = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(e), "Patient ID (or leave blank to register new)");
    gtk_box_pack_start(GTK_BOX(c), e, FALSE, FALSE, 10);
    GtkWidget *em = gtk_check_button_new_with_label("Emergency");
    gtk_box_pack_start(GTK_BOX(c), em, FALSE, FALSE, 10);
    gtk_widget_show_all(d);

    if (gtk_dialog_run(GTK_DIALOG(d)) == GTK_RESPONSE_ACCEPT) {
        const char *pid = gtk_entry_get_text(GTK_ENTRY(e));
        Patient *p = strlen(pid) > 0 ? findPatientByID(pid) : NULL;

        if (p) {
            int is_em = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(em)) ? 1 : 0;
            do_enqueue(pid, p->name, is_em);
            gtk_widget_destroy(d);
            return;
        }

        gtk_widget_destroy(d);

        GtkWidget *rd = gtk_dialog_new_with_buttons("Quick Register Walk-in Patient",
            GTK_WINDOW(main_window), GTK_DIALOG_MODAL,
            "Register & Enqueue", GTK_RESPONSE_ACCEPT,
            "Cancel", GTK_RESPONSE_REJECT, NULL);
        GtkWidget *rc = gtk_dialog_get_content_area(GTK_DIALOG(rd));
        gtk_box_set_spacing(GTK_BOX(rc), 8);

        GtkWidget *nml = gtk_label_new("Register new patient and add to queue:");
        gtk_style_context_add_class(gtk_widget_get_style_context(nml), "dim-label");
        gtk_box_pack_start(GTK_BOX(rc), nml, FALSE, FALSE, 0);

        GtkWidget *nm_e = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(nm_e), "Full Name *");
        gtk_box_pack_start(GTK_BOX(rc), nm_e, FALSE, FALSE, 0);

        GtkWidget *ic_e = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(ic_e), "IC Number (000000-00-0000)");
        gtk_box_pack_start(GTK_BOX(rc), ic_e, FALSE, FALSE, 0);

        GtkWidget *ph_e = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(ph_e), "Phone");
        gtk_box_pack_start(GTK_BOX(rc), ph_e, FALSE, FALSE, 0);

        GtkWidget *rem = gtk_check_button_new_with_label("Emergency");
        gtk_box_pack_start(GTK_BOX(rc), rem, FALSE, FALSE, 0);

        gtk_widget_show_all(rd);

        if (gtk_dialog_run(GTK_DIALOG(rd)) == GTK_RESPONSE_ACCEPT) {
            const char *nm = gtk_entry_get_text(GTK_ENTRY(nm_e));
            if (strlen(nm) == 0) {
                show_notification("Name is required!", "error");
            } else {
                Patient *np = createPatientNode();
                if (np) {
                    generatePatientID(np->patientID);
                    strncpy(np->name, nm, MAX_NAME - 1);
                    strncpy(np->icNumber, gtk_entry_get_text(GTK_ENTRY(ic_e)), MAX_IC - 1);
                    strncpy(np->phone, gtk_entry_get_text(GTK_ENTRY(ph_e)), MAX_PHONE - 1);
                    strcpy(np->email, "walkin@uthm.edu.my");
                    strcpy(np->faculty, "FSKTM");
                    strcpy(np->program, "Walk-in");
                    strcpy(np->gender, "Unknown");
                    np->age = 0;
                    strcpy(np->bloodType, "O+");
                    strcpy(np->allergies, "None");
                    strcpy(np->emergencyContact, "N/A");

                    insertPatientToList(np);
                    insertPatientToBST(&patientBST, np);
                    hashInsert(np->patientID);
                    appendAuditLog("INSERT_PATIENT", np->patientID, "SUCCESS");
                    savePatients();

                    int is_em = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rem)) ? 1 : 0;
                    do_enqueue(np->patientID, np->name, is_em);
                    gchar *msg = g_strdup_printf("Walk-in registered: %s (%s)", np->name, np->patientID);
                    update_status(msg);
                    g_free(msg);
                }
            }
        }
        gtk_widget_destroy(rd);
        return;
    }
    gtk_widget_destroy(d);
}

void on_queue_next(GtkButton *btn, gpointer data) {
    (void)btn; (void)data;
    if (!walkInQueue.front) { show_notification("Queue is empty.", "info"); return; }
    QueueNode *temp = walkInQueue.front;
    walkInQueue.front = temp->next;
    if (!walkInQueue.front) walkInQueue.rear = NULL;
    walkInQueue.size--;

    GtkWidget *sl = g_object_get_data(G_OBJECT(content_stack), "now-serving-label");
    if (sl) {
        gtk_label_set_text(GTK_LABEL(sl), temp->name);
    }

    gchar *msg = g_strdup_printf("Now serving: %s (%s)", temp->name, temp->patientID);
    update_status(msg);
    g_free(msg);
    free(temp);
    update_queue_display();
}

/* ==================== REPORTS VIEW ==================== */

GtkWidget* create_report_view(void) {
    GtkWidget *sc = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sc), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 18);
    gtk_widget_set_margin_start(box, 24); gtk_widget_set_margin_end(box, 24);
    gtk_widget_set_margin_top(box, 24); gtk_widget_set_margin_bottom(box, 24);
    gtk_container_add(GTK_CONTAINER(sc), box);

    GtkWidget *hb = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *tl = gtk_label_new("Reports & Analytics");
    gtk_style_context_add_class(gtk_widget_get_style_context(tl), "card-title");
    gtk_box_pack_start(GTK_BOX(hb), tl, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), hb, FALSE, FALSE, 0);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 14);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 14);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_box_pack_start(GTK_BOX(box), grid, FALSE, FALSE, 0);

    set_stat(stat_card("Total Appointments", "office-calendar", "blue"), 0);
    set_stat(stat_card("Avg Wait Time", "clock", "teal"), 15);
    set_stat(stat_card("No-Show Rate", "dialog-warning", "amber"), 5);
    set_stat(stat_card("Satisfaction", "emblem-favorite", "green"), 92);

    GtkWidget *dep = create_card("Department Distribution");
    gtk_box_pack_start(GTK_BOX(box), dep, TRUE, TRUE, 0);
    GtkWidget *da = gtk_drawing_area_new();
    gtk_widget_set_size_request(da, -1, 200);
    g_signal_connect(da, "draw", G_CALLBACK(draw_chart), NULL);
    gtk_container_add(GTK_CONTAINER(dep), da);

    return sc;
}

gboolean draw_chart(GtkWidget *w, cairo_t *cr, gpointer data) {
    (void)data;
    int W = gtk_widget_get_allocated_width(w);
    int H = gtk_widget_get_allocated_height(w);
    cairo_set_source_rgb(cr, 0.97, 0.97, 0.97);
    cairo_paint(cr);

    int vals[] = {45, 72, 38, 91, 56};
    const char *lbls[] = {"General", "Dental", "Mental", "Physio", "Other"};
    int n = 5, bw = (W - 80) / n;

    for (int i = 0; i < n; i++) {
        int bh = (vals[i] * (H - 60)) / 100;
        int x = 40 + i * bw + bw / 4;
        int y = H - 30 - bh;
        double clrs[][3] = {{0,0.4,0.8},{0,0.64,0.64},{1,0.42,0.21},{0.16,0.79,0.59},{0.6,0.6,0.6}};
        cairo_set_source_rgb(cr, clrs[i][0], clrs[i][1], clrs[i][2]);
        cairo_rectangle(cr, x, y, bw / 2, bh);
        cairo_fill(cr);
        cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
        cairo_set_font_size(cr, 10);
        cairo_move_to(cr, x + bw / 4 - 10, H - 12);
        cairo_show_text(cr, lbls[i]);
    }
    return FALSE;
}

/* ==================== SEARCH VIEW ==================== */

GtkWidget* create_search_view(void) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_style_context_add_class(gtk_widget_get_style_context(box), "content-area");
    gtk_widget_set_margin_start(box, 24); gtk_widget_set_margin_end(box, 24);
    gtk_widget_set_margin_top(box, 24); gtk_widget_set_margin_bottom(box, 24);

    GtkWidget *sc = create_card("Advanced Search");
    gtk_box_pack_start(GTK_BOX(box), sc, FALSE, FALSE, 0);
    GtkWidget *sb = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(sb, 16); gtk_widget_set_margin_end(sb, 16);
    gtk_widget_set_margin_top(sb, 16); gtk_widget_set_margin_bottom(sb, 16);
    gtk_container_add(GTK_CONTAINER(sc), sb);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 14);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_box_pack_start(GTK_BOX(sb), grid, FALSE, FALSE, 0);

    GtkWidget *id_e = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(id_e), "Patient ID...");
    GtkWidget *nm_e = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(nm_e), "Name...");
    GtkWidget *dt_e = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(dt_e), "Date (DD/MM/YYYY)");
    GtkWidget *dc = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dc), "All Departments");
    const char *deps[] = {"General","Dental","Mental Health","Physiotherapy",NULL};
    for (int i = 0; deps[i]; i++) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dc), deps[i]);

    gtk_grid_attach(GTK_GRID(grid), form_row("Patient ID", id_e), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), form_row("Name", nm_e), 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), form_row("Date", dt_e), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), form_row("Department", dc), 1, 1, 1, 1);

    GtkWidget *sh = gtk_button_new_with_label("Search");
    gtk_style_context_add_class(gtk_widget_get_style_context(sh), "primary");
    gtk_widget_set_halign(sh, GTK_ALIGN_END);
    gtk_box_pack_start(GTK_BOX(sb), sh, FALSE, FALSE, 0);
    g_object_set_data(G_OBJECT(content_stack), "search-id", id_e);
    g_object_set_data(G_OBJECT(content_stack), "search-name", nm_e);
    g_object_set_data(G_OBJECT(content_stack), "search-date", dt_e);
    g_object_set_data(G_OBJECT(content_stack), "search-dept", dc);
    g_signal_connect(sh, "clicked", G_CALLBACK(on_search_execute), NULL);

    GtkWidget *rc = create_card("Results");
    gtk_box_pack_start(GTK_BOX(box), rc, TRUE, TRUE, 0);
    GtkWidget *rsc = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(rc), rsc);
    GtkWidget *rl = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(rsc), rl);
    g_object_set_data(G_OBJECT(content_stack), "search-results", rl);

    return box;
}

void on_search_execute(GtkButton *btn, gpointer data) {
    (void)btn; (void)data;
    const char *id = gtk_entry_get_text(GTK_ENTRY(g_object_get_data(G_OBJECT(content_stack), "search-id")));
    const char *nm = gtk_entry_get_text(GTK_ENTRY(g_object_get_data(G_OBJECT(content_stack), "search-name")));
    const char *dt = gtk_entry_get_text(GTK_ENTRY(g_object_get_data(G_OBJECT(content_stack), "search-date")));

    GtkWidget *rl = g_object_get_data(G_OBJECT(content_stack), "search-results");
    if (!rl) return;
    GList *ch = gtk_container_get_children(GTK_CONTAINER(rl));
    g_list_free_full(ch, (GDestroyNotify)gtk_widget_destroy);

    Patient *cur = patientList;
    while (cur) {
        int match = 1;
        if (strlen(id) > 0 && strcmp(cur->patientID, id) != 0) match = 0;
        if (match && strlen(nm) > 0 && !strstr(cur->name, nm)) match = 0;
        if (match) {
            GtkWidget *row = gtk_list_box_row_new();
            GtkWidget *rb = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
            gtk_widget_set_margin_start(rb, 12); gtk_widget_set_margin_end(rb, 12);
            gtk_widget_set_margin_top(rb, 8); gtk_widget_set_margin_bottom(rb, 8);
            gchar *txt = g_strdup_printf("%s - %s (%s) %s", cur->patientID, cur->name, cur->icNumber, cur->phone);
            GtkWidget *lb = gtk_label_new(txt);
            g_free(txt);
            gtk_box_pack_start(GTK_BOX(rb), lb, TRUE, TRUE, 0);
            gtk_container_add(GTK_CONTAINER(row), rb);
            gtk_list_box_insert(GTK_LIST_BOX(rl), row, -1);
        }
        cur = cur->next;
    }

    Appointment *acur = appointmentList;
    while (acur) {
        int match = 1;
        if (strlen(id) > 0 && strcmp(acur->patientID, id) != 0) match = 0;
        if (match && strlen(dt) > 0 && strcmp(acur->date, dt) != 0) match = 0;
        if (match) {
            GtkWidget *row = gtk_list_box_row_new();
            GtkWidget *rb = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
            gtk_widget_set_margin_start(rb, 12); gtk_widget_set_margin_end(rb, 12);
            gtk_widget_set_margin_top(rb, 8); gtk_widget_set_margin_bottom(rb, 8);
            gchar *txt = g_strdup_printf("APT: %s | %s | %s | %s", acur->appointmentID, acur->patientID, acur->date, acur->status);
            GtkWidget *lb = gtk_label_new(txt);
            g_free(txt);
            gtk_box_pack_start(GTK_BOX(rb), lb, TRUE, TRUE, 0);
            gtk_container_add(GTK_CONTAINER(row), rb);
            gtk_list_box_insert(GTK_LIST_BOX(rl), row, -1);
        }
        acur = acur->next;
    }

    gtk_widget_show_all(rl);
    update_status("Search complete.");
}

/* ==================== SETTINGS VIEW ==================== */

GtkWidget* create_settings_view(void) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_style_context_add_class(gtk_widget_get_style_context(box), "content-area");
    gtk_widget_set_margin_start(box, 24); gtk_widget_set_margin_end(box, 24);
    gtk_widget_set_margin_top(box, 24); gtk_widget_set_margin_bottom(box, 24);

    GtkWidget *tl = gtk_label_new("System Settings");
    gtk_style_context_add_class(gtk_widget_get_style_context(tl), "card-title");
    gtk_widget_set_halign(tl, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(box), tl, FALSE, FALSE, 0);

    GtkWidget *uc = create_card("User Management");
    gtk_box_pack_start(GTK_BOX(box), uc, FALSE, FALSE, 0);
    GtkWidget *ub = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_start(ub, 16); gtk_widget_set_margin_end(ub, 16);
    gtk_widget_set_margin_top(ub, 12); gtk_widget_set_margin_bottom(ub, 12);
    gtk_container_add(GTK_CONTAINER(uc), ub);
    GtkWidget *mu = gtk_button_new_with_label("Manage Users");
    gtk_style_context_add_class(gtk_widget_get_style_context(mu), "primary");
    g_signal_connect(mu, "clicked", G_CALLBACK(on_manage_users), NULL);
    gtk_box_pack_start(GTK_BOX(ub), mu, FALSE, FALSE, 0);

    GtkWidget *dc = create_card("Data Management");
    gtk_box_pack_start(GTK_BOX(box), dc, FALSE, FALSE, 0);
    GtkWidget *db = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_start(db, 16); gtk_widget_set_margin_end(db, 16);
    gtk_widget_set_margin_top(db, 12); gtk_widget_set_margin_bottom(db, 12);
    gtk_container_add(GTK_CONTAINER(dc), db);
    GtkWidget *bkp = gtk_button_new_with_label("Backup Data");
    gtk_style_context_add_class(gtk_widget_get_style_context(bkp), "success");
    g_signal_connect(bkp, "clicked", G_CALLBACK(on_backup), NULL);
    gtk_box_pack_start(GTK_BOX(db), bkp, FALSE, FALSE, 0);
    GtkWidget *rst = gtk_button_new_with_label("Restore Data");
    gtk_style_context_add_class(gtk_widget_get_style_context(rst), "warning");
    g_signal_connect(rst, "clicked", G_CALLBACK(on_restore), NULL);
    gtk_box_pack_start(GTK_BOX(db), rst, FALSE, FALSE, 0);

    GtkWidget *ac = create_card("Audit Log");
    gtk_box_pack_start(GTK_BOX(box), ac, TRUE, TRUE, 0);
    GtkWidget *as = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(ac), as);
    GtkWidget *av = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(av), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(av), TRUE);
    gtk_container_add(GTK_CONTAINER(as), av);

    FILE *f = fopen("data/audit_log.txt", "r");
    if (f) {
        fseek(f, 0, SEEK_END); long sz = ftell(f); rewind(f);
        if (sz > 0) {
            char *buf = (char*)malloc(sz + 1);
            fread(buf, 1, sz, f); buf[sz] = '\0';
            GtkTextBuffer *tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(av));
            gtk_text_buffer_set_text(tb, buf, -1);
            free(buf);
        }
        fclose(f);
    }

    return box;
}

void on_manage_users(GtkButton *btn, gpointer data) {
    (void)btn; (void)data;
    GtkWidget *d = gtk_dialog_new_with_buttons("User Management",
        GTK_WINDOW(main_window), GTK_DIALOG_MODAL, "Add", GTK_RESPONSE_ACCEPT, "Close", GTK_RESPONSE_CLOSE, NULL);
    gtk_window_set_default_size(GTK_WINDOW(d), 500, 400);

    GtkWidget *c = gtk_dialog_get_content_area(GTK_DIALOG(d));
    GtkListStore *s = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    User *cur = userList;
    while (cur) {
        GtkTreeIter it; gtk_list_store_append(s, &it);
        gtk_list_store_set(s, &it, 0, cur->username, 1, cur->role, 2, cur->fullName, 3, cur->isActive ? "Active" : "Inactive", -1);
        cur = cur->next;
    }
    GtkWidget *tv = gtk_tree_view_new_with_model(GTK_TREE_MODEL(s));
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(tv), GTK_TREE_VIEW_GRID_LINES_HORIZONTAL);
    const char *ucols[] = {"Username", "Role", "Full Name", "Status"};
    for (int i = 0; i < 4; i++) {
        GtkCellRenderer *r = gtk_cell_renderer_text_new();
        gtk_tree_view_append_column(GTK_TREE_VIEW(tv), gtk_tree_view_column_new_with_attributes(ucols[i], r, "text", i, NULL));
    }
    GtkWidget *sc = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(sc), tv);
    gtk_box_pack_start(GTK_BOX(c), sc, TRUE, TRUE, 0);
    gtk_widget_show_all(d);
    gtk_dialog_run(GTK_DIALOG(d));
    gtk_widget_destroy(d);
}

void on_backup(GtkButton *btn, gpointer data) {
    (void)btn; (void)data;
    backupData();
    show_notification("Backup completed.", "success");
}

void on_restore(GtkButton *btn, gpointer data) {
    (void)btn; (void)data;
    show_confirmation("Restore Data", "This will overwrite current data. Continue?", G_CALLBACK(do_restore), NULL);
}

void do_restore(void) {
    restoreFromBackup();
    show_notification("Data restored from latest backup.", "success");
}

/* ==================== MAIN WINDOW CREATION ==================== */

void create_main_window(void) {
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(main_window), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER);

    create_header();

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(main_window), vbox);

    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), paned, TRUE, TRUE, 0);

    create_sidebar();
    gtk_paned_pack1(GTK_PANED(paned), sidebar_box, FALSE, FALSE);

    content_stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(content_stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(content_stack), 250);

    gtk_stack_add_named(GTK_STACK(content_stack), create_dashboard_view(), "dashboard");
    gtk_stack_add_named(GTK_STACK(content_stack), create_patient_list_view(), "patients");
    gtk_stack_add_named(GTK_STACK(content_stack), create_patient_form_view(), "patient-form");
    gtk_stack_add_named(GTK_STACK(content_stack), create_appointment_list_view(), "appointments");
    gtk_stack_add_named(GTK_STACK(content_stack), create_appointment_form_view(), "appointment-form");
    gtk_stack_add_named(GTK_STACK(content_stack), create_queue_view(), "queue");
    gtk_stack_add_named(GTK_STACK(content_stack), create_report_view(), "reports");
    gtk_stack_add_named(GTK_STACK(content_stack), create_search_view(), "search");
    gtk_stack_add_named(GTK_STACK(content_stack), create_settings_view(), "settings");

    gtk_paned_pack2(GTK_PANED(paned), content_stack, TRUE, FALSE);

    status_bar = gtk_statusbar_new();
    gtk_box_pack_end(GTK_BOX(vbox), status_bar, FALSE, FALSE, 0);

    g_signal_connect(main_window, "destroy", G_CALLBACK(on_main_window_closed), NULL);

    switch_to_view("dashboard");
    update_dashboard_stats();
    update_status("System ready.");
}


