

// for operator*
//      - if one polynomial is significantly larger than the other change which one determines the thread number?
//      - 
#include <iostream>

#include "poly.h"

polynomial::polynomial() {      //constructs and empty map
    std::unordered_map<power, coeff> m;
    poly_map = m;
}

// Constructor exists in poly.h

polynomial::polynomial(const polynomial &other) {           // PASSED
    poly_map = other.poly_map;
}

void polynomial::print() const{                     // PASSED
    std::vector<std::pair<power, coeff>> vec = canonical_form();
    for (auto &[p, c] : vec) {
        if (p == 0) {
            std::cout << c << " + ";
        }
        else if (c == 1) {
            std::cout << "x^" << p << " + ";
        }
        else {
            std::cout << c << "x^" << p << " + ";
        }
    }
    std::cout << std::endl;
}

polynomial &polynomial::operator=(const polynomial &other) {            // Works Now?
    if (this == &other) {   // makes sure its not the same location 
        return *this;
    }
    this->poly_map = other.poly_map;    // copy the poly_map to the new polynomial
    return *this;
}

polynomial polynomial::operator+(const polynomial& poly) const {              // PASSED
    polynomial p1(poly);                   // copy map into new polynomial
    for (auto &[p, c] : poly_map) {         // loop through this poly_map
        p1.poly_map[p] += c;                            // map[] works such that if something doesn't exist it just creates a value of 0 so it works for both creating new polys and adding to original polys
    }
    return p1;
}

polynomial polynomial::operator+(int x) const {                   // PASSED
    polynomial p1(*this);  // copy polynomial to new return value
    p1.poly_map[0] += x;    // add the integer to the new polynomial
    return p1;              // return new polynomial
}

polynomial operator+(int x, const polynomial& poly) {       // PASSED
    polynomial p1;
    p1 = poly + x;
    return p1;
}
// 

void num_multiply_functions_per_thread(const std::unordered_map<power,coeff>& poly_pairs_to_run, const polynomial& continuous_poly, polynomial& poly_sum) {

    for (auto &[power1, coeff1 ] : poly_pairs_to_run) {       // loops through poly_pairs_to_run

        for (auto &[power2, coeff2] : continuous_poly.poly_map) {                    // loop through polynomial

            poly_sum.poly_map[(power1 + power2)] += (coeff1 * coeff2);                 // multiply each one and place them in new polynomial
        }
    }
}


polynomial polynomial::operator*(const polynomial& poly) const {        // figure out why this doesn't work
    polynomial p1;
    unsigned int threads_available = 8;//std::thread::hardware_concurrency();                // number of threads I'm using  (hardware concurrency doesn't work IDK Y)

    std::vector<std::unordered_map<power, coeff>> chunked_version_of_map(threads_available);
    std::vector<polynomial> solved_maps(threads_available);          // should be a size equal to the number of threads I'm using

    // this probably needs to be changed to something else
    if (poly.poly_map.size() > poly_map.size()) {                      // if the number of threads is more than the number of multiply functions being performed
        return poly * (*this);                        // then only use a number of threads equal to the number of mulitplies
    }

    // allocates threads based on the number available
    int count = 0;
    for (auto &[p,c] : poly_map) {                           // divides the poly_map into the 8 unordered_maps in the vectors
        chunked_version_of_map[count % threads_available][p] = c;
        count++;
    }

    std::vector<std::thread> vector_of_threads;

    for (unsigned int index = 0; index < threads_available; index++) {

        std::thread temp_thread(num_multiply_functions_per_thread, std::cref(chunked_version_of_map[index]), std::cref(poly), std::ref(solved_maps[index]));
        vector_of_threads.push_back(std::move(temp_thread));
    }

    for (auto& t : vector_of_threads) {
        t.join();
    }

    for (auto poly : solved_maps) {
        p1 = p1 + poly;                        // sum the returns from all the threads
    }

    return p1;
}


        // original operation
/* 
// returns a polynomial that has taken one value and multiplied it by every value in the polynomial
polynomial polynomial::multiply(const std::pair<power, coeff> &multiply_by) const {
    polynomial p1;
    for (auto &[p,c] : poly_map) {                              // loop through polynomial
    
        p1.poly_map[(p + multiply_by.first)] = (c * multiply_by.second);                 // multiply each one and place them in new polynomial
    }
    return p1;
}

polynomial polynomial::operator*(const polynomial& poly) const {      
    polynomial p1;
    for (auto &poly_pair : poly_map) {
        p1 = p1 + poly.multiply(poly_pair);
    } 
    return p1;
}  */

polynomial polynomial::operator*(int x) const {                 // PASSED
    polynomial p1;
    for (auto &[p,c] : poly_map) {
        p1.poly_map[p] = (c * x);
    }
    return p1;
}

polynomial operator*(int x, const polynomial& poly) {           // PASSED
    polynomial p1;
    p1 = poly * x;
    return p1;
}
/* 
polynomial polynomial::operator-(const polynomial& poly) const {
    polynomial p1(*this);                   // copy map into new polynomial
    for (auto &[p, c] : poly.poly_map) {         // loop through this poly_map
        p1.poly_map[p] -= c;                            // map[] works such that if something doesn't exist it just creates a value of 0 so it works for both creating new polys and adding to original polys
    }

        // remove the zeros coefficients
    for (auto term = p1.poly_map.begin(); term != p1.poly_map.end();) {
        if ((*term).second == 0) {      // if the coefficient is 0 remove it
            term = p1.poly_map.erase(term);     // returns pointer to next position after the remove
        }
        else {
            term++;                             // increment term
        }
    } 
    return p1;
}
 */
/* polynomial polynomial::operator-=(const polynomial& other){
    for (auto &[p,c] : other.poly_map) {
        poly_map[p] -= c;
    }

    // remove 0 coefficients
    for (auto term = poly_map.begin(); term != poly_map.end();) {
        if ((*term).second == 0) {      // if the coefficient is 0 remove it
            term = poly_map.erase(term);     // returns pointer to next position after the remove
        }
        else {
            term++;                             // increment term
        }
    } 
    return *this;
} */

polynomial polynomial::operator%(const polynomial& poly) const {

    polynomial r(*this);        // r = n
    int r_degree;
    int poly_degree = poly.find_degree_of();

    while ((r_degree = r.find_degree_of()) >= poly_degree) {
        // get leading terms
        power lead_divisor_power = poly_degree;
        coeff lead_divisor_coeff = poly.poly_map.at(lead_divisor_power);
        power lead_remainder_power = r_degree;
        coeff lead_remainder_coeff = r.poly_map.at(lead_remainder_power);
        
        // divide leading terms    t = lead(r) / lead(d)
        power new_power_factor = lead_remainder_power - lead_divisor_power;
        coeff new_coeff_factor = lead_remainder_coeff / lead_divisor_coeff;

        // get the single quotient value
        //polynomial q;               // q = 0
        //q.poly_map[new_power_factor] = new_coeff_factor;



        // multiply quotient and the divisor(poly) then subtract from r
        // polynomial subtract = poly * q;
        std::unordered_map<power, coeff> subtract;
        for (auto &[p,c] : poly.poly_map) {
            subtract[p + new_power_factor] = c * new_coeff_factor;
        } 




        // These 2 for loops perform the subtraction rather than sending it to a function this makes it run slightly faster
        for (auto [p,c] : subtract) {
            r.poly_map[p] -= c;
        }
        // remove 0 coefficients
        for (auto term = r.poly_map.begin(); term != r.poly_map.end();) {
            if ((*term).second == 0) {      // if the coefficient is 0 remove it
                term = r.poly_map.erase(term);     // returns pointer to next position after the remove
            }
            else {
                term++;                             // increment term
            }
        } 
    }

    polynomial p1;
    for (auto &[p,c] : r.poly_map) {
        if (c != 0) {
            p1.poly_map[p] = c;
        }
    }
    if (p1.poly_map.size() == 0) {  // if the remainder is zero then p1 will have nothing initialized so set a {0,0} pair
        p1.poly_map[0] = 0;
    }

    return p1;
}

/*
polynomial polynomial::operator%(const polynomial& poly) const {
    //polynomial p1;
    polynomial r(*this);        // r = n
    //polynomial div(poly);       // creates div b/c find_degree is not const
    while (r.find_degree_of() >= poly.find_degree_of() && !r.poly_map.empty()) {
        // get leading terms
        power lead_divisor_power = poly.find_degree_of();
        coeff lead_divisor_coeff = poly.poly_map.at(lead_divisor_power);
        power lead_remainder_power = r.find_degree_of();
        coeff lead_remainder_coeff = r.poly_map.at(lead_remainder_power);
        
        // divide leading terms    t = lead(r) / lead(d)
        power new_power_factor = lead_remainder_power - lead_divisor_power;
        coeff new_coeff_factor = lead_remainder_coeff / lead_divisor_coeff;

        // get the single quotient value
        polynomial q;               // q = 0
        q.poly_map[new_power_factor] = new_coeff_factor;

        // multiply quotient and the divisor(poly) then subtract from r
        polynomial subtract = poly * q;
        r = r - subtract;
    }

    polynomial p1;
    for (auto &[p,c] : r.poly_map) {
        if (c != 0) {
            p1.poly_map[p] = c;
        }
    }
    if (p1.poly_map.size() == 0) {  // if the remainder is zero then p1 will have nothing initialized so set a {0,0} pair
        p1.poly_map[0] = 0;
    }

    return p1;
}*/

size_t polynomial::find_degree_of() const{
    power greatest_power = 0;
    for (auto &[p,c] : poly_map) {
        if (p > greatest_power) {
            greatest_power = p;
        }
    }
    return greatest_power;
}

std::vector<std::pair<power, coeff>> polynomial::canonical_form() const {               // PASSED

    std::vector<std::pair<power, coeff>> vec = {};
    for (auto poly_pair : poly_map) {           // converts to a vector
        if (poly_pair.second != 0) {                                       // if the coefficient is zero don't include it in the canonical_form      
            vec.push_back(poly_pair);
        }
    }

    if (vec.size() == 0) {      // if the vector is empty create a {{0,0}} vec || otherwise perform the sort
        vec.push_back({0,0});
    }
    else {
        std::sort(vec.begin(), vec.end(), std::greater<>());
    }

    return vec;
}