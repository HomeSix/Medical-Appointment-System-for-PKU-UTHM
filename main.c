#include "medical_system_gui.h"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    loadUsers();
    loadPatients();
    loadAppointments();
    loadSchedule();

    setup_css();
    create_login_window();

    gtk_main();

    if (main_window) gtk_widget_destroy(main_window);
    freeAll();
    return 0;
}
