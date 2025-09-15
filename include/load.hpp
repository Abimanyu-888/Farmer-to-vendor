#ifndef USER_DATA_HPP
#define USER_DATA_HPP
#include<iostream>
#include<string>
#include<vector>
#include "json.hpp"
#include <fstream> 
using json = nlohmann::json;

struct farmer_data {
    std::string name;
    std::string username;
    std::string email;
    std::string password;
    std::vector<std::string> products;
    std::vector<std::string> orders;
    std::string state;
    int Total_Revenue;
    
    farmer_data(const std::string& thename,const std::string& theusername,const std::string& theemail,const std::string& thepassword,const std::string& thestate):
    name(thename),
    username(theusername),
    email(theemail), 
    password(thepassword),
    state(thestate),
    Total_Revenue(0)
    {}
    farmer_data(const std::string& thename,const std::string& theusername,const std::string& theemail,const std::string& thepassword,const std::string& thestate,const int& theTotal_Revenue,const std::vector<std::string>& theproducts,const std::vector<std::string>& theorders):
    name(thename),
    username(theusername),
    email(theemail), 
    password(thepassword),
    state(thestate),
    Total_Revenue(theTotal_Revenue),
    products(theproducts),
    orders(theorders)
    {}
};
struct farmer_link{
    farmer_data* data=nullptr;
    farmer_link* next=NULL;
};

struct buyer_data {
    std::string name;
    std::string username;
    std::string email;
    std::string password;
    std::vector<std::string> orders;
    std::string state;
    buyer_data(const std::string& thename,const std::string& theusername,const std::string& theemail,const std::string& thepassword,const std::string& thestate):
    name(thename), 
    username(theusername),
    email(theemail),
    password(thepassword),
    state(thestate)
    {}
    buyer_data(const std::string& thename,const std::string& theusername,const std::string& theemail,const std::string& thepassword,const std::string& thestate,const std::vector<std::string>& theorders):
    name(thename), 
    username(theusername),
    email(theemail),
    password(thepassword),
    state(thestate),
    orders(theorders)
    {}
};
struct buyer_link{
    buyer_data* data=nullptr;
    buyer_link* next=NULL;
};

struct product_data {
    std::string product_id;
    std::string product_name;
    std::string category;
    std::string owner;
    int price = 0;
    int stock = 0;
    std::string unit;
    std::string about;

    
    product_data(const std::string& theproduct_id,const std::string& thename,const std::string& thecategory,const std::string& theowner,int theprice,int totalstock,const std::string& theunit,const std::string& theabout):
    product_id(theproduct_id), 
    product_name(thename),
    category(thecategory), 
    owner(theowner),
    price(theprice), 
    stock(totalstock),
    unit(theunit), 
    about(theabout){}
};
struct product_link{
    product_data* data=nullptr;
    product_link* next=NULL;
};

struct order_data {
    std::string order_id;
    std::string product_id;
    int quantity = 0;
    bool isEmpty = true;
    
    order_data() = default;
    
    order_data(const std::string& theorder_id,const std::string& theproduct_id,int thequantity):
    order_id(theorder_id), product_id(theproduct_id),
    quantity(thequantity), isEmpty(false) {}
};
struct order_link{
    order_data* data=nullptr;
    order_link* next=NULL;
};

struct email_link{
    std::string* username=nullptr;
    std::string* email=nullptr;
    email_link* next=NULL;
};

class hash_tables{
    farmer_link** farmers;
    buyer_link** buyers;
    product_link** products;
    order_link** orders;
    email_link** email_username;
    int size;
    
    uint32_t fnv1a(std::string s){
        const uint32_t bais=2166136261u;
        const uint32_t prime=16777619u;
        uint32_t hash=bais;
        for(unsigned char c:s){
            hash^=c;
            hash*=prime;
        }
        return hash;
    }

