#include <gtk/gtk.h>
#define ARCHIVO_GLADE "prototipo.glade"

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkBuilder * constructor = gtk_builder_new();
    gtk_builder_add_from_file(constructor, ARCHIVO_GLADE, NULL);

    GtkWidget * ventana = GTK_WIDGET(gtk_builder_get_object(constructor, "ventana"));

    gtk_builder_connect_signals(constructor, NULL);

    g_object_unref(constructor);

    gtk_widget_show(ventana);
    gtk_main();
    return 0;
}
