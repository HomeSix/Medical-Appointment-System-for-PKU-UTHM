#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 50
#define MAX_IC 20
#define MAX_PHONE 15
#define MAX_EMAIL 50
#define MAX_DATE 20
#define MAX_TIME 10
#define MAX_DEPT 30
#define MAX_STATUS 20

typedef struct Appointment {
    int patientId;
    char patientName[MAX_NAME + 1];
    char icNumber[MAX_IC + 1];
    char phoneNumber[MAX_PHONE + 1];
    char email[MAX_EMAIL + 1];
    char appointmentDate[MAX_DATE + 1];
    char appointmentTime[MAX_TIME + 1];
    char doctorName[MAX_NAME + 1];
    char department[MAX_DEPT + 1];
    char status[MAX_STATUS + 1];
    struct Appointment* next;
} Appointment;

typedef struct {
    Appointment* head;
    int nextId;
} AppointmentList;

AppointmentList list;

typedef struct {
    GtkWidget* window;
    GtkWidget* content_area;
    GtkWidget* status_bar;
    GtkWidget* entry_name;
    GtkWidget* entry_ic;
    GtkWidget* entry_phone;
    GtkWidget* entry_email;
    GtkWidget* entry_date;
    GtkWidget* entry_time;
    GtkWidget* entry_doctor;
    GtkWidget* entry_dept;
    GtkWidget* status_combo;
    GtkWidget* search_entry;
    GtkWidget* filter_dept_entry;
    GtkWidget* filter_date_entry;
    GtkWidget* remove_entry;
    GtkWidget* current_view_label;
} AppWidgets;

void initList(AppointmentList* list) {
    list->head = NULL;
    list->nextId = 1;
}

int isEmpty(AppointmentList* list) {
    return list->head == NULL;
}

void getValidStatus(int choice, char* status) {
    switch(choice) {
        case 0: strcpy(status, "Pending"); break;
        case 1: strcpy(status, "Confirmed"); break;
        case 2: strcpy(status, "Completed"); break;
        case 3: strcpy(status, "Cancelled"); break;
        default: strcpy(status, "Pending");
    }
}