    void farmer_hashtable(int& size){
        std::ifstream file("db/json/farmers.json");
        if (!file.is_open()) {
            throw std::runtime_error("Could not open farmers.json");
        }
        json data;
        
        file>>data;
        for(const auto& user:data){     
            uint32_t index=fnv1a(user["username"])%size;
            farmer_data* new_user=new farmer_data(user["name"],user["username"],user["email"],user["password"],user["state"],user["Total_Revenue"],user["products"].get<std::vector<std::string>>(),user["orders"].get<std::vector<std::string>>());
            farmer_link* newnode=new farmer_link;
            newnode->data=new_user;
            if(farmers[index]){
                newnode->next = farmers[index];
                farmers[index] = newnode;
            }
            else{
                farmers[index]=newnode;
            }

            index=fnv1a(user["email"])%(size*2);
            std::string* new_email=new std::string(user["email"]);
            std::string* new_username=new std::string(user["username"]);
            email_link* email_node=new email_link;
            email_node->email=new_email;
            email_node->username=new_username;
            if(email_username[index]){
                email_node->next=email_username[index];
                email_username[index]=email_node;
            }
            else{
                email_username[index]=email_node;
            }
        }
    } 
    void buyer_hashtable(int& size){
        std::ifstream file("db/json/buyers.json");
        if (!file.is_open()) {
            throw std::runtime_error("Could not open buyers.json");
        }
        json data;
        file>>data;
        for(const auto& user:data){
            uint32_t index=fnv1a(user["username"])%size;
            buyer_data* new_user=new buyer_data(user["name"],user["username"],user["email"],user["password"],user["state"],user["orders"].get<std::vector<std::string>>());
            buyer_link* newnode=new buyer_link;
            newnode->data=new_user;
            if(buyers[index]){
                newnode->next = buyers[index];
                buyers[index] = newnode;
            }
            else{
                buyers[index]=newnode;
            }

            index=fnv1a(user["email"])%(size*2);
            std::string* new_email=new std::string(user["email"]);
            std::string* new_username=new std::string(user["username"]);
            email_link* email_node=new email_link;
            email_node->email=new_email;
            email_node->username=new_username;
            if(email_username[index]){
                email_node->next=email_username[index];
                email_username[index]=email_node;
            }
            else{
                email_username[index]=email_node;
            }
        }
    } 
    void product_hashtable(int& size){
        std::ifstream file("db/json/products.json");
        if (!file.is_open()) {
            throw std::runtime_error("Could not open products.json");
        }
        json data;
        
        file>>data;
        for(const auto& product:data){
            uint32_t index=fnv1a(product["product_id"])%size;
            product_data* new_product=new product_data(product["product_id"],product["product_name"],product["category"],product["owner"],product["price"],product["stock"],product["unit"],product["about"]);
            product_link* newnode=new product_link;
            newnode->data=new_product;
            if(products[index]){
                newnode->next = products[index];
                products[index] = newnode;
            }
            else{
                products[index]=newnode;
            }
        }
    } 
    void order_hashtable(int& size){
        std::ifstream file("db/json/orders.json");
        if (!file.is_open()) {
            throw std::runtime_error("Could not open orders.json");
        }
        json data;
        
        file>>data;
        for(const auto& order:data){
            uint32_t index=fnv1a(order["order_id"])%size;
            order_data* new_order=new order_data(order["order_id"],order["product_id"],order["quantity"]);
            order_link* newnode=new order_link;
            newnode->data=new_order;
            if(orders[index]){
                newnode->next = orders[index];
                orders[index] = newnode;
            }
            else{
                orders[index]=newnode;
            }
        }
    }

public:
    hash_tables(){
        size=100;
        email_username=new email_link*[size*2];
        farmers=new farmer_link*[size];
        buyers=new buyer_link*[size];
        products=new product_link*[size];
        orders=new order_link*[size];
        for(int i=0;i<size*2;i++){
            email_username[i]=nullptr;
        }
        for (int i = 0; i < size; ++i) {
            farmers[i] = nullptr;
            buyers[i] = nullptr;
            products[i] = nullptr;
            orders[i] = nullptr;
        }
        farmer_hashtable(size);
        buyer_hashtable(size);
        product_hashtable(size);
        order_hashtable(size);
    }
    farmer_data* findFarmer(std::string s){
        uint32_t index=fnv1a(s)%size;
        farmer_link* node=farmers[index];
        while(node){
            if(node->data->username==s) return node->data;
            node=node->next;
        }
        return nullptr;
    }
    buyer_data* findBuyer(std::string s){
        uint32_t index=fnv1a(s)%size;
        buyer_link* node=buyers[index];
        while(node){
            if(node->data->username==s) return node->data;
            node=node->next;
        }
        return nullptr;
    }
    product_data* findProduct(std::string s){
        uint32_t index=fnv1a(s)%size;
        product_link* node=products[index];
        while(node){
            if(node->data->product_id==s) return node->data;
            node=node->next;
        }
        return nullptr;
    }
    order_data* findOrder(std::string s){
        uint32_t index=fnv1a(s)%size;
        order_link* node=orders[index];
        while(node){
            if(node->data->order_id==s) return node->data;
            node=node->next;
        }
        return nullptr;
    }

