#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>
#include <stdlib.h>

// Animation state
static double circle_y = -50;  // Initial position of the circle
static double image_scale = 0.0;  // Start with the image fully scaled down
static double image_rotation = 0.0;  // Initial rotation angle
static GtkWidget *drawing_area;
static GdkPixbuf *image_pixbuf;

// Gradient Colors for the Background
typedef struct {
    double r1, g1, b1;  // Start color (RGB)
    double r2, g2, b2;  // End color (RGB)
} Gradient;

static Gradient background_gradient = {0.1, 0.1, 0.2, 0.4, 0.1, 0.6};  // Dark blue to teal

// Floating "Flower" Positions
typedef struct {
    double x, y;  // Position
    double speed; // Floating speed
} Flower;

#define NUM_FLOWERS 10
static Flower flowers[NUM_FLOWERS];

// Initialize floating "flowers"
static void init_flowers(int width, int height) {
    for (int i = 0; i < NUM_FLOWERS; i++) {
        flowers[i].x = rand() % width;
        flowers[i].y = rand() % height;
        flowers[i].speed = 0.5 + (rand() % 100) / 100.0;  // Random speed
    }
}

// Update flower positions
static void update_flowers(int height) {
    for (int i = 0; i < NUM_FLOWERS; i++) {
        flowers[i].y -= flowers[i].speed;
        if (flowers[i].y < 0) {
            flowers[i].y = height;  // Reset to bottom
            flowers[i].x = rand() % 400;  // Random x position
        }
    }
}

// Function to scale and rotate the image
static gboolean scale_and_rotate_image(gpointer user_data) {
    GtkWidget *widget = GTK_WIDGET(user_data);

    if (image_scale < 1.0) {
        image_scale += 0.05;  // Increment scale
        image_rotation += 10.0;  // Increment rotation angle
        gtk_widget_queue_draw(widget);  // Trigger a redraw
        return TRUE;  // Continue the animation
    }

    // Ensure the image is upright at the end
    image_rotation = 0.0;  // Reset rotation to 0 degrees
    gtk_widget_queue_draw(widget);  // Final redraw to straighten the image

    return FALSE;  // Stop the timer when fully scaled
}

// Draw function to render the circle, text, image, and background
static void draw_content(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
    // Draw gradient background
    cairo_pattern_t *pattern = cairo_pattern_create_linear(0, 0, 0, height);
    cairo_pattern_add_color_stop_rgb(pattern, 0, background_gradient.r1, background_gradient.g1, background_gradient.b1);
    cairo_pattern_add_color_stop_rgb(pattern, 1, background_gradient.r2, background_gradient.g2, background_gradient.b2);
    cairo_set_source(cr, pattern);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_fill(cr);
    cairo_pattern_destroy(pattern);

    // Draw floating "flowers"
    cairo_set_source_rgb(cr, 1, 1, 1);  // White color
    for (int i = 0; i < NUM_FLOWERS; i++) {
        cairo_arc(cr, flowers[i].x, flowers[i].y, 5, 0, 2 * G_PI);  // Small circle
        cairo_fill(cr);
    }

    // Update flower positions
    update_flowers(height);

    // Draw the circle
    if (circle_y <= 300) {  // Draw the circle only during its animation
        cairo_set_source_rgb(cr, 1, 0.5, 0);  // Orange color
        cairo_arc(cr, width / 2, circle_y, 30, 0, 2 * G_PI);  // Circle in the middle
        cairo_fill(cr);

        // Draw the text
        cairo_set_source_rgb(cr, 1, 1, 1);  // White color
        cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 20);
        cairo_move_to(cr, width / 2 - 100, circle_y + 50);
        cairo_show_text(cr, "Hey, look at this view");
    }

    // Draw and transform the image
    if (circle_y > 300) {  // Draw the image after the circle animation completes
        cairo_translate(cr, width / 2, height / 2);  // Move to center
        cairo_rotate(cr, image_rotation * (M_PI / 180.0));  // Apply rotation
        cairo_scale(cr, image_scale, image_scale);  // Apply scaling
        cairo_translate(cr, -gdk_pixbuf_get_width(image_pixbuf) / 2, -gdk_pixbuf_get_height(image_pixbuf) / 2);

        gdk_cairo_set_source_pixbuf(cr, image_pixbuf, 0, 0);
        cairo_paint(cr);
    }
}

// Timer callback for circle animation
static gboolean on_timer_tick(gpointer user_data) {
    GtkWidget *widget = GTK_WIDGET(user_data);

    circle_y += 5;  // Move the circle down
    if (circle_y > 300) {  // Transition point
        g_timeout_add(50, scale_and_rotate_image, widget);  // Start scaling and rotation
        return FALSE;  // Stop the circle animation
    }

    gtk_widget_queue_draw(widget);  // Request a redraw
    return TRUE;  // Continue the timer
}

// Application activate callback
static void on_activate(GtkApplication *app, gpointer user_data) {
    g_print("Activating application.\n");

    // Create the main window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Animation");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // Load the image
    const char *image_path = "./animation.jpg";  // Ensure the image is in the same folder
    image_pixbuf = gdk_pixbuf_new_from_file(image_path, NULL);
    if (!image_pixbuf) {
        g_error("Failed to load image. Ensure the file path '%s' is correct.", image_path);
    }

    // Create a drawing area
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 800, 600);

    // Set the draw function for the drawing area
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), draw_content, NULL, NULL);

    // Add the drawing area to the window
    gtk_window_set_child(GTK_WINDOW(window), drawing_area);

    // Initialize floating flowers
    init_flowers(800, 600);

    // Start the circle animation timer
    g_timeout_add(30, on_timer_tick, drawing_area);

    // Show the main window
    gtk_window_present(GTK_WINDOW(window));
}

// Main function
int main(int argc, char *argv[]) {
    // Create a new GtkApplication
    GtkApplication *app = gtk_application_new("com.example.fancy_animation", G_APPLICATION_DEFAULT_FLAGS);

    // Connect the activate signal
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    // Run the application
    int status = g_application_run(G_APPLICATION(app), argc, argv);

    // Cleanup
    g_object_unref(app);
    return status;
}