void insertSorted(AppointmentList* list, Appointment* newNode) {
    if(isEmpty(list) || strcasecmp(newNode->patientName, list->head->patientName) < 0) {
        newNode->next = list->head;
        list->head = newNode;
    } else {
        Appointment* current = list->head;
        while(current->next && strcasecmp(newNode->patientName, current->next->patientName) > 0) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

void addAppointment(AppointmentList* list, const char* name, const char* ic, const char* phone,
                   const char* email, const char* date, const char* time, const char* doctor,
                   const char* dept, int statusIdx) {
    Appointment* newNode = (Appointment*)malloc(sizeof(Appointment));
    if(!newNode) return;

    newNode->patientId = list->nextId++;
    strncpy(newNode->patientName, name, MAX_NAME); newNode->patientName[MAX_NAME] = '\0';
    strncpy(newNode->icNumber, ic, MAX_IC); newNode->icNumber[MAX_IC] = '\0';
    strncpy(newNode->phoneNumber, phone, MAX_PHONE); newNode->phoneNumber[MAX_PHONE] = '\0';
    strncpy(newNode->email, email, MAX_EMAIL); newNode->email[MAX_EMAIL] = '\0';
    strncpy(newNode->appointmentDate, date, MAX_DATE); newNode->appointmentDate[MAX_DATE] = '\0';
    strncpy(newNode->appointmentTime, time, MAX_TIME); newNode->appointmentTime[MAX_TIME] = '\0';
    strncpy(newNode->doctorName, doctor, MAX_NAME); newNode->doctorName[MAX_NAME] = '\0';
    strncpy(newNode->department, dept, MAX_DEPT); newNode->department[MAX_DEPT] = '\0';
    getValidStatus(statusIdx, newNode->status);
    newNode->next = NULL;
    insertSorted(list, newNode);
}

int removeAppointment(AppointmentList* list, int patientId) {
    if(isEmpty(list)) return 0;
    Appointment* current = list->head;
    Appointment* prev = NULL;
    while(current) {
        if(current->patientId == patientId) {
            if(prev) prev->next = current->next;
            else list->head = current->next;
            free(current);
            return 1;
        }
        prev = current;
        current = current->next;
    }
    return 0;
}

int updateAppointment(AppointmentList* list, int patientId, const char* name, const char* ic,
                     const char* phone, const char* email, const char* date, const char* time,
                     const char* doctor, const char* dept, int statusIdx) {
    Appointment* current = list->head;
    while(current) {
        if(current->patientId == patientId) {
            if(strlen(name) > 0) strncpy(current->patientName, name, MAX_NAME);
            if(strlen(ic) > 0) strncpy(current->icNumber, ic, MAX_IC);
            if(strlen(phone) > 0) strncpy(current->phoneNumber, phone, MAX_PHONE);
            if(strlen(email) > 0) strncpy(current->email, email, MAX_EMAIL);
            if(strlen(date) > 0) strncpy(current->appointmentDate, date, MAX_DATE);
            if(strlen(time) > 0) strncpy(current->appointmentTime, time, MAX_TIME);
            if(strlen(doctor) > 0) strncpy(current->doctorName, doctor, MAX_NAME);
            if(strlen(dept) > 0) strncpy(current->department, dept, MAX_DEPT);
            getValidStatus(statusIdx, current->status);
            return 1;
        }
        current = current->next;
    }
    return 0;
}

Appointment* searchAppointment(AppointmentList* list, int patientId) {
    Appointment* current = list->head;
    while(current) {
        if(current->patientId == patientId) return current;
        current = current->next;
    }
    return NULL;
}

int countAppointments(AppointmentList* list) {
    int count = 0;
    Appointment* current = list->head;
    while(current) { count++; current = current->next; }
    return count;
}

void getAllAppointments(AppointmentList* list, char* buffer, int bufferSize) {
    strcpy(buffer, "");
    if(isEmpty(list)) { strcpy(buffer, "No appointments available."); return; }

    char temp[600];
    Appointment* current = list->head;
    while(current) {
        snprintf(temp, sizeof(temp),
            "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
            "📋 ID: %d\n"
            "👤 Name: %s\n"
            "🆔 IC: %s\n"
            "📞 Phone: %s\n"
            "📧 Email: %s\n"
            "📅 Date: %s  ⏰ Time: %s\n"
            "👨‍⚕️ Doctor: %s\n"
            "🏥 Department: %s\n"
            "✅ Status: %s\n"
            "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n",
            current->patientId, current->patientName, current->icNumber,
            current->phoneNumber, current->email, current->appointmentDate,
            current->appointmentTime, current->doctorName, current->department, current->status);
        strcat(buffer, temp);
        current = current->next;
    }
}

void getAppointmentsByDept(AppointmentList* list, const char* dept, char* buffer) {
    strcpy(buffer, "");
    int found = 0;
    Appointment* current = list->head;
    char temp[400];
    while(current) {
        if(strcasecmp(current->department, dept) == 0) {
            snprintf(temp, sizeof(temp),
                "ID: %d | %s | Dr. %s | %s | %s\n",
                current->patientId, current->patientName,
                current->doctorName, current->appointmentDate, current->status);
            strcat(buffer, temp);
            found = 1;
        }
        current = current->next;
    }
    if(!found) strcpy(buffer, "No appointments found in this department.");
}

void getAppointmentsByDate(AppointmentList* list, const char* date, char* buffer) {
    strcpy(buffer, "");
    int found = 0;
    Appointment* current = list->head;
    char temp[400];
    while(current) {
        if(strcmp(current->appointmentDate, date) == 0) {
            snprintf(temp, sizeof(temp),
                "ID: %d | %s | %s | %s | %s\n",
                current->patientId, current->patientName,
                current->doctorName, current->department, current->status);
            strcat(buffer, temp);
            found = 1;
        }
        current = current->next;
    }
    if(!found) strcpy(buffer, "No appointments found on this date.");
}

void getStatistics(AppointmentList* list, char* buffer) {
    if(isEmpty(list)) { strcpy(buffer, "No appointments to show statistics."); return; }

    int total = countAppointments(list);
    int pending = 0, confirmed = 0, completed = 0, cancelled = 0;

    Appointment* current = list->head;
    while(current) {
        if(strcasecmp(current->status, "Pending") == 0) pending++;
        else if(strcasecmp(current->status, "Confirmed") == 0) confirmed++;
        else if(strcasecmp(current->status, "Completed") == 0) completed++;
        else if(strcasecmp(current->status, "Cancelled") == 0) cancelled++;
        current = current->next;
    }

    snprintf(buffer, 800,
        "╔══════════════════════════════════════════╗\n"
        "║     📊 APPOINTMENT STATISTICS 📊          ║\n"
        "╠══════════════════════════════════════════╣\n"
        "║  Total Appointments: %d                    ║\n"
        "╠══════════════════════════════════════════╣\n"
        "║  🟡 Pending:    %-3d  (%.1f%%)              ║\n"
        "║  🔵 Confirmed:  %-3d  (%.1f%%)              ║\n"
        "║  🟢 Completed:  %-3d  (%.1f%%)              ║\n"
        "║  🔴 Cancelled:  %-3d  (%.1f%%)              ║\n"
        "╚══════════════════════════════════════════╝",
        total,
        pending, (float)pending / total * 100,
        confirmed, (float)confirmed / total * 100,
        completed, (float)completed / total * 100,
        cancelled, (float)cancelled / total * 100);
}

void saveToFile(AppointmentList* list, const char* filename) {
    FILE* file = fopen(filename, "w");
    if(!file) return;
    Appointment* current = list->head;
    while(current) {
        fprintf(file, "%d|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
                current->patientId, current->patientName,
                current->icNumber, current->phoneNumber,
                current->email, current->appointmentDate,
                current->appointmentTime, current->doctorName,
                current->department, current->status);
        current = current->next;
    }
    fclose(file);
}

void loadFromFile(AppointmentList* list, const char* filename) {
    FILE* file = fopen(filename, "r");
    if(!file) return;
    char line[500];
    int maxId = 0;
    while(fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        if(strlen(line) == 0) continue;
        Appointment* newNode = (Appointment*)malloc(sizeof(Appointment));
        if(!newNode) continue;
        sscanf(line, "%d|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]",
               &newNode->patientId, newNode->patientName, newNode->icNumber,
               newNode->phoneNumber, newNode->email, newNode->appointmentDate,
               newNode->appointmentTime, newNode->doctorName, newNode->department, newNode->status);
        if(newNode->patientId > maxId) maxId = newNode->patientId;
        newNode->next = NULL;
        insertSorted(list, newNode);
    }
    list->nextId = maxId + 1;
    fclose(file);
}

void showMessage(GtkWidget* parent, const char* message, const char* title, gboolean is_error) {
    GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        is_error ? GTK_MESSAGE_ERROR : GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK, "%s", message);
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void updateStatusBar(AppWidgets* app, const char* message) {
    gtk_label_set_text(GTK_LABEL(app->status_bar), message);
}

void updateContent(AppWidgets* app, const char* content) {
    GtkTextBuffer* buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->content_area));
    gtk_text_buffer_set_text(buf, content, -1);
}

