#include <gtk/gtk.h>
#include <lxpanel/plugin.h>

typedef struct {
    GtkWidget *label;
    GtkClipboard *clipboard;
} ClipboardMonitor;

static void on_text_received(
    GtkClipboard *clipboard,
    const gchar *text,
    gpointer user_data
) {
    ClipboardMonitor *monitor = (ClipboardMonitor *)user_data;
    if (text && strlen(text) > 0) {
        gchar *display_text = g_strdup(text);
        gchar *p = display_text;
        while (*p) {
            if (*p == '\n' || *p == '\r' || *p == '\t') *p = ' ';
            p++;
        }
        gtk_label_set_text(GTK_LABEL(monitor->label), display_text);
        gtk_widget_set_tooltip_text(monitor->label, text);
        g_free(display_text);
    } else {
        gtk_label_set_text(GTK_LABEL(monitor->label), "(empty)");
    }
}

static void on_clipboard_owner_change(
    GtkClipboard *clipboard,
    GdkEvent *event,
    gpointer user_data
) {
    gtk_clipboard_request_text(clipboard, on_text_received, user_data);
}

static GtkWidget *clipboard_constructor(
    LXPanel *panel,
    config_setting_t *settings
) {
    ClipboardMonitor *monitor = g_new0(ClipboardMonitor, 1);

    GtkWidget *plugin = gtk_event_box_new();
    lxpanel_plugin_set_data(plugin, monitor, g_free);

    monitor->label = gtk_label_new("Waiting...");
    gtk_label_set_xalign(GTK_LABEL(monitor->label), 0.0);
    gtk_label_set_yalign(GTK_LABEL(monitor->label), 0.5);
    gtk_label_set_width_chars(GTK_LABEL(monitor->label), 30);
    gtk_label_set_max_width_chars(GTK_LABEL(monitor->label), 90);
    gtk_label_set_ellipsize(GTK_LABEL(monitor->label), PANGO_ELLIPSIZE_END);

    gtk_container_add(GTK_CONTAINER(plugin), monitor->label);

    monitor->clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    g_signal_connect(monitor->clipboard, "owner-change",
        G_CALLBACK(on_clipboard_owner_change), monitor);

    gtk_widget_show_all(plugin);
    gtk_clipboard_request_text(monitor->clipboard, on_text_received, monitor);

    return plugin;
}

FM_DEFINE_MODULE(lxpanel_gtk, clipboard)

LXPanelPluginInit fm_module_init_lxpanel_gtk = {
    .name = "clipboard",
    .description = "Clipboard Monitor",
    .new_instance = clipboard_constructor,
    .config = NULL,
    .reconfigure = NULL,
    .button_press_event = NULL
};