    void addFarmer(farmer_data* new_user){
        uint32_t index=fnv1a(new_user->username)%size;
        farmer_link* newnode=new farmer_link;
        newnode->data=new_user;
        if(farmers[index]){
            newnode->next = farmers[index];
            farmers[index] = newnode;
        }
        else{
            farmers[index]=newnode;
        }

        index=fnv1a(new_user->email)%(size*2);
        std::string* new_email=new std::string(new_user->email);
        std::string* new_username=new std::string(new_user->username);
        email_link* email_node=new email_link;
        email_node->email=new_email;
        email_node->username=new_username;
        if(email_username[index]){
            email_node->next=email_username[index];
            email_username[index]=email_node;
        }
        else{
            email_username[index]=email_node;
        }


    }
    void addBuyer(buyer_data* new_user){
        uint32_t index=fnv1a(new_user->username)%size;
        buyer_link* newnode=new buyer_link;
        newnode->data=new_user;
        if(buyers[index]){
            newnode->next = buyers[index];
            buyers[index] = newnode;
        }
        else{
            buyers[index]=newnode;
        }
        
        index=fnv1a(new_user->email)%(size*2);
        std::string* new_email=new std::string(new_user->email);
        std::string* new_username=new std::string(new_user->username);
        email_link* email_node=new email_link;
        email_node->email=new_email;
        email_node->username=new_username;
        if(email_username[index]){
            email_node->next=email_username[index];
            email_username[index]=email_node;
        }
        else{
            email_username[index]=email_node;
        }
    }
    void addProduct(product_data* new_product){
        uint32_t index=fnv1a(new_product->product_id)%size;
        product_link* newnode=new product_link;
            newnode->data=new_product;
            if(products[index]){
                newnode->next = products[index];
                products[index] = newnode;
            }
            else{
                products[index]=newnode;
            }
    }
    void addOrder(order_data* new_order){
        uint32_t index=fnv1a(new_order->order_id)%size;
        order_link* newnode=new order_link;
            newnode->data=new_order;
            if(orders[index]){
                newnode->next = orders[index];
                orders[index] = newnode;
            }
            else{
                orders[index]=newnode;
            }
    }
    std::string* findUsername(std::string& theEmail){
        uint32_t index=fnv1a(theEmail)%(size*2);
        email_link* node=email_username[index];
        while(node){
            if(*(node->email)==theEmail) return node->username;
            node=node->next;
        }
        return nullptr;
    }
    void delete_product(std::string id){
        uint32_t index=fnv1a(id)%size;
        product_link* node=products[index];
        product_link* parent=nullptr;
        while(node != nullptr && node->data->product_id!=id){
            parent=node;
            node=node->next;
        }
        if (node == nullptr) {
            return; 
        }
        if(parent) parent->next=node->next;
        else products[index]=node->next;
        delete node->data;
        delete node;
    }   
    ~hash_tables(){
        for(int i=0;i<size;i++){
            farmer_link* curr1=farmers[i];
            while(curr1){

                farmer_link* next=curr1->next;
                delete curr1->data;
                delete curr1;
                curr1=next;
            }
            buyer_link* curr2=buyers[i];
            while(curr2){
                buyer_link* next=curr2->next;
                delete curr2->data;
                delete curr2;
                curr2=next;
            }
            product_link* curr3=products[i];
            while(curr3){
                product_link* next=curr3->next;
                delete curr3->data;
                delete curr3;
                curr3=next;
            }
            order_link* curr4=orders[i];
            while(curr4){
                order_link* next=curr4->next;
                delete curr4->data;
                delete curr4;
                curr4=next;
            }
        }
        delete[] farmers;
        delete[] buyers;
        delete[] products;
        delete[] orders;

        for(int i=0;i<size*2;i++){
            email_link* curr=email_username[i];
            while(curr){
                email_link* next=curr->next;
                delete curr->email;
                delete curr->username;
                delete curr;
                curr=next;
            }
        }
        delete[] email_username;
    }
};

#endif