void clearForm(AppWidgets* app) {
    gtk_entry_set_text(GTK_ENTRY(app->entry_name), "");
    gtk_entry_set_text(GTK_ENTRY(app->entry_ic), "");
    gtk_entry_set_text(GTK_ENTRY(app->entry_phone), "");
    gtk_entry_set_text(GTK_ENTRY(app->entry_email), "");
    gtk_entry_set_text(GTK_ENTRY(app->entry_date), "");
    gtk_entry_set_text(GTK_ENTRY(app->entry_time), "");
    gtk_entry_set_text(GTK_ENTRY(app->entry_doctor), "");
    gtk_entry_set_text(GTK_ENTRY(app->entry_dept), "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(app->status_combo), 0);
}

void on_add_clicked(GtkWidget* widget, AppWidgets* app) {
    const char* name = gtk_entry_get_text(GTK_ENTRY(app->entry_name));
    const char* ic = gtk_entry_get_text(GTK_ENTRY(app->entry_ic));
    const char* phone = gtk_entry_get_text(GTK_ENTRY(app->entry_phone));
    const char* email = gtk_entry_get_text(GTK_ENTRY(app->entry_email));
    const char* date = gtk_entry_get_text(GTK_ENTRY(app->entry_date));
    const char* time = gtk_entry_get_text(GTK_ENTRY(app->entry_time));
    const char* doctor = gtk_entry_get_text(GTK_ENTRY(app->entry_doctor));
    const char* dept = gtk_entry_get_text(GTK_ENTRY(app->entry_dept));
    gint active = gtk_combo_box_get_active(GTK_COMBO_BOX(app->status_combo));

    if(strlen(name) == 0 || strlen(date) == 0 || strlen(time) == 0) {
        showMessage(app->window, "Please fill in required fields (Name, Date, Time)", "Error", TRUE);
        return;
    }

    addAppointment(&list, name, ic, phone, email, date, time, doctor, dept, active);
    clearForm(app);
    char buf[100];
    snprintf(buf, sizeof(buf), "✅ Added appointment! Total: %d", countAppointments(&list));
    updateStatusBar(app, buf);
    showMessage(app->window, "Appointment added successfully!", "Success", FALSE);
}

void on_view_all_clicked(GtkWidget* widget, AppWidgets* app) {
    char buffer[6000];
    getAllAppointments(&list, buffer, sizeof(buffer));
    updateContent(app, buffer);
    gtk_label_set_text(GTK_LABEL(app->current_view_label), "📋 All Appointments");
    char buf[100];
    snprintf(buf, sizeof(buf), "Showing %d appointments", countAppointments(&list));
    updateStatusBar(app, buf);
}

void on_remove_clicked(GtkWidget* widget, AppWidgets* app) {
    const char* id_str = gtk_entry_get_text(GTK_ENTRY(app->remove_entry));
    int id = atoi(id_str);
    if(id <= 0) {
        showMessage(app->window, "Please enter a valid Patient ID", "Error", TRUE);
        return;
    }
    if(removeAppointment(&list, id)) {
        gtk_entry_set_text(GTK_ENTRY(app->remove_entry), "");
        char buf[100];
        snprintf(buf, sizeof(buf), "✅ Removed appointment ID %d", id);
        updateStatusBar(app, buf);
        showMessage(app->window, "Appointment removed successfully!", "Success", FALSE);
    } else {
        showMessage(app->window, "Appointment not found!", "Error", TRUE);
    }
}

void on_search_clicked(GtkWidget* widget, AppWidgets* app) {
    const char* id_str = gtk_entry_get_text(GTK_ENTRY(app->search_entry));
    int id = atoi(id_str);
    Appointment* apt = searchAppointment(&list, id);
    char buffer[1500];
    if(apt) {
        snprintf(buffer, sizeof(buffer),
            "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
            "🔍 SEARCH RESULT - ID: %d\n"
            "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
            "👤 Name: %s\n"
            "🆔 IC: %s\n"
            "📞 Phone: %s\n"
            "📧 Email: %s\n"
            "📅 Date: %s  ⏰ Time: %s\n"
            "👨‍⚕️ Doctor: %s\n"
            "🏥 Department: %s\n"
            "✅ Status: %s\n"
            "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━",
            apt->patientId, apt->patientName, apt->icNumber, apt->phoneNumber,
            apt->email, apt->appointmentDate, apt->appointmentTime,
            apt->doctorName, apt->department, apt->status);
    } else {
        strcpy(buffer, "❌ Appointment not found.");
    }
    updateContent(app, buffer);
    gtk_label_set_text(GTK_LABEL(app->current_view_label), "🔍 Search Result");
    updateStatusBar(app, id ? (apt ? "Found!" : "Not found") : "Enter ID to search");
}

void on_filter_dept_clicked(GtkWidget* widget, AppWidgets* app) {
    const char* dept = gtk_entry_get_text(GTK_ENTRY(app->filter_dept_entry));
    char buffer[3000];
    getAppointmentsByDept(&list, dept, buffer);
    updateContent(app, buffer);
    char label[100];
    snprintf(label, sizeof(label), "📁 Department: %s", dept);
    gtk_label_set_text(GTK_LABEL(app->current_view_label), label);
    updateStatusBar(app, "Filtered by department");
}

void on_filter_date_clicked(GtkWidget* widget, AppWidgets* app) {
    const char* date = gtk_entry_get_text(GTK_ENTRY(app->filter_date_entry));
    char buffer[3000];
    getAppointmentsByDate(&list, date, buffer);
    updateContent(app, buffer);
    char label[100];
    snprintf(label, sizeof(label), "📅 Date: %s", date);
    gtk_label_set_text(GTK_LABEL(app->current_view_label), label);
    updateStatusBar(app, "Filtered by date");
}

void on_statistics_clicked(GtkWidget* widget, AppWidgets* app) {
    char buffer[1000];
    getStatistics(&list, buffer);
    updateContent(app, buffer);
    gtk_label_set_text(GTK_LABEL(app->current_view_label), "📊 Statistics");
    updateStatusBar(app, "Statistics generated");
}

void on_save_clicked(GtkWidget* widget, AppWidgets* app) {
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Save Appointments",
        GTK_WINDOW(app->window), GTK_FILE_CHOOSER_ACTION_SAVE,
        "Cancel", GTK_RESPONSE_CANCEL, "Save", GTK_RESPONSE_ACCEPT, NULL);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        saveToFile(&list, filename);
        g_free(filename);
        updateStatusBar(app, "💾 Data saved successfully!");
        showMessage(app->window, "Data saved to file!", "Success", FALSE);
    }
    gtk_widget_destroy(dialog);
}

