#include <gtk/gtk.h>
#include <time.h>
#include <pthread.h>
#define ARCHIVO_GLADE "prototipo.glade"
#define LONGITUD_CADENA 50
#define CANTIDAD_PAQUETES 10
#define PASSWORD "1234"
#define CANTIDAD_INTENTOS 3

int paquetes_instalados [CANTIDAD_PAQUETES];
int semaforo = 0, intentos = 0, soporte_no_oficial = 0, paquete_protegido = CANTIDAD_PAQUETES-1;
double valor_barra_progreso = 0;
char operacion_actual [LONGITUD_CADENA];

/* ventanas */
GtkWidget * ventana_principal, * ventana_busqueda, * ventana_recomendaciones, * ventana_operacion_finalizada, * ventana_password, * ventana_progreso;

/* checkbuttons */
GtkCheckButton * checkbuttons [CANTIDAD_PAQUETES];

/* botones */
GtkButton * boton_no_oficial;

/* labels */
GtkLabel * nombres_paquetes [CANTIDAD_PAQUETES];
GtkLabel * campo_intentos, * exito_fallo_operacion, * tipo_operacion;

/* entry */
GtkEntry * password;
GtkSearchEntry * paquete_a_buscar_busqueda, * paquete_a_buscar_principal;

/* progress bar */
GtkProgressBar * barra_progreso;

void no_oficial()
{
    if (soporte_no_oficial)
        gtk_label_set_text(exito_fallo_operacion, "Se eliminó el soporte para paquetes no-oficiales.");
    else
        gtk_label_set_text(exito_fallo_operacion, "Se estableció el soporte para paquetes no-oficiales.");
    soporte_no_oficial = !soporte_no_oficial;
    strcpy(operacion_actual, "No-oficial...");
    gtk_widget_set_sensitive((GtkWidget *) ventana_principal, FALSE);
    gtk_widget_show(ventana_password);
}

