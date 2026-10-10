#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <exception>
#include <cctype>
#include "httplib.h"

using namespace std;
#define ll long long

string padZeros(ll num, size_t len) {
    string s = to_string(num);
    if (s.length() < len) {
        s.insert(0, len - s.length(), '0');
    }
    return s;
}

string runKaprekar(string input_str) {
    try {
        size_t len = input_str.length();
        string dec_str = input_str;
        sort(dec_str.begin(), dec_str.end(), greater<char>());
        
        string inc_str = input_str;
        sort(inc_str.begin(), inc_str.end());
        
        ll n_dec = stoll(dec_str);
        ll n_inc = stoll(inc_str);
        ll diff = n_dec - n_inc;
       
        vector<ll> history;
        string output = dec_str + "\n" + inc_str + "\n" + "diff: " + padZeros(diff, len) + "\n\n";
        
        while (true) {
            auto it = find(history.begin(), history.end(), diff);
            
            if (it != history.end()) {
                for (auto i = it; i != history.end(); ++i) {
                    output += padZeros(*i, len) + " -> ";
                }
                output += padZeros(diff, len) + "\n";
                break;
            }
           
            history.push_back(diff);
         
            string current_str = padZeros(diff, len);
            
            string next_dec = current_str;
            sort(next_dec.begin(), next_dec.end(), greater<char>());
            
            string next_inc = current_str;
            sort(next_inc.begin(), next_inc.end());
            
            n_dec = stoll(next_dec);
            n_inc = stoll(next_inc);
            
            diff = n_dec - n_inc;
            
            output += next_dec + "\n" + next_inc + "\n" + "diff: " + padZeros(diff, len) + "\n\n";
        }
        return output;
    } catch (...) {
        return "Error: Invalid input format. Please enter a valid number.";
    }
}

// Keep every sorted digit arrangement safely within signed 64-bit range.
const ll kMaxInput = 999999999999999999LL;
const ll kMaxResults = 50;

bool parseNumber(const string &text, ll &number) {
    if (text.empty() || text.size() > 18 ||
        !all_of(text.begin(), text.end(), [](unsigned char c) {
            return isdigit(c);
        })) {
        return false;
    }
    try {
        number = stoll(text);
        return number <= kMaxInput;
    } catch (...) {
        return false;
    }
}

int main() {
    httplib::Server svr;

    svr.Get("/", [](const httplib::Request &, httplib::Response &res) {
        const string html = R"HTML(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
