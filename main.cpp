#include <crow.h>
#include <iostream>
#include <filesystem> 
#include "include/load.hpp"
int main()
{
    hash_tables tables;
    crow::mustache::set_base("templates");

    crow::SimpleApp app;
    CROW_ROUTE(app, "/")([](){
        auto page = crow::mustache::load("landing_page.html");
        return page.render();
    });

    CROW_ROUTE(app, "/sign_in")([](){
        auto page = crow::mustache::load("sign_in.html");
        return page.render();
    });
    CROW_ROUTE(app, "/sign_in_post").methods("POST"_method)([&tables](const crow::request& req) -> crow::response {
        auto req_body = crow::query_string(("?" + req.body).c_str()); 

        std::string email=req_body.get("email");
        std::string pass=req_body.get("password");
        std::string type=req_body.get("type");
        std::string* user=tables.findUsername(email);
        if(user){
            crow::mustache::context ctx;
            ctx["username"]=*user;
            if(type=="Farmer"){
                farmer_data* data=tables.findFarmer(*user);
                if(data){
                    if(data->password==pass){
                        auto page=crow::mustache::load("farmer/farmer_dashboard.html");
                        return crow::response(page.render(ctx));
                    }
                }
                auto page=crow::mustache::load("error.html");
                return crow::response(page.render());
            }
            else if(type=="Buyer"){
                buyer_data* data=tables.findBuyer(*user);
                if(data){
                    if(data->password==pass){
                        auto page=crow::mustache::load("buyer/buyer_home.html");
                        return crow::response(page.render(ctx));
                    }
                }
                auto page=crow::mustache::load("error.html");
                return crow::response(page.render());
            }
        }
        auto page=crow::mustache::load("error.html");
        return crow::response(page.render());
        
    });
    CROW_ROUTE(app, "/sign_up")([](){
        auto page = crow::mustache::load("sign_up.html");
        return page.render();
    });
    CROW_ROUTE(app, "/sign_up_post").methods("POST"_method)([&tables](const crow::request& req) -> crow::response {
        auto req_body = crow::query_string(("?" + req.body).c_str());
        std::string name=req_body.get("name");
        std::string username=req_body.get("username");
        std::string email=req_body.get("email");
        std::string pass=req_body.get("password");
        std::string type=req_body.get("type");
        if(type=="Farmer"){
            farmer_data* new_user=new farmer_data(name,username,email,pass);
            tables.addFarmer(new_user);
            auto page=crow::mustache::load("farmer.html");
            return crow::response(page.render());
        }
        else{
            buyer_data* new_user=new buyer_data(name,username,email,pass);
            tables.addBuyer(new_user);
            auto page=crow::mustache::load("buyer/buyer_home.html");
            return crow::response(page.render());
        }
    });

    app.bindaddr("0.0.0.0").port(18080).multithreaded().run();
}
