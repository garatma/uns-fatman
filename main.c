#include <gtk/gtk.h>
#include <time.h>
#define ARCHIVO_GLADE "prototipo.glade"
#define LONGITUD_CADENA 50
#define CANTIDAD_PAQUETES 10
#define PASSWORD "1234"
#define CANTIDAD_INTENTOS 3

int paquetes_instalados [CANTIDAD_PAQUETES];
int semaforo = 0, intentos = 0;

/* ventanas */
GtkWidget * ventana_principal, * ventana_busqueda, * ventana_recomendaciones, * ventana_operacion_finalizada, * ventana_password, * ventana_progreso;

/* checkbuttons */
GtkCheckButton * checkbuttons [CANTIDAD_PAQUETES];

/* botones */

/* labels */
GtkLabel * nombres_paquetes [CANTIDAD_PAQUETES];
GtkLabel * campo_intentos, * exito_fallo_operacion, * tipo_operacion;

/* entrey */
GtkEntry * password;


/* progress bar */
GtkProgressBar * barra_progreso;

void checked(GtkCheckButton * boton)
{
    int indice_boton = 0, encontre = 0;
    while (!encontre) encontre = boton == checkbuttons[indice_boton++];
    indice_boton--;

    if (!semaforo) {
        printf("Botón checkeado.\n");
        if (paquetes_instalados[indice_boton]) {
            if (gtk_toggle_button_get_inconsistent((GtkToggleButton *) boton))
                gtk_toggle_button_set_inconsistent((GtkToggleButton *) boton, FALSE);
            else {
                /* marcar como inconsistente */
                semaforo = 1;
                gtk_toggle_button_set_active((GtkToggleButton *) boton, TRUE);
                gtk_toggle_button_set_inconsistent((GtkToggleButton *) boton, TRUE);
                gtk_toggle_button_set_active((GtkToggleButton *) boton, FALSE);
                semaforo = 0;
            }
        }
    }
}

void limpiar_ventana_password()
{
    gtk_label_set_text(campo_intentos, "Quedan 3 intentos");
    gtk_entry_set_text(password, "");
    intentos = 0;
}

/* callbacks */
void actualizar()
{
    gtk_widget_hide(ventana_principal);
    gtk_widget_show(ventana_password);
    gtk_entry_set_text(password, "");
}

void progreso()
{
    gtk_widget_show(ventana_progreso);
    gtk_widget_show(ventana_password);
    gtk_widget_show(ventana_principal);

    gtk_progress_bar_set_fraction(barra_progreso, 0.10);
    for (int i = 0; i < 1000000; ++i) {
        printf("%i", i);
    }
    gtk_progress_bar_set_fraction(barra_progreso, 0.20);
    for (int i = 0; i < 1000000; ++i) {
        printf("%i", i);
    }
    sleep(4);
    for (int i = 0; i < 50; ++i) {
        gtk_progress_bar_set_fraction(barra_progreso,(gdouble) i/100);
    }
    sleep(4);
    /* printf("hola\n"); */
    /* char cad [5]; */
    /* for (int i = 0; i <= 100; ++i) { */
    /*     sleep(1); */
    /*     /1* gtk_progress_bar_set_fraction(barra_progreso, i/100); *1/ */
    /*     sprintf(cad, "%i", i); */
    /*     gtk_progress_bar_set_text(barra_progreso, cad); */
    /* } */
    /* for (int i = 0; i <= 100; ++i) { */
    /*     gtk_progress_bar_set_fraction(barra_progreso, i/100); */
    /*     for (int j = 0; j < 3000; ++j) */
    /*         printf("%i\n", j); */
    /*     printf("%i - ", i); */
    /* } */
    /* printf("chau\n"); */
    /* gtk_label_set_text(exito_fallo_operacion, "Operación finalizada con éxito."); */
    /* gtk_widget_hide(ventana_progreso); */
    /* gtk_widget_show(ventana_operacion_finalizada); */
}

void volver_menu_principal()
{
    gtk_widget_hide(ventana_operacion_finalizada);
    gtk_widget_show(ventana_principal);
}

