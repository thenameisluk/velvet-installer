#include <gtkmm/builder.h>
#include <gtkmm/application.h>
#include <gtkmm/stack.h>

class velvetInsatller:public Gtk::Window{
    Glib::RefPtr<Gtk::Builder> content;
    public:
    velvetInsatller(){
        set_title("velvet installer");
        set_default_size(500,500);
        set_resizable(false);

        content = Gtk::Builder::create_from_file("./test.ui");
        auto stack = content->get_widget<Gtk::Stack>("content_box");
        set_child(*stack);
    }
    
};

int main(int argc, char **argv){
    auto app = Gtk::Application::create("org.vevet-os.velvet-installer");

    app->make_window_and_run<velvetInsatller>(argc,argv);
}