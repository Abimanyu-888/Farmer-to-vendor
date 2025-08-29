#ifndef USER_DATA_HPP
#define USER_DATA_HPP
#include<iostream>
#include<string>
#include "json.hpp"
#include <fstream> 
using json = nlohmann::json;
class farmer_data{
public:
    std::string name;
    std::string username;
    std::string email;
    std::string password;
    std::string products[50];
    std::string orders[50];
    bool isEmpty;
    farmer_data(){isEmpty=true;};
    farmer_data(std::string thename,std::string theusername,std::string theemail,std::string thepassword){
        name=thename;
        username=theusername;
        email=theemail;
        password=thepassword;
        isEmpty=false;
    }
};

class buyer_data{
public:
    std::string name;
    std::string username;
    std::string email;
    std::string password;
    std::string orders[50];
    bool isEmpty;
    buyer_data(){isEmpty=true;};
    buyer_data(std::string thename,std::string theusername,std::string theemail,std::string thepassword){
        name=thename;
        username=theusername;
        email=theemail;
        password=thepassword;
        isEmpty=false;
    }
};

//FNV-1a hashing
uint32_t fnv1a(std::string s){
    const uint32_t bais=2166136261u;
    const uint32_t prime=16777619u;
    uint32_t hash=bais;
    for(unsigned char c:s){
        hash^=c;//xor operation
        hash*=prime;
    }
    return hash%100;

}
farmer_data* farmer_hashtable(){
    std::ifstream file("db/json/farmers.json");
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return nullptr;
    }
    json data;
    farmer_data* hashArr=new farmer_data[100];
    file>>data;
    for(const auto& user:data){
        uint32_t index=fnv1a(user["username"]);
        if(hashArr[index].isEmpty){
            hashArr[index]=farmer_data(user["name"],user["username"],user["email"],user["password"]);
        }
        else{
            while(!hashArr[index].isEmpty){
                index=(index+1)%100;
            }
            hashArr[index]=farmer_data(user["name"],user["username"],user["email"],user["password"]);
        }
    }
    return hashArr;
} 
buyer_data* buyer_hashtable(){
    std::ifstream file("db/json/buyers.json");
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return nullptr;
    }
    json data;
    buyer_data* hashArr=new buyer_data[100];
    file>>data;
    for(const auto& user:data){
        uint32_t index=fnv1a(user["username"]);
        if(hashArr[index].isEmpty){
            hashArr[index]=buyer_data(user["name"],user["username"],user["email"],user["password"]);
        }
        else{
            while(!hashArr[index].isEmpty){
                index=(index+1)%100;
            }
            hashArr[index]=buyer_data(user["name"],user["username"],user["email"],user["password"]);
        }
    }
    return hashArr;
} 
#endif