void checked(GtkCheckButton * boton)
{
    int indice_boton = 0, encontre = 0;
    while (!encontre) encontre = boton == checkbuttons[indice_boton++];
    indice_boton--;

    if (!semaforo) {
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

void instalar()
{
    int instalar = 0;
    for (int i = 0; i < CANTIDAD_PAQUETES; ++i) {
        if (!paquetes_instalados[i] && gtk_toggle_button_get_active((GtkToggleButton *) checkbuttons[i])) {
            paquetes_instalados[i] = 1;
            instalar = 1;
        }
    }
    gtk_widget_set_sensitive((GtkWidget *) ventana_principal, FALSE);
    if (instalar) {
        strcpy(operacion_actual, "Instalando...");
        gtk_label_set_text(exito_fallo_operacion, "Instalación finalizada con éxito.");
        gtk_widget_show(ventana_password);
    }
    else {
        gtk_label_set_text(exito_fallo_operacion, "No eligió paquetes a instalar.");
        gtk_widget_show(ventana_operacion_finalizada);
    }
}

void desinstalar()
{
    int desinstalar = 0;
    for (int i = 0; i < CANTIDAD_PAQUETES; ++i) {
        if (paquetes_instalados[i] && gtk_toggle_button_get_inconsistent((GtkToggleButton *) checkbuttons[i]))
            desinstalar = 1;
    }
    gtk_widget_set_sensitive((GtkWidget *) ventana_principal, FALSE);
    if (desinstalar) {
        strcpy(operacion_actual, "Desinstalando...");
        gtk_label_set_text(exito_fallo_operacion, "Desinstalación finalizada con éxito.");
        gtk_widget_show(ventana_password);
    }
    else {
        gtk_label_set_text(exito_fallo_operacion, "No eligió paquetes a eliminar.");
        gtk_widget_show(ventana_operacion_finalizada);
    }

}

void recomendaciones()
{
    gtk_widget_hide(ventana_principal);
    gtk_widget_show(ventana_recomendaciones);
}

void buscar()
{
    if (!strlen(gtk_entry_get_text((GtkEntry *) paquete_a_buscar_busqueda)) && !strlen(gtk_entry_get_text((GtkEntry *) paquete_a_buscar_principal))) {
        gtk_widget_set_sensitive((GtkWidget *) ventana_principal, FALSE);
        gtk_label_set_text(exito_fallo_operacion, "Debe ingresar el nombre del paquete a buscar.");
        gtk_widget_show(ventana_operacion_finalizada);
    }
    else {
        gtk_widget_hide(ventana_principal);
        gtk_widget_show(ventana_busqueda);
    }
    gtk_entry_set_text((GtkEntry *) paquete_a_buscar_principal, "");
    gtk_entry_set_text((GtkEntry *) paquete_a_buscar_busqueda, "");
}

void actualizar()
{
    strcpy(operacion_actual, "Actualizando...");
    gtk_label_set_text(exito_fallo_operacion, "Actualización finalizada con éxito.");
    /* gtk_widget_hide(ventana_principal); */
    gtk_widget_set_sensitive((GtkWidget *) ventana_principal, FALSE);
    gtk_widget_show(ventana_password);
}

int * progreso_cien(void * arg)
{
    gtk_widget_hide(ventana_progreso);
    gtk_widget_show(ventana_operacion_finalizada);
    return NULL;
}

void * simular_barra_progreso(void * arg)
{
    struct timespec ts;
    ts.tv_nsec = 1000000000;
    ts.tv_nsec = 10000000;
    ts.tv_sec = 0;
    for (valor_barra_progreso = 0; valor_barra_progreso <= 1; valor_barra_progreso += 0.01) {
        nanosleep(&ts, NULL);
        gtk_progress_bar_set_fraction(barra_progreso, valor_barra_progreso);
    }
    valor_barra_progreso = 0;
    gdk_threads_add_idle((GSourceFunc) progreso_cien, NULL);
    pthread_exit(NULL);
}

void progreso()
{
    gtk_progress_bar_set_fraction(barra_progreso, 0);
    pthread_t hilo_barra_progreso;
    pthread_create(&hilo_barra_progreso, NULL, simular_barra_progreso, NULL);
}

void volver_menu_principal()
{
    gtk_widget_hide(ventana_operacion_finalizada);
    gtk_widget_hide(ventana_recomendaciones);
    gtk_widget_hide(ventana_busqueda);
    gtk_widget_show(ventana_principal);
    gtk_widget_set_sensitive((GtkWidget *) ventana_principal, TRUE);
}

void control_paquete_protegido()
{
    printf("control\n");
    if (paquetes_instalados[paquete_protegido] && gtk_toggle_button_get_inconsistent((GtkToggleButton *) checkbuttons[paquete_protegido])) {
        printf("protegido\n");
        /* tratando de eliminar el paquete protegido */
        gtk_label_set_text(exito_fallo_operacion, "No puede eliminar paquetes protegidos.");
        gtk_toggle_button_set_active((GtkToggleButton *) checkbuttons[paquete_protegido], TRUE);
        gtk_toggle_button_set_inconsistent((GtkToggleButton *) checkbuttons[paquete_protegido], FALSE);
        gtk_widget_show(ventana_operacion_finalizada);
    }
    else {
        printf("todo bien\n");
        for (int i = 0; i < CANTIDAD_PAQUETES; ++i) {
            if (paquetes_instalados[i] && gtk_toggle_button_get_inconsistent((GtkToggleButton *) checkbuttons[i])) {
                printf("eliminar paquete %i\n", i);
                paquetes_instalados[i] = 0;
                gtk_toggle_button_set_inconsistent((GtkToggleButton *) checkbuttons[i], FALSE);
                gtk_toggle_button_set_active((GtkToggleButton *) checkbuttons[i], FALSE);
            }
        }
        gtk_label_set_text(tipo_operacion, operacion_actual);
        gtk_widget_show(ventana_progreso);
        progreso();
    }
}

void password_ingresada()
{
    gtk_widget_grab_focus((GtkWidget *) password);
    intentos++;
    if (strcmp(PASSWORD, gtk_entry_get_text(password))) {
        if (intentos == CANTIDAD_INTENTOS) {
            gtk_label_set_text(exito_fallo_operacion, "Contraseña incorrecta. Operación cancelada.");
            gtk_widget_hide(ventana_password);
            gtk_widget_show(ventana_operacion_finalizada);
            limpiar_ventana_password();
        }
        else {
            char cadena [LONGITUD_CADENA];
            if (CANTIDAD_INTENTOS - intentos == 1)
                sprintf(cadena, "Queda %i intento", CANTIDAD_INTENTOS-intentos);
            else
                sprintf(cadena, "Quedan %i intentos", CANTIDAD_INTENTOS-intentos);
            gtk_label_set_text(campo_intentos, cadena);
            gtk_entry_set_text(password, "");
        }
    }
    else {
        gtk_widget_hide(ventana_password);
        if (!strcmp(operacion_actual, "No-oficial...")) {
            gtk_widget_hide(ventana_password);
            gtk_widget_show(ventana_operacion_finalizada);
        }
        else {
            if (!strcmp(operacion_actual, "Desinstalando...")) control_paquete_protegido();
            else {
                gtk_label_set_text(tipo_operacion, operacion_actual);
                gtk_widget_show(ventana_progreso);
                progreso();
            }
        }
        limpiar_ventana_password();
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
        if (gtk_toggle_button_get_active((GtkToggleButton *) checkbuttons[i]))
            paquetes_instalados[i] = 1;
    }
}

void inicializar_entries(GtkBuilder * constructor)
{
    paquete_a_buscar_busqueda = (GtkSearchEntry *) GTK_WIDGET(gtk_builder_get_object(constructor, "paquete_a_buscar_busqueda"));
    paquete_a_buscar_principal = (GtkSearchEntry *) GTK_WIDGET(gtk_builder_get_object(constructor, "paquete_a_buscar_principal"));
    password = (GtkEntry *) GTK_WIDGET(gtk_builder_get_object(constructor, "password"));
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkBuilder * constructor = gtk_builder_new();
    gtk_builder_add_from_file(constructor, ARCHIVO_GLADE, NULL);

    inicializar_ventanas(constructor);
    inicializar_checkbuttons(constructor);
    inicializar_labels(constructor);
    inicializar_entries(constructor);

    barra_progreso = (GtkProgressBar *) GTK_WIDGET(gtk_builder_get_object(constructor, "barra_progreso"));
    boton_no_oficial = (GtkButton *) GTK_WIDGET(gtk_builder_get_object(constructor, "boton_no_oficial"));

    /* conectar señales */
    gtk_builder_add_callback_symbol(constructor, "actualizar", actualizar);
    gtk_builder_add_callback_symbol(constructor, "instalar", instalar);
    gtk_builder_add_callback_symbol(constructor, "desinstalar", desinstalar);
    gtk_builder_add_callback_symbol(constructor, "checked", (GCallback) checked);
    gtk_builder_add_callback_symbol(constructor, "password_ingresada", password_ingresada);
    gtk_builder_add_callback_symbol(constructor, "volver_menu_principal", volver_menu_principal);
    gtk_builder_add_callback_symbol(constructor, "no_oficial", no_oficial);
    gtk_builder_add_callback_symbol(constructor, "recomendaciones", recomendaciones);
    gtk_builder_add_callback_symbol(constructor, "buscar", buscar);

    gtk_builder_connect_signals(constructor, NULL);

    g_object_unref(constructor);

    gtk_widget_show(ventana_principal);
    gtk_main();
    return 0;
}
