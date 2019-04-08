#include <stdio.h>
#include <gtk/gtk.h>

static void t_index(GtkWidget *dummy, gpointer widget);
static void blinkled(GtkWidget *dummy, gpointer widget);
static void print_hello (GtkWidget *widget, gpointer   data);
static void quit (GtkWidget *widget, gpointer data);
gboolean destroy (GtkWidget *window);
int main (int argc, char *argv[]){
  GtkWidget *window, *button, *box;

    gtk_init (&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "First Window");
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_set_size_request (window, 200, 200);
    gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    button = gtk_button_new_with_label ("Go");
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (t_index), (gpointer) window);

    box = gtk_box_new (TRUE, 1);
    gtk_box_pack_end (GTK_BOX (box), button, TRUE, TRUE, 1);
    gtk_container_add (GTK_CONTAINER (window), box);

    gtk_widget_show_all (window);
    gtk_main ();
    return 0;
}
static void print_hello (GtkWidget *widget, gpointer   data){
  g_print ("Hello World\n");
}

static void t_index(GtkWidget *dummy, gpointer widget){
    (void)dummy;
    GtkWidget *window, *button, *grid;
    gtk_widget_hide(widget);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "KDD912 INDEX");
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_set_size_request (window, 400, 300);
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);



    grid = gtk_grid_new ();
    gtk_container_add (GTK_CONTAINER (window), grid);

    button = gtk_button_new_with_label ("BLINKLED");
    g_signal_connect (button, "clicked", G_CALLBACK (blinkled), (gpointer) window);
    gtk_grid_attach (GTK_GRID (grid), button, 0, 0, 1, 1);

    // button = gtk_button_new_with_label ("SENSOR");
    // g_signal_connect (button, "clicked", G_CALLBACK (blinleled), (gpointer) window);
    // gtk_grid_attach (GTK_GRID (grid), button, 1, 0, 1, 1);

    button = gtk_button_new_with_label ("Quit");
    g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_main_quit), window);
    gtk_grid_attach (GTK_GRID (grid), button, 0, 1, 2, 1);

    gtk_widget_show_all (window);
    gtk_main ();
    g_print ("Close\n");
}
static void quit (GtkWidget *widget, gpointer data){
  gtk_main_quit();
  g_print ("Close\n");
}
static void blinkled(GtkWidget *dummy, gpointer widget){
    (void)dummy;
    GtkWidget *window, *button, *grid;
    gtk_widget_hide(widget);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "KDD912 BLINKLED");
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_set_size_request (window, 400, 300);
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    grid = gtk_grid_new ();
    gtk_container_add (GTK_CONTAINER (window), grid);

    button = gtk_button_new_with_label ("Start");
    g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
    gtk_grid_attach (GTK_GRID (grid), button, 0, 0, 1, 1);

    button = gtk_button_new_with_label ("Stop");
    g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
    gtk_grid_attach (GTK_GRID (grid), button, 1, 0, 1, 1);

    button = gtk_button_new_with_label ("Return Index");
    g_signal_connect_swapped (button, "clicked", G_CALLBACK (t_index), (gpointer) window);
    gtk_grid_attach (GTK_GRID (grid), button, 0, 1, 2, 1);


    button = gtk_button_new_with_label ("Exit");
    g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
    gtk_grid_attach (GTK_GRID (grid), button, 0, 1, 3, 1);



    // box = gtk_box_new (TRUE, 1);
    // gtk_box_pack_end (GTK_BOX (box), button, TRUE, TRUE, 1);
    // gtk_container_add (GTK_CONTAINER (window), box);
    gtk_widget_show_all (window);
    gtk_main ();
    g_print ("Close\n");
}
gboolean destroy (GtkWidget *widget){
    gtk_widget_destroy (widget);
    return TRUE;
}
// static void SENSOR(GtkWidget *dummy, gpointer widget){
//     (void)dummy;
//     GtkWidget *window, *button, *box;
//
//     window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
//     gtk_window_set_title (GTK_WINDOW (window), "KDD912 INDEX");
//     gtk_container_set_border_width (GTK_CONTAINER (window), 10);
//     gtk_widget_set_size_request (window, 400, 300);
//     gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);;
//     g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy), widget);
//
//     button = gtk_button_new_with_label ("BLINKLED");
//     g_signal_connect (button, "clicked", G_CALLBACK (blinleled), NULL);
//     gtk_grid_attach (GTK_GRID (grid), button, 0, 0, 1, 1);
//
//     button = gtk_button_new_with_label ("SENSOR");
//     g_signal_connect (button, "clicked", G_CALLBACK (blinleled), NULL);
//     gtk_grid_attach (GTK_GRID (grid), button, 1, 0, 1, 1);
//
//     button = gtk_button_new_with_label ("Quit");
//     g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
//     gtk_grid_attach (GTK_GRID (grid), button, 0, 1, 2, 1);
//
//     button = gtk_button_new_with_label ("Go back to Window A");
//     g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (crete_second_wind), (gpointer) window);
//     gtk_widget_hide(widget);
//
//     box = gtk_box_new (TRUE, 1);
//     gtk_box_pack_end (GTK_BOX (box), button, TRUE, TRUE, 1);
//     gtk_container_add (GTK_CONTAINER (window), box);
//     gtk_widget_show_all (window);
// }