void password_ingresada()
{
    intentos++;
    if (strcmp(PASSWORD, gtk_entry_get_text(password))) {
        /* contraseña incorrecta */
        if (intentos == CANTIDAD_INTENTOS) {
            /* cancelar operación */
            limpiar_ventana_password();
            gtk_label_set_text(exito_fallo_operacion, "Contraseña incorrecta. Operación cancelada");
            gtk_widget_hide(ventana_password);
            gtk_widget_show(ventana_operacion_finalizada);
        }
        else {
            char cadena [LONGITUD_CADENA];
            if (CANTIDAD_INTENTOS - intentos == 1)
                sprintf(cadena, "Queda %i intento", CANTIDAD_INTENTOS-intentos);
            else
                sprintf(cadena, "Quedan %i intentos", CANTIDAD_INTENTOS-intentos);
            gtk_label_set_text(campo_intentos, cadena);
        }
    }
    else {
        /* contraseña correcta */
        gtk_label_set_text(tipo_operacion, "Actualizando...");
        limpiar_ventana_password();
        gtk_widget_hide(ventana_password);
        gtk_widget_show(ventana_progreso);
        progreso();
    }
}

void inicializar_labels(GtkBuilder * constructor)
{
    campo_intentos = (GtkLabel *) GTK_WIDGET(gtk_builder_get_object(constructor, "campo_intentos"));
    exito_fallo_operacion = (GtkLabel *) GTK_WIDGET(gtk_builder_get_object(constructor, "exito_fallo_operacion"));
    tipo_operacion = (GtkLabel *) GTK_WIDGET(gtk_builder_get_object(constructor, "tipo_operacion"));
}

void inicializar_ventanas(GtkBuilder * constructor)
{
    ventana_principal = GTK_WIDGET(gtk_builder_get_object(constructor, "ventana_principal"));
    ventana_busqueda = GTK_WIDGET(gtk_builder_get_object(constructor, "ventana_busqueda"));
    ventana_recomendaciones = GTK_WIDGET(gtk_builder_get_object(constructor, "ventana_recomendaciones"));
    ventana_operacion_finalizada = GTK_WIDGET(gtk_builder_get_object(constructor, "ventana_operacion_finalizada"));
    ventana_password = GTK_WIDGET(gtk_builder_get_object(constructor, "ventana_password"));
    ventana_progreso = GTK_WIDGET(gtk_builder_get_object(constructor, "ventana_progreso"));
}

void inicializar_checkbuttons(GtkBuilder * constructor)
{
    char nombre [LONGITUD_CADENA];
    for (int i = 0; i < CANTIDAD_PAQUETES; ++i) {
        sprintf(nombre, "checkbutton%i", i);
        checkbuttons[i] = (GtkCheckButton *) GTK_WIDGET(gtk_builder_get_object(constructor, nombre));
        if (gtk_toggle_button_get_active((GtkToggleButton *) checkbuttons[i])) {
            printf("%s instalado\n", nombre);
            paquetes_instalados[i] = 1;
        }
    }
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkBuilder * constructor = gtk_builder_new();
    gtk_builder_add_from_file(constructor, ARCHIVO_GLADE, NULL);

    inicializar_ventanas(constructor);
    inicializar_checkbuttons(constructor);
    inicializar_labels(constructor);

    password = (GtkEntry *) GTK_WIDGET(gtk_builder_get_object(constructor, "password"));
    barra_progreso = (GtkProgressBar *) GTK_WIDGET(gtk_builder_get_object(constructor, "barra_progreso"));

    /* conectar señales */
    gtk_builder_add_callback_symbol(constructor, "actualizar", actualizar);
    gtk_builder_add_callback_symbol(constructor, "checked", (GCallback) checked);
    gtk_builder_add_callback_symbol(constructor, "password_ingresada", password_ingresada);
    gtk_builder_add_callback_symbol(constructor, "volver_menu_principal", volver_menu_principal);

    gtk_builder_connect_signals(constructor, NULL);

    g_object_unref(constructor);

    gtk_widget_show(ventana_principal);
    /* gtk_widget_hide(ventana_principal); */
    gtk_main();
    return 0;
}
