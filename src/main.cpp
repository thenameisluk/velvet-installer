#include <iostream>
#include <string>

#include <gtkmm/builder.h>
#include <gtkmm/application.h>
#include <gtkmm/stack.h>
#include <gtkmm/aboutdialog.h>
#include <gtkmm/button.h>
#include <gtkmm/textview.h>

#include <glibmm.h>
#include <glibmm/binding.h>


#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include <functional>
#include <fstream>

std::string exec(const char* cmd) {
    std::ostringstream output;

    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output << buffer;
    }

    pclose(pipe);
    return output.str();
}

void execCallBack(const char* cmd,std::function<void(const char*)> callback){
    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        callback(buffer);
    }

    pclose(pipe);
}

std::string cat(const char* path){
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error(std::string("Error: Could not open file ")+path);
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

class lsblk{
    
};

class velvetInsatller:public Gtk::Window{
    Glib::RefPtr<Gtk::Builder> content = Gtk::Builder::create_from_file("./main.ui");
    //content
    Gtk::Stack* stack = content->get_widget<Gtk::Stack>("content_box");
    Gtk::Button* install_btn = content->get_widget<Gtk::Button>("btn_install");
    Gtk::Button* abt_btn = content->get_widget<Gtk::Button>("btn_about");
    Gtk::TextView* tv_log = content->get_widget<Gtk::TextView>("tv_log");

    Gtk::AboutDialog* abt;

    public:
    velvetInsatller():abt(){
        set_title("velvet installer");
        set_default_size(500,500);
        set_resizable(false);

        // Glib::

        //main content
        set_child(*stack);
        install_btn->signal_clicked().connect(sigc::mem_fun(*this,&velvetInsatller::on_install_btn));

        //about dialog
        abt_btn->signal_clicked().connect(sigc::mem_fun(*this,&velvetInsatller::on_abt_clicked));
    }
    void on_progress(int prog){

    }
    void on_new_state(std::string state){

    }

    void on_new_log(std::string log){
        tv_log->get_buffer()->set_text(tv_log->get_buffer()->get_text().append(log));
    }

    void on_install_btn(){
        std::cout << exec("/usr/bin/lsblk --raw -o NAME,MOUNTPOINTS -n") << std::endl;

        execCallBack("/usr/bin/lsblk --raw -o NAME,MOUNTPOINTS -n",sigc::mem_fun(*this,&velvetInsatller::on_new_log));
        
        auto children = stack->get_children();
        
        stack->set_visible_child(*(children[1]));
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