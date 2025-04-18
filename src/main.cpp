#include <iostream>
#include <string>

#include <gtkmm/builder.h>
#include <gtkmm/application.h>
#include <gtkmm/stack.h>
#include <gtkmm/aboutdialog.h>
#include <gtkmm/button.h>
#include <gtkmm/textview.h>
#include <gtkmm/dropdown.h>
#include <gtkmm/stringlist.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/picture.h>
#include <gtkmm/image.h>
#include <gtkmm/scrolledwindow.h>

#include <glibmm.h>
#include <glibmm/binding.h>

#include <string>
#include <mutex>
#include <queue>
#include <thread>

#include "shell.hpp"

std::vector<std::string> split(std::string text,char c){
    bool prev = true;
    std::vector<std::string> out;

    std::string element;

    for(int i = 0;i<text.length();i++){
        if(text.at(i)==c){
            if(!prev){
                out.push_back(element);
                element = "";
                prev = true;

            }
        }else{
            element+=text.at(i);
            prev = false;
        }
    }

    return out;
}

std::vector<std::vector<std::string>> parseTable(std::string text,int skip = 0){
    return {{}};
}

class velvetInsatller:public Gtk::Window{
    Glib::RefPtr<Gtk::Builder> content = Gtk::Builder::create_from_file("./main.ui");
    //content
    Gtk::Stack* stack = content->get_widget<Gtk::Stack>("content_box");
    Gtk::Button* install_btn = content->get_widget<Gtk::Button>("btn_install");
    Gtk::Button* abt_btn = content->get_widget<Gtk::Button>("btn_about");
    Gtk::TextView* tv_log = content->get_widget<Gtk::TextView>("tv_log");
    Gtk::DropDown* list = content->get_widget<Gtk::DropDown>("device_list");
    Gtk::Image* statimg = content->get_widget<Gtk::Image>("statusIMG");
    Gtk::ScrolledWindow* scrol = content->get_widget<Gtk::ScrolledWindow>("out_scroll");

    Gtk::Picture logo;

    Gtk::AboutDialog* abt;

    Glib::RefPtr<Gtk::StringList> devices = Gtk::StringList::create();

    std::mutex lock_log;
    std::queue<std::string> queue_log;
    float progress = 0;
    std::string state;

    public:
    velvetInsatller():abt(){
        set_title("velvet installer");
        set_default_size(500,500);
        set_resizable(false);

        logo.set_filename("./assets/luk.png");

        list->set_model(devices);
        refresh_devices();

        Glib::signal_idle().connect(sigc::mem_fun(*this,&velvetInsatller::tick));        

        //main content
        set_child(*stack);
        install_btn->signal_clicked().connect(sigc::mem_fun(*this,&velvetInsatller::on_install_btn));

        //about dialog
        abt_btn->signal_clicked().connect(sigc::mem_fun(*this,&velvetInsatller::on_abt_clicked));
    }

    void refresh_devices(){
        while(devices->get_n_items())
            devices->remove(0);

        for(std::string dev : split(exec("SHUT=\"yes\" /usr/bin/vtinstall"),'\n')){
            devices->append(dev);
        }
    }
    bool tick(){
        std::unique_lock lock(lock_log);

        while(!queue_log.empty()){
            std::string log = queue_log.front();
            tv_log->get_buffer()->set_text(tv_log->get_buffer()->get_text().append(log));

            queue_log.pop();
        }

        auto adj = scrol->get_vadjustment();
        adj->set_value(adj->get_upper() - adj->get_page_size());

        return true;
    }

    void on_new_log(std::string log){
        std::unique_lock lock(lock_log);

        queue_log.push(log);
    }

    void on_install_exit(int code){
        if(code==0){
            statimg->set("./assets/success.png");
        }else{
            statimg->set("./assets/fail.png");
        }
    }

    void on_install_btn(){
        
        std::string device = devices->get_string(list->get_selected());

        std::cout << "selected  " << device << std::endl;

        if(device=="")
            return;

        std::thread([this,device](){
            std::string command = "pkexec /usr/bin/vtinstall "+device+" shut";
            on_new_log(command);
            execCallBack(command.c_str(),sigc::mem_fun(*this,&velvetInsatller::on_new_log),sigc::mem_fun(*this,&velvetInsatller::on_install_exit));

        }).detach();
        
        
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

        abt->set_logo(logo.get_paintable());
        
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


    auto app = Gtk::Application::create("org.vevet-os.velvet-installer");

    app->make_window_and_run<velvetInsatller>(argc,argv);
}