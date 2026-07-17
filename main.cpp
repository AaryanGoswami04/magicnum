#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <exception>
#include "httplib.h"

using namespace std;
#define ll long long

string padZeros(ll num, int len) {
    string s = to_string(num);
    if (s.length() < len) {
        s.insert(0, len - s.length(), '0');
    }
    return s;
}

string runKaprekar(string input_str) {
    try {
        int len = input_str.length();
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

int main() {
    httplib::Server svr;

    // Serve the User Interface
    svr.Get("/", [](const httplib::Request &, httplib::Response &res) {
        std::string html_content = R"HTML(<!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Calculator</title>
            <style>
                body { font-family: system-ui, sans-serif; background: #f4f4f9; padding: 20px; display: flex; justify-content: center; }
                .container { background: #fff; padding: 25px; border-radius: 8px; box-shadow: 0 4px 10px rgba(0,0,0,0.1); width: 100%; max-width: 400px; }
                input { width: 100%; box-sizing: border-box; padding: 10px; margin-bottom: 15px; border: 1px solid #ccc; border-radius: 4px; font-size: 16px; }
                button { width: 100%; padding: 10px; background: #007bff; color: white; border: none; border-radius: 4px; font-size: 16px; cursor: pointer; margin-bottom: 15px; }
                button:hover { background: #0056b3; }
                pre { background: #282c34; color: #abb2bf; padding: 15px; border-radius: 4px; overflow-x: auto; white-space: pre-wrap; margin-bottom: 0;}
            </style>
        </head>
        <body>
            <div class="container">
                <input type="number" id="inputNum" placeholder="Enter a number">
                <button onclick="calculate()">Calculate</button>
                <pre id="output">Output will appear here...</pre>
            </div>
            <script>
                async function calculate() {
                    const input = document.getElementById('inputNum').value;
                    if (!input) return;
                    document.getElementById('output').textContent = "Calculating...";
                    try {
                        const response = await fetch(`/api/kaprekar?num=${input}`);
                        const result = await response.text();
                        document.getElementById('output').textContent = result;
                    } catch (err) {
                        document.getElementById('output').textContent = "Network error occurred.";
                    }
                }
            </script>
        </body>
        </html>)HTML";
        
        res.set_content(html_content, "text/html");
    });

    // Serve the Computation Logic
    svr.Get("/api/kaprekar", [](const httplib::Request &req, httplib::Response &res) {
        if (req.has_param("num")) {
            std::string num = req.get_param_value("num");
            res.set_content(runKaprekar(num), "text/plain");
        } else {
            res.set_content("Error: No number provided", "text/plain");
        }
    });

    std::cout << "Server starting on port 8080...\n";
    svr.listen("0.0.0.0", 8080);
    return 0;
}