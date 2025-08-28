#ifndef USER_DATA_HPP
#define USER_DATA_HPP
#include<iostream>
#include<string>
#include "json.hpp"
#include <fstream> 
using json = nlohmann::json;
using namespace std;
class user_data{
public:
    string name;
    string username;
    string email;
    string password;
    string products[50];
    string orders[50];
    bool isEmpty;
    user_data(){isEmpty=true;};
    user_data(string thename,string theusername,string theemail,string thepassword){
        name=thename;
        username=theusername;
        email=theemail;
        password=thepassword;
        isEmpty=false;
    }
};

//FNV-1a hashing
uint32_t fnv1a(string s){
    const uint32_t bais=2166136261u;
    const uint32_t prime=16777619u;
    uint32_t hash=bais;
    for(unsigned char c:s){
        hash^=c;//xor operation
        hash*=prime;
    }
    return hash%100;

}
user_data* user_hashtable(){
    ifstream file("db/json/farmers.json");
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return nullptr;
    }
    json data;
    user_data* hashArr=new user_data[100];
    file>>data;
    for(const auto& user:data){
        uint32_t index=fnv1a(user["username"]);
        if(hashArr[index].isEmpty){
            hashArr[index]=user_data(user["name"],user["username"],user["email"],user["password"]);
        }
        else{
            while(!hashArr[index].isEmpty){
                index=(index+1)%100;
            }
            hashArr[index]=user_data(user["name"],user["username"],user["email"],user["password"]);
        }
    }
    return hashArr;
} 
#endif