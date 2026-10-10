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
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Magic number calculator</title>
    <style>
        * { box-sizing: border-box; }
        body { font-family: system-ui, sans-serif; background: #f4f4f9; padding: 20px; margin: 0; display: flex; justify-content: center; }
        .container { background: #fff; padding: 25px; border-radius: 8px; box-shadow: 0 4px 10px rgba(0,0,0,0.1); width: 100%; max-width: 650px; }
        h1 { font-size: 24px; margin-top: 0; }
        p { color: #555; line-height: 1.5; }
        label { display: block; margin-bottom: 6px; font-weight: 600; }
        input { width: 100%; padding: 10px; margin-bottom: 15px; border: 1px solid #ccc; border-radius: 4px; font-size: 16px; }
        button { width: 100%; padding: 10px; background: #007bff; color: white; border: none; border-radius: 4px; font-size: 16px; cursor: pointer; margin-bottom: 15px; }
        button:hover { background: #0056b3; }
        button:disabled { opacity: 0.6; cursor: wait; }
        pre { background: #282c34; color: #e2e8f0; padding: 15px; border-radius: 4px; overflow-x: auto; white-space: pre-wrap; overflow-wrap: anywhere; margin-bottom: 0; }
    </style>
</head>
<body>
    <main class="container">
        <h1>Magic number calculator</h1>
        <form id="rangeForm">
            <label for="start">Start</label>
            <input type="text" inputmode="numeric" pattern="[0-9]{1,18}" maxlength="18" id="start" placeholder="e.g. 100" required>
            <label for="end">End</label>
            <input type="text" inputmode="numeric" pattern="[0-9]{1,18}" maxlength="18" id="end" placeholder="e.g. 110" required>
            <button type="submit" id="calculateButton">Calculate</button>
        </form>
        <pre id="output" role="status" aria-live="polite">Output will appear here...</pre>
    </main>
    <script>
        document.getElementById('rangeForm').addEventListener('submit', async (event) => {
            event.preventDefault();
            const start = document.getElementById('start').value;
            const end = document.getElementById('end').value;
            const output = document.getElementById('output');
            const button = document.getElementById('calculateButton');
            // Compare as BigInt to avoid rounding large integer inputs.
            if (!/^[0-9]{1,18}$/.test(start) || !/^[0-9]{1,18}$/.test(end)) {
                output.textContent = 'Please enter non-negative whole numbers with at most 18 digits.';
                return;
            }
            if (BigInt(start) > BigInt(end)) {
                output.textContent = 'Start must be less than or equal to End.';
                return;
            }
            button.disabled = true;
            output.textContent = 'Calculating...';
            try {
                const params = new URLSearchParams({ start, end });
                const response = await fetch(`/api/kaprekar?${params}`);
                const result = await response.text();
                output.textContent = response.ok ? result : `Request failed: ${result}`;
            } catch (error) {
                output.textContent = 'Network error occurred. Please try again.';
            } finally {
                button.disabled = false;
            }
        });
    </script>
</body>
</html>)HTML";
        res.set_content(html, "text/html; charset=utf-8");
    });

    svr.Get("/api/kaprekar", [](const httplib::Request &req, httplib::Response &res) {
        if (!req.has_param("start") || !req.has_param("end")) {
            res.status = 400;
            res.set_content("Please provide both start and end.", "text/plain; charset=utf-8");
            return;
        }

        ll start, end;
        if (!parseNumber(req.get_param_value("start"), start) ||
            !parseNumber(req.get_param_value("end"), end)) {
            res.status = 400;
            res.set_content("Enter non-negative whole numbers with at most 18 digits.", "text/plain; charset=utf-8");
            return;
        }
        if (start > end) {
            res.status = 400;
            res.set_content("Start must be less than or equal to End.", "text/plain; charset=utf-8");
            return;
        }

        // Bound the count before computing any routines, even for huge ranges.
        const ll count = min(kMaxResults, end - start + 1);
        string output = "Showing " + to_string(count) + " number(s) from " +
            to_string(start) + " to " + to_string(start + count - 1) + ".\n";
        if (end - start + 1 > kMaxResults) {
            output += "Range contains more than 50 numbers; showing only the first 50.\n";
        }
        for (ll offset = 0; offset < count; ++offset) {
            const ll number = start + offset;
            output += "\n========================================\n";
            output += "Number: " + to_string(number) + "\n";
            output += "========================================\n";
            output += runKaprekar(to_string(number));
        }
        res.set_content(output, "text/plain; charset=utf-8");
    });

    cout << "Server starting on port 8080...\n";
    if (!svr.listen("0.0.0.0", 8080)) {
        cerr << "Failed to start the server on port 8080.\n";
        return 1;
    }
    return 0;
}
