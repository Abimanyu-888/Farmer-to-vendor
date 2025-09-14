#include "crow.h"
#include "crow/middlewares/cookie_parser.h"
#include <iostream>
#include <filesystem> 
#include "include/load.hpp"
#include <map>
#include <utility>
std::map<std::string, std::pair<std::string, std::string>> active_sessions;

int main()
{
    crow::mustache::set_base("templates");
    auto landing = crow::mustache::load("landing_page.html");
    hash_tables tables;

    crow::App<crow::CookieParser> app;
    CROW_ROUTE(app, "/")([&app,&tables](const crow::request& req, crow::response& res){
        std::string session_id = app.get_context<crow::CookieParser>(req).get_cookie("session_id");
        if (session_id.empty() || active_sessions.find(session_id) == active_sessions.end()) {
            res.code = 303;
            res.add_header("Location", "/landing_page");
            res.end();
        }
        auto& session_info = active_sessions.at(session_id);
        std::string username = session_info.first;
        std::string user_type = session_info.second;
        if (user_type == "Farmer") {
            res.code = 303;
            res.add_header("Location", "/farmer");
            res.end();
        }
        else{
            res.code = 303;
            res.add_header("Location", "/buyer");
            res.end();
        }

    });
    CROW_ROUTE(app, "/landing_page")([&landing](){
        auto page = landing;
        return page.render();
    });

    CROW_ROUTE(app, "/farmer")([&app](const crow::request& req) -> crow::response {
        std::string session_id = app.get_context<crow::CookieParser>(req).get_cookie("session_id");
        if (session_id.empty() || active_sessions.find(session_id) == active_sessions.end()) {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }

        auto& session_info = active_sessions.at(session_id);
        std::string username = session_info.first;
        std::string user_type = session_info.second;

        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }

        crow::mustache::context ctx;
        ctx["username"]=username;
        auto page=crow::mustache::load("farmer/farmer_dashboard.html");
        return crow::response(page.render(ctx));

    });
    CROW_ROUTE(app, "/buyer")([&app](const crow::request& req) -> crow::response {
        std::string session_id = app.get_context<crow::CookieParser>(req).get_cookie("session_id");
        if (session_id.empty() || active_sessions.find(session_id) == active_sessions.end()) {
            crow::response res(303);
            res.add_header("Location",  "/error");
            return res;
        }
        auto& session_info = active_sessions.at(session_id);
        std::string username = session_info.first;
        std::string user_type = session_info.second;

        if (user_type != "Buyer") {
            crow::response res(303);
            res.add_header("Location",  "/error");
            return res;
        }


        auto page=crow::mustache::load("buyer/buyer_home.html");
        return crow::response(page.render());

    });
    CROW_ROUTE(app, "/sign_in")([](){
        auto page = crow::mustache::load("sign_in.html");
        return page.render();
    });
    CROW_ROUTE(app, "/sign_in_post").methods("POST"_method)([&app,&tables](const crow::request& req, crow::response& res) {
        auto req_body = crow::query_string(("?" + req.body).c_str()); 

        std::string email=req_body.get("email");
        std::string pass=req_body.get("password");
        std::string type=req_body.get("type");
        std::string* user=tables.findUsername(email);

        if (!user) {
            res.redirect("/error");
            res.end();
            return;
        }

        bool login_success = false;
        if(type=="Farmer"){
            farmer_data* data=tables.findFarmer(*user);
            if(data && data->password==pass){
                login_success= true;
            }
        }
        else{
            buyer_data* data=tables.findBuyer(*user);
            if(data && data->password==pass){
                login_success=true;
            }
        }
        if(login_success){
            std::string session_id = "session_" + std::to_string(rand());
            active_sessions[session_id] = {*user, type};

            app.get_context<crow::CookieParser>(req).set_cookie("session_id", session_id);
            res.code = 303;
            if (type=="Farmer") {
                res.add_header("Location", "/farmer");
            }
            else{
                res.add_header("Location", "/buyer");
            }
        }
        else{
            res.code = 303;
            res.add_header("Location", "/error");
        }
        res.end();
    });
    CROW_ROUTE(app, "/sign_up")([](){
        auto page = crow::mustache::load("sign_up.html");
        return page.render();
    });
    CROW_ROUTE(app, "/sign_up_post").methods("POST"_method)([&app, &tables](const crow::request& req, crow::response& res){
        auto req_body = crow::query_string(("?" + req.body).c_str());
        std::string name=req_body.get("name");
        std::string username=req_body.get("username");
        std::string email=req_body.get("email");
        std::string pass=req_body.get("password");
        std::string state=req_body.get("state");
        std::string type=req_body.get("type");

        std::string session_id = "session_" + std::to_string(rand());
        active_sessions[session_id] = {username, type};
        app.get_context<crow::CookieParser>(req).set_cookie("session_id", session_id);
        
        res.code = 303;


        if(type=="Farmer"){
            std::string farmname=req_body.get("farm_name");
            farmer_data* new_user=new farmer_data(name,username,email,pass,state);
            tables.addFarmer(new_user);
            res.add_header("Location", "/farmer");
        }
        else{
            buyer_data* new_user=new buyer_data(name,username,email,pass,state);
            tables.addBuyer(new_user);
            res.add_header("Location", "/buyer");
        }
        res.end();
    });
    CROW_ROUTE(app, "/error")([](){
        auto page=crow::mustache::load("error.html");
        return crow::response(page.render());
    });

    CROW_ROUTE(app, "/logout")([&app](const crow::request& req, crow::response& res){
        std::string session_id = app.get_context<crow::CookieParser>(req).get_cookie("session_id");

        if (!session_id.empty()) {
            active_sessions.erase(session_id);
        }
        app.get_context<crow::CookieParser>(req).set_cookie("session_id", "").max_age(0);
        res.add_header("Location","/");
        res.code=303;
        res.end();
    });
    app.bindaddr("0.0.0.0").port(18080).multithreaded().run();
}
