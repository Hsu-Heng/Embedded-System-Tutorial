#include <stdio.h>
#include <gtk/gtk.h>

static void crete_first_wind(GtkWidget *dummy, gpointer widget);
static void crete_second_wind(GtkWidget *dummy, gpointer widget);

gboolean destroy (GtkWidget *window);

int main (int argc, char *argv[]){
    GtkWidget *window, *button, *box;

    gtk_init (&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "First Window");
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_set_size_request (window, 300, 300);
    gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);

    button = gtk_button_new_with_label ("Go to Window B");
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (crete_first_wind), (gpointer) window);

    box = gtk_box_new (TRUE, 1);
    gtk_box_pack_end (GTK_BOX (box), button, TRUE, TRUE, 1);
    gtk_container_add (GTK_CONTAINER (window), box);

    gtk_widget_show_all (window);
    gtk_main ();
    return 0;
}

static void crete_first_wind(GtkWidget *dummy, gpointer widget){
    (void)dummy;
    GtkWidget *window, *button, *box;

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Second Window");
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_set_size_request (window, 300, 300);
    gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);;
    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy), widget);

    button = gtk_button_new_with_label ("Go back to Window A");
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (crete_second_wind), (gpointer) window);
    gtk_widget_hide(widget);

    box = gtk_box_new (TRUE, 1);
    gtk_box_pack_end (GTK_BOX (box), button, TRUE, TRUE, 1);
    gtk_container_add (GTK_CONTAINER (window), box);
    gtk_widget_show_all (window);
}

static void crete_second_wind(GtkWidget *dummy, gpointer widget){
    (void)dummy;
    GtkWidget *window, *button, *box;

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Second Window");
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_set_size_request (window, 300, 300);
    gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);;
    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy), widget);

    button = gtk_button_new_with_label ("Go back to Window B");
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (crete_first_wind), (gpointer) window);
    gtk_widget_hide(widget);

    box = gtk_box_new (TRUE, 1);
    gtk_box_pack_end (GTK_BOX (box), button, TRUE, TRUE, 1);
    gtk_container_add (GTK_CONTAINER (window), box);
    gtk_widget_show_all (window);
}

gboolean destroy (GtkWidget *widget){
    gtk_widget_destroy (widget);
    return TRUE;
}