void on_load_clicked(GtkWidget* widget, AppWidgets* app) {
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Load Appointments",
        GTK_WINDOW(app->window), GTK_FILE_CHOOSER_ACTION_OPEN,
        "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);
    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        loadFromFile(&list, filename);
        g_free(filename);
        char buf[100];
        snprintf(buf, sizeof(buf), "📂 Loaded! Total: %d", countAppointments(&list));
        updateStatusBar(app, buf);
        showMessage(app->window, "Data loaded from file!", "Success", FALSE);
    }
    gtk_widget_destroy(dialog);
}

void applyCSS() {
    GtkCssProvider* provider = gtk_css_provider_new();
    const char* css =
        "window { background-color: #f0f4f8; } "
        "headerbar { background-color: #2c3e50; color: white; } "
        ".sidebar { background-color: #34495e; color: white; } "
        ".sidebar button { background-color: #3498db; color: white; border: none; padding: 12px; margin: 4px; border-radius: 6px; } "
        ".sidebar button:hover { background-color: #2980b9; } "
        ".sidebar button:active { background-color: #1a5276; } "
        ".content { background-color: white; border-radius: 8px; } "
        ".footer { background-color: #2c3e50; color: white; } "
        "textview { background-color: #ffffff; padding: 10px; } "
        ".form-title { font-size: 16px; font-weight: bold; color: #2c3e50; } "
        "entry { padding: 8px; border-radius: 4px; border: 1px solid #bdc3c7; } "
        ".stat-label { color: #2c3e50; font-weight: bold; }";

    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    initList(&list);
    applyCSS();

    AppWidgets* app = g_malloc(sizeof(AppWidgets));

    app->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(app->window), "🏥 Medical Appointment System");
    gtk_window_set_default_size(GTK_WINDOW(app->window), 1200, 800);
    gtk_window_set_position(GTK_WINDOW(app->window), GTK_WIN_POS_CENTER);
    g_signal_connect(app->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget* main_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(main_grid), 0);
    gtk_grid_set_column_spacing(GTK_GRID(main_grid), 0);
    gtk_container_add(GTK_CONTAINER(app->window), main_grid);

    GtkWidget* header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_grid_attach(GTK_GRID(main_grid), header, 0, 0, 3, 1);
    gtk_widget_set_hexpand(header, TRUE);
    gtk_widget_set_size_request(header, -1, 60);

    GtkCssProvider* header_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(header_provider, "background: #2c3e50; padding: 10px;", -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(header), GTK_STYLE_PROVIDER(header_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    GtkWidget* header_icon = gtk_image_new_from_icon_name("medical-symbolic", GTK_ICON_SIZE_DIALOG);
    gtk_box_pack_start(GTK_BOX(header), header_icon, FALSE, FALSE, 10);

    GtkWidget* header_label = gtk_label_new("🏥 Medical Appointment System - PKU UTHM");
    gtk_style_context_add_class(gtk_widget_get_style_context(header_label), "header-title");
    PangoAttrList* attrs = pango_attr_list_new();
    pango_attr_list_insert(attrs, pango_attr_weight_new(PANGO_WEIGHT_BOLD));
    pango_attr_list_insert(attrs, pango_attr_size_new(20 * PANGO_SCALE));
    gtk_label_set_attributes(GTK_LABEL(header_label), attrs);
    gtk_box_pack_start(GTK_BOX(header), header_label, FALSE, FALSE, 5);

    GtkWidget* header_stats = gtk_label_new(NULL);
    gtk_box_pack_end(GTK_BOX(header), header_stats, FALSE, FALSE, 10);

    GtkWidget* sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_grid_attach(GTK_GRID(main_grid), sidebar, 0, 1, 1, 1);
    gtk_widget_set_size_request(sidebar, 220, -1);

    GtkCssProvider* sidebar_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(sidebar_provider, "background: #34495e; padding: 10px;", -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(sidebar), GTK_STYLE_PROVIDER(sidebar_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    GtkWidget* sidebar_label = gtk_label_new("📋 Menu");
    gtk_style_context_add_class(gtk_widget_get_style_context(sidebar_label), "sidebar-title");
    PangoAttrList* sl_attr = pango_attr_list_new();
    pango_attr_list_insert(sl_attr, pango_attr_weight_new(PANGO_WEIGHT_BOLD));
    pango_attr_list_insert(sl_attr, pango_attr_size_new(18 * PANGO_SCALE));
    pango_attr_list_insert(sl_attr, pango_attr_foreground_new(65535, 65535, 65535));
    gtk_label_set_attributes(GTK_LABEL(sidebar_label), sl_attr);
    gtk_box_pack_start(GTK_BOX(sidebar), sidebar_label, FALSE, FALSE, 15);

    #define MAKE_BUTTON(label, callback, data) \
        GtkWidget* btn_##callback = gtk_button_new_with_label(label); \
        g_signal_connect(btn_##callback, "clicked", G_CALLBACK(callback), data); \
        gtk_box_pack_start(GTK_BOX(sidebar), btn_##callback, FALSE, FALSE, 3);

    MAKE_BUTTON("📋 View All Appointments", on_view_all_clicked, app)
    MAKE_BUTTON("➕ Add Appointment", on_add_clicked, app)
    MAKE_BUTTON("🔍 Search by ID", on_search_clicked, app)
    MAKE_BUTTON("🗑️ Remove by ID", on_remove_clicked, app)
    MAKE_BUTTON("📊 Statistics", on_statistics_clicked, app)
    MAKE_BUTTON("💾 Save to File", on_save_clicked, app)
    MAKE_BUTTON("📂 Load from File", on_load_clicked, app)

    GtkWidget* search_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(sidebar), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 10);

    GtkWidget* filter_label = gtk_label_new("🔎 Quick Filter");
    PangoAttrList* fl_attr = pango_attr_list_new();
    pango_attr_list_insert(fl_attr, pango_attr_weight_new(PANGO_WEIGHT_BOLD));
    pango_attr_list_insert(fl_attr, pango_attr_foreground_new(65535, 65535, 65535));
    gtk_label_set_attributes(GTK_LABEL(filter_label), fl_attr);
    gtk_box_pack_start(GTK_BOX(sidebar), filter_label, FALSE, FALSE, 5);

    app->filter_dept_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->filter_dept_entry), "Department");
    gtk_box_pack_start(GTK_BOX(sidebar), app->filter_dept_entry, FALSE, FALSE, 3);

    GtkWidget* filter_dept_btn = gtk_button_new_with_label("Filter by Dept");
    g_signal_connect(filter_dept_btn, "clicked", G_CALLBACK(on_filter_dept_clicked), app);
    gtk_box_pack_start(GTK_BOX(sidebar), filter_dept_btn, FALSE, FALSE, 3);

    app->filter_date_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->filter_date_entry), "Date (YYYY-MM-DD)");
    gtk_box_pack_start(GTK_BOX(sidebar), app->filter_date_entry, FALSE, FALSE, 3);

    GtkWidget* filter_date_btn = gtk_button_new_with_label("Filter by Date");
    g_signal_connect(filter_date_btn, "clicked", G_CALLBACK(on_filter_date_clicked), app);
    gtk_box_pack_start(GTK_BOX(sidebar), filter_date_btn, FALSE, FALSE, 3);

    GtkWidget* center_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_grid_attach(GTK_GRID(main_grid), center_box, 1, 1, 1, 1);

    GtkWidget* current_view = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(center_box), current_view, FALSE, FALSE, 5);

    app->current_view_label = gtk_label_new("📋 All Appointments");
    PangoAttrList* cv_attr = pango_attr_list_new();
    pango_attr_list_insert(cv_attr, pango_attr_weight_new(PANGO_WEIGHT_BOLD));
    pango_attr_list_insert(cv_attr, pango_attr_size_new(16 * PANGO_SCALE));
    gtk_label_set_attributes(GTK_LABEL(app->current_view_label), cv_attr);
    gtk_box_pack_start(GTK_BOX(current_view), app->current_view_label, FALSE, FALSE, 5);

    GtkWidget* toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(center_box), toolbar, FALSE, FALSE, 5);

    app->search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->search_entry), "Search Patient ID");
    gtk_box_pack_start(GTK_BOX(toolbar), app->search_entry, FALSE, FALSE, 5);

    GtkWidget* search_btn = gtk_button_new_with_label("🔍 Search");
    g_signal_connect(search_btn, "clicked", G_CALLBACK(on_search_clicked), app);
    gtk_box_pack_start(GTK_BOX(toolbar), search_btn, FALSE, FALSE, 5);

    app->remove_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->remove_entry), "Remove ID");
    gtk_box_pack_start(GTK_BOX(toolbar), app->remove_entry, FALSE, FALSE, 5);

    GtkWidget* remove_btn = gtk_button_new_with_label("🗑️ Remove");
    g_signal_connect(remove_btn, "clicked", G_CALLBACK(on_remove_clicked), app);
    gtk_box_pack_start(GTK_BOX(toolbar), remove_btn, FALSE, FALSE, 5);

    GtkWidget* form_frame = gtk_frame_new("➕ Add New Appointment");
    gtk_box_pack_start(GTK_BOX(center_box), form_frame, FALSE, FALSE, 10);
    gtk_container_set_border_width(GTK_CONTAINER(form_frame), 10);

    GtkWidget* form_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(form_grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(form_grid), 10);
    gtk_container_add(GTK_CONTAINER(form_frame), form_grid);

    app->entry_name = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(app->entry_name), "Patient Name *");
    app->entry_ic = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(app->entry_ic), "IC Number");
    app->entry_phone = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(app->entry_phone), "Phone");
    app->entry_email = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(app->entry_email), "Email");
    app->entry_date = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(app->entry_date), "Date (YYYY-MM-DD) *");
    app->entry_time = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(app->entry_time), "Time (HH:MM) *");
    app->entry_doctor = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(app->entry_doctor), "Doctor Name");
    app->entry_dept = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(app->entry_dept), "Department");

    app->status_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(app->status_combo), NULL, "Pending");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(app->status_combo), NULL, "Confirmed");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(app->status_combo), NULL, "Completed");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(app->status_combo), NULL, "Cancelled");
    gtk_combo_box_set_active(GTK_COMBO_BOX(app->status_combo), 0);

    #define ATTACH_ENTRY(row, widget) gtk_grid_attach(GTK_GRID(form_grid), widget, 0, row, 2, 1);
    ATTACH_ENTRY(0, app->entry_name)
    ATTACH_ENTRY(1, app->entry_ic)
    ATTACH_ENTRY(2, app->entry_phone)
    ATTACH_ENTRY(3, app->entry_email)
    ATTACH_ENTRY(4, app->entry_date)
    ATTACH_ENTRY(5, app->entry_time)
    ATTACH_ENTRY(6, app->entry_doctor)
    ATTACH_ENTRY(7, app->entry_dept)

    GtkWidget* status_label = gtk_label_new("Status:");
    gtk_grid_attach(GTK_GRID(form_grid), status_label, 0, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(form_grid), app->status_combo, 1, 8, 1, 1);

    GtkWidget* add_btn = gtk_button_new_with_label("➕ Add Appointment");
    g_signal_connect(add_btn, "clicked", G_CALLBACK(on_add_clicked), app);
    gtk_grid_attach(GTK_GRID(form_grid), add_btn, 0, 9, 2, 1);

    GtkWidget* content_frame = gtk_frame_new("📄 Results");
    gtk_box_pack_start(GTK_BOX(center_box), content_frame, TRUE, TRUE, 5);

    app->content_area = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(app->content_area), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(app->content_area), FALSE);

    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled), app->content_area);
    gtk_container_add(GTK_CONTAINER(content_frame), scrolled);

    GtkWidget* footer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_grid_attach(GTK_GRID(main_grid), footer, 0, 2, 3, 1);
    gtk_widget_set_hexpand(footer, TRUE);
    gtk_widget_set_size_request(footer, -1, 35);

    GtkCssProvider* footer_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(footer_provider, "background: #2c3e50; padding: 8px;", -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(footer), GTK_STYLE_PROVIDER(footer_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    app->status_bar = gtk_label_new("Ready - Medical Appointment System v1.0");
    gtk_box_pack_start(GTK_BOX(footer), app->status_bar, FALSE, FALSE, 10);
    gtk_widget_set_halign(app->status_bar, GTK_ALIGN_START);

    char footer_text[100];
    snprintf(footer_text, sizeof(footer_text), "Total Appointments: %d | PKU UTHM Medical System", countAppointments(&list));
    GtkWidget* footer_right = gtk_label_new(footer_text);
    gtk_box_pack_end(GTK_BOX(footer), footer_right, FALSE, FALSE, 10);

    gtk_widget_show_all(app->window);
    gtk_main();
    return 0;
}