#include "crow.h"
#include "crow/middlewares/cookie_parser.h"
#include "crow/middlewares/session.h"
#include <iostream>
#include <filesystem>
#include "include/load.hpp"
#include <vector>
#include <string>
#include <any> 
#include "include/helper.hpp"
#include <fstream> 
#include <filesystem>

using Session = crow::SessionMiddleware<crow::InMemoryStore>;

int main() {
    crow::mustache::set_base("templates");
    hash_tables tables;

    crow::App<crow::CookieParser, Session> app;

    app.get_middleware<Session>();

    CROW_ROUTE(app, "/")([&app](const crow::request& req) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type.empty()) {
            crow::response res(303);
            res.add_header("Location", "/landing_page");
            return res;
        }

        crow::response res(303);
        if (user_type == "Farmer") {
            res.add_header("Location", "/farmer/dashboard");
        } else {
            res.add_header("Location", "/buyer");
        }
        return res;
    });

    CROW_ROUTE(app, "/landing_page")([]() {
        auto page = crow::mustache::load("landing_page.html");
        return page.render();
    });

    CROW_ROUTE(app, "/buyer")([&app](const crow::request& req) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");
        
        if (user_type != "Buyer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }

        auto page = crow::mustache::load("buyer/buyer_home.html");
        return crow::response(page.render());
    });

    CROW_ROUTE(app, "/sign_in")([]() {
        auto page = crow::mustache::load("sign_in.html");
        return page.render();
    });

    CROW_ROUTE(app, "/sign_in_post").methods("POST"_method)([&app, &tables](const crow::request& req, crow::response& res) {
        auto req_body = crow::query_string(("?" + req.body).c_str());
        std::string email = req_body.get("email");
        std::string pass = req_body.get("password");
        std::string type = req_body.get("type");
        std::string* user = tables.findUsername(email);

        if (!user) {
            res.redirect("/error");
            res.end();
            return;
        }

        bool login_success = false;
        if (type == "Farmer") {
            farmer_data* data = tables.findFarmer(*user);
            if (data && data->password == pass) {
                login_success = true;
            }
        } else {
            buyer_data* data = tables.findBuyer(*user);
            if (data && data->password == pass) {
                login_success = true;
            }
        }

        if (login_success) {
            auto& session = app.get_context<Session>(req);
            session.set("username", *user);
            session.set("user_type", type);

            res.code = 303;
            if (type == "Farmer") {
                res.add_header("Location", "/farmer/dashboard");
            } else {
                res.add_header("Location", "/buyer");
            }
        } else {
            res.code = 303;
            res.add_header("Location", "/error");
        }
        res.end();
    });

    CROW_ROUTE(app, "/sign_up")([]() {
        auto page = crow::mustache::load("sign_up.html");
        return page.render();
    });

    CROW_ROUTE(app, "/sign_up_post").methods("POST"_method)([&app, &tables](const crow::request& req, crow::response& res) {
        auto req_body = crow::query_string(("?" + req.body).c_str());
        std::string name = req_body.get("name");
        std::string username = req_body.get("username");
        std::string email = req_body.get("email");
        std::string pass = req_body.get("password");
        std::string state = req_body.get("state");
        std::string type = req_body.get("type");

        auto& session = app.get_context<Session>(req);
        session.set("username", username);
        session.set("user_type", type);
        res.code = 303;
        if (type == "Farmer") {
            farmer_data* new_user = new farmer_data(name, username, email, pass, state);
            tables.addFarmer(new_user);
            res.add_header("Location", "/farmer/dashboard");
        } else {
            buyer_data* new_user = new buyer_data(name, username, email, pass, state);
            tables.addBuyer(new_user);
            res.add_header("Location", "/buyer");
        }
        res.end();
    });

    CROW_ROUTE(app, "/error")([]() {
        auto page = crow::mustache::load("error.html");
        return crow::response(page.render());
    });

    CROW_ROUTE(app, "/logout")([&app](const crow::request& req, crow::response& res) {
        auto& session = app.get_context<Session>(req);

        for (const auto& key : session.keys()) {
            session.remove(key);
        }

        res.add_header("Location", "/landing_page");
        res.code = 303;
        res.end();
    });


    CROW_ROUTE(app, "/farmer/dashboard")([&app,&tables](const crow::request& req) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }

        std::string username = session.get<std::string>("username");
        farmer_data* data = tables.findFarmer(username);
        crow::mustache::context ctx;
        ctx["username"] = username;
        ctx["revenue"]=data->Total_Revenue;
        ctx["pending_orders_count"]=data->orders.size();
        ctx["products_count"]=data->products.size();

        crow::json::wvalue::list stock_list;
        for(auto& prod_id:data->products){
            product_data* prod_data=tables.findProduct(prod_id);
            stock_list.emplace_back(crow::json::wvalue{
                {"name", prod_data->product_name}, 
                {"stock", prod_data->stock},
                {"unit",prod_data->unit}
            });
        }
        ctx["stock_list"]=std::move(stock_list);

        auto page = crow::mustache::load("farmer/farmer_dashboard.html");
        return crow::response(page.render(ctx));
    });

    CROW_ROUTE(app, "/farmer/products")([&app,&tables](const crow::request& req ) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }
        std::string username = session.get<std::string>("username");
        farmer_data* data = tables.findFarmer(username);
        crow::mustache::context ctx;
        crow::json::wvalue::list product_list;
        for(auto& prod_id:data->products){
            product_data* prod_data=tables.findProduct(prod_id);
            bool is_in_stock = (prod_data->stock > 0);
            product_list.emplace_back(crow::json::wvalue{
                {"name", prod_data->product_name}, 
                {"stock", prod_data->stock},
                {"unit",prod_data->unit},
                {"category",prod_data->category},
                {"price",prod_data->price},
                {"id",prod_data->product_id},
                {"is_in_stock", is_in_stock}
            });
        }
        ctx["product_list"]=std::move(product_list);
        auto page = crow::mustache::load("farmer/farmer_products.html");
        return crow::response(page.render(ctx));
    });

    CROW_ROUTE(app,"/farmer/orders")([&app](const crow::request& req ) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }

        auto page = crow::mustache::load("farmer/farmer_orders.html");
        return crow::response(page.render());
    });

    CROW_ROUTE(app,"/farmer/settings")([&app,&tables](const crow::request& req ) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }
        std::string username = session.get<std::string>("username");
        farmer_data* data = tables.findFarmer(username);
        crow::mustache::context ctx;
        ctx["username"] = username;
        ctx["name"]=data->name;
        ctx["email"]=data->email;
        ctx["phoneNo"]=1234567890;
        auto page = crow::mustache::load("farmer/farmer_settings.html");
        return crow::response(page.render(ctx));
    });

    CROW_ROUTE(app,"/farmer/add_product")([&app](const crow::request& req ) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }

        auto page = crow::mustache::load("farmer/add_product.html");
        return crow::response(page.render());
    });

    CROW_ROUTE(app,"/farmer/new_product").methods("POST"_method)([&app,&tables](const crow::request& req) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }
        std::string username = session.get<std::string>("username");

        auto req_body = crow::query_string(("?" + req.body).c_str());
        std::string id = generate_product_id();
        std::string name=req_body.get("productName");
        std::string category=req_body.get("category");
        int price=std::stoi(req_body.get("price"));
        int stock=std::stoi(req_body.get("stock"));
        std::string unit=req_body.get("unit");
        std::string about=req_body.get("about");
        product_data* new_data=new product_data(id,name,category,username,price,stock,unit,about);
        tables.addProduct(new_data);

        farmer_data* update_Farmerdata = tables.findFarmer(username);
        update_Farmerdata->products.push_back(id);

        crow::response res;
        res.code = 302;
        res.set_header("Location", "/farmer/products");
        return res;
    });
    CROW_ROUTE(app, "/farmer/edit_product/<string>")([&app,&tables](const crow::request& req,const std::string& id) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }
        product_data* data=tables.findProduct(id);
        crow::mustache::context ctx;
        ctx["product_name"]=data->product_name;
        ctx["category"]=data->category;
        ctx["about"]=data->about;
        ctx["id"]=id;
        ctx["price"]=data->price;
        ctx["unit"]=data->unit;
        ctx["stock"]=data->stock;


        auto page = crow::mustache::load("farmer/edit_product.html");
        return crow::response(page.render(ctx));
    });
    CROW_ROUTE(app, "/farmer/delete_product/<string>")([&app,&tables](const crow::request& req,const std::string& id) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }

        std::string username = session.get<std::string>("username");
        farmer_data* update_Farmerdata = tables.findFarmer(username);
        update_Farmerdata->products.erase(std::remove(update_Farmerdata->products.begin(), update_Farmerdata->products.end(), id), update_Farmerdata->products.end());
        tables.delete_product(id);
        crow::response res;
        res.code = 302;
        res.set_header("Location", "/farmer/products");
        return res;
    });
    
    CROW_ROUTE(app, "/farmer/edit_product_post/<string>").methods("POST"_method)([&app,&tables](const crow::request& req,const std::string& id) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }
        auto req_body = crow::query_string(("?" + req.body).c_str());
        product_data* data=tables.findProduct(id);
        data->product_name=req_body.get("productName");
        data->category=req_body.get("category");
        data->about=req_body.get("about");
        data->price=std::stoi(req_body.get("price"));
        data->stock=std::stoi(req_body.get("stock"));
        data->unit=req_body.get("unit");

        crow::response res(303); 
        res.add_header("Location", "/farmer/products");
        return res;
    });

    app.bindaddr("0.0.0.0").port(18080).multithreaded().run();
}

