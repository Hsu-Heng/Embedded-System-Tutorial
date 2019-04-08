#include <gtk/gtk.h>
#include <pthread.h>
#include <wiringPi.h>
typedef int bool;
#define true 1
#define false 0
bool b = false;
int pin1 = 24, pin2 = 25;

pthread_t led;
static void
print_hello (GtkWidget *widget, gpointer   data)
{
  g_print ("Hello World\n");
}

void* doSomeThing(void *arg)
{
  while(1){
    if(b ==true)
    {
      digitalWrite (pin1, HIGH) ; delay (500) ;
      digitalWrite (pin1,  LOW) ; delay (500) ;
  	 digitalWrite (pin2, HIGH) ; delay (500) ;
      digitalWrite (pin2,  LOW) ; delay (500) ;

    }
    else
    {
      digitalWrite (pin1,  LOW) ;
     digitalWrite (pin2, LOW) ;
     break;
    }
  }
  digitalWrite (pin1,  LOW) ;
 digitalWrite (pin2, LOW) ;
  pthread_exit(&led);
}

static void blinleled (GtkWidget *widget, gpointer data){
  b = true;
  g_print ("Open\n");
  pthread_create(&led, NULL, doSomeThing, NULL);
}
static void clockled (GtkWidget *widget, gpointer data){
  b = false;
  g_print ("Close\n");
}
static void quit (GtkWidget *widget, gpointer data){
  b = false;
  gtk_main_quit();
  g_print ("Close\n");
}

int main (int argc, char *argv[]){
  GtkBuilder *builder;
  GObject *window;
  GObject *button;
  wiringPiSetup () ;
  printf ("Raspberry Pi blink\n") ;
  pinMode (pin1, OUTPUT) ;
  pinMode (pin2, OUTPUT) ;
  digitalWrite (pin1,  LOW) ;
  digitalWrite (pin2, LOW) ;
  gtk_init (&argc, &argv);

  /* Construct a GtkBuilder instance and load our UI description */
  builder = gtk_builder_new ();
  gtk_builder_add_from_file (builder, "builder.ui", NULL);

  /* Connect signal handlers to the constructed widgets. */
  window = gtk_builder_get_object (builder, "window");
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  button = gtk_builder_get_object (builder, "button1");
  g_signal_connect (button, "clicked", G_CALLBACK (blinleled), NULL);

  button = gtk_builder_get_object (builder, "button2");
  g_signal_connect (button, "clicked", G_CALLBACK (clockled), NULL);

  button = gtk_builder_get_object (builder, "quit");
  g_signal_connect (button, "clicked", G_CALLBACK (quit), NULL);

  gtk_main ();
  return 0;
}
