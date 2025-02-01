#include <iostream>
#include <string>

#include <gtkmm/builder.h>
#include <gtkmm/application.h>
#include <gtkmm/stack.h>
#include <gtkmm/aboutdialog.h>
#include <gtkmm/button.h>

class velvetInsatller:public Gtk::Window{
    Glib::RefPtr<Gtk::Builder> content;

    Gtk::AboutDialog* abt;

    public:
    velvetInsatller():abt(){
        set_title("velvet installer");
        set_default_size(500,500);
        set_resizable(false);

        //main content
        content = Gtk::Builder::create_from_file("./main.ui");
        auto stack = content->get_widget<Gtk::Stack>("content_box");
        set_child(*stack);

        //about dialog
        auto abt_btn = content->get_widget<Gtk::Button>("btn_about");

        abt_btn->signal_clicked().connect(sigc::mem_fun(*this,&velvetInsatller::on_abt_clicked));
    }

    void on_abt_clicked(){
        if(abt){
            abt->show();
            std::cout << "about already exists" << std::endl;
            return;
        }
            
        
        abt = new Gtk::AboutDialog();

        abt->set_program_name("velvet installer");
        abt->set_version("0.0.1");
        abt->set_transient_for(*this);
        
        abt->set_authors({"luk <luk@iaml.uk>"});
        abt->set_license_type(Gtk::License::MIT_X11);
        abt->set_website("https://github.com/thenameisluk/velvet-installer");
        
        abt->show();

        abt->signal_close_request().connect(sigc::mem_fun(*this,&velvetInsatller::on_abt_close),true);
    }
    bool on_abt_close(){
        std::cout << "destroying about" << std::endl;
        delete abt;
        abt = nullptr;

        return 0;
    }
    
};

int main(int argc, char **argv){
    //https://unix.stackexchange.com/questions/203136/how-do-i-run-gui-applications-as-root-by-using-pkexec    
    // if(getuid()){
    //     std::cout << "root access required" << std::endl;
    //     std::string cmd = "pkexec ";
    //     char p[100];
    //     readlink("/proc/self/exe",p,99);
    //     cmd.append(p);
    //     std::cout << cmd << std::endl;
    //     return system(cmd.c_str());
    // }
    // std::cout << "running as root" << std::endl;


    auto app = Gtk::Application::create("org.vevet-os.velvet-installer");

    app->make_window_and_run<velvetInsatller>(argc,argv);